#include <sngcm/ast/InitDone.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <sngcm/ast/BasicType.hpp>
#include <sngcm/ast/Expression.hpp>
#include <sngcm/ast/TypeExpr.hpp>
#include <sngcm/ast/Literal.hpp>
#include <soulng/util/InitDone.hpp>
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
#include <sngcm/cmlexer/CmajorLexer.hpp>
#include <sngcm/cmlexer/ContainerFileLexer.hpp>
#include <sngcm/cmparser/ProjectFile.hpp>
#include <sngcm/cmparser/SolutionFile.hpp>
#include <sngcm/cmparser/CompileUnit.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/CharacterData.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/lexer/ParsingException.hpp>
#include <soulng/util/Util.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Json.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Time.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <chrono>

struct InitDone
{
    InitDone()
    {
        soulng::util::Init();
        sngcm::ast::Init();
        cmajor::symbols::Init();
        sngxml::xpath::Init();
    }
    ~InitDone()
    {
        sngxml::xpath::Done();
        cmajor::symbols::Done();
        sngcm::ast::Done();
        soulng::util::Done();
    }
};

using namespace sngcm::ast;
using namespace soulng::util;
using namespace soulng::unicode;
using namespace cmajor::symbols;
using namespace cmajor::build;
using namespace cmajor::binder;
using namespace sngxml::dom;

const char* version = "3.5.0";

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
    int32_t numAssertions, const std::string& unitTestFilePath, std::string& mainObjectFilePath)
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
        new LongLiteralNode(Span(), 4))); // 4 64-bit integers per entry
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
    mainObjectFilePath = boundMainCompileUnit.ObjectFilePath();
}

struct UnitTest
{
    UnitTest() : prevUnitTest(BeginUnitTest()) {}
    ~UnitTest() { EndUnitTest(prevUnitTest); }
    bool prevUnitTest;
};

int RunUnitTest(sngcm::ast::Project* project)
{
    unitTestsFound = true;
    int exitCode = system(project->ExecutableFilePath().c_str());
    return exitCode;
}

void TestUnit(FileTable* fileTable, sngcm::ast::Project* project, CompileUnitNode* testUnit, const std::string& testName, Element* sourceFileElement, std::unique_ptr<Module>& rootModule)
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
        rootModule.reset(new Module(project->Name(), project->ModuleFilePath(), project->GetTarget()));
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
        rootModule->PrepareForCompilation(project->References(), project->GetTarget());
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
        std::string mainObjectFilePath;
        int32_t numUnitTestAssertions = GetNumUnitTestAssertions();
        CreateMainUnit(objectFilePaths, *rootModule, emittingContext, &attributeBinder, testName, numUnitTestAssertions, unitTestFilePath, mainObjectFilePath);
        GenerateLibrary(rootModule.get(), objectFilePaths, project->LibraryFilePath());
        Link(Target::program, project->ExecutableFilePath(), project->LibraryFilePath(), rootModule->LibraryFilePaths(), mainObjectFilePath, *rootModule);
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
            std::unique_ptr<sngxml::dom::Element> testElement(new sngxml::dom::Element(U"test"));
            testElement->SetAttribute(U"name", ToUtf32(testName));
            testElement->SetAttribute(U"exitCode", ToUtf32(std::to_string(exitCode)));
            for (int32_t i = 0; i < numUnitTestAssertions; ++i)
            {
                std::unique_ptr<sngxml::dom::Element> assertionElement(new sngxml::dom::Element(U"assertion"));
                assertionElement->SetAttribute(U"index", ToUtf32(std::to_string(i)));
                std::u32string assertionResultStr = U"empty";
                assertionElement->SetAttribute(U"result", assertionResultStr);
                if (i < assertionLineNumbers.size())
                {
                    assertionElement->SetAttribute(U"line", ToUtf32(std::to_string(assertionLineNumbers[i])));
                }
                testElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(assertionElement.release()));
            }
            sourceFileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(testElement.release()));
        }
    }
    catch (const soulng::lexer::ParsingException& ex)
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
        std::unique_ptr<sngxml::dom::Element> testElement(new sngxml::dom::Element(U"test"));
        testElement->SetAttribute(U"name", ToUtf32(testName));
        testElement->SetAttribute(U"compileError", ToUtf32(compileErrorMessage));
        sourceFileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(testElement.release()));
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

void TestSourceFile(bool& first, sngcm::ast::Project* project, const std::string& sourceFilePath, const std::string& onlyTest, sngxml::dom::Element* projectElement, 
    std::unique_ptr<Module>& rootModule)
{
    std::unique_ptr<sngxml::dom::Element> sourceFileElement(new sngxml::dom::Element(U"sourceFile"));
    sourceFileElement->SetAttribute(U"name", ToUtf32(Path::GetFileNameWithoutExtension(sourceFilePath)));
    MappedInputFile sourceFile(sourceFilePath);
    FileTable fileTable;
    uint32_t fileIndex = fileTable.RegisterFilePath(sourceFilePath);
    ParsingContext parsingContext;
    std::u32string s(ToUtf32(std::string(sourceFile.Begin(), sourceFile.End())));
    CmajorLexer lexer(s, sourceFilePath, fileIndex);
    std::unique_ptr<CompileUnitNode> compileUnit = CompileUnitParser::Parse(lexer, &parsingContext);
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
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(sourceFileElement.release()));
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

void TestProject(sngcm::ast::Project* project, const std::string& onlySourceFile, const std::string& onlyTest, sngxml::dom::Element* parentElement, std::unique_ptr<Module>& rootModule)
{
    if (project->GetTarget() != Target::unitTest)
    {
        throw std::runtime_error("project '" + ToUtf8(project->Name()) + "' is not a unit testing project");
    }
    bool first = true;
    std::unique_ptr<sngxml::dom::Element> projectElement(new sngxml::dom::Element(U"project"));
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
    parentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(projectElement.release()));
}

bool TestProject(const std::string& projectFileName, const std::string& onlySourceFile, const std::string& onlyTest, sngxml::dom::Element* parentElement, 
    std::unique_ptr<Module>& rootModule)
{
    std::string config = GetConfig();
    MappedInputFile projectFile(projectFileName);
    std::u32string p(ToUtf32(std::string(projectFile.Begin(), projectFile.End())));
    ContainerFileLexer containerFileLexer(p, projectFileName, 0);
    std::unique_ptr<sngcm::ast::Project> project = ProjectFileParser::Parse(containerFileLexer, config, sngcm::ast::BackEnd::llvm);
    project->ResolveDeclarations();
    if (project->GetTarget() != Target::unitTest)
    {
        return false;
    }
    TestProject(project.get(), onlySourceFile, onlyTest, parentElement, rootModule);
    return true;
}

bool TestSolution(const std::string& solutionFileName, const std::string& onlySourceFile, const std::string& onlyTest, sngxml::dom::Element* cmunitElement,
    std::unique_ptr<Module>& rootModule)
{
    std::unique_ptr<sngxml::dom::Element> solutionElement(new sngxml::dom::Element(U"solution"));
    MappedInputFile solutionFile(solutionFileName);
    std::u32string s(ToUtf32(std::string(solutionFile.Begin(), solutionFile.End())));
    ContainerFileLexer containerFileLexer(s, solutionFileName, 0);
    std::unique_ptr<sngcm::ast::Solution> solution = SolutionFileParser::Parse(containerFileLexer);
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
        cmunitElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(solutionElement.release()));
    }
    return containsUnitTestProject;
}

std::unique_ptr<sngxml::dom::Document> GenerateHtmlReport(sngxml::dom::Document* testDoc)
{
    std::unique_ptr<sngxml::dom::Document> reportDoc(new sngxml::dom::Document());
    std::unique_ptr<sngxml::dom::Element> htmlElement(new sngxml::dom::Element(U"html"));
    std::unique_ptr<sngxml::dom::Element> headElement(new sngxml::dom::Element(U"head"));
    std::unique_ptr<sngxml::dom::Element> titleElement(new sngxml::dom::Element(U"title"));
    titleElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"Unit Test Report")));
    std::unique_ptr<sngxml::dom::Element> styleElement(new sngxml::dom::Element(U"style"));
    std::u32string style = 
        U"body { max-width: 800px; } h1, h2, h3, h4, h5, h6 { color: #005ab4; font-family: sans-serif; } table { boder-collapse: collapse; } table, th, td { text-align: left; border: 1px solid #dddddd; padding: 8px; }";
    styleElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(style)));
    std::unique_ptr<sngxml::dom::Element> bodyElement(new sngxml::dom::Element(U"body"));
    std::unique_ptr<sngxml::dom::Element> h1Element(new sngxml::dom::Element(U"h1"));
    h1Element->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"Unit Test Report")));
    bodyElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(h1Element.release()));
    sngxml::dom::Element* cmunitElement = testDoc->DocumentElement();

    std::unique_ptr<sngxml::dom::Element> paramTableElement(new sngxml::dom::Element(U"table"));

    std::u32string configuration = cmunitElement->GetAttribute(U"config");
    std::unique_ptr<sngxml::dom::Element> trConfigElement(new sngxml::dom::Element(U"tr"));
    std::unique_ptr<sngxml::dom::Element> thConfigElement(new sngxml::dom::Element(U"th"));
    thConfigElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"configuration")));
    trConfigElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(thConfigElement.release()));
    std::unique_ptr<sngxml::dom::Element> tdConfigElement(new sngxml::dom::Element(U"td"));
    tdConfigElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(configuration)));
    trConfigElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(tdConfigElement.release()));
    paramTableElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(trConfigElement.release()));

    std::u32string start = cmunitElement->GetAttribute(U"start");
    std::unique_ptr<sngxml::dom::Element> trStartElement(new sngxml::dom::Element(U"tr"));
    std::unique_ptr<sngxml::dom::Element> thStartElement(new sngxml::dom::Element(U"th"));
    thStartElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"start")));
    trStartElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(thStartElement.release()));
    std::unique_ptr<sngxml::dom::Element> tdStartElement(new sngxml::dom::Element(U"td"));
    tdStartElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(start)));
    trStartElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(tdStartElement.release()));
    paramTableElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(trStartElement.release()));

    std::u32string end = cmunitElement->GetAttribute(U"end");
    std::unique_ptr<sngxml::dom::Element> trEndElement(new sngxml::dom::Element(U"tr"));
    std::unique_ptr<sngxml::dom::Element> thEndElement(new sngxml::dom::Element(U"th"));
    thEndElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"end")));
    trEndElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(thEndElement.release()));
    std::unique_ptr<sngxml::dom::Element> tdEndElement(new sngxml::dom::Element(U"td"));
    tdEndElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(end)));
    trEndElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(tdEndElement.release()));
    paramTableElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(trEndElement.release()));

    std::u32string duration = cmunitElement->GetAttribute(U"duration");
    std::unique_ptr<sngxml::dom::Element> trDurationElement(new sngxml::dom::Element(U"tr"));
    std::unique_ptr<sngxml::dom::Element> thDurationElement(new sngxml::dom::Element(U"th"));
    thDurationElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"duration")));
    trDurationElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(thDurationElement.release()));
    std::unique_ptr<sngxml::dom::Element> tdDurationElement(new sngxml::dom::Element(U"td"));
    tdDurationElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(duration)));
    trDurationElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(tdDurationElement.release()));
    paramTableElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(trDurationElement.release()));

    std::u32string file = cmunitElement->GetAttribute(U"file");
    std::unique_ptr<sngxml::dom::Element> trFileElement(new sngxml::dom::Element(U"tr"));
    std::unique_ptr<sngxml::dom::Element> thFileElement(new sngxml::dom::Element(U"th"));
    thFileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"file")));
    trFileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(thFileElement.release()));
    std::unique_ptr<sngxml::dom::Element> tdFileElement(new sngxml::dom::Element(U"td"));
    tdFileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(file)));
    trFileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(tdFileElement.release()));
    paramTableElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(trFileElement.release()));

    std::u32string test = cmunitElement->GetAttribute(U"test");
    std::unique_ptr<sngxml::dom::Element> trTestElement(new sngxml::dom::Element(U"tr"));
    std::unique_ptr<sngxml::dom::Element> thTestElement(new sngxml::dom::Element(U"th"));
    thTestElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"test")));
    trTestElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(thTestElement.release()));
    std::unique_ptr<sngxml::dom::Element> tdTestElement(new sngxml::dom::Element(U"td"));
    tdTestElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(test)));
    trTestElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(tdTestElement.release()));
    paramTableElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(trTestElement.release()));

    bodyElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(paramTableElement.release()));

    std::unique_ptr<sngxml::dom::Element> h2ComponentsElement(new sngxml::dom::Element(U"h2"));
    h2ComponentsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"Components")));
    bodyElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(h2ComponentsElement.release()));

    std::unique_ptr<sngxml::dom::Element> ulElement(new sngxml::dom::Element(U"ul"));
    std::unique_ptr<sngxml::xpath::XPathObject> components = sngxml::xpath::Evaluate(U"cmunit/components/component/text()", testDoc);
    if (components && components->Type() == sngxml::xpath::XPathObjectType::nodeSet)
    {
        sngxml::xpath::XPathNodeSet* componentsSet = static_cast<sngxml::xpath::XPathNodeSet*>(components.get());
        int n = componentsSet->Length();
        for (int i = 0; i < n; ++i)
        {
            sngxml::dom::Node* componentNode = (*componentsSet)[i];
            if (componentNode->GetNodeType() == sngxml::dom::NodeType::textNode)
            {
                sngxml::dom::Text* component = static_cast<sngxml::dom::Text*>(componentNode);
                std::unique_ptr<sngxml::dom::Element> liElement(new sngxml::dom::Element(U"li"));
                liElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(component->Data())));
                ulElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(liElement.release()));
            }
        }
    }
    bodyElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(ulElement.release()));

    std::unique_ptr<sngxml::dom::Element> h2Element(new sngxml::dom::Element(U"h2"));
    h2Element->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"Results")));
    bodyElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(h2Element.release()));

    std::unique_ptr<sngxml::dom::Element> tableElement(new sngxml::dom::Element(U"table"));

    std::unique_ptr<sngxml::dom::Element> trHeaderElement(new sngxml::dom::Element(U"tr"));
    std::unique_ptr<sngxml::dom::Element> thNameElement(new sngxml::dom::Element(U"th"));
    thNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"name")));
    trHeaderElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(thNameElement.release()));

    std::unique_ptr<sngxml::dom::Element> thCountElement(new sngxml::dom::Element(U"th"));
    thCountElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"count")));
    trHeaderElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(thCountElement.release()));

    std::unique_ptr<sngxml::dom::Element> thPassedElement(new sngxml::dom::Element(U"th"));
    thPassedElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"passed")));
    trHeaderElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(thPassedElement.release()));

    std::unique_ptr<sngxml::dom::Element> thFailedElement(new sngxml::dom::Element(U"th"));
    thFailedElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"failed")));
    trHeaderElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(thFailedElement.release()));

    std::unique_ptr<sngxml::dom::Element> thEmptyElement(new sngxml::dom::Element(U"th"));
    thEmptyElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"empty")));
    trHeaderElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(thEmptyElement.release()));

    std::unique_ptr<sngxml::dom::Element> thExitCodeElement(new sngxml::dom::Element(U"th"));
    thExitCodeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"exit code")));
    trHeaderElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(thExitCodeElement.release()));

    std::unique_ptr<sngxml::dom::Element> thCompileErrorElement(new sngxml::dom::Element(U"th"));
    thCompileErrorElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"compile error")));
    trHeaderElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(thCompileErrorElement.release()));

    std::unique_ptr<sngxml::dom::Element> thExceptionElement(new sngxml::dom::Element(U"th"));
    thExceptionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"exception")));
    trHeaderElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(thExceptionElement.release()));

    std::vector<std::pair<std::u32string, sngxml::dom::Element*>> failedAssertions;

    tableElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(trHeaderElement.release()));

    std::unique_ptr<sngxml::xpath::XPathObject> tests = sngxml::xpath::Evaluate(U"//test", testDoc);
    if (tests && tests->Type() == sngxml::xpath::XPathObjectType::nodeSet)
    {
        sngxml::xpath::XPathNodeSet* testElements = static_cast<sngxml::xpath::XPathNodeSet*>(tests.get());
        int n = testElements->Length();
        for (int i = 0; i < n; ++i)
        {
            sngxml::dom::Node* testNode = (*testElements)[i];
            if (testNode->GetNodeType() == sngxml::dom::NodeType::elementNode)
            {
                sngxml::dom::Element* testElement = static_cast<sngxml::dom::Element*>(testNode);
                std::u32string name = testElement->GetAttribute(U"name");
                std::u32string fullTestName = name;
                sngxml::dom::ParentNode* sourceFileNode = testElement->Parent();
                if (sourceFileNode)
                {
                    if (sourceFileNode->GetNodeType() == sngxml::dom::NodeType::elementNode)
                    {
                        sngxml::dom::Element* sourceFileElement = static_cast<sngxml::dom::Element*>(sourceFileNode);
                        name = sourceFileElement->GetAttribute(U"name");
                        fullTestName = name + U"." + fullTestName;
                        sngxml::dom::ParentNode* projectNode = sourceFileElement->Parent();
                        if (projectNode)
                        {
                            if (projectNode->GetNodeType() == sngxml::dom::NodeType::elementNode)
                            {
                                sngxml::dom::Element* projectElement = static_cast<sngxml::dom::Element*>(projectNode);
                                name = projectElement->GetAttribute(U"name");
                                fullTestName = name + U"." + fullTestName;
                                sngxml::dom::ParentNode* parentNode = projectElement->Parent();
                                if (parentNode)
                                {
                                    if (parentNode->GetNodeType() == sngxml::dom::NodeType::elementNode)
                                    {
                                        sngxml::dom::Element* parentElement = static_cast<sngxml::dom::Element*>(parentNode);
                                        if (parentElement->Name() == U"solution")
                                        {
                                            sngxml::dom::Element* solutionElement = parentElement;
                                            name = solutionElement->GetAttribute(U"name");
                                            fullTestName = name + U"." + fullTestName;
                                        }

                                        std::unique_ptr<sngxml::dom::Element> trElement(new sngxml::dom::Element(U"tr"));
                                        std::unique_ptr<sngxml::dom::Element> tdNameElement(new sngxml::dom::Element(U"td"));
                                        sngxml::dom::Element* nameElement = tdNameElement.get();
                                        tdNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(fullTestName)));
                                        trElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(tdNameElement.release()));
                                        std::unique_ptr<sngxml::dom::Element> tdCountElement(new sngxml::dom::Element(U"td"));
                                        tdCountElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(testElement->GetAttribute(U"count"))));
                                        trElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(tdCountElement.release()));
                                        std::unique_ptr<sngxml::xpath::XPathObject> assertions = sngxml::xpath::Evaluate(U"assertion", testElement);
                                        int passed = 0;
                                        int failed = 0;
                                        int empty = 0;
                                        int count = 0;
                                        if (assertions && assertions->Type() == sngxml::xpath::XPathObjectType::nodeSet)
                                        {
                                            sngxml::xpath::XPathNodeSet* assertionElements = static_cast<sngxml::xpath::XPathNodeSet*>(assertions.get());
                                            count = assertionElements->Length();
                                            for (int i = 0; i < count; ++i)
                                            {
                                                sngxml::dom::Node* assertionNode = (*assertionElements)[i];
                                                if (assertionNode->GetNodeType() == sngxml::dom::NodeType::elementNode)
                                                {
                                                    sngxml::dom::Element* assertionElement = static_cast<sngxml::dom::Element*>(assertionNode);
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
                                        std::unique_ptr<sngxml::dom::Element> tdPassedElement(new sngxml::dom::Element(U"td"));
                                        tdPassedElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(ToUtf32(std::to_string(passed)))));
                                        trElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(tdPassedElement.release()));

                                        std::unique_ptr<sngxml::dom::Element> tdFailedElement(new sngxml::dom::Element(U"td"));
                                        tdFailedElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(ToUtf32(std::to_string(failed)))));
                                        trElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(tdFailedElement.release()));

                                        if (count == 0)
                                        {
                                            std::unique_ptr<sngxml::dom::Element> tdEmptyElement(new sngxml::dom::Element(U"td"));
                                            tdEmptyElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(testElement->GetAttribute(U"count"))));
                                            trElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(tdEmptyElement.release()));
                                        }
                                        else
                                        {
                                            std::unique_ptr<sngxml::dom::Element> tdEmptyElement(new sngxml::dom::Element(U"td"));
                                            tdEmptyElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(ToUtf32(std::to_string(empty)))));
                                            trElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(tdEmptyElement.release()));
                                        }

                                        std::unique_ptr<sngxml::dom::Element> tdExitCodeElement(new sngxml::dom::Element(U"td"));
                                        tdExitCodeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(testElement->GetAttribute(U"exitCode"))));
                                        trElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(tdExitCodeElement.release()));
                                        std::unique_ptr<sngxml::dom::Element> tdCompileErrorElement(new sngxml::dom::Element(U"td")); 
                                        tdCompileErrorElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(testElement->GetAttribute(U"compileError"))));
                                        trElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(tdCompileErrorElement.release()));
                                        std::unique_ptr<sngxml::dom::Element> tdExceptionElement(new sngxml::dom::Element(U"td")); 
                                        tdExceptionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(testElement->GetAttribute(U"exception"))));
                                        trElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(tdExceptionElement.release()));
                                        tableElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(trElement.release()));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    bodyElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(tableElement.release()));

    if (!failedAssertions.empty())
    {
        std::unique_ptr<sngxml::dom::Element> h2Element(new sngxml::dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"Failed Assertions")));
        bodyElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(h2Element.release()));

        std::unique_ptr<sngxml::dom::Element> failedAssertionsTable(new sngxml::dom::Element(U"table"));

        std::unique_ptr<sngxml::dom::Element> trHeaderElement(new sngxml::dom::Element(U"tr"));

        std::unique_ptr<sngxml::dom::Element> thNameElement(new sngxml::dom::Element(U"th"));
        thNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"name")));
        trHeaderElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(thNameElement.release()));

        std::unique_ptr<sngxml::dom::Element> thIndexElement(new sngxml::dom::Element(U"th"));
        thIndexElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"index")));
        trHeaderElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(thIndexElement.release()));

        std::unique_ptr<sngxml::dom::Element> thLineElement(new sngxml::dom::Element(U"th"));
        thLineElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"line")));
        trHeaderElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(thLineElement.release()));

        failedAssertionsTable->AppendChild(std::unique_ptr<sngxml::dom::Node>(trHeaderElement.release()));

        int n = failedAssertions.size();
        for (int i = 0; i < n; ++i)
        {
            const std::pair<std::u32string, sngxml::dom::Element*>& p = failedAssertions[i];
            std::unique_ptr<sngxml::dom::Element> trAssertionElement(new sngxml::dom::Element(U"tr"));
            sngxml::dom::Element* assertionElement = p.second;

            std::unique_ptr<sngxml::dom::Element> tdNameElement(new sngxml::dom::Element(U"td"));
            std::u32string attr = U"background-color: #ff3300; font-family: sans-serif; font-size: 13px; font-weight: bold;";
            tdNameElement->SetAttribute(U"style", attr);
            tdNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(p.first)));
            trAssertionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(tdNameElement.release()));

            std::unique_ptr<sngxml::dom::Element> tdIndexElement(new sngxml::dom::Element(U"td"));
            tdIndexElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(assertionElement->GetAttribute(U"index"))));
            trAssertionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(tdIndexElement.release()));

            std::unique_ptr<sngxml::dom::Element> tdLineElement(new sngxml::dom::Element(U"td"));
            tdLineElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(assertionElement->GetAttribute(U"line"))));
            trAssertionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(tdLineElement.release()));

            failedAssertionsTable->AppendChild(std::unique_ptr<sngxml::dom::Node>(trAssertionElement.release()));
        }

        bodyElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(failedAssertionsTable.release()));
    }

    headElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(titleElement.release()));
    headElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(styleElement.release()));
    htmlElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(headElement.release()));
    htmlElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(bodyElement.release()));
    reportDoc->AppendChild(std::unique_ptr<sngxml::dom::Node>(htmlElement.release()));
    return reportDoc;
}

int main(int argc, const char** argv)
{
    std::unique_ptr<Module> rootModule;
    try
    {
        InitDone initDone;
        SetCompilerVersion(version);
        std::unique_ptr<sngxml::dom::Element> cmunitElement(new sngxml::dom::Element(U"cmunit"));
        cmunitElement->SetAttribute(U"start", ToUtf32(GetCurrentDateTime().ToString()));
        std::unique_ptr<sngxml::dom::Element> componentsElement(new sngxml::dom::Element(U"components"));
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
                        std::unique_ptr<sngxml::dom::Element> componentElement(new sngxml::dom::Element(U"component"));
                        componentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(ToUtf32(Path::GetFileName(solutionFileName)))));
                        componentsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(componentElement.release()));
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
                        std::unique_ptr<sngxml::dom::Element> componentElement(new sngxml::dom::Element(U"component"));
                        componentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(ToUtf32(Path::GetFileName(projectFileName)))));
                        componentsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(componentElement.release()));
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
        cmunitElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(componentsElement.release()));
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
        sngxml::dom::Document testDoc;
        testDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(cmunitElement.release()));
        if (outFile.empty())
        {
            outFile = "cmunit";
            outFile.append("-").append(GetCurrentDateTime().ToString(true, true, false, false));
        }
        std::string cmunitFileName = GetFullPath(boost::filesystem::path(outFile).replace_extension(".xml").generic_string());
        std::ofstream cmunitXmlFile(cmunitFileName);
        soulng::util::CodeFormatter formatter(cmunitXmlFile);
        formatter.SetIndentSize(2);
        testDoc.Write(formatter);
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            std::cout << "==> " << cmunitFileName << std::endl;
        }
        std::unique_ptr<sngxml::dom::Document> reportDoc = GenerateHtmlReport(&testDoc);
        std::string cmunitReportFileName = Path::ChangeExtension(cmunitFileName, ".html");
        std::ofstream cmunitHtmlFile(cmunitReportFileName);
        soulng::util::CodeFormatter htmlFormatter(cmunitHtmlFile);
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
