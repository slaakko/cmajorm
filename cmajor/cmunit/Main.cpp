#include <cmajor/ast/InitDone.hpp>
#include <cmajor/ast/Identifier.hpp>
#include <cmajor/ast/BasicType.hpp>
#include <cmajor/ast/Expression.hpp>
#include <cmajor/ast/TypeExpr.hpp>
#include <cmajor/ast/Literal.hpp>
#include <cmajor/parsing/InitDone.hpp>
#include <cmajor/util/InitDone.hpp>
#include <cmajor/symbols/InitDone.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/SymbolCreatorVisitor.hpp>
#include <cmajor/symbols/Warning.hpp>
#include <cmajor/binder/BoundStatement.hpp>
#include <cmajor/binder/AttributeBinder.hpp>
#include <cmajor/binder/TypeBinder.hpp>
#include <cmajor/binder/StatementBinder.hpp>
#include <cmajor/binder/ControlFlowAnalyzer.hpp>
#include <cmajor/build/Build.hpp>
#include <cmajor/codegen/EmittingContext.hpp>
#include <cmajor/codegen/Interface.hpp>
#include <cmajor/parser/Project.hpp>
#include <cmajor/parser/Solution.hpp>
#include <cmajor/parser/CompileUnit.hpp>
#include <cmajor/parsing/Exception.hpp>
#include <cmajor/dom/Document.hpp>
#include <cmajor/dom/Element.hpp>
#include <cmajor/dom/CharacterData.hpp>
#include <cmajor/dom/Parser.hpp>
#include <cmajor/xpath/InitDone.hpp>
#include <cmajor/xpath/XPathEvaluate.hpp>
#include <cmajor/util/Util.hpp>
#include <cmajor/util/Path.hpp>
#include <cmajor/util/Json.hpp>
#include <cmajor/util/Unicode.hpp>
#include <cmajor/util/Time.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <chrono>

struct InitDone
{
    InitDone()
    {
        cmajor::ast::Init();
        cmajor::symbols::Init();
        cmajor::parsing::Init();
        cmajor::util::Init();
        cmajor::xpath::Init();
    }
    ~InitDone()
    {
        cmajor::xpath::Done();
        cmajor::util::Done();
        cmajor::parsing::Done();
        cmajor::symbols::Done();
        cmajor::ast::Done();
    }
};

using namespace cmajor::ast;
using namespace cmajor::util;
using namespace cmajor::unicode;
using namespace cmajor::symbols;
using namespace cmajor::parser;
using namespace cmajor::parsing;
using namespace cmajor::build;
using namespace cmajor::binder;
using namespace cmajor::dom;
using namespace cmajor::xpath;

const char* version = "3.3.0";

void PrintHelp()
{
    std::cout << "Cmajor Unit Test Engine version " << version << std::endl;
    std::cout << "Usage: cmunit [options] { project.cmp | solution.cms }" << std::endl;
    std::cout << "Options:\n" <<
        "--verbose (-v)\n" <<
        "   be verbose\n" <<
        "--help (-h)\n" <<
        "   print this help message\n" <<
        "--config=CONFIG | -c=CONFIG\n" <<
        "   run unit tests using configuration CONFIG (default is \"debug\")\n" <<
        "--file=FILE | -f=FILE\n" <<
        "   run only unit tests in file FILE\n" <<
        "--test=TEST | -t=TEST\n" <<
        "   run only unit test TEST\n" <<
        "--gen-debug-info (-g)\n" <<
        "   generate debug info (on by default in debug configuration)\n" <<
        "--no-debug-info (-n)\n" <<
        "   don't generate debug info even for debug build\n" <<
        std::endl;
}

CompileUnit* compileUnitGrammar = nullptr;

bool unitTestsFound = false;

void CreateSymbols(SymbolTable& symbolTable, CompileUnitNode* testUnit)
{
    SymbolCreatorVisitor symbolCreator(symbolTable);
    symbolTable.SetCurrentCompileUnit(testUnit);
    testUnit->Accept(symbolCreator);
}

std::unique_ptr<BoundCompileUnit> BindTypes(Module& module, CompileUnitNode* testUnit, AttributeBinder* attributeBinder)
{
    std::unique_ptr<BoundCompileUnit> boundCompileUnit(new BoundCompileUnit(module, testUnit, attributeBinder));
    boundCompileUnit->PushBindingTypes();
    TypeBinder typeBinder(*boundCompileUnit);
    testUnit->Accept(typeBinder);
    boundCompileUnit->PopBindingTypes();
    return boundCompileUnit;
}

void BindStatements(BoundCompileUnit& boundCompileUnit)
{
    StatementBinder statementBinder(boundCompileUnit);
    boundCompileUnit.GetCompileUnitNode()->Accept(statementBinder);
}

void CreateMainUnit(std::vector<std::string>& objectFilePaths, Module& module, cmajor::codegen::EmittingContext& emittingContext, AttributeBinder* attributeBinder, const std::string& testName,
    int32_t numAssertions, const std::string& unitTestFilePath)
{
    CompileUnitNode mainCompileUnit(Span(), boost::filesystem::path(module.OriginalFilePath()).parent_path().append("__main__.cm").generic_string());
    mainCompileUnit.SetSynthesizedUnit();
    mainCompileUnit.GlobalNs()->AddMember(new NamespaceImportNode(Span(), new IdentifierNode(Span(), U"System")));
    mainCompileUnit.GlobalNs()->AddMember(MakePolymorphicClassArray(module.GetSymbolTable().PolymorphicClasses(), U"@polymorphicClassArray"));
    mainCompileUnit.GlobalNs()->AddMember(MakeStaticClassArray(module.GetSymbolTable().ClassesHavingStaticConstructor(), U"@staticClassArray"));
    FunctionNode* mainFunction(new FunctionNode(Span(), Specifiers::public_, new IntNode(Span()), U"main", nullptr));
#ifndef _WIN32
    mainFunction->AddParameter(new ParameterNode(Span(), new IntNode(Span()), new IdentifierNode(Span(), U"argc")));
    mainFunction->AddParameter(new ParameterNode(Span(), new PointerNode(Span(), new PointerNode(Span(), new CharNode(Span()))), new IdentifierNode(Span(), U"argv")));
#endif
    mainFunction->SetProgramMain();
    CompoundStatementNode* mainFunctionBody = new CompoundStatementNode(Span());
    ConstructionStatementNode* constructExitCode = new ConstructionStatementNode(Span(), new IntNode(Span()), new IdentifierNode(Span(), U"exitCode"));
    mainFunctionBody->AddStatement(constructExitCode);
    InvokeNode* invokeStartUnitTest = new InvokeNode(Span(), new IdentifierNode(Span(), U"RtStartUnitTest")); 
    invokeStartUnitTest->AddArgument(new IntLiteralNode(Span(), numAssertions));
    invokeStartUnitTest->AddArgument(new StringLiteralNode(Span(), unitTestFilePath));
    invokeStartUnitTest->AddArgument(new DivNode(Span(),
        new InvokeNode(Span(), new DotNode(Span(), new IdentifierNode(Span(), U"@polymorphicClassArray"), new IdentifierNode(Span(), U"Length"))),
        new LongLiteralNode(Span(), 3))); // 3 64-bit integers per entry
    invokeStartUnitTest->AddArgument(new InvokeNode(Span(), new DotNode(Span(), new IdentifierNode(Span(), U"@polymorphicClassArray"), new IdentifierNode(Span(), U"CBegin"))));
    invokeStartUnitTest->AddArgument(new DivNode(Span(),
        new InvokeNode(Span(), new DotNode(Span(), new IdentifierNode(Span(), U"@staticClassArray"), new IdentifierNode(Span(), U"Length"))),
        new LongLiteralNode(Span(), 2))); // 2 64-bit integers per entry
    invokeStartUnitTest->AddArgument(new InvokeNode(Span(), new DotNode(Span(), new IdentifierNode(Span(), U"@staticClassArray"), new IdentifierNode(Span(), U"CBegin"))));
    ExpressionStatementNode* rtStartUnitTestCall = new ExpressionStatementNode(Span(), invokeStartUnitTest);
    mainFunctionBody->AddStatement(rtStartUnitTestCall);
#ifdef _WIN32
    ConstructionStatementNode* argc = new ConstructionStatementNode(Span(), new IntNode(Span()), new IdentifierNode(Span(), U"argc"));
    argc->AddArgument(new InvokeNode(Span(), new IdentifierNode(Span(), U"RtArgc")));
    mainFunctionBody->AddStatement(argc);
    ConstructionStatementNode* argv = new ConstructionStatementNode(Span(), new ConstNode(Span(), new PointerNode(Span(), new PointerNode(Span(), new CharNode(Span())))), new IdentifierNode(Span(), U"argv"));
    argv->AddArgument(new InvokeNode(Span(), new IdentifierNode(Span(), U"RtArgv")));
    mainFunctionBody->AddStatement(argv);
#endif
    CompoundStatementNode* tryBlock = new CompoundStatementNode(Span());
    InvokeNode* invokeTest = new InvokeNode(Span(), new IdentifierNode(Span(), ToUtf32(testName)));
    StatementNode* callMainStatement = new ExpressionStatementNode(Span(), invokeTest);
    tryBlock->AddStatement(callMainStatement);
    TryStatementNode* tryStatement = new TryStatementNode(Span(), tryBlock);
    CompoundStatementNode* catchBlock = new CompoundStatementNode(Span());
    InvokeNode* consoleError = new InvokeNode(Span(), new DotNode(Span(), new IdentifierNode(Span(), U"System.Console"), new IdentifierNode(Span(), U"Error")));
    DotNode* writeLine = new DotNode(Span(), consoleError, new IdentifierNode(Span(), U"WriteLine"));
    InvokeNode* printEx = new InvokeNode(Span(), writeLine);
    InvokeNode* exToString = new InvokeNode(Span(), new DotNode(Span(), new IdentifierNode(Span(), U"ex"), new IdentifierNode(Span(), U"ToString")));
    printEx->AddArgument(exToString);
    ExpressionStatementNode* printExStatement = new ExpressionStatementNode(Span(), printEx);
    catchBlock->AddStatement(printExStatement);
    InvokeNode* setEx = new InvokeNode(Span(), new IdentifierNode(Span(), U"RtSetUnitTestException"));
    ConstructionStatementNode* constructExStr = new ConstructionStatementNode(Span(), new IdentifierNode(Span(), U"string"), new IdentifierNode(Span(), U"@ex"));
    CloneContext cloneContext;
    constructExStr->AddArgument(exToString->Clone(cloneContext));
    catchBlock->AddStatement(constructExStr);
    InvokeNode* exStr = new InvokeNode(Span(), new DotNode(Span(), new IdentifierNode(Span(), U"@ex"), new IdentifierNode(Span(), U"Chars")));
    setEx->AddArgument(exStr);
    ExpressionStatementNode* setExStatement = new ExpressionStatementNode(Span(), setEx);
    catchBlock->AddStatement(setExStatement);
    AssignmentStatementNode* assignExitCodeStatement = new AssignmentStatementNode(Span(), new IdentifierNode(Span(), U"exitCode"), new IntLiteralNode(Span(), 1));
    catchBlock->AddStatement(assignExitCodeStatement);
    CatchNode* catchAll = new CatchNode(Span(), new ConstNode(Span(), new LValueRefNode(Span(), new IdentifierNode(Span(), U"System.Exception"))), new IdentifierNode(Span(), U"ex"), catchBlock);
    tryStatement->AddCatch(catchAll);
    mainFunctionBody->AddStatement(tryStatement);
    InvokeNode* invokeEndUnitTest = new InvokeNode(Span(), new IdentifierNode(Span(), U"RtEndUnitTest"));
    invokeEndUnitTest->AddArgument(new StringLiteralNode(Span(), testName));
    invokeEndUnitTest->AddArgument(new IdentifierNode(Span(), U"exitCode"));
    ExpressionStatementNode* endUnitTestCall = new ExpressionStatementNode(Span(), invokeEndUnitTest);
    mainFunctionBody->AddStatement(endUnitTestCall);
    ReturnStatementNode* returnStatement = new ReturnStatementNode(Span(), new IdentifierNode(Span(), U"exitCode"));
    mainFunctionBody->AddStatement(returnStatement);
    mainFunction->SetBody(mainFunctionBody);
    mainCompileUnit.GlobalNs()->AddMember(mainFunction);
    SymbolCreatorVisitor symbolCreator(module.GetSymbolTable());
    mainCompileUnit.Accept(symbolCreator);
    BoundCompileUnit boundMainCompileUnit(module, &mainCompileUnit, attributeBinder);
    boundMainCompileUnit.PushBindingTypes();
    TypeBinder typeBinder(boundMainCompileUnit);
    mainCompileUnit.Accept(typeBinder);
    boundMainCompileUnit.PopBindingTypes();
    StatementBinder statementBinder(boundMainCompileUnit);
    mainCompileUnit.Accept(statementBinder);
    if (boundMainCompileUnit.HasGotos())
    {
        AnalyzeControlFlow(boundMainCompileUnit);
    }
    GenerateCode(emittingContext, boundMainCompileUnit);
    objectFilePaths.push_back(boundMainCompileUnit.ObjectFilePath());
}

struct UnitTest
{
    UnitTest() : prevUnitTest(BeginUnitTest()) {}
    ~UnitTest() { EndUnitTest(prevUnitTest); }
    bool prevUnitTest;
};

int RunUnitTest(cmajor::ast::Project* project)
{
    unitTestsFound = true;
    int exitCode = system(project->ExecutableFilePath().c_str());
    return exitCode;
}

void TestUnit(FileTable* fileTable, cmajor::ast::Project* project, CompileUnitNode* testUnit, const std::string& testName, Element* sourceFileElement, std::unique_ptr<Module>& rootModule)
{
    bool compileError = false;
    std::string compileErrorMessage;
    std::vector<int32_t> assertionLineNumbers;
    ResetUnitTestAssertionNumber();
    SetAssertionLineNumberVector(&assertionLineNumbers);
    std::string unitTestFilePath = Path::Combine(Path::GetDirectoryName(project->ExecutableFilePath()), "test.xml");
    try
    {
        std::string config = GetConfig();
        rootModule.reset(new Module(project->Name(), project->ModuleFilePath()));
        rootModule->SetRootModule();
        SetRootModuleForCurrentThread(rootModule.get());
        rootModule->SetCurrentToolName(U"cmc");
        rootModule->SetCurrentProjectName(project->Name());
        int16_t numFiles = fileTable->NumFilePaths();
        for (int16_t i = 0; i < numFiles; ++i)
        {
            rootModule->GetFileTable().RegisterFilePath(fileTable->GetFilePath(i));
        }
        AttributeBinder attributeBinder(rootModule.get());
        rootModule->PrepareForCompilation(project->References());
        cmajor::symbols::MetaInit(rootModule->GetSymbolTable());
        CreateSymbols(rootModule->GetSymbolTable(), testUnit);
        std::vector<std::string> objectFilePaths;
        cmajor::codegen::EmittingContext emittingContext(GetOptimizationLevel());
        {
            UnitTest unitTest;
            std::unique_ptr<BoundCompileUnit> boundCompileUnit = BindTypes(*rootModule, testUnit, &attributeBinder);
            BindStatements(*boundCompileUnit);
            if (boundCompileUnit->HasGotos())
            {
                AnalyzeControlFlow(*boundCompileUnit);
            }
            GenerateCode(emittingContext, *boundCompileUnit);
            objectFilePaths.push_back(boundCompileUnit->ObjectFilePath());
        }
        int32_t numUnitTestAssertions = GetNumUnitTestAssertions();
        CreateMainUnit(objectFilePaths, *rootModule, emittingContext, &attributeBinder, testName, numUnitTestAssertions, unitTestFilePath);
        GenerateLibrary(rootModule.get(), objectFilePaths, project->LibraryFilePath());
        Link(project->ExecutableFilePath(), project->LibraryFilePath(), rootModule->LibraryFilePaths(), *rootModule);
        boost::filesystem::remove(unitTestFilePath);
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            std::cout << ">> " << testName << std::endl;
        }
        int exitCode = RunUnitTest(project);
        if (FileExists(unitTestFilePath))
        {
            std::unique_ptr<Document> testResultDoc = ReadDocument(unitTestFilePath);
            sourceFileElement->AppendChild(testResultDoc->DocumentElement()->CloneNode(true));
        }
        else
        {
            std::unique_ptr<cmajor::dom::Element> testElement(new cmajor::dom::Element(U"test"));
            testElement->SetAttribute(U"name", ToUtf32(testName));
            testElement->SetAttribute(U"exitCode", ToUtf32(std::to_string(exitCode)));
            for (int32_t i = 0; i < numUnitTestAssertions; ++i)
            {
                std::unique_ptr<cmajor::dom::Element> assertionElement(new cmajor::dom::Element(U"assertion"));
                assertionElement->SetAttribute(U"index", ToUtf32(std::to_string(i)));
                std::u32string assertionResultStr = U"empty";
                assertionElement->SetAttribute(U"result", assertionResultStr);
                if (i < assertionLineNumbers.size())
                {
                    assertionElement->SetAttribute(U"line", ToUtf32(std::to_string(assertionLineNumbers[i])));
                }
                testElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(assertionElement.release()));
            }
            sourceFileElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(testElement.release()));
        }
    }
    catch (const ParsingException& ex)
    {
        compileError = true;
        compileErrorMessage = ex.what();
    }
    catch (const Exception& ex)
    {
        compileError = true;
        compileErrorMessage = ex.What();
    }
    catch (const std::exception& ex)
    {
        compileError = true;
        compileErrorMessage = ex.what();
    }
    if (compileError)
    {
        std::unique_ptr<cmajor::dom::Element> testElement(new cmajor::dom::Element(U"test"));
        testElement->SetAttribute(U"name", ToUtf32(testName));
        testElement->SetAttribute(U"compileError", ToUtf32(compileErrorMessage));
        sourceFileElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(testElement.release()));
    }
}

bool TestNameEquals(const std::string& testName, const std::string& testUnitName)
{
    std::vector<std::string> testNameComponents = Split(testName, '.');
    std::vector<std::string> testUnitNameComponents = Split(testUnitName, '.');
    int n = std::min(testNameComponents.size(), testUnitNameComponents.size());
    for (int i = 0; i < n; ++i)
    {
        int k = testNameComponents.size() - i - 1;
        int l = testUnitNameComponents.size() - i - 1;
        if (testNameComponents[k] != testUnitNameComponents[l]) return false;
    }
    return true;
}

std::vector<std::pair<std::unique_ptr<CompileUnitNode>, std::string>> SplitIntoTestUnits(CompileUnitNode* compileUnit)
{
    std::vector<std::pair<std::unique_ptr<CompileUnitNode>, std::string>> testUnits;
    CloneContext makeUnitTestUnitContext;
    makeUnitTestUnitContext.SetMakeTestUnits();
    std::unique_ptr<CompileUnitNode> environmentNode(static_cast<CompileUnitNode*>(compileUnit->Clone(makeUnitTestUnitContext)));
    for (FunctionNode* unitTestFunction : makeUnitTestUnitContext.UnitTestFunctions())
    {
        std::string unitTestName = ToUtf8(unitTestFunction->GroupId());
        CloneContext testUnitContext;
        std::pair<std::unique_ptr<CompileUnitNode>, std::string> testUnit = std::make_pair(
            std::unique_ptr<CompileUnitNode>(static_cast<CompileUnitNode*>(environmentNode->Clone(testUnitContext))), unitTestName);
        NamespaceNode* ns = testUnit.first->GlobalNs();
        FunctionNode* unitTestFun = unitTestFunction;
        ns->AddMember(unitTestFun);
        testUnits.push_back(std::move(testUnit));
    }
    makeUnitTestUnitContext.UnitTestFunctions().clear();
    return testUnits;
}

void TestSourceFile(bool& first, cmajor::ast::Project* project, const std::string& sourceFilePath, const std::string& onlyTest, cmajor::dom::Element* projectElement, 
    std::unique_ptr<Module>& rootModule)
{
    std::unique_ptr<cmajor::dom::Element> sourceFileElement(new cmajor::dom::Element(U"sourceFile"));
    sourceFileElement->SetAttribute(U"name", ToUtf32(Path::GetFileNameWithoutExtension(sourceFilePath)));
    if (!compileUnitGrammar)
    {
        compileUnitGrammar = CompileUnit::Create();
    }
    MappedInputFile sourceFile(sourceFilePath);
    FileTable fileTable;
    uint32_t fileIndex = fileTable.RegisterFilePath(sourceFilePath);
    ParsingContext parsingContext;
    std::u32string s(ToUtf32(std::string(sourceFile.Begin(), sourceFile.End())));
    std::unique_ptr<CompileUnitNode> compileUnit(compileUnitGrammar->Parse(&s[0], &s[0] + s.length(), fileIndex, sourceFilePath, &parsingContext));
    std::vector<std::pair<std::unique_ptr<CompileUnitNode>, std::string>> testUnits = SplitIntoTestUnits(compileUnit.get());
    if (!testUnits.empty())
    {
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            if (first)
            {
                first = false;
                std::cout << ToUtf8(project->Name()) << " : " << GetConfig() << std::endl;
            }
            std::cout << "> " << sourceFilePath << std::endl;
        }
    }
    for (const auto& p : testUnits)
    {
        if (!onlyTest.empty())
        {
            if (!TestNameEquals(onlyTest, p.second))
            {
                continue;
            }
        }
        TestUnit(&fileTable, project, p.first.get(), p.second, sourceFileElement.get(), rootModule);
    }
    projectElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(sourceFileElement.release()));
}

bool SourceFileNameEquals(const std::string& fileName, const std::string& sourceFilePath)
{
    boost::filesystem::path filePath = fileName;
    std::vector<std::string> filePathComponents = Split(filePath.generic_string(), '/');
    std::vector<std::string> sourceFilePathComponents = Split(sourceFilePath, '/');
    int n = std::min(filePathComponents.size(), sourceFilePathComponents.size());
    for (int i = 0; i < n; ++i)
    {
        int k = filePathComponents.size() - i - 1;
        int l = sourceFilePathComponents.size() - i - 1;
        if (filePathComponents[k] != sourceFilePathComponents[l]) return false;
    }
    return true;
}

void TestProject(cmajor::ast::Project* project, const std::string& onlySourceFile, const std::string& onlyTest, cmajor::dom::Element* parentElement, std::unique_ptr<Module>& rootModule)
{
    if (project->GetTarget() != Target::unitTest)
    {
        throw std::runtime_error("project '" + ToUtf8(project->Name()) + "' is not a unit testing project");
    }
    bool first = true;
    std::unique_ptr<cmajor::dom::Element> projectElement(new cmajor::dom::Element(U"project"));
    projectElement->SetAttribute(U"name", project->Name());
    std::string config = GetConfig();
    for (const std::string& sourceFilePath : project->SourceFilePaths())
    {
        if (!onlySourceFile.empty())
        {
            if (!SourceFileNameEquals(onlySourceFile, sourceFilePath))
            {
                continue;
            }
        }
        TestSourceFile(first, project, sourceFilePath, onlyTest, projectElement.get(), rootModule);
    }
    parentElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(projectElement.release()));
}

cmajor::parser::Solution* solutionGrammar = nullptr;
cmajor::parser::Project* projectGrammar = nullptr;

bool TestProject(const std::string& projectFileName, const std::string& onlySourceFile, const std::string& onlyTest, cmajor::dom::Element* parentElement, 
    std::unique_ptr<Module>& rootModule)
{
    std::string config = GetConfig();
    if (!projectGrammar)
    {
        projectGrammar = cmajor::parser::Project::Create();
    }
    MappedInputFile projectFile(projectFileName);
    std::u32string p(ToUtf32(std::string(projectFile.Begin(), projectFile.End())));
    std::unique_ptr<cmajor::ast::Project> project(projectGrammar->Parse(&p[0], &p[0] + p.length(), 0, projectFileName, config));
    project->ResolveDeclarations();
    if (project->GetTarget() != Target::unitTest)
    {
        return false;
    }
    TestProject(project.get(), onlySourceFile, onlyTest, parentElement, rootModule);
    return true;
}

bool TestSolution(const std::string& solutionFileName, const std::string& onlySourceFile, const std::string& onlyTest, cmajor::dom::Element* cmunitElement,
    std::unique_ptr<Module>& rootModule)
{
    std::unique_ptr<cmajor::dom::Element> solutionElement(new cmajor::dom::Element(U"solution"));
    if (!solutionGrammar)
    {
        solutionGrammar = cmajor::parser::Solution::Create();
    }
    if (!projectGrammar)
    {
        projectGrammar = cmajor::parser::Project::Create();
    }
    MappedInputFile solutionFile(solutionFileName);
    std::u32string s(ToUtf32(std::string(solutionFile.Begin(), solutionFile.End())));
    std::unique_ptr<cmajor::ast::Solution> solution(solutionGrammar->Parse(&s[0], &s[0] + s.length(), 0, solutionFileName));
    solutionElement->SetAttribute(U"name", solution->Name());
    solution->ResolveDeclarations();
    std::string config = GetConfig();
    bool containsUnitTestProject = false;
    for (const std::string& projectFilePath : solution->ProjectFilePaths())
    {
        bool unitTestProject = TestProject(projectFilePath, onlySourceFile, onlyTest, solutionElement.get(), rootModule);
        if (unitTestProject)
        {
            containsUnitTestProject = true;
        }
    }
    if (containsUnitTestProject)
    {
        cmunitElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(solutionElement.release()));
    }
    return containsUnitTestProject;
}

std::unique_ptr<cmajor::dom::Document> GenerateHtmlReport(cmajor::dom::Document* testDoc)
{
    std::unique_ptr<cmajor::dom::Document> reportDoc(new cmajor::dom::Document());
    std::unique_ptr<cmajor::dom::Element> htmlElement(new cmajor::dom::Element(U"html"));
    std::unique_ptr<cmajor::dom::Element> headElement(new cmajor::dom::Element(U"head"));
    std::unique_ptr<cmajor::dom::Element> titleElement(new cmajor::dom::Element(U"title"));
    titleElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"Unit Test Report")));
    std::unique_ptr<cmajor::dom::Element> styleElement(new cmajor::dom::Element(U"style"));
    std::u32string style = 
        U"body { max-width: 800px; } h1, h2, h3, h4, h5, h6 { color: #005ab4; font-family: sans-serif; } table { boder-collapse: collapse; } table, th, td { text-align: left; border: 1px solid #dddddd; padding: 8px; }";
    styleElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(style)));
    std::unique_ptr<cmajor::dom::Element> bodyElement(new cmajor::dom::Element(U"body"));
    std::unique_ptr<cmajor::dom::Element> h1Element(new cmajor::dom::Element(U"h1"));
    h1Element->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"Unit Test Report")));
    bodyElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(h1Element.release()));
    cmajor::dom::Element* cmunitElement = testDoc->DocumentElement();

    std::unique_ptr<cmajor::dom::Element> paramTableElement(new cmajor::dom::Element(U"table"));

    std::u32string configuration = cmunitElement->GetAttribute(U"config");
    std::unique_ptr<cmajor::dom::Element> trConfigElement(new cmajor::dom::Element(U"tr"));
    std::unique_ptr<cmajor::dom::Element> thConfigElement(new cmajor::dom::Element(U"th"));
    thConfigElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"configuration")));
    trConfigElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thConfigElement.release()));
    std::unique_ptr<cmajor::dom::Element> tdConfigElement(new cmajor::dom::Element(U"td"));
    tdConfigElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(configuration)));
    trConfigElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdConfigElement.release()));
    paramTableElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(trConfigElement.release()));

    std::u32string start = cmunitElement->GetAttribute(U"start");
    std::unique_ptr<cmajor::dom::Element> trStartElement(new cmajor::dom::Element(U"tr"));
    std::unique_ptr<cmajor::dom::Element> thStartElement(new cmajor::dom::Element(U"th"));
    thStartElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"start")));
    trStartElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thStartElement.release()));
    std::unique_ptr<cmajor::dom::Element> tdStartElement(new cmajor::dom::Element(U"td"));
    tdStartElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(start)));
    trStartElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdStartElement.release()));
    paramTableElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(trStartElement.release()));

    std::u32string end = cmunitElement->GetAttribute(U"end");
    std::unique_ptr<cmajor::dom::Element> trEndElement(new cmajor::dom::Element(U"tr"));
    std::unique_ptr<cmajor::dom::Element> thEndElement(new cmajor::dom::Element(U"th"));
    thEndElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"end")));
    trEndElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thEndElement.release()));
    std::unique_ptr<cmajor::dom::Element> tdEndElement(new cmajor::dom::Element(U"td"));
    tdEndElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(end)));
    trEndElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdEndElement.release()));
    paramTableElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(trEndElement.release()));

    std::u32string duration = cmunitElement->GetAttribute(U"duration");
    std::unique_ptr<cmajor::dom::Element> trDurationElement(new cmajor::dom::Element(U"tr"));
    std::unique_ptr<cmajor::dom::Element> thDurationElement(new cmajor::dom::Element(U"th"));
    thDurationElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"duration")));
    trDurationElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thDurationElement.release()));
    std::unique_ptr<cmajor::dom::Element> tdDurationElement(new cmajor::dom::Element(U"td"));
    tdDurationElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(duration)));
    trDurationElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdDurationElement.release()));
    paramTableElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(trDurationElement.release()));

    std::u32string file = cmunitElement->GetAttribute(U"file");
    std::unique_ptr<cmajor::dom::Element> trFileElement(new cmajor::dom::Element(U"tr"));
    std::unique_ptr<cmajor::dom::Element> thFileElement(new cmajor::dom::Element(U"th"));
    thFileElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"file")));
    trFileElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thFileElement.release()));
    std::unique_ptr<cmajor::dom::Element> tdFileElement(new cmajor::dom::Element(U"td"));
    tdFileElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(file)));
    trFileElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdFileElement.release()));
    paramTableElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(trFileElement.release()));

    std::u32string test = cmunitElement->GetAttribute(U"test");
    std::unique_ptr<cmajor::dom::Element> trTestElement(new cmajor::dom::Element(U"tr"));
    std::unique_ptr<cmajor::dom::Element> thTestElement(new cmajor::dom::Element(U"th"));
    thTestElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"test")));
    trTestElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thTestElement.release()));
    std::unique_ptr<cmajor::dom::Element> tdTestElement(new cmajor::dom::Element(U"td"));
    tdTestElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(test)));
    trTestElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdTestElement.release()));
    paramTableElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(trTestElement.release()));

    bodyElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(paramTableElement.release()));

    std::unique_ptr<cmajor::dom::Element> h2ComponentsElement(new cmajor::dom::Element(U"h2"));
    h2ComponentsElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"Components")));
    bodyElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(h2ComponentsElement.release()));

    std::unique_ptr<cmajor::dom::Element> ulElement(new cmajor::dom::Element(U"ul"));
    std::unique_ptr<XPathObject> components = Evaluate(U"cmunit/components/component/text()", testDoc);
    if (components && components->Type() == cmajor::xpath::XPathObjectType::nodeSet)
    {
        XPathNodeSet* componentsSet = static_cast<XPathNodeSet*>(components.get());
        int n = componentsSet->Length();
        for (int i = 0; i < n; ++i)
        {
            cmajor::dom::Node* componentNode = (*componentsSet)[i];
            if (componentNode->GetNodeType() == cmajor::dom::NodeType::textNode)
            {
                cmajor::dom::Text* component = static_cast<cmajor::dom::Text*>(componentNode);
                std::unique_ptr<cmajor::dom::Element> liElement(new cmajor::dom::Element(U"li"));
                liElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(component->Data())));
                ulElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(liElement.release()));
            }
        }
    }
    bodyElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(ulElement.release()));

    std::unique_ptr<cmajor::dom::Element> h2Element(new cmajor::dom::Element(U"h2"));
    h2Element->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"Results")));
    bodyElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(h2Element.release()));

    std::unique_ptr<cmajor::dom::Element> tableElement(new cmajor::dom::Element(U"table"));

    std::unique_ptr<cmajor::dom::Element> trHeaderElement(new cmajor::dom::Element(U"tr"));
    std::unique_ptr<cmajor::dom::Element> thNameElement(new cmajor::dom::Element(U"th"));
    thNameElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"name")));
    trHeaderElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thNameElement.release()));

    std::unique_ptr<cmajor::dom::Element> thCountElement(new cmajor::dom::Element(U"th"));
    thCountElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"count")));
    trHeaderElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thCountElement.release()));

    std::unique_ptr<cmajor::dom::Element> thPassedElement(new cmajor::dom::Element(U"th"));
    thPassedElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"passed")));
    trHeaderElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thPassedElement.release()));

    std::unique_ptr<cmajor::dom::Element> thFailedElement(new cmajor::dom::Element(U"th"));
    thFailedElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"failed")));
    trHeaderElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thFailedElement.release()));

    std::unique_ptr<cmajor::dom::Element> thEmptyElement(new cmajor::dom::Element(U"th"));
    thEmptyElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"empty")));
    trHeaderElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thEmptyElement.release()));

    std::unique_ptr<cmajor::dom::Element> thExitCodeElement(new cmajor::dom::Element(U"th"));
    thExitCodeElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"exit code")));
    trHeaderElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thExitCodeElement.release()));

    std::unique_ptr<cmajor::dom::Element> thCompileErrorElement(new cmajor::dom::Element(U"th"));
    thCompileErrorElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"compile error")));
    trHeaderElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thCompileErrorElement.release()));

    std::unique_ptr<cmajor::dom::Element> thExceptionElement(new cmajor::dom::Element(U"th"));
    thExceptionElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"exception")));
    trHeaderElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thExceptionElement.release()));

    std::vector<std::pair<std::u32string, cmajor::dom::Element*>> failedAssertions;

    tableElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(trHeaderElement.release()));

    std::unique_ptr<XPathObject> tests = Evaluate(U"//test", testDoc);
    if (tests && tests->Type() == XPathObjectType::nodeSet)
    {
        XPathNodeSet* testElements = static_cast<XPathNodeSet*>(tests.get());
        int n = testElements->Length();
        for (int i = 0; i < n; ++i)
        {
            cmajor::dom::Node* testNode = (*testElements)[i];
            if (testNode->GetNodeType() == cmajor::dom::NodeType::elementNode)
            {
                cmajor::dom::Element* testElement = static_cast<cmajor::dom::Element*>(testNode);
                std::u32string name = testElement->GetAttribute(U"name");
                std::u32string fullTestName = name;
                cmajor::dom::ParentNode* sourceFileNode = testElement->Parent();
                if (sourceFileNode)
                {
                    if (sourceFileNode->GetNodeType() == cmajor::dom::NodeType::elementNode)
                    {
                        cmajor::dom::Element* sourceFileElement = static_cast<cmajor::dom::Element*>(sourceFileNode);
                        name = sourceFileElement->GetAttribute(U"name");
                        fullTestName = name + U"." + fullTestName;
                        cmajor::dom::ParentNode* projectNode = sourceFileElement->Parent();
                        if (projectNode)
                        {
                            if (projectNode->GetNodeType() == cmajor::dom::NodeType::elementNode)
                            {
                                cmajor::dom::Element* projectElement = static_cast<cmajor::dom::Element*>(projectNode);
                                name = projectElement->GetAttribute(U"name");
                                fullTestName = name + U"." + fullTestName;
                                cmajor::dom::ParentNode* parentNode = projectElement->Parent();
                                if (parentNode)
                                {
                                    if (parentNode->GetNodeType() == cmajor::dom::NodeType::elementNode)
                                    {
                                        cmajor::dom::Element* parentElement = static_cast<cmajor::dom::Element*>(parentNode);
                                        if (parentElement->Name() == U"solution")
                                        {
                                            cmajor::dom::Element* solutionElement = parentElement;
                                            name = solutionElement->GetAttribute(U"name");
                                            fullTestName = name + U"." + fullTestName;
                                        }

                                        std::unique_ptr<cmajor::dom::Element> trElement(new cmajor::dom::Element(U"tr"));
                                        std::unique_ptr<cmajor::dom::Element> tdNameElement(new cmajor::dom::Element(U"td"));
                                        cmajor::dom::Element* nameElement = tdNameElement.get();
                                        tdNameElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(fullTestName)));
                                        trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdNameElement.release()));
                                        std::unique_ptr<cmajor::dom::Element> tdCountElement(new cmajor::dom::Element(U"td"));
                                        tdCountElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(testElement->GetAttribute(U"count"))));
                                        trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdCountElement.release()));
                                        std::unique_ptr<XPathObject> assertions = Evaluate(U"assertion", testElement);
                                        int passed = 0;
                                        int failed = 0;
                                        int empty = 0;
                                        int count = 0;
                                        if (assertions && assertions->Type() == cmajor::xpath::XPathObjectType::nodeSet)
                                        {
                                            XPathNodeSet* assertionElements = static_cast<XPathNodeSet*>(assertions.get());
                                            count = assertionElements->Length();
                                            for (int i = 0; i < count; ++i)
                                            {
                                                cmajor::dom::Node* assertionNode = (*assertionElements)[i];
                                                if (assertionNode->GetNodeType() == cmajor::dom::NodeType::elementNode)
                                                {
                                                    cmajor::dom::Element* assertionElement = static_cast<cmajor::dom::Element*>(assertionNode);
                                                    std::u32string result = assertionElement->GetAttribute(U"result");
                                                    if (result == U"passed")
                                                    {
                                                        ++passed;
                                                    }
                                                    else if (result == U"failed")
                                                    {
                                                        ++failed;
                                                        failedAssertions.push_back(std::make_pair(fullTestName, assertionElement));
                                                    }
                                                    else
                                                    {
                                                        ++empty;
                                                    }
                                                }
                                            }
                                        }
                                        std::u32string fontAttr = U"font-family: sans-serif; font-size: 13px; font-weight: bold;";
                                        if (count > 0)
                                        {
                                            if (passed == count)
                                            {
                                                nameElement->SetAttribute(U"style", U"background-color: #00e600; " + fontAttr);
                                            }
                                            else if (failed > 0)
                                            {
                                                nameElement->SetAttribute(U"style", U"background-color: #ff3300; " + fontAttr);
                                            }
                                            else
                                            {
                                                nameElement->SetAttribute(U"style", U"background-color: #ffff00; " + fontAttr);
                                            }
                                        }
                                        else
                                        {
                                            nameElement->SetAttribute(U"style", U"background-color: #ffff00; " + fontAttr);
                                        }
                                        std::unique_ptr<cmajor::dom::Element> tdPassedElement(new cmajor::dom::Element(U"td"));
                                        tdPassedElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(ToUtf32(std::to_string(passed)))));
                                        trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdPassedElement.release()));

                                        std::unique_ptr<cmajor::dom::Element> tdFailedElement(new cmajor::dom::Element(U"td"));
                                        tdFailedElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(ToUtf32(std::to_string(failed)))));
                                        trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdFailedElement.release()));

                                        if (count == 0)
                                        {
                                            std::unique_ptr<cmajor::dom::Element> tdEmptyElement(new cmajor::dom::Element(U"td"));
                                            tdEmptyElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(testElement->GetAttribute(U"count"))));
                                            trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdEmptyElement.release()));
                                        }
                                        else
                                        {
                                            std::unique_ptr<cmajor::dom::Element> tdEmptyElement(new cmajor::dom::Element(U"td"));
                                            tdEmptyElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(ToUtf32(std::to_string(empty)))));
                                            trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdEmptyElement.release()));
                                        }

                                        std::unique_ptr<cmajor::dom::Element> tdExitCodeElement(new cmajor::dom::Element(U"td"));
                                        tdExitCodeElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(testElement->GetAttribute(U"exitCode"))));
                                        trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdExitCodeElement.release()));
                                        std::unique_ptr<cmajor::dom::Element> tdCompileErrorElement(new cmajor::dom::Element(U"td")); 
                                        tdCompileErrorElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(testElement->GetAttribute(U"compileError"))));
                                        trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdCompileErrorElement.release()));
                                        std::unique_ptr<cmajor::dom::Element> tdExceptionElement(new cmajor::dom::Element(U"td")); 
                                        tdExceptionElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(testElement->GetAttribute(U"exception"))));
                                        trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdExceptionElement.release()));
                                        tableElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(trElement.release()));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    bodyElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tableElement.release()));

    if (!failedAssertions.empty())
    {
        std::unique_ptr<cmajor::dom::Element> h2Element(new cmajor::dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"Failed Assertions")));
        bodyElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(h2Element.release()));

        std::unique_ptr<cmajor::dom::Element> failedAssertionsTable(new cmajor::dom::Element(U"table"));

        std::unique_ptr<cmajor::dom::Element> trHeaderElement(new cmajor::dom::Element(U"tr"));

        std::unique_ptr<cmajor::dom::Element> thNameElement(new cmajor::dom::Element(U"th"));
        thNameElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"name")));
        trHeaderElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thNameElement.release()));

        std::unique_ptr<cmajor::dom::Element> thIndexElement(new cmajor::dom::Element(U"th"));
        thIndexElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"index")));
        trHeaderElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thIndexElement.release()));

        std::unique_ptr<cmajor::dom::Element> thLineElement(new cmajor::dom::Element(U"th"));
        thLineElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"line")));
        trHeaderElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thLineElement.release()));

        failedAssertionsTable->AppendChild(std::unique_ptr<cmajor::dom::Node>(trHeaderElement.release()));

        int n = failedAssertions.size();
        for (int i = 0; i < n; ++i)
        {
            const std::pair<std::u32string, cmajor::dom::Element*>& p = failedAssertions[i];
            std::unique_ptr<cmajor::dom::Element> trAssertionElement(new cmajor::dom::Element(U"tr"));
            cmajor::dom::Element* assertionElement = p.second;

            std::unique_ptr<cmajor::dom::Element> tdNameElement(new cmajor::dom::Element(U"td"));
            std::u32string attr = U"background-color: #ff3300; font-family: sans-serif; font-size: 13px; font-weight: bold;";
            tdNameElement->SetAttribute(U"style", attr);
            tdNameElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(p.first)));
            trAssertionElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdNameElement.release()));

            std::unique_ptr<cmajor::dom::Element> tdIndexElement(new cmajor::dom::Element(U"td"));
            tdIndexElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(assertionElement->GetAttribute(U"index"))));
            trAssertionElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdIndexElement.release()));

            std::unique_ptr<cmajor::dom::Element> tdLineElement(new cmajor::dom::Element(U"td"));
            tdLineElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(assertionElement->GetAttribute(U"line"))));
            trAssertionElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdLineElement.release()));

            failedAssertionsTable->AppendChild(std::unique_ptr<cmajor::dom::Node>(trAssertionElement.release()));
        }

        bodyElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(failedAssertionsTable.release()));
    }

    headElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(titleElement.release()));
    headElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(styleElement.release()));
    htmlElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(headElement.release()));
    htmlElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(bodyElement.release()));
    reportDoc->AppendChild(std::unique_ptr<cmajor::dom::Node>(htmlElement.release()));
    return reportDoc;
}

int main(int argc, const char** argv)
{
    std::unique_ptr<Module> rootModule;
    try
    {
        InitDone initDone;
        SetCompilerVersion(version);
        std::unique_ptr<cmajor::dom::Element> cmunitElement(new cmajor::dom::Element(U"cmunit"));
        cmunitElement->SetAttribute(U"start", ToUtf32(GetCurrentDateTime().ToString()));
        std::unique_ptr<cmajor::dom::Element> componentsElement(new cmajor::dom::Element(U"components"));
        std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
        SetGlobalFlag(GlobalFlags::unitTest);
        std::vector<std::string> projectsAndSolutions;
        std::string onlySourceFile;
        std::string onlyTest;
        std::string outFile;
        bool noDebugInfo = false;
        bool unitTestProjectsFound = false;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (!arg.empty() && arg[0] == '-')
            {
                if (arg == "--help" || arg == "-h")
                {
                    PrintHelp();
                    return 0;
                }
                else if (arg == "--verbose" || arg == "-v")
                {
                    SetGlobalFlag(GlobalFlags::verbose);
                }
                else if (arg == "--link-with-debug-runtime" || arg == "-d")
                {
                    SetGlobalFlag(GlobalFlags::linkWithDebugRuntime);
                }
                else if (arg == "--link-using-ms-link" || arg == "-m")
                {
                    SetGlobalFlag(GlobalFlags::linkUsingMsLink);
                }
                else if (arg == "--emit-llvm")
                {
                    SetGlobalFlag(GlobalFlags::emitLlvm);
                }
                else if (arg == "--gen-debug-info" || arg == "-g")
                {
                    SetGlobalFlag(GlobalFlags::generateDebugInfo);
                }
                else if (arg == "--no-debug-info" || arg == "-n")
                {
                    noDebugInfo = true;
                }
                else if (arg.find('=') != std::string::npos)
                {
                    std::vector<std::string> components = Split(arg, '=');
                    if (components.size() == 2)
                    {
                        if (components[0] == "--config" || components[0] == "-c")
                        {
                            if (components[1] == "release")
                            {
                                SetGlobalFlag(GlobalFlags::release);
                            }
                            else if (components[1] != "debug")
                            {
                                throw std::runtime_error("unknown configuration '" + components[1] + "'");
                            }
                        }
                        else if (components[0] == "--file" || components[0] == "-f")
                        {
                            onlySourceFile = components[1];
                            boost::filesystem::path p = onlySourceFile;
                            if (p.extension().empty())
                            {
                                onlySourceFile.append(".cm");
                            }
                        }
                        else if (components[0] == "--test" || components[0] == "-t")
                        {
                            onlyTest = components[1];
                        }
                        else if (components[0] == "--out" || components[0] == "-o")
                        {
                            outFile = components[1];
                        }
                        else
                        {
                            throw std::runtime_error("unknown option '" + arg + "'");
                        }
                    }
                    else
                    {
                        throw std::runtime_error("unknown option '" + arg + "'");
                    }
                }
                else
                {
                    throw std::runtime_error("unknown option '" + arg + "'");
                }
            }
            else
            {
                projectsAndSolutions.push_back(arg);
            }
        }
#ifndef _WIN32
        noDebugInfo = true;
#endif
        if (!GetGlobalFlag(GlobalFlags::release) && !noDebugInfo)
        {
            SetGlobalFlag(GlobalFlags::generateDebugInfo);
        }
        if (GetGlobalFlag(GlobalFlags::release))
        {
            cmunitElement->SetAttribute(U"config", U"release");
        }
        else
        {
            cmunitElement->SetAttribute(U"config", U"debug");
        }
        if (projectsAndSolutions.empty())
        {
            PrintHelp();
            throw std::runtime_error("no projects and/or solutions given");
        }
        else
        {
            if (GetGlobalFlag(GlobalFlags::verbose))
            {
                std::cout << "Cmajor unit test engine version " << version << std::endl;
            }
            if (onlySourceFile.empty())
            {
                cmunitElement->SetAttribute(U"file", U"*");
            }
            else
            {
                cmunitElement->SetAttribute(U"file", ToUtf32(onlySourceFile));
            }
            if (onlyTest.empty())
            {
                cmunitElement->SetAttribute(U"test", U"*");
            }
            else
            {
                cmunitElement->SetAttribute(U"test", ToUtf32(onlyTest));
            }
            int n = projectsAndSolutions.size();
            for (int i = 0; i < n; ++i)
            {
                const std::string& projectOrSolution = projectsAndSolutions[i];
                boost::filesystem::path fp(projectOrSolution);
                if (fp.extension() == ".cms")
                {
                    if (!boost::filesystem::exists(fp))
                    {
                        throw std::runtime_error("solution file '" + fp.generic_string() + " not found");
                    }
                    else
                    {
                        std::string solutionFileName = GetFullPath(fp.generic_string());
                        std::unique_ptr<cmajor::dom::Element> componentElement(new cmajor::dom::Element(U"component"));
                        componentElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(ToUtf32(Path::GetFileName(solutionFileName)))));
                        componentsElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(componentElement.release()));
                        bool solutionContainsUnitTestProject = TestSolution(solutionFileName, onlySourceFile, onlyTest, cmunitElement.get(), rootModule);
                        if (solutionContainsUnitTestProject)
                        {
                            unitTestProjectsFound = true;
                        }
                    }
                }
                else if (fp.extension() == ".cmp")
                {
                    if (!boost::filesystem::exists(fp))
                    {
                        throw std::runtime_error("project file '" + fp.generic_string() + " not found");
                    }
                    else
                    {
                        std::string projectFileName = GetFullPath(fp.generic_string());
                        std::unique_ptr<cmajor::dom::Element> componentElement(new cmajor::dom::Element(U"component"));
                        componentElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(ToUtf32(Path::GetFileName(projectFileName)))));
                        componentsElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(componentElement.release()));
                        bool projectIsUnitTestProject = TestProject(projectFileName, onlySourceFile, onlyTest, cmunitElement.get(), rootModule);
                        if (projectIsUnitTestProject)
                        {
                            unitTestProjectsFound = true;
                        }
                    }
                }
                else
                {
                    throw std::runtime_error("Argument '" + fp.generic_string() + "' has invalid extension. Not Cmajor solution (.cms) or project (.cmp) file.");
                }
            }
            if (!unitTestProjectsFound)
            {
                throw std::runtime_error("given solutions/projects contain no unit test projects");
            }
            if (!unitTestsFound)
            {
                throw std::runtime_error("no unit tests found in given solutions/projects/files");
            }
        }
        cmunitElement->SetAttribute(U"end", ToUtf32(GetCurrentDateTime().ToString()));
        cmunitElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(componentsElement.release()));
        std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
        int durationSecs = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
        int hours = durationSecs / 3600;
        int mins = (durationSecs / 60) % 60;
        int secs = durationSecs % 60;
        std::string durationStr;
        durationStr.append(1, '0' + (hours / 10));
        durationStr.append(1, '0' + (hours % 10));
        durationStr.append(1, ':');
        durationStr.append(1, '0' + (mins / 10));
        durationStr.append(1, '0' + (mins % 10));
        durationStr.append(1, ':');
        durationStr.append(1, '0' + (secs / 10));
        durationStr.append(1, '0' + (secs % 10));
        cmunitElement->SetAttribute(U"duration", ToUtf32(durationStr));
        cmajor::dom::Document testDoc;
        testDoc.AppendChild(std::unique_ptr<cmajor::dom::Node>(cmunitElement.release()));
        if (outFile.empty())
        {
            outFile = "cmunit";
            outFile.append("-").append(GetCurrentDateTime().ToString(true, true, false, false));
        }
        std::string cmunitFileName = GetFullPath(boost::filesystem::path(outFile).replace_extension(".xml").generic_string());
        std::ofstream cmunitXmlFile(cmunitFileName);
        cmajor::util::CodeFormatter formatter(cmunitXmlFile);
        formatter.SetIndentSize(2);
        testDoc.Write(formatter);
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            std::cout << "==> " << cmunitFileName << std::endl;
        }
        std::unique_ptr<cmajor::dom::Document> reportDoc = GenerateHtmlReport(&testDoc);
        std::string cmunitReportFileName = Path::ChangeExtension(cmunitFileName, ".html");
        std::ofstream cmunitHtmlFile(cmunitReportFileName);
        cmajor::util::CodeFormatter htmlFormatter(cmunitHtmlFile);
        htmlFormatter.SetIndentSize(2);
        reportDoc->Write(htmlFormatter);
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            std::cout << "==> " << cmunitReportFileName << std::endl;
        }
    }
    catch (const ParsingException& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    catch (const Exception& ex)
    {
        std::cerr << ex.What() << std::endl;
        return 1;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
