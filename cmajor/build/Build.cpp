// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/Build.hpp>
#include <cmajor/build/Action.hpp>
#include <cmajor/build/MessageBody.hpp>
#include <cmajor/cmtoolchain/ToolChains.hpp>
#include <cmajor/codegen/EmittingContext.hpp>
#include <cmajor/codegen/Interface.hpp>
#include <cmajor/cmtoolchain/ToolChains.hpp>
#include <soulng/lexer/XmlParsingLog.hpp>
#include <sngcm/cmlexer/ContainerFileLexer.hpp>
#include <sngcm/cmlexer/CmajorLexer.hpp>
#include <sngcm/cmparser/ProjectFile.hpp>
#include <sngcm/cmparser/SolutionFile.hpp>
#include <sngcm/cmparser/CompileUnit.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/lexer/ParsingException.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/binder/BoundFunction.hpp>
#include <cmajor/binder/TypeBinder.hpp>
#include <cmajor/binder/OverloadResolution.hpp>
#include <cmajor/binder/StatementBinder.hpp>
#include <cmajor/binder/BoundStatement.hpp>
#include <cmajor/binder/ControlFlowAnalyzer.hpp>
#include <cmajor/binder/AttributeBinder.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/Warning.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/ModuleCache.hpp>
#include <cmajor/symbols/SymbolWriter.hpp>
#include <cmajor/symbols/SymbolReader.hpp>
#include <cmajor/symbols/SymbolCreatorVisitor.hpp>
#include <cmajor/symbols/Meta.hpp>
#include <cmajor/symbols/Error.hpp>
#include <cmajor/ast2dom/Ast2Dom.hpp>
#include <cmajor/bdt2dom/Bdt2Dom.hpp>
#include <cmajor/cmdoclib/Input.hpp>
#include <cmajor/cmdoclib/Global.hpp>
#include <cmajor/cmdoclib/ParserDoc.hpp>
#include <cmajor/cmdoclib/SourceCodePrinter.hpp>
#include <cmajor/cmdoclib/SymbolTableXml.hpp>
#include <cmajor/cmdoclib/File.hpp>
#ifdef _WIN32
#include <cmajor/cmres/ResourceProcessor.hpp>
#endif
#include <system-x/object/Archive.hpp>
#include <system-x/object/Link.hpp>
#include <system-x/object/Resource.hpp>
#include <system-x/intermediate/CppProjectFileGenerator.hpp>
#include <system-x/intermediate/CppCodeGenerator.hpp>
#include <sngjson/json/JsonLexer.hpp>
#include <sngjson/json/JsonParser.hpp>
#include <sngcm/ast/Attribute.hpp>
#include <sngcm/ast/Function.hpp>
#include <sngcm/ast/BasicType.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <sngcm/ast/TypeExpr.hpp>
#include <sngcm/ast/Expression.hpp>
#include <sngcm/ast/Literal.hpp>
#include <sngcm/ast/SystemFileIndex.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/System.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Log.hpp>
#include <soulng/util/Time.hpp>
#include <soulng/util/Sha1.hpp>
#include <soulng/util/Process.hpp>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <mutex>
#include <chrono>
#include <thread>
#include <list>
#include <condition_variable>

using namespace sngcm::ast;
using namespace cmajor::symbols;
using namespace cmajor::binder;
using namespace soulng::util;
using namespace soulng::unicode;

namespace cmajor { namespace build {

LogFileWriter* buildLogWriter = nullptr;

void SetBuildLogWriter(LogFileWriter* buildLogWriter_)
{
    buildLogWriter = buildLogWriter_;
}

bool stopBuild = false;

void StopBuild()
{
    stopBuild = true;
}

void ResetStopBuild()
{
    stopBuild = false;
}

Solution* currentSolution = nullptr;

std::vector<std::unique_ptr<CompileUnitNode>> ParseSourcesInMainThread(Module* module, const std::vector<std::string>& sourceFilePaths, bool& stop)
{
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        std::string s;
        if (sourceFilePaths.size() != 1)
        {
            s = "s";
        }
        LogMessage(module->LogStreamId(), "Parsing " + std::to_string(sourceFilePaths.size()) + " source file" + s + " in main thread...");
    }
    std::vector<std::unique_ptr<CmajorLexer>> lexers;
    std::vector<std::unique_ptr<CompileUnitNode>> compileUnits;
    try
    {
        for (const std::string& sourceFilePath : sourceFilePaths)
        {
            if (stop)
            {
                return std::vector<std::unique_ptr<CompileUnitNode>>();
            }
            if (boost::filesystem::file_size(sourceFilePath) == 0)
            {
                std::unique_ptr<CompileUnitNode> compileUnit(new CompileUnitNode(Span(), boost::uuids::nil_uuid(), sourceFilePath));
                compileUnit->SetHash(GetSha1MessageDigest(""));
                int32_t fileIndex = module->GetFileTable().RegisterFilePath(sourceFilePath);
                compileUnits.push_back(std::move(compileUnit));
                lexers.push_back(std::unique_ptr<CmajorLexer>());
            }
            else
            {
                MappedInputFile sourceFile(sourceFilePath);
                int32_t fileIndex = module->GetFileTable().RegisterFilePath(sourceFilePath);
                ParsingContext parsingContext;
                std::string fileContent(sourceFile.Begin(), sourceFile.End());
                std::u32string s(ToUtf32(fileContent));
                std::unique_ptr<CmajorLexer> lexer(new CmajorLexer(s, sourceFilePath, fileIndex));
                try
                {
                    soulng::lexer::XmlParsingLog log(std::cout);
                    if (GetGlobalFlag(GlobalFlags::debugParsing))
                    {
                        lexer->SetLog(&log);
                    }
                    boost::uuids::uuid moduleId = module->Id();
                    std::unique_ptr<CompileUnitNode> compileUnit = CompileUnitParser::Parse(*lexer, &moduleId, &parsingContext);
                    compileUnit->SetHash(GetSha1MessageDigest(fileContent));
                    if (GetGlobalFlag(GlobalFlags::ast2xml))
                    {
                        std::unique_ptr<sngxml::dom::Document> ast2xmlDoc = cmajor::ast2dom::GenerateAstDocument(compileUnit.get());
                        std::string ast2xmlFilePath = Path::ChangeExtension(sourceFilePath, ".ast.xml");
                        std::ofstream ast2xmlFile(ast2xmlFilePath);
                        CodeFormatter formatter(ast2xmlFile);
                        formatter.SetIndentSize(1);
                        ast2xmlDoc->Write(formatter);
                    }
                    if (GetGlobalFlag(GlobalFlags::generateDebugInfo) || GetGlobalFlag(GlobalFlags::cmdoc))
                    {
                        compileUnit->ComputeLineStarts(s);
                    }
                    compileUnits.push_back(std::move(compileUnit));
                }
                catch (...)
                {
                    lexers.push_back(std::move(lexer));
                    throw;
                }
                lexers.push_back(std::move(lexer));
            }
        }
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            std::string s;
            if (sourceFilePaths.size() != 1)
            {
                s = "s";
            }
            LogMessage(module->LogStreamId(), "Source file" + s + " parsed.");
        }
    }
    catch (...)
    {
        module->SetLexers(std::move(lexers));
        throw;
    }
    module->SetLexers(std::move(lexers));
    return compileUnits;
}

struct ParserData
{
    ParserData(const boost::uuids::uuid& moduleId_, const std::vector<std::string>& sourceFilePaths_, std::vector<std::unique_ptr<CompileUnitNode>>& compileUnits_, 
        std::vector<std::unique_ptr<CmajorLexer>>& lexers_, const std::vector<uint32_t>& fileIndeces_, std::vector<std::exception_ptr>& exceptions_, bool& stop_) :
        moduleId(moduleId_), sourceFilePaths(sourceFilePaths_), compileUnits(compileUnits_), lexers(lexers_), fileIndeces(fileIndeces_), stop(stop_), exceptions(exceptions_)
    {
    }
    boost::uuids::uuid moduleId;
    const std::vector<std::string>& sourceFilePaths;
    std::vector<std::unique_ptr<CompileUnitNode>>& compileUnits;
    std::vector<std::unique_ptr<CmajorLexer>>& lexers;
    const std::vector<uint32_t>& fileIndeces;
    std::list<int> indexQueue;
    std::mutex indexQueueMutex;
    bool& stop;
    std::vector<std::exception_ptr>& exceptions;
};

void ParseSourceFile(ParserData* parserData)
{
    int index = -1;
    try
    {
        while (!parserData->stop)
        {
            {
                std::lock_guard<std::mutex> lock(parserData->indexQueueMutex);
                if (parserData->indexQueue.empty()) return;
                index = parserData->indexQueue.front();
                parserData->indexQueue.pop_front();
            }
            const std::string& sourceFilePath = parserData->sourceFilePaths[index];
            if (boost::filesystem::file_size(sourceFilePath) == 0)
            {
                std::unique_ptr<CompileUnitNode> compileUnit(new CompileUnitNode(Span(), boost::uuids::nil_uuid(), sourceFilePath));
                compileUnit->SetHash(GetSha1MessageDigest(""));
                parserData->compileUnits[index].reset(compileUnit.release());
                parserData->lexers[index].reset();
            }
            else
            {
                MappedInputFile sourceFile(sourceFilePath);
                ParsingContext parsingContext;
                int fileIndex = parserData->fileIndeces[index];
                std::string fileContent(sourceFile.Begin(), sourceFile.End());
                std::u32string s(ToUtf32(fileContent));
                std::unique_ptr<CmajorLexer> lexer(new CmajorLexer(s, sourceFilePath, fileIndex));
                try
                {
                    std::unique_ptr<CompileUnitNode> compileUnit = CompileUnitParser::Parse(*lexer, &parserData->moduleId, &parsingContext);
                    compileUnit->SetHash(GetSha1MessageDigest(fileContent));
                    if (GetGlobalFlag(GlobalFlags::ast2xml))
                    {
                        std::unique_ptr<sngxml::dom::Document> ast2xmlDoc = cmajor::ast2dom::GenerateAstDocument(compileUnit.get());
                        std::string ast2xmlFilePath = Path::ChangeExtension(sourceFilePath, ".ast.xml");
                        std::ofstream ast2xmlFile(ast2xmlFilePath);
                        CodeFormatter formatter(ast2xmlFile);
                        formatter.SetIndentSize(1);
                        ast2xmlDoc->Write(formatter);
                    }
                    if (GetGlobalFlag(GlobalFlags::generateDebugInfo) || GetGlobalFlag(GlobalFlags::cmdoc))
                    {
                        compileUnit->ComputeLineStarts(s);
                    }
                    parserData->compileUnits[index].reset(compileUnit.release());
                }
                catch (...)
                {
                    parserData->lexers[index].reset(lexer.release());
                    throw;
                }
                parserData->lexers[index].reset(lexer.release());
            }
        }
    }
    catch (...)
    {
        if (index != -1)
        {
            parserData->exceptions[index] = std::current_exception();
            parserData->stop = true;
        }
    }
}

std::vector<std::unique_ptr<CompileUnitNode>> ParseSourcesConcurrently(Module* module, const std::vector<std::string>& sourceFilePaths, int numThreads, bool& stop)
{
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        std::string s;
        if (sourceFilePaths.size() != 1)
        {
            s = "s";
        }
        LogMessage(module->LogStreamId(), "Parsing " + std::to_string(sourceFilePaths.size()) + " source file" + s + " using " + std::to_string(numThreads) + " threads...");
    }
    std::vector<std::unique_ptr<CompileUnitNode>> compileUnits;
    std::vector<std::unique_ptr<CmajorLexer>> lexers;
    try
    {
        int n = int(sourceFilePaths.size());
        compileUnits.resize(n);
        lexers.resize(n);
        std::vector<uint32_t> fileIndeces;
        for (int i = 0; i < n; ++i)
        {
            const std::string& sourceFilePath = sourceFilePaths[i];
            int32_t fileIndex = module->GetFileTable().RegisterFilePath(sourceFilePath);
            fileIndeces.push_back(fileIndex);
        }
        std::vector<std::exception_ptr> exceptions;
        exceptions.resize(n);
        ParserData parserData(module->Id(), sourceFilePaths, compileUnits, lexers, fileIndeces, exceptions, stop);
        for (int i = 0; i < n; ++i)
        {
            parserData.indexQueue.push_back(i);
        }
        std::vector<std::thread> threads;
        for (int i = 0; i < numThreads; ++i)
        {
            threads.push_back(std::thread(ParseSourceFile, &parserData));
            if (parserData.stop) break;
        }
        int numStartedThreads = int(threads.size());
        for (int i = 0; i < numStartedThreads; ++i)
        {
            if (threads[i].joinable())
            {
                threads[i].join();
            }
        }
        for (int i = 0; i < n; ++i)
        {
            if (exceptions[i])
            {
                std::rethrow_exception(exceptions[i]);
            }
        }
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            std::string s;
            if (sourceFilePaths.size() != 1)
            {
                s = "s";
            }
            LogMessage(module->LogStreamId(), "Source file" + s + " parsed.");
        }
    }
    catch (...)
    {
        module->SetLexers(std::move(lexers));
        throw;
    }
    module->SetLexers(std::move(lexers));
    return compileUnits;
}

std::vector<std::unique_ptr<CompileUnitNode>> ParseSources(Module* module, const std::vector<std::string>& sourceFilePaths, bool& stop)
{
    try
    {
        int numThreads = std::min(static_cast<int>(std::thread::hardware_concurrency()), static_cast<int>(sourceFilePaths.size()));
        if (numThreads <= 1)
        {
            numThreads = 1;
        }
        if (numThreads == 1 || GetGlobalFlag(GlobalFlags::debugParsing))
        {
            return ParseSourcesInMainThread(module, sourceFilePaths, stop);
        }
        else
        {
            return ParseSourcesConcurrently(module, sourceFilePaths, numThreads, stop);
        }
    }
    catch (soulng::lexer::ParsingException& ex) 
    {
        ex.SetProject(ToUtf8(module->Name()));
        ex.SetModule(module);
        throw;
    }
    catch (const AttributeNotUniqueException& ex)
    {
        throw Exception(ex.what(), ex.GetSpan(), ex.ModuleId(), ex.PrevSpan(), ex.PrevModuleId());
    }
}

void Preprocess(Project* project, std::vector<std::unique_ptr<CompileUnitNode>>& compileUnits)
{
    std::string projectHashContent;
    for (const std::string& dependsOnId : project->DependsOnIds())
    {
        projectHashContent.append(dependsOnId);
    }
    for (std::unique_ptr<CompileUnitNode>& compileUnit : compileUnits)
    {
        if (compileUnit->GlobalNs()->HasUnnamedNs())
        {
            sngcm::ast::AddNamespaceImportsForUnnamedNamespaces(*compileUnit);
        }
        projectHashContent.append(compileUnit->Hash());
    }
    project->SetHash(GetSha1MessageDigest(projectHashContent));
}

void CreateSymbols(SymbolTable& symbolTable, const std::vector<std::unique_ptr<CompileUnitNode>>& compileUnits, bool& stop)
{
    SymbolCreatorVisitor symbolCreator(symbolTable);
    for (const std::unique_ptr<CompileUnitNode>& compileUnit : compileUnits)
    {
        if (stop)
        {
            return;
        }
        symbolTable.SetCurrentCompileUnit(compileUnit.get());
        compileUnit->Accept(symbolCreator);
    }
}

std::vector<std::unique_ptr<BoundCompileUnit>> BindTypes(Module& module, const std::vector<std::unique_ptr<CompileUnitNode>>& compileUnits, AttributeBinder* attributeBinder, 
    bool& stop)
{
    std::vector<std::unique_ptr<BoundCompileUnit>> boundCompileUnits;
    for (const std::unique_ptr<CompileUnitNode>& compileUnit : compileUnits)
    {
        if (stop)
        {
            return std::vector<std::unique_ptr<BoundCompileUnit>>();
        }
        std::unique_ptr<BoundCompileUnit> boundCompileUnit(new BoundCompileUnit(module, compileUnit.get(), attributeBinder));
        boundCompileUnit->PushBindingTypes();
        TypeBinder typeBinder(*boundCompileUnit);
        compileUnit->Accept(typeBinder);
        boundCompileUnit->PopBindingTypes();
        boundCompileUnits.push_back(std::move(boundCompileUnit));
    }
    return boundCompileUnits;
}

void BindStatements(BoundCompileUnit& boundCompileUnit)
{
    StatementBinder statementBinder(boundCompileUnit);
    boundCompileUnit.GetCompileUnitNode()->Accept(statementBinder);
}

void GenerateLibraryCpp(Module* module, const std::vector<std::string>& objectFilePaths, const std::string& libraryFilePath)
{
    if (GetGlobalFlag(GlobalFlags::disableCodeGen)) return;
    if (GetGlobalFlag(GlobalFlags::verbose) && !GetGlobalFlag(GlobalFlags::unitTest))
    {
        LogMessage(module->LogStreamId(), "Creating library...");
    }
    const Tool& libraryManagerTool = GetLibraryManagerTool(GetPlatform(), GetToolChain());
    const Configuration& configuration = GetToolConfiguration(libraryManagerTool, GetConfig());
    module->SetCurrentToolName(ToUtf32(libraryManagerTool.commandName));
    std::string command;
    std::string errors;
    command.append(libraryManagerTool.commandName);
    for (const std::string& arg : configuration.args)
    {
        std::string a = arg;
        if (a.find('$') != std::string::npos)
        {
            if (a.find("$LIBRARY_FILE$") != std::string::npos)
            {
                a = soulng::util::Replace(a, "$LIBRARY_FILE$", QuotedPath(libraryFilePath));
            }
            else if (a.find("$OBJECT_FILES$") != std::string::npos)
            {
                std::string value;
                bool first = true;
                for (const std::string& objectFilePath : objectFilePaths)
                {
                    if (first)
                    {
                        first = false;
                    }
                    else
                    {
                        value.append(" ");
                    }
                    value.append(QuotedPath(objectFilePath));
                }
                a = soulng::util::Replace(a, "$OBJECT_FILES$", value);
            }
        }
        command.append(1, ' ').append(a);
    }
    try
    {
        Process::Redirections redirections = Process::Redirections::processStdErr;
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            redirections = redirections | Process::Redirections::processStdOut;
        }
        Process process(command, redirections);
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            while (!process.Eof(Process::StdHandle::stdOut))
            {
                std::string line = process.ReadLine(Process::StdHandle::stdOut);
                if (!line.empty())
                {
                    LogMessage(module->LogStreamId(), PlatformStringToUtf8(line));
                }
            }
        }
        errors = process.ReadToEnd(Process::StdHandle::stdErr);
        process.WaitForExit();
        int exitCode = process.ExitCode();
        if (exitCode != 0)
        {
            throw std::runtime_error("executing '" + command + "' failed with exit code: " + std::to_string(exitCode));
        }
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("generating library '" + libraryFilePath + "' failed: " + ex.what() + ":\nerrors:\n" + errors);
    }
    if (GetGlobalFlag(GlobalFlags::verbose) && !GetGlobalFlag(GlobalFlags::unitTest))
    {
        LogMessage(module->LogStreamId(), "==> " + libraryFilePath);
    }
}

#ifdef _WIN32

void GenerateLibraryLlvm(Module* module, const std::vector<std::string>& objectFilePaths, const std::string& libraryFilePath)
{
    if (GetGlobalFlag(GlobalFlags::verbose) && !GetGlobalFlag(GlobalFlags::unitTest))
    {
        LogMessage(module->LogStreamId(), "Creating library...");
    }
    module->SetCurrentToolName(U"llvm-lib");
    std::vector<std::string> args;
    args.push_back("/out:" + QuotedPath(libraryFilePath));
    int n = objectFilePaths.size();
    for (int i = 0; i < n; ++i)
    {
        args.push_back(QuotedPath(objectFilePaths[i]));
    }
    std::string libCommandLine = "llvm-lib";
    std::string errors;
    for (const std::string& arg : args)
    {
        libCommandLine.append(1, ' ').append(arg);
    }
    try
    {
        Process::Redirections redirections = Process::Redirections::processStdErr;
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            redirections = redirections | Process::Redirections::processStdOut;
        }
        Process process(libCommandLine, redirections);
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            while (!process.Eof(Process::StdHandle::stdOut))
            {
                std::string line = process.ReadLine(Process::StdHandle::stdOut);
                if (!line.empty())
                {
                    LogMessage(module->LogStreamId(), PlatformStringToUtf8(line));
                }
            }
        }
        errors = process.ReadToEnd(Process::StdHandle::stdErr);
        process.WaitForExit();
        int exitCode = process.ExitCode();
        if (exitCode != 0)
        {
            throw std::runtime_error("generating library '" + libraryFilePath + "' failed with exit code " + std::to_string(exitCode) + ": " + errors);
        }
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("generating library '" + libraryFilePath + "' failed: " + ex.what() + ":\nerrors:\n" + errors);
    }
    if (GetGlobalFlag(GlobalFlags::verbose) && !GetGlobalFlag(GlobalFlags::unitTest))
    {
        LogMessage(module->LogStreamId(), "==> " + libraryFilePath);
    }
}

void GenerateLibrarySystemX(Module* module, const std::vector<std::string>& objectFilePaths, const std::string& libraryFilePath)
{
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(module->LogStreamId(), "Creating library...");
    }
    boost::filesystem::remove(libraryFilePath);
    try
    {
        cmsx::object::CreateArchive(module->LogStreamId(), libraryFilePath, objectFilePaths, GetGlobalFlag(GlobalFlags::verbose));
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("generating library '" + libraryFilePath + "' failed: " + ex.what());
    }
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(module->LogStreamId(), "==> " + libraryFilePath);
    }
}

#else

void GenerateLibraryLlvm(Module* module, const std::vector<std::string>& objectFilePaths, const std::string& libraryFilePath)
{
    if (GetGlobalFlag(GlobalFlags::verbose) && !GetGlobalFlag(GlobalFlags::unitTest))
    {
        LogMessage(module->LogStreamId(), "Creating library...");
    }
    boost::filesystem::remove(libraryFilePath);
    module->SetCurrentToolName(U"ar");
    std::vector<std::string> args;
    args.push_back("-o " + QuotedPath(libraryFilePath));
    int n = objectFilePaths.size();
    for (int i = 0; i < n; ++i)
    {
        args.push_back(QuotedPath(objectFilePaths[i]));
    }
    std::string errors;
    std::string libCommandLine = "ar q";
    for (const std::string& arg : args)
    {
        libCommandLine.append(1, ' ').append(arg);
    }
    try
    {
        Process::Redirections redirections = Process::Redirections::processStdErr;
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            redirections = redirections | Process::Redirections::processStdOut;
        }
        Process process(libCommandLine, redirections);
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            while (!process.Eof(Process::StdHandle::stdOut))
            {
                std::string line = process.ReadLine(Process::StdHandle::stdOut);
                if (!line.empty())
                {
                    LogMessage(module->LogStreamId(), PlatformStringToUtf8(line));
                }
            }
        }
        errors = process.ReadToEnd(Process::StdHandle::stdErr);
        process.WaitForExit();
        int exitCode = process.ExitCode();
        if (exitCode != 0)
        {
            throw std::runtime_error("executing '" + libCommandLine + "' failed with exit code: " + std::to_string(exitCode));
        }
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("generating library '" + libraryFilePath + "' failed: " + ex.what() + ":\nerrors:\n" + errors);
    }
    if (GetGlobalFlag(GlobalFlags::verbose) && !GetGlobalFlag(GlobalFlags::unitTest))
    {
        LogMessage(module->LogStreamId(), "==> " + libraryFilePath);
    }
}

#endif

void GenerateLibrary(Module* module, const std::vector<std::string>& objectFilePaths, const std::string& libraryFilePath)
{
    if (GetBackEnd() == cmajor::symbols::BackEnd::llvm)
    {
        GenerateLibraryLlvm(module, objectFilePaths, libraryFilePath);
    }
    else if (GetBackEnd() == cmajor::symbols::BackEnd::cmcpp)
    {
        GenerateLibraryCpp(module, objectFilePaths, libraryFilePath);
    }
#ifdef _WIN32
    else if (GetBackEnd() == cmajor::symbols::BackEnd::cmsx)
    {
        GenerateLibrarySystemX(module, objectFilePaths, libraryFilePath);
    }
#endif
}

std::string GetBoostLibDirFromCompilerConfigXml()
{
    std::string cmajorConfigDir = Path::Combine(CmajorRootDir(), "config");
    std::string compilerConfigXmlFilePath = Path::Combine(cmajorConfigDir, "compiler-config.xml");
    if (boost::filesystem::exists(compilerConfigXmlFilePath))
    {
        std::unique_ptr<sngxml::dom::Document> compilerConfigDoc = sngxml::dom::ReadDocument(compilerConfigXmlFilePath);
        std::unique_ptr<sngxml::xpath::XPathObject> result = sngxml::xpath::Evaluate(U"/compiler/" + ToUtf32(GetPlatform()) + U"/" + ToUtf32(GetToolChain()) + U"/boost", compilerConfigDoc.get());
        if (result)
        {
            if (result->Type() == sngxml::xpath::XPathObjectType::nodeSet)
            {
                sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(result.get());
                int n = nodeSet->Length();
                for (int i = 0; i < n; ++i)
                {
                    sngxml::dom::Node* node = (*nodeSet)[i];
                    if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                    {
                        sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                        std::u32string libraryDirAttr = element->GetAttribute(U"libraryDir");
                        if (!libraryDirAttr.empty())
                        {
                            return ToUtf8(libraryDirAttr);
                        }
                    }
                }
            }
        }
    }
    return std::string();
}

std::string MakeGccLibraryName(const std::string& libraryFilePath)
{
    if (StartsWith(libraryFilePath, "lib") && EndsWith(libraryFilePath, ".a"))
    {
        return "-l" + Path::GetFileName(libraryFilePath.substr(3, libraryFilePath.length() - (3 + 2)));
    }
    return libraryFilePath;
}

const char* dynamicRuntimeArg = "-lcmrt430cpp";
const char* dynamicDebugRuntimeArg = "-lcmrt430cppd";

void LinkCpp(Target target, const std::string& executableFilePath, const std::string& libraryFilePath, const std::vector<std::string>& libraryFilePaths, const std::string& mainObjectFilePath, Module& module)
{
    if (GetGlobalFlag(GlobalFlags::disableCodeGen)) return;
    if (GetGlobalFlag(GlobalFlags::verbose) && !GetGlobalFlag(GlobalFlags::unitTest))
    {
        LogMessage(module.LogStreamId(), "Linking...");
    }
    boost::filesystem::path bdp = executableFilePath;
    bdp.remove_filename();
    boost::filesystem::create_directories(bdp);
    std::string platform = GetPlatform();
    std::string toolChain = GetToolChain();
    const Tool& linkerTool = GetLinkerTool(platform, toolChain);
    const Configuration& configuration = GetToolConfiguration(linkerTool, GetConfig());
    std::string toolChainDir = GetFullPath(Path::Combine(Path::Combine(CmajorRootDir(), "lib"), toolChain));
    std::string binDir = GetFullPath(Path::Combine(CmajorRootDir(), "bin"));
    std::string linkCommandLine;
    linkCommandLine = linkerTool.commandName;
    std::set<std::string> libraryDirectories;
    libraryDirectories.insert(toolChainDir);
    libraryDirectories.insert(binDir);
    std::set<std::string> libraryNames;
    for (const std::string& libraryFilePath : libraryFilePaths)
    {
        std::string libraryDir = GetFullPath(Path::GetDirectoryName(libraryFilePath));
        libraryDirectories.insert(libraryDir);
        std::string libraryName = MakeGccLibraryName(libraryFilePath);
        libraryNames.insert(libraryName);
    }
    std::string boostLibDir = GetBoostLibDirFromCompilerConfigXml();
    if (!boostLibDir.empty())
    {
        libraryDirectories.insert(GetFullPath(boostLibDir));
    }
    std::string libraryPathFlag;
    for (const std::string& arg : configuration.args)
    {
        if (arg.find('$') != std::string::npos)
        {
            if (arg.find("$LIBRARY_PATH_FLAG$") != std::string::npos)
            {
                libraryPathFlag = soulng::util::Replace(arg, "$LIBRARY_PATH_FLAG$", "");
            }
            else if (arg.find("$LIBRARY_DIRECTORIES$") != std::string::npos)
            {
                std::string libraryPathArgs;
                bool first = true;
                for (const std::string& libraryDir : libraryDirectories)
                {
                    if (first)
                    {
                        first = false;
                    }
                    else
                    {
                        libraryPathArgs.append(1, ' ');
                    }
                    libraryPathArgs.append(libraryPathFlag).append(QuotedPath(libraryDir));
                }
                linkCommandLine.append(1, ' ').append(soulng::util::Replace(arg, "$LIBRARY_DIRECTORIES$", libraryPathArgs));
            }
            else if (arg.find("$RUNTIME_LIBS$") != std::string::npos)
            {
                std::string libs = soulng::util::Replace(arg, "$RUNTIME_LIBS$", "");
                if (libs == "DYNAMIC")
                {
                    if (GetGlobalFlag(GlobalFlags::linkWithDebugRuntime))
                    {
                        libraryNames.insert(dynamicDebugRuntimeArg);
                    }
                    else
                    {
                        libraryNames.insert(dynamicRuntimeArg);
                    }
                }
                else
                {
                    std::vector<std::string> runtimeLibs = Split(libs, ';');
                    for (const std::string& runtimeLib : runtimeLibs)
                    {
                        libraryNames.insert(runtimeLib);
                    }
                }
            }
            else if (arg.find("$LIBRARY_FILES$") != std::string::npos)
            {
                std::string libFilesArg;
                for (const std::string& libraryName : libraryNames)
                {
                    libFilesArg.append(1, ' ').append(QuotedPath(libraryName));
                }
                linkCommandLine.append(1, ' ').append(soulng::util::Replace(arg, "$LIBRARY_FILES$", libFilesArg));
            }
            else if (arg.find("$RESOURCE_FILE$") != std::string::npos)
            {
                if (!module.ResourceFilePath().empty())
                {
                    linkCommandLine.append(1, ' ').append(soulng::util::Replace(arg, "$RESOURCE_FILE$", QuotedPath(module.ResourceFilePath())));
                }
            }
            else if (arg.find("$MAIN_OBJECT_FILE$") != std::string::npos)
            {
                std::string mainObjectPath = mainObjectFilePath;
                linkCommandLine.append(1, ' ').append(soulng::util::Replace(arg, "$MAIN_OBJECT_FILE$", QuotedPath(mainObjectPath)));
            }
            else if (arg.find("$EXECUTABLE_FILE$") != std::string::npos)
            {
                std::string exePath = executableFilePath;
                if (Path::HasExtension(exePath))
                {
                    exePath = Path::ChangeExtension(exePath, "");
                }
                linkCommandLine.append(1, ' ').append(soulng::util::Replace(arg, "$EXECUTABLE_FILE$", QuotedPath(exePath)));
            }
            else if (arg.find("$DEBUG_INFORMATION_FILE$") != std::string::npos)
            {
                linkCommandLine.append(1, ' ').append(soulng::util::Replace(arg, "$DEBUG_INFORMATION_FILE$", QuotedPath(Path::ChangeExtension(executableFilePath, linkerTool.debugInformationFileExtension))));
            }
            else if (arg.find("$ENTRY$") != std::string::npos)
            {
                if (target == Target::winguiapp)
                {
                    linkCommandLine.append(1, ' ').append(soulng::util::Replace(arg, "$ENTRY$", "wWinMain"));
                }
                else
                {
                    linkCommandLine.append(1, ' ').append(soulng::util::Replace(arg, "$ENTRY$", "wmain"));
                }
            }
            else if (arg.find("$SUBSYSTEM$") != std::string::npos)
            {
                if (target == Target::winguiapp)
                {
                    linkCommandLine.append(1, ' ').append(soulng::util::Replace(arg, "$SUBSYSTEM$", "windows"));
                }
                else
                {
                    linkCommandLine.append(1, ' ').append(soulng::util::Replace(arg, "$SUBSYSTEM$", "console"));
                }
            }
        }
        else
        {
            linkCommandLine.append(1, ' ').append(arg);
        }
    }
    std::string errors;
    try
    {
        Process::Redirections redirections = Process::Redirections::processStdErr;
        Process process(linkCommandLine, redirections);
        errors = process.ReadToEnd(Process::StdHandle::stdErr);
        process.WaitForExit();
        int exitCode = process.ExitCode();
        if (exitCode != 0)
        {
            throw std::runtime_error("executing '" + linkCommandLine + "' failed with exit code: " + std::to_string(exitCode));
        }
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("linking executable '" + executableFilePath + "' failed: " + ex.what() + ":\nerrors:\n" + errors);
    }
    if (GetGlobalFlag(GlobalFlags::verbose) && !GetGlobalFlag(GlobalFlags::unitTest))
    {
        LogMessage(module.LogStreamId(), "==> " + executableFilePath);
    }
}

std::string GetCppSolutionDirectoryPath(Project* project)
{
    if (currentSolution == nullptr) return std::string();
    const Tool& libraryManagerTool = GetLibraryManagerTool(GetPlatform(), GetToolChain());
    const Configuration& configuration = GetToolConfiguration(libraryManagerTool, GetConfig());
    return GetFullPath(Path::Combine(Path::Combine(Path::Combine(Path::Combine(Path::GetDirectoryName(currentSolution->FilePath()), "cpp"), GetToolChain()), GetConfig()), configuration.outputDirectory));
}

void CreateCppProjectFileSystemX(Project* project, Module& module, const std::string& mainObjectFilePath)
{
    cmsx::intermediate::GenerateProjectFile(project, &module, mainObjectFilePath, true);
}

void CreateCppProjectFile(Project* project, Module& module, const std::string& mainSourceFilePath, const std::string& libraryFilePath, const std::vector<std::string>& libraryFilePaths)
{
    std::string toolChain = GetToolChain();
    if (toolChain != "vs") return;
    if (GetGlobalFlag(GlobalFlags::verbose) && !GetGlobalFlag(GlobalFlags::unitTest))
    {
        LogMessage(module.LogStreamId(), "Creating project file...");
    }
    const Tool& projectFileGeneratorTool = GetProjectFileGeneratorTool(GetPlatform(), toolChain);
    std::set<std::string> libraryDirectories;
    std::set<std::string> libraryNames;
    if (!GetGlobalFlag(GlobalFlags::disableCodeGen))
    {
        std::string libraryDir = GetFullPath(Path::GetDirectoryName(libraryFilePath));
        libraryDirectories.insert(libraryDir);
        std::string libraryName = QuotedPath(Path::GetFileName(libraryFilePath));
        libraryNames.insert(libraryName);
    }
    std::string solutionDir = GetCppSolutionDirectoryPath(project);
    if (!solutionDir.empty())
    {
        libraryDirectories.insert(solutionDir);
    }
    std::string cmajorLibDir = GetFullPath(Path::Combine(CmajorRootDir(), "lib"));
    std::string toolChainDir = QuotedPath(GetFullPath(Path::Combine(Path::Combine(CmajorRootDir(), "lib"), GetToolChain())));
    libraryDirectories.insert(cmajorLibDir);
    libraryDirectories.insert(toolChainDir);
#ifdef _WIN32
    std::string boostLibDir = GetFullPath(GetBoostLibDirFromCompilerConfigXml());
    libraryDirectories.insert(boostLibDir);
#endif
    for (const std::string& libFilePath : libraryFilePaths)
    {
        if (GetGlobalFlag(GlobalFlags::disableCodeGen))
        {
            if (Path::GetFileName(libFilePath) == Path::GetFileName(libraryFilePath))
            {
                continue;
            }
        }
        std::string libraryDir = GetFullPath(Path::GetDirectoryName(libFilePath));
        libraryDirectories.insert(libraryDir);
        std::string libraryName = QuotedPath(Path::GetFileName(libFilePath));
        libraryNames.insert(libraryName);
    }
    std::string config = GetConfig();
    const Configuration& configuration = GetToolConfiguration(projectFileGeneratorTool, config);
    std::string commandLine;
    std::string errorFilePath;
    std::string projectFilePath;
    std::string options;
    std::string errors;
    commandLine = projectFileGeneratorTool.commandName;
    for (const std::string& arg : configuration.args)
    {
        if (arg.find('$') != std::string::npos)
        {
            if (arg.find("$PROJECT_NAME$") != std::string::npos)
            {
                commandLine.append(1, ' ').append(soulng::util::Replace(arg, "$PROJECT_NAME$", ToUtf8(project->Name())));
            }
            else if (arg.find("$PROJECT_FILE_PATH$") != std::string::npos)
            {
                projectFilePath = GetFullPath(Path::Combine(Path::Combine(Path::Combine(Path::Combine(Path::Combine(project->SourceBasePath().generic_string(), "lib"), "cpp"), toolChain), config), ToUtf8(project->Name()) + projectFileGeneratorTool.outputFileExtension));
                commandLine.append(1, ' ').append(soulng::util::Replace(arg, "$PROJECT_FILE_PATH$", projectFilePath));
            }
            else if (arg.find("$PROJECT_TARGET$") != std::string::npos)
            {
                commandLine.append(1, ' ').append(soulng::util::Replace(arg, "$PROJECT_TARGET$", TargetStr(project->GetTarget())));
            }
            else if (arg.find("$PROJECT_CONFIG$") != std::string::npos)
            {
                commandLine.append(1, ' ').append(soulng::util::Replace(arg, "$PROJECT_CONFIG$", config));
            }
            else if (arg.find("$RUNTIME_LIBS$") != std::string::npos)
            {
                std::string runtimeLibs = soulng::util::Replace(arg, "$RUNTIME_LIBS$", "");
                std::vector<std::string> libs = soulng::util::Split(runtimeLibs, ';');
                for (const std::string& lib : libs)
                {
                    libraryNames.insert(lib);
                }
            }
            else if (arg.find("$LIBRARY_DIRECTORIES$") != std::string::npos)
            {
                std::string libraryDirectoriesStr;
                bool first = true;
                for (const std::string& libraryDirectory : libraryDirectories)
                {
                    if (first)
                    {
                        first = false;
                    }
                    else
                    {
                        libraryDirectoriesStr.append(1, ';');
                    }
                    libraryDirectoriesStr.append(QuotedPath(libraryDirectory));
                }
                commandLine.append(1, ' ').append(soulng::util::Replace(arg, "$LIBRARY_DIRECTORIES$", libraryDirectoriesStr));
            }
            else if (arg.find("$LIBRARY_FILE_NAMES$") != std::string::npos)
            {
                std::string libraryNamesStr;
                bool first = true;
                for (const std::string& libraryName : libraryNames)
                {
                    if (first)
                    {
                        first = false;
                    }
                    else
                    {
                        libraryNamesStr.append(1, ';');
                    }
                    libraryNamesStr.append(QuotedPath(libraryName));
                }
                commandLine.append(1, ' ').append(soulng::util::Replace(arg, "$LIBRARY_FILE_NAMES$", libraryNamesStr));
            }
            else if (arg.find("$GENERATE_ASSEMBLY_FILE_OPTION$") != std::string::npos)
            {
                if (GetGlobalFlag(GlobalFlags::emitLlvm))
                {
                    options.append(soulng::util::Replace(arg, "$GENERATE_ASSEMBLY_FILE_OPTION$", ""));
                }
                else
                {
                    continue;
                }
            }
            else if (arg.find("$GENERATE_JUST_MY_CODE_OPTION$") != std::string::npos)
            {
                if (GetGlobalFlag(GlobalFlags::justMyCodeDebugging))
                {
                    options.append(soulng::util::Replace(arg, "$GENERATE_JUST_MY_CODE_OPTION$", ""));
                }
                else
                {
                    continue;
                }
            }
            else if (arg.find("$ENABLE_RUNTIME_TYPE_INFORMATION_OPTION$") != std::string::npos)
            {
                options.append(soulng::util::Replace(arg, "$ENABLE_RUNTIME_TYPE_INFORMATION_OPTION$", ""));
            }
            else if (arg.find("$OPTIONS$") != std::string::npos)
            {
                if (!options.empty())
                {
                    commandLine.append(1, ' ').append(soulng::util::Replace(arg, "$OPTIONS$", QuotedPath(options)));
                }
                else
                {
                    continue;
                }
            }
            else if (arg.find("$SOURCE_FILES$") != std::string::npos)
            {
                std::string sourceFilePaths;
                for (const std::string& sourceFilePath : project->RelativeSourceFilePaths())
                {
                    sourceFilePaths.append(1, ' ');
                    sourceFilePaths.append(QuotedPath(Path::ChangeExtension(sourceFilePath, ".cpp")));
                }
                sourceFilePaths.append(1, ' ');
                sourceFilePaths.append(QuotedPath(mainSourceFilePath));
                commandLine.append(1, ' ').append(soulng::util::Replace(arg, "$SOURCE_FILES$", sourceFilePaths));
            }
        }
        else
        {
            commandLine.append(1, ' ').append(arg);
        }
    }
    try
    {
        Process::Redirections redirections = Process::Redirections::processStdErr;
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            redirections = redirections | Process::Redirections::processStdOut;
        }
        Process process(commandLine, redirections);
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            while (!process.Eof(Process::StdHandle::stdOut))
            {
                std::string line = process.ReadLine(Process::StdHandle::stdOut);
                if (!line.empty())
                {
                    LogMessage(module.LogStreamId(), PlatformStringToUtf8(line));
                }
            }
        }
        errors = process.ReadToEnd(Process::StdHandle::stdErr);
        process.WaitForExit();
        int exitCode = process.ExitCode();
        if (exitCode != 0)
        {
            throw std::runtime_error("executing '" + commandLine + "' failed with exit code: " + std::to_string(exitCode));
        }
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("generating C++ project file '" + projectFilePath + "' for project '" + ToUtf8(project->Name()) + "' failed: " + ex.what() + ":\nerrors:\n" + errors);
    }
    if (GetGlobalFlag(GlobalFlags::verbose) && !GetGlobalFlag(GlobalFlags::unitTest))
    {
        LogMessage(module.LogStreamId(), "==> " + projectFilePath);
    }
}

void CreateCppSolutionFile(Solution* solution, const std::vector<Project*>& projects)
{
    std::string toolChain = GetToolChain();
    if (toolChain != "vs") return;
    std::string solutionFilePath;
    const Tool& solutionFileGeneratorTool = GetSolutionFileGeneratorTool(GetPlatform(), toolChain);
    if (GetGlobalFlag(GlobalFlags::verbose) && !GetGlobalFlag(GlobalFlags::unitTest))
    {
        LogMessage(-1, "Creating solution file...");
    }
    std::string commandLine;
    std::string projectFilePath;
    std::string errors;
    commandLine = solutionFileGeneratorTool.commandName;
    std::string config = GetConfig();
    const Configuration& configuration = GetToolConfiguration(solutionFileGeneratorTool, config);
    for (const std::string& arg : configuration.args)
    {
        if (arg.find('$') != std::string::npos)
        {
            if (arg.find("$SOLUTION_NAME$") != std::string::npos)
            {
                commandLine.append(1, ' ').append(soulng::util::Replace(arg, "$SOLUTION_NAME$", ToUtf8(solution->Name())));
            }
            else if (arg.find("$SOLUTION_FILE_PATH$") != std::string::npos)
            {
                solutionFilePath = soulng::util::Replace(arg, "$SOLUTION_FILE_PATH$",
                    QuotedPath(Path::Combine(Path::Combine(Path::Combine(Path::Combine(Path::GetDirectoryName(solution->FilePath()), "cpp"), toolChain), config),
                        Path::ChangeExtension(Path::GetFileName(solution->FilePath()), ".sln"))));
                commandLine.append(1, ' ').append(solutionFilePath);
            }
            else if (arg.find("$PROJECT_FILE_PATHS$") != std::string::npos)
            {
                std::string projectFilePaths;
                int n = solution->RelativeProjectFilePaths().size();
                for (int i = 0; i < n; ++i)
                {
                    const std::string& relativeProjectFilePath = solution->RelativeProjectFilePaths()[i];
                    std::string projectFilePath = "../../../" + Path::GetDirectoryName(relativeProjectFilePath) + "/lib/cpp/" + toolChain + "/" + config  + "/" + Path::ChangeExtension(Path::GetFileName(relativeProjectFilePath), ".vcxproj");
                    projectFilePaths.append(" ").append(QuotedPath(projectFilePath));
                }
                commandLine.append(1, ' ').append(soulng::util::Replace(arg, "$PROJECT_FILE_PATHS$", projectFilePaths));
            }
        }
        else
        {
            commandLine.append(1, ' ').append(arg);
        }
    }
    try
    {
        Process::Redirections redirections = Process::Redirections::processStdErr;
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            redirections = redirections | Process::Redirections::processStdOut;
        }
        Process process(commandLine, redirections);
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            while (!process.Eof(Process::StdHandle::stdOut))
            {
                std::string line = process.ReadLine(Process::StdHandle::stdOut);
                if (!line.empty())
                {
                    LogMessage(-1, PlatformStringToUtf8(line));
                }
            }
        }
        errors = process.ReadToEnd(Process::StdHandle::stdErr);
        process.WaitForExit();
        int exitCode = process.ExitCode();
        if (exitCode != 0)
        {
            throw std::runtime_error("executing '" + commandLine + "' failed with exit code: " + std::to_string(exitCode));
        }
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("generating C++ solution file '" + solutionFilePath + "' for solution '" + ToUtf8(solution->Name()) + "' failed: " + ex.what() + ":\nerrors:\n" + errors);
    }
    if (GetGlobalFlag(GlobalFlags::verbose) && !GetGlobalFlag(GlobalFlags::unitTest))
    {
        LogMessage(-1, "==> " + solutionFilePath);
    }
}

#ifdef _WIN32

void CreateDefFile(const std::string& defFilePath, Module& module)
{
    std::ofstream defFile(defFilePath);
    CodeFormatter formatter(defFile);
    formatter.WriteLine("EXPORTS");
    formatter.IncIndent();
    for (const std::string& fun : module.AllExportedFunctions())
    {
        formatter.WriteLine(fun);
    }
    for (const std::string& fun : module.ExportedFunctions())
    {
        formatter.WriteLine(fun);
    }
    for (const std::string& data : module.AllExportedData())
    {
        formatter.WriteLine(data + " DATA");
    }
    for (const std::string& data : module.ExportedData())
    {
        formatter.WriteLine(data + " DATA");
    }
}

void LinkLlvm(Target target, const std::string& executableFilePath, const std::string& libraryFilePath, const std::vector<std::string>& libraryFilePaths, const std::string& mainObjectFilePath, Module& module)
{
    if (GetGlobalFlag(GlobalFlags::verbose) && !GetGlobalFlag(GlobalFlags::unitTest))
    {
        LogMessage(module.LogStreamId(), "Linking...");
    }
    if (GetGlobalFlag(GlobalFlags::linkUsingMsLink))
    {
        module.SetCurrentToolName(U"link");
    }
    else
    {
        module.SetCurrentToolName(U"lld-link");
    }
    boost::filesystem::path bdp = executableFilePath;
    bdp.remove_filename();
    boost::filesystem::create_directories(bdp);
    std::vector<std::string> args;
    args.push_back("/machine:x64");
    if (target == Target::winguiapp)
    {
        args.push_back("/entry:WinMain");
        args.push_back("/subsystem:windows");
    }
    else
    {
        args.push_back("/entry:main");
    }
    args.push_back("/debug");
    args.push_back("/out:" + QuotedPath(executableFilePath));
    args.push_back("/stack:16777216");
    std::string cmrtLibName = "cmrt430.lib";
    if (GetGlobalFlag(GlobalFlags::linkWithDebugRuntime))
    {
        cmrtLibName = "cmrt430d.lib";
    }
    args.push_back(QuotedPath(GetFullPath(Path::Combine(Path::Combine(CmajorRootDir(), "lib"), cmrtLibName))));
    args.push_back(QuotedPath(mainObjectFilePath));
    int n = libraryFilePaths.size();
    for (int i = 0; i < n; ++i)
    {
        args.push_back(QuotedPath(libraryFilePaths[i]));
    }
    if (!module.ResourceFilePath().empty())
    {
        args.push_back(QuotedPath(module.ResourceFilePath()));
    }
    std::string errors;
    std::string linkCommandLine;
    if (GetGlobalFlag(GlobalFlags::linkUsingMsLink))
    {
        linkCommandLine = "link";
    }
    else
    {
        linkCommandLine = "lld-link";
    }
    for (const std::string& arg : args)
    {
        linkCommandLine.append(1, ' ').append(arg);
    }
    try
    {
        Process::Redirections redirections = Process::Redirections::processStdErr;
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            redirections = redirections | Process::Redirections::processStdOut;
        }
        Process process(linkCommandLine, redirections);
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            while (!process.Eof(Process::StdHandle::stdOut))
            {
                std::string line = process.ReadLine(Process::StdHandle::stdOut);
                if (!line.empty())
                {
                    LogMessage(-1, PlatformStringToUtf8(line));
                }
            }
        }
        errors = process.ReadToEnd(Process::StdHandle::stdErr);
        process.WaitForExit();
        int exitCode = process.ExitCode();
        if (exitCode != 0)
        {
            throw std::runtime_error("executing '" + linkCommandLine + "' failed with exit code: " + std::to_string(exitCode));
        }
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("linking executable '" + executableFilePath + "' failed: " + ex.what() + ":\nerrors:\n" + errors);
    }
    if (GetGlobalFlag(GlobalFlags::verbose) && !GetGlobalFlag(GlobalFlags::unitTest))
    {
        LogMessage(module.LogStreamId(), "==> " + executableFilePath);
    }
}

void LinkSystemX(const std::string& executableFilePath, const std::string& libraryFilePath, const std::vector<std::string>& libraryFilePaths, const std::string& mainObjectFilePath, Module& module)
{
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(module.LogStreamId(), "Linking...");
    }
    std::string classIdFileName = Path::ChangeExtension(libraryFilePath, ".clsid");
    MakeClassIdFile(module.GetSymbolTable().PolymorphicClasses(), classIdFileName);
    module.SetCurrentToolName(U"sxlink");
    boost::filesystem::path bdp = executableFilePath;
    bdp.remove_filename();
    boost::filesystem::create_directories(bdp);
    std::vector<std::string> binaryFileNames;
    std::string systemMainObjectFilePath = GetFullPath(Path::Combine(CmajorRootDir(), "system-x/system/Main/Main.o"));
    binaryFileNames.push_back(systemMainObjectFilePath);
    binaryFileNames.push_back(mainObjectFilePath);
    int n = libraryFilePaths.size();
    for (int i = n - 1; i >= 0; --i)
    {
        binaryFileNames.push_back(libraryFilePaths[i]);
    }
    try
    {
        cmsx::object::Link(module.LogStreamId(), executableFilePath, binaryFileNames, classIdFileName, GetGlobalFlag(GlobalFlags::verbose));
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("linking executable '" + executableFilePath + "' failed: " + ex.what());
    }
}

#else

void CreateDynamicListFile(const std::string& dynamicListFilePath, Module& module)
{
    std::ofstream defFile(dynamicListFilePath);
    CodeFormatter formatter(defFile);
    formatter.WriteLine("{");
    formatter.IncIndent();
    for (const std::string& fun : module.AllExportedFunctions())
    {
        formatter.WriteLine(fun + ";");
    }
    for (const std::string& fun : module.ExportedFunctions())
    {
        formatter.WriteLine(fun + ";");
    }
    for (const std::string& data : module.AllExportedData())
    {
        formatter.WriteLine(data + ";");
    }
    for (const std::string& data : module.ExportedData())
    {
        formatter.WriteLine(data + ";");
    }
    formatter.DecIndent();
    formatter.WriteLine("};");
}

void LinkLlvm(Target target, const std::string& executableFilePath, const std::string& libraryFilePath, const std::vector<std::string>& libraryFilePaths, const std::string& mainObjectFilePath, Module& module)
{
    if (GetGlobalFlag(GlobalFlags::verbose) && !GetGlobalFlag(GlobalFlags::unitTest))
    {
        LogMessage(module.LogStreamId(), "Linking...");
    }
    module.SetCurrentToolName(U"clang++");
    boost::filesystem::path bdp = executableFilePath;
    bdp.remove_filename();
    boost::filesystem::create_directories(bdp);
    std::vector<std::string> args;
    args.push_back("-L" + Path::Combine(CmajorRootDir(), "lib"));
    char* cmajorLibDir = getenv("CMAJOR_LIBDIR");
    if (cmajorLibDir && *cmajorLibDir)
    {
        args.push_back("-L" + std::string(cmajorLibDir));
    }
    args.push_back(QuotedPath(mainObjectFilePath));
    args.push_back("-Xlinker --start-group");
    int n = libraryFilePaths.size();
    args.push_back(QuotedPath(libraryFilePaths.back()));
    for (int i = 0; i < n - 1; ++i)
    {
        args.push_back(QuotedPath(libraryFilePaths[i]));
    }
    if (GetGlobalFlag(GlobalFlags::linkWithDebugRuntime))
    {
        std::string cmrtLibName = "cmrtd";
        args.push_back("-l" + cmrtLibName);
    }
    else
    {
        std::string cmrtLibName = "cmrt";
        args.push_back("-l" + cmrtLibName);
    }
    args.push_back("-lboost_filesystem -lboost_iostreams -lboost_system -lbz2 -lz");
    args.push_back("-Xlinker --end-group");
    args.push_back("-o " + QuotedPath(executableFilePath));
    std::string linkCommandLine;
    std::string errors;
    linkCommandLine = "clang++";
    for (const std::string& arg : args)
    {
        linkCommandLine.append(1, ' ').append(arg);
    }
    try
    {
        Process::Redirections redirections = Process::Redirections::processStdErr;
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            redirections = redirections | Process::Redirections::processStdOut;
        }
        Process process(linkCommandLine, redirections);
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            while (!process.Eof(Process::StdHandle::stdOut))
            {
                std::string line = process.ReadLine(Process::StdHandle::stdOut);
                if (!line.empty())
                {
                    LogMessage(-1, PlatformStringToUtf8(line));
                }
            }
        }
        errors = process.ReadToEnd(Process::StdHandle::stdErr);
        process.WaitForExit();
        int exitCode = process.ExitCode();
        if (exitCode != 0)
        {
            throw std::runtime_error("executing '" + linkCommandLine + "' failed with exit code: " + std::to_string(exitCode));
        }
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("linking executable '" + executableFilePath + "' failed: " + ex.what() + ":\nerrors:\n" + errors);
    }
    if (GetGlobalFlag(GlobalFlags::verbose) && !GetGlobalFlag(GlobalFlags::unitTest))
    {
        LogMessage(module.LogStreamId(), "==> " + executableFilePath);
    }
}

#endif

void Link(Target target, const std::string& executableFilePath, const std::string& libraryFilePath, const std::vector<std::string>& libraryFilePaths, const std::string& mainObjectFilePath, Module& module)
{
    if (GetBackEnd() == cmajor::symbols::BackEnd::llvm)
    {
        LinkLlvm(target, executableFilePath, libraryFilePath, libraryFilePaths, mainObjectFilePath, module);
    }
    else if (GetBackEnd() == cmajor::symbols::BackEnd::cmcpp)
    {
        LinkCpp(target, executableFilePath, libraryFilePath, libraryFilePaths, mainObjectFilePath, module);
    }
#ifdef _WIN32
    else if (GetBackEnd() == cmajor::symbols::BackEnd::cmsx)
    {
        LinkSystemX(executableFilePath, libraryFilePath, libraryFilePaths, mainObjectFilePath, module);
    }
#endif
}

void CleanProject(Project* project)
{
    std::string config = GetConfig();
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(project->LogStreamId(), "Cleaning project '" + ToUtf8(project->Name()) + "' (" + project->FilePath() + ") using " + config + " configuration...");
    }
    boost::filesystem::path mfp = project->ModuleFilePath();
    RemoveModuleFromCache(project->ModuleFilePath());
    mfp.remove_filename();
    boost::filesystem::remove_all(mfp);
    if (project->GetTarget() == Target::program || project->GetTarget() == Target::winguiapp || project->GetTarget() == Target::winapp)
    {
        boost::filesystem::path efp = project->ExecutableFilePath();
        efp.remove_filename();
        boost::filesystem::remove_all(efp);
    }
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(project->LogStreamId(), "Project '" + ToUtf8(project->Name()) + "' cleaned successfully.");
    }
}

void CheckMainFunctionSymbol(Module& module)
{
    FunctionSymbol* userMain = module.GetSymbolTable().MainFunctionSymbol();
    if (!userMain)
    {
        throw Exception("program has no main function", Span(), boost::uuids::nil_uuid());
    }
    if (!userMain->Parameters().empty())
    {
        if (userMain->Parameters().size() != 2)
        {
            throw Exception("main function must either take no parameters or take two parameters", userMain->GetSpan(), userMain->SourceModuleId());
        }
        if (!TypesEqual(userMain->Parameters()[0]->GetType(), module.GetSymbolTable().GetTypeByName(U"int")))
        {
            throw Exception("first parameter of main function must be of int type", userMain->GetSpan(), userMain->SourceModuleId());
        }
        if (!TypesEqual(userMain->Parameters()[1]->GetType(), module.GetSymbolTable().GetTypeByName(U"char")->AddConst(
            userMain->GetSpan(), userMain->SourceModuleId())->AddPointer(userMain->GetSpan(), userMain->SourceModuleId())->AddPointer(userMain->GetSpan(), userMain->SourceModuleId())))
        {
            throw Exception("second parameter of main function must be of 'const char**' type", userMain->GetSpan(), userMain->SourceModuleId());
        }
    }
    if (userMain->ReturnType() && !userMain->ReturnType()->IsVoidType())
    {
        if (!TypesEqual(userMain->ReturnType(), module.GetSymbolTable().GetTypeByName(U"int")))
        {
            throw Exception("main function must either be void function or return an int", userMain->GetSpan(), userMain->SourceModuleId());
        }
    }
}

void CreateJsonRegistrationUnit(std::vector<std::string>& objectFilePaths, Module& module, cmajor::codegen::EmittingContext& emittingContext, AttributeBinder* attributeBinder)
{
    CompileUnitNode jsonRegistrationCompileUnit(Span(), boost::uuids::nil_uuid(), boost::filesystem::path(module.OriginalFilePath()).parent_path().append("__json__.cm").generic_string());
    jsonRegistrationCompileUnit.SetSynthesizedUnit();
    jsonRegistrationCompileUnit.GlobalNs()->AddMember(new NamespaceImportNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"System")));
    jsonRegistrationCompileUnit.GlobalNs()->AddMember(new NamespaceImportNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"System.Json")));
    FunctionNode* jsonRegistrationFunction(new FunctionNode(Span(), boost::uuids::nil_uuid(), Specifiers::public_, new IntNode(Span(), boost::uuids::nil_uuid()), U"RegisterJsonClasses", nullptr));
    jsonRegistrationFunction->SetReturnTypeExpr(new VoidNode(Span(), boost::uuids::nil_uuid()));
    CompoundStatementNode* jsonRegistrationFunctionBody = new CompoundStatementNode(Span(), boost::uuids::nil_uuid());
    const std::unordered_set<std::u32string>& jsonClasses = module.GetSymbolTable().JsonClasses();
    for (const std::u32string& jsonClass : jsonClasses)
    {
        InvokeNode* invokeRegisterJsonClass = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RegisterJsonClass"));
        invokeRegisterJsonClass->AddArgument(new TypeNameNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), jsonClass)));
        invokeRegisterJsonClass->AddArgument(new DotNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), jsonClass), 
            new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"Create")));
        ExpressionStatementNode* registerStatement = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), invokeRegisterJsonClass);
        jsonRegistrationFunctionBody->AddStatement(registerStatement);
    }
    jsonRegistrationFunction->SetBody(jsonRegistrationFunctionBody);
    jsonRegistrationCompileUnit.GlobalNs()->AddMember(jsonRegistrationFunction);
    SymbolCreatorVisitor symbolCreator(module.GetSymbolTable());
    jsonRegistrationCompileUnit.Accept(symbolCreator);
    BoundCompileUnit boundJsonCompileUnit(module, &jsonRegistrationCompileUnit, attributeBinder);
    boundJsonCompileUnit.PushBindingTypes();
    TypeBinder typeBinder(boundJsonCompileUnit);
    jsonRegistrationCompileUnit.Accept(typeBinder);
    boundJsonCompileUnit.PopBindingTypes();
    StatementBinder statementBinder(boundJsonCompileUnit);
    jsonRegistrationCompileUnit.Accept(statementBinder);
    if (boundJsonCompileUnit.HasGotos())
    {
        AnalyzeControlFlow(boundJsonCompileUnit);
    }
    cmajor::codegen::GenerateCode(emittingContext, boundJsonCompileUnit);
    objectFilePaths.push_back(boundJsonCompileUnit.ObjectFilePath());
}

void CreateMainUnitLlvm(std::vector<std::string>& objectFilePaths, Module& module, cmajor::codegen::EmittingContext& emittingContext, AttributeBinder* attributeBinder,
    std::string& mainObjectFilePath)
{
    CompileUnitNode mainCompileUnit(Span(), boost::uuids::nil_uuid(), boost::filesystem::path(module.OriginalFilePath()).parent_path().append("__main__.cm").generic_string());
    mainCompileUnit.SetSynthesizedUnit();
    mainCompileUnit.SetProgramMainUnit();
    mainCompileUnit.GlobalNs()->AddMember(new NamespaceImportNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"System")));
    mainCompileUnit.GlobalNs()->AddMember(MakePolymorphicClassArray(module.GetSymbolTable().PolymorphicClasses(), U"@polymorphicClassArray"));
    mainCompileUnit.GlobalNs()->AddMember(MakeStaticClassArray(module.GetSymbolTable().ClassesHavingStaticConstructor(), U"@staticClassArray"));
    FunctionNode* mainFunction(new FunctionNode(Span(), boost::uuids::nil_uuid(), Specifiers::public_, new IntNode(Span(), boost::uuids::nil_uuid()), U"main", nullptr));
#ifndef _WIN32
    mainFunction->AddParameter(new ParameterNode(Span(), boost::uuids::nil_uuid(), new IntNode(Span(), boost::uuids::nil_uuid()), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"argc")));
    mainFunction->AddParameter(new ParameterNode(Span(), boost::uuids::nil_uuid(), new PointerNode(Span(), boost::uuids::nil_uuid(), new PointerNode(Span(), boost::uuids::nil_uuid(), 
        new CharNode(Span(), boost::uuids::nil_uuid()))), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"argv")));
#endif
    mainFunction->SetProgramMain();
    CompoundStatementNode* mainFunctionBody = new CompoundStatementNode(Span(), boost::uuids::nil_uuid());
    ConstructionStatementNode* constructExitCode = new ConstructionStatementNode(Span(), boost::uuids::nil_uuid(), new IntNode(Span(), boost::uuids::nil_uuid()), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exitCode"));
    mainFunctionBody->AddStatement(constructExitCode);
    ExpressionStatementNode* rtInitCall = nullptr;
    if (GetGlobalFlag(GlobalFlags::profile))
    {
        InvokeNode* invokeRtInit = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RtStartProfiling"));
        invokeRtInit->AddArgument(new DivNode(Span(), boost::uuids::nil_uuid(),
            new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"@polymorphicClassArray"), 
                new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"Length"))),
            new LongLiteralNode(Span(), boost::uuids::nil_uuid(), 4))); // 4 64-bit integers per entry
        invokeRtInit->AddArgument(new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), 
            new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"@polymorphicClassArray"), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"CBegin"))));
        invokeRtInit->AddArgument(new DivNode(Span(), boost::uuids::nil_uuid(),
            new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"@staticClassArray"), 
                new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"Length"))),
            new LongLiteralNode(Span(), boost::uuids::nil_uuid(), 2))); // 2 64-bit integers per entry
        invokeRtInit->AddArgument(new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), 
            new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"@staticClassArray"), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"CBegin"))));
        invokeRtInit->AddArgument(new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"GlobalInitCompileUnits"));
        rtInitCall = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), invokeRtInit);
    }
    else
    {
        InvokeNode* invokeRtInit = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RtInit"));
        invokeRtInit->AddArgument(new DivNode(Span(), boost::uuids::nil_uuid(),
            new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"@polymorphicClassArray"),
                new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"Length"))),
            new LongLiteralNode(Span(), boost::uuids::nil_uuid(), 4))); // 4 64-bit integers per entry
        invokeRtInit->AddArgument(new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), 
            new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"@polymorphicClassArray"), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"CBegin"))));
        invokeRtInit->AddArgument(new DivNode(Span(), boost::uuids::nil_uuid(),
            new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"@staticClassArray"), 
                new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"Length"))),
            new LongLiteralNode(Span(), boost::uuids::nil_uuid(), 2))); // 2 64-bit integers per entry
        invokeRtInit->AddArgument(new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), 
            new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"@staticClassArray"), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"CBegin"))));
        invokeRtInit->AddArgument(new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"GlobalInitCompileUnits"));
        rtInitCall = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), invokeRtInit);
    }
    mainFunctionBody->AddStatement(rtInitCall);
#ifdef _WIN32
    ConstructionStatementNode* argc = new ConstructionStatementNode(Span(), boost::uuids::nil_uuid(), new IntNode(Span(), boost::uuids::nil_uuid()), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"argc"));
    argc->AddArgument(new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RtArgc")));
    mainFunctionBody->AddStatement(argc);
    ConstructionStatementNode* argv = new ConstructionStatementNode(Span(), boost::uuids::nil_uuid(), new ConstNode(Span(), boost::uuids::nil_uuid(), 
        new PointerNode(Span(), boost::uuids::nil_uuid(), new PointerNode(Span(), boost::uuids::nil_uuid(), new CharNode(Span(), boost::uuids::nil_uuid())))), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"argv"));
    argv->AddArgument(new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RtArgv")));
    mainFunctionBody->AddStatement(argv);
#endif
    CompoundStatementNode* tryBlock = new CompoundStatementNode(Span(), boost::uuids::nil_uuid());
    if (!module.GetSymbolTable().JsonClasses().empty())
    {
        ExpressionStatementNode* registerJsonClassesCall = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), 
            new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RegisterJsonClasses")));
        tryBlock->AddStatement(registerJsonClassesCall);
    }
    FunctionSymbol* userMain = module.GetSymbolTable().MainFunctionSymbol();
    InvokeNode* invokeMain = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), userMain->GroupName()));
    if (!userMain->Parameters().empty())
    {
        invokeMain->AddArgument(new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"argc"));
        invokeMain->AddArgument(new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"argv"));
    }
    StatementNode* callMainStatement = nullptr;
    if (!userMain->ReturnType() || userMain->ReturnType()->IsVoidType())
    {
        callMainStatement = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), invokeMain);
    }
    else
    {
        callMainStatement = new AssignmentStatementNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exitCode"), invokeMain);
    }
    InvokeNode* invokeInitialize = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"Initialize"));
    StatementNode* callInitializeStatement = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), invokeInitialize);
    tryBlock->AddStatement(callInitializeStatement);
    tryBlock->AddStatement(callMainStatement);
    TryStatementNode* tryStatement = new TryStatementNode(Span(), boost::uuids::nil_uuid(), tryBlock);
    CompoundStatementNode* catchBlock = new CompoundStatementNode(Span(), boost::uuids::nil_uuid());
    InvokeNode* consoleError = new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"System.Console"), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"Error")));
    DotNode* writeLine = new DotNode(Span(), boost::uuids::nil_uuid(), consoleError, new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"WriteLine"));
    InvokeNode* printEx = new InvokeNode(Span(), boost::uuids::nil_uuid(), writeLine);
    InvokeNode* exToString = new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"ex"), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"ToString")));
    printEx->AddArgument(exToString);
    ExpressionStatementNode* printExStatement = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), printEx);
    catchBlock->AddStatement(printExStatement);
    AssignmentStatementNode* assignExitCodeStatement = new AssignmentStatementNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exitCode"), 
        new IntLiteralNode(Span(), boost::uuids::nil_uuid(), 1));
    catchBlock->AddStatement(assignExitCodeStatement);
    CatchNode* catchAll = new CatchNode(Span(), boost::uuids::nil_uuid(), new ConstNode(Span(), boost::uuids::nil_uuid(), new LValueRefNode(Span(), boost::uuids::nil_uuid(), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"System.Exception"))), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"ex"), catchBlock);
    tryStatement->AddCatch(catchAll);
    mainFunctionBody->AddStatement(tryStatement);
    ExpressionStatementNode* rtDoneCall = nullptr;
    if (GetGlobalFlag(GlobalFlags::profile))
    {
        rtDoneCall = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), new InvokeNode(Span(), boost::uuids::nil_uuid(), 
            new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RtEndProfiling")));
    }
    else
    {
        rtDoneCall = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RtDone")));
    }
    mainFunctionBody->AddStatement(rtDoneCall);
    InvokeNode* exitCall = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RtExit"));
    exitCall->AddArgument(new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exitCode"));
    ExpressionStatementNode* rtExitCall = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), exitCall);
    mainFunctionBody->AddStatement(rtExitCall);
    ReturnStatementNode* returnStatement = new ReturnStatementNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exitCode"));
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
    cmajor::codegen::GenerateCode(emittingContext, boundMainCompileUnit);
    mainObjectFilePath = boundMainCompileUnit.ObjectFilePath();
}

void CreateMainUnitCpp(std::vector<std::string>& objectFilePaths, Module& module, cmajor::codegen::EmittingContext& emittingContext, AttributeBinder* attributeBinder,
    std::string& mainObjectFilePath, std::string& mainSourceFilePath)
{
    bool vsToolChain = GetToolChain() == "vs";
    CompileUnitNode mainCompileUnit(Span(), boost::uuids::nil_uuid(), boost::filesystem::path(module.OriginalFilePath()).parent_path().append("__main__.cm").generic_string());
    mainCompileUnit.SetSynthesizedUnit();
    mainCompileUnit.SetProgramMainUnit();
    mainCompileUnit.GlobalNs()->AddMember(new NamespaceImportNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"System")));
    mainCompileUnit.GlobalNs()->AddMember(MakePolymorphicClassArray(module.GetSymbolTable().PolymorphicClasses(), U"__polymorphicClassArray"));
    mainCompileUnit.GlobalNs()->AddMember(MakeStaticClassArray(module.GetSymbolTable().ClassesHavingStaticConstructor(), U"__staticClassArray"));
    std::string platform = GetPlatform();
    FunctionNode* mainFunction = nullptr;
    if (platform == "windows")
    {
        mainFunction = new FunctionNode(Span(), boost::uuids::nil_uuid(), Specifiers::public_, new IntNode(Span(), boost::uuids::nil_uuid()), U"wmain", nullptr);
    }
    else
    {
        mainFunction = new FunctionNode(Span(), boost::uuids::nil_uuid(), Specifiers::public_, new IntNode(Span(), boost::uuids::nil_uuid()), U"main", nullptr);
    }
    if (platform != "windows")
    {
        mainFunction->AddParameter(new ParameterNode(Span(), boost::uuids::nil_uuid(), new IntNode(Span(), boost::uuids::nil_uuid()), 
            new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"argc")));
        mainFunction->AddParameter(new ParameterNode(Span(), boost::uuids::nil_uuid(), new PointerNode(Span(), boost::uuids::nil_uuid(), 
            new PointerNode(Span(), boost::uuids::nil_uuid(), new CharNode(Span(), boost::uuids::nil_uuid()))), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"argv")));
    }
/*
    if (!vsToolChain)
    {
        mainFunction->AddParameter(new ParameterNode(Span(), new IntNode(Span()), new IdentifierNode(Span(), U"argc")));
        mainFunction->AddParameter(new ParameterNode(Span(), new PointerNode(Span(), new PointerNode(Span(), new CharNode(Span()))), new IdentifierNode(Span(), U"argv")));
    }
*/
    mainFunction->SetProgramMain();
    CompoundStatementNode* mainFunctionBody = new CompoundStatementNode(Span(), boost::uuids::nil_uuid());
    ConstructionStatementNode* constructExitCode = new ConstructionStatementNode(Span(), boost::uuids::nil_uuid(), new IntNode(Span(), boost::uuids::nil_uuid()), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exitCode"));
    mainFunctionBody->AddStatement(constructExitCode);
    ExpressionStatementNode* rtInitCall = nullptr;
    if (GetGlobalFlag(GlobalFlags::profile))
    {
        InvokeNode* invokeRtInit = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RtStartProfiling"));
        invokeRtInit->AddArgument(new DivNode(Span(), boost::uuids::nil_uuid(),
            new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"__polymorphicClassArray"), 
                new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"Length"))),
            new LongLiteralNode(Span(), boost::uuids::nil_uuid(), 4))); // 4 64-bit integers per entry
        invokeRtInit->AddArgument(new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), 
            new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"__polymorphicClassArray"), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"CBegin"))));
        invokeRtInit->AddArgument(new DivNode(Span(), boost::uuids::nil_uuid(),
            new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"__staticClassArray"), 
                new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"Length"))),
            new LongLiteralNode(Span(), boost::uuids::nil_uuid(), 2))); // 2 64-bit integers per entry
        invokeRtInit->AddArgument(new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), 
            new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"__staticClassArray"), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"CBegin"))));
        invokeRtInit->AddArgument(new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"GlobalInitCompileUnits"));
        rtInitCall = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), invokeRtInit);
    }
    else
    {
        InvokeNode* invokeRtInit = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RtInit"));
        invokeRtInit->AddArgument(new DivNode(Span(), boost::uuids::nil_uuid(),
            new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), 
                new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"__polymorphicClassArray"), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"Length"))),
            new LongLiteralNode(Span(), boost::uuids::nil_uuid(), 4))); // 4 64-bit integers per entry
        invokeRtInit->AddArgument(new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), 
            new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"__polymorphicClassArray"), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"CBegin"))));
        invokeRtInit->AddArgument(new DivNode(Span(), boost::uuids::nil_uuid(),
            new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"__staticClassArray"), 
                new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"Length"))),
            new LongLiteralNode(Span(), boost::uuids::nil_uuid(), 2))); // 2 64-bit integers per entry
        invokeRtInit->AddArgument(new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), 
            new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"__staticClassArray"), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"CBegin"))));
        invokeRtInit->AddArgument(new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"GlobalInitCompileUnits"));
        rtInitCall = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), invokeRtInit);
    }
    mainFunctionBody->AddStatement(rtInitCall);

    if (platform == "windows")
    {
        ConstructionStatementNode* argc = new ConstructionStatementNode(Span(), boost::uuids::nil_uuid(), new IntNode(Span(), boost::uuids::nil_uuid()), 
            new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"argc"));
        argc->AddArgument(new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RtArgc")));
        mainFunctionBody->AddStatement(argc);
        ConstructionStatementNode* argv = new ConstructionStatementNode(Span(), boost::uuids::nil_uuid(), new ConstNode(Span(), boost::uuids::nil_uuid(), 
            new PointerNode(Span(), boost::uuids::nil_uuid(), new PointerNode(Span(), boost::uuids::nil_uuid(), new CharNode(Span(), boost::uuids::nil_uuid())))), 
            new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"argv"));
        argv->AddArgument(new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RtArgv")));
        mainFunctionBody->AddStatement(argv);
    }

    /*
    if (vsToolChain)
    {
        ConstructionStatementNode* argc = new ConstructionStatementNode(Span(), new IntNode(Span()), new IdentifierNode(Span(), U"argc"));
        argc->AddArgument(new InvokeNode(Span(), new IdentifierNode(Span(), U"RtArgc")));
        mainFunctionBody->AddStatement(argc);
        ConstructionStatementNode* argv = new ConstructionStatementNode(Span(), new ConstNode(Span(), new PointerNode(Span(), new PointerNode(Span(), new CharNode(Span())))), new IdentifierNode(Span(), U"argv"));
        argv->AddArgument(new InvokeNode(Span(), new IdentifierNode(Span(), U"RtArgv")));
        mainFunctionBody->AddStatement(argv);
    }
*/
    CompoundStatementNode* tryBlock = new CompoundStatementNode(Span(), boost::uuids::nil_uuid());
    if (!module.GetSymbolTable().JsonClasses().empty())
    {
        ExpressionStatementNode* registerJsonClassesCall = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), 
            new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RegisterJsonClasses")));
        tryBlock->AddStatement(registerJsonClassesCall);
    }
    FunctionSymbol* userMain = module.GetSymbolTable().MainFunctionSymbol();
    InvokeNode* invokeMain = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), userMain->GroupName()));
    if (!userMain->Parameters().empty())
    {
        invokeMain->AddArgument(new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"argc"));
        invokeMain->AddArgument(new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"argv"));
    }
    StatementNode* callMainStatement = nullptr;
    if (!userMain->ReturnType() || userMain->ReturnType()->IsVoidType())
    {
        callMainStatement = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), invokeMain);
    }
    else
    {
        callMainStatement = new AssignmentStatementNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exitCode"), invokeMain);
    }
    InvokeNode* invokeInitialize = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"Initialize"));
    StatementNode* callInitializeStatement = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), invokeInitialize);
    tryBlock->AddStatement(callInitializeStatement);
    tryBlock->AddStatement(callMainStatement);
    TryStatementNode* tryStatement = new TryStatementNode(Span(), boost::uuids::nil_uuid(), tryBlock);
    CompoundStatementNode* catchBlock = new CompoundStatementNode(Span(), boost::uuids::nil_uuid());
    InvokeNode* consoleError = new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"System.Console"), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"Error")));
    DotNode* writeLine = new DotNode(Span(), boost::uuids::nil_uuid(), consoleError, new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"WriteLine"));
    InvokeNode* printEx = new InvokeNode(Span(), boost::uuids::nil_uuid(), writeLine);
    InvokeNode* exToString = new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"ex"), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"ToString")));
    printEx->AddArgument(exToString);
    ExpressionStatementNode* printExStatement = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), printEx);
    catchBlock->AddStatement(printExStatement);
    AssignmentStatementNode* assignExitCodeStatement = new AssignmentStatementNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exitCode"), 
        new IntLiteralNode(Span(), boost::uuids::nil_uuid(), 1));
    catchBlock->AddStatement(assignExitCodeStatement);
    CatchNode* catchAll = new CatchNode(Span(), boost::uuids::nil_uuid(), new ConstNode(Span(), boost::uuids::nil_uuid(), new LValueRefNode(Span(), boost::uuids::nil_uuid(), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"System.Exception"))), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"ex"), catchBlock);
    tryStatement->AddCatch(catchAll);
    mainFunctionBody->AddStatement(tryStatement);
    ExpressionStatementNode* rtDoneCall = nullptr;
    if (GetGlobalFlag(GlobalFlags::profile))
    {
        rtDoneCall = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), new InvokeNode(Span(), boost::uuids::nil_uuid(), 
            new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RtEndProfiling")));
    }
    else
    {
        rtDoneCall = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RtDone")));
    }
    mainFunctionBody->AddStatement(rtDoneCall);
    InvokeNode* exitCall = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RtExit"));
    exitCall->AddArgument(new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exitCode"));
    ExpressionStatementNode* rtExitCall = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), exitCall);
    mainFunctionBody->AddStatement(rtExitCall);
    ReturnStatementNode* returnStatement = new ReturnStatementNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exitCode"));
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
    cmajor::codegen::GenerateCode(emittingContext, boundMainCompileUnit);
    mainObjectFilePath = boundMainCompileUnit.ObjectFilePath();
    mainSourceFilePath = Path::ChangeExtension(boundMainCompileUnit.LLFilePath(), ".cpp");
}

void CreateMainUnitSystemX(std::vector<std::string>& objectFilePaths, Module& module, cmajor::codegen::EmittingContext& emittingContext, AttributeBinder* attributeBinder,
    std::string& mainObjectFilePath)
{
    CompileUnitNode mainCompileUnit(Span(), boost::uuids::nil_uuid(), boost::filesystem::path(module.OriginalFilePath()).parent_path().append("__main__.cm").generic_string());
    mainCompileUnit.SetSynthesizedUnit();
    mainCompileUnit.SetProgramMainUnit();
    mainCompileUnit.GlobalNs()->AddMember(new NamespaceImportNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"System")));
    FunctionNode* mainFunction(new FunctionNode(Span(), boost::uuids::nil_uuid(), Specifiers::public_, new IntNode(Span(), boost::uuids::nil_uuid()), U"main", nullptr));
    mainFunction->AddParameter(new ParameterNode(Span(), boost::uuids::nil_uuid(), new IntNode(Span(), boost::uuids::nil_uuid()), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"argc")));
    mainFunction->AddParameter(new ParameterNode(Span(), boost::uuids::nil_uuid(), new PointerNode(Span(), boost::uuids::nil_uuid(), new PointerNode(Span(), boost::uuids::nil_uuid(), 
        new CharNode(Span(), boost::uuids::nil_uuid()))), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"argv")));
    mainFunction->AddParameter(new ParameterNode(Span(), boost::uuids::nil_uuid(), new PointerNode(Span(), boost::uuids::nil_uuid(), new PointerNode(Span(), boost::uuids::nil_uuid(), 
        new CharNode(Span(), boost::uuids::nil_uuid()))), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"envp")));
    mainFunction->SetProgramMain();
    CompoundStatementNode* mainFunctionBody = new CompoundStatementNode(Span(), boost::uuids::nil_uuid());
    ConstructionStatementNode* constructExitCode = new ConstructionStatementNode(Span(), boost::uuids::nil_uuid(), new IntNode(Span(), boost::uuids::nil_uuid()), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exitCode"));
    mainFunctionBody->AddStatement(constructExitCode);
    CompoundStatementNode* tryBlock = new CompoundStatementNode(Span(), boost::uuids::nil_uuid());
    InvokeNode* invokeSetupEnvironment = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"StartupSetupEnvironment"));
    invokeSetupEnvironment->AddArgument(new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"envp"));
    StatementNode* callSetEnvironmentStatement = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), invokeSetupEnvironment);
    tryBlock->AddStatement(callSetEnvironmentStatement);
    FunctionSymbol* userMain = module.GetSymbolTable().MainFunctionSymbol();
    InvokeNode* invokeMain = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), userMain->GroupName()));
    if (!userMain->Parameters().empty())
    {
        invokeMain->AddArgument(new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"argc"));
        invokeMain->AddArgument(new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"argv"));
    }
    StatementNode* callMainStatement = nullptr;
    if (!userMain->ReturnType() || userMain->ReturnType()->IsVoidType())
    {
        callMainStatement = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), invokeMain);
    }
    else
    {
        callMainStatement = new AssignmentStatementNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exitCode"), invokeMain);
    }
    tryBlock->AddStatement(callMainStatement);
    TryStatementNode* tryStatement = new TryStatementNode(Span(), boost::uuids::nil_uuid(), tryBlock);
    CompoundStatementNode* catchBlock = new CompoundStatementNode(Span(), boost::uuids::nil_uuid());
    InvokeNode* consoleError = new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"System.Console"), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"Error")));
    DotNode* writeLine = new DotNode(Span(), boost::uuids::nil_uuid(), consoleError, new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"WriteLine"));
    InvokeNode* printEx = new InvokeNode(Span(), boost::uuids::nil_uuid(), writeLine);
    InvokeNode* exToString = new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"ex"), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"ToString")));
    printEx->AddArgument(exToString);
    ExpressionStatementNode* printExStatement = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), printEx);
    catchBlock->AddStatement(printExStatement);
    AssignmentStatementNode* assignExitCodeStatement = new AssignmentStatementNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exitCode"), 
        new IntLiteralNode(Span(), boost::uuids::nil_uuid(), 1));
    catchBlock->AddStatement(assignExitCodeStatement);
    CatchNode* catchAll = new CatchNode(Span(), boost::uuids::nil_uuid(), new ConstNode(Span(), boost::uuids::nil_uuid(), new LValueRefNode(Span(), boost::uuids::nil_uuid(), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"System.Exception"))), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"ex"), catchBlock);
    tryStatement->AddCatch(catchAll);
    mainFunctionBody->AddStatement(tryStatement);
    ReturnStatementNode* returnStatement = new ReturnStatementNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exitCode"));
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
    cmajor::codegen::GenerateCode(emittingContext, boundMainCompileUnit);
    mainObjectFilePath = boundMainCompileUnit.ObjectFilePath();
}

void CreateMainUnitLlvmWindowsGUI(std::vector<std::string>& objectFilePaths, Module& module, cmajor::codegen::EmittingContext& emittingContext, AttributeBinder* attributeBinder,
    std::string& mainObjectFilePath)
{
    CompileUnitNode mainCompileUnit(Span(), boost::uuids::nil_uuid(), boost::filesystem::path(module.OriginalFilePath()).parent_path().append("__main__.cm").generic_string());
    mainCompileUnit.SetSynthesizedUnit();
    mainCompileUnit.SetProgramMainUnit();
    mainCompileUnit.GlobalNs()->AddMember(new NamespaceImportNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"System")));
    mainCompileUnit.GlobalNs()->AddMember(MakePolymorphicClassArray(module.GetSymbolTable().PolymorphicClasses(), U"@polymorphicClassArray"));
    mainCompileUnit.GlobalNs()->AddMember(MakeStaticClassArray(module.GetSymbolTable().ClassesHavingStaticConstructor(), U"@staticClassArray"));
    FunctionNode* mainFunction(new FunctionNode(Span(), boost::uuids::nil_uuid(), Specifiers::public_ | Specifiers::winapi, new IntNode(Span(), boost::uuids::nil_uuid()), U"WinMain", nullptr));
    mainFunction->AddParameter(new ParameterNode(Span(), boost::uuids::nil_uuid(), new PointerNode(Span(), boost::uuids::nil_uuid(), new VoidNode(Span(), boost::uuids::nil_uuid())), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"instance")));
    mainFunction->AddParameter(new ParameterNode(Span(), boost::uuids::nil_uuid(), new PointerNode(Span(), boost::uuids::nil_uuid(), new VoidNode(Span(), boost::uuids::nil_uuid())), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"prevInstance")));
    mainFunction->AddParameter(new ParameterNode(Span(), boost::uuids::nil_uuid(), new PointerNode(Span(), boost::uuids::nil_uuid(), new CharNode(Span(), boost::uuids::nil_uuid())), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"commandLine")));
    mainFunction->AddParameter(new ParameterNode(Span(), boost::uuids::nil_uuid(), new IntNode(Span(), boost::uuids::nil_uuid()), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"cmdShow")));
    mainFunction->SetProgramMain();
    CompoundStatementNode* mainFunctionBody = new CompoundStatementNode(Span(), boost::uuids::nil_uuid());
    ConstructionStatementNode* constructExitCode = new ConstructionStatementNode(Span(), boost::uuids::nil_uuid(), new IntNode(Span(), boost::uuids::nil_uuid()), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exitCode"));
    mainFunctionBody->AddStatement(constructExitCode);
    ExpressionStatementNode* rtInitCall = nullptr;
    InvokeNode* invokeRtInit = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RtInit"));
    invokeRtInit->AddArgument(new DivNode(Span(), boost::uuids::nil_uuid(),
        new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"@polymorphicClassArray"), 
            new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"Length"))),
        new LongLiteralNode(Span(), boost::uuids::nil_uuid(), 4))); // 4 64-bit integers per entry
    invokeRtInit->AddArgument(new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"@polymorphicClassArray"), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"CBegin"))));
    invokeRtInit->AddArgument(new DivNode(Span(), boost::uuids::nil_uuid(),
        new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"@staticClassArray"), 
            new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"Length"))),
        new LongLiteralNode(Span(), boost::uuids::nil_uuid(), 2))); // 2 64-bit integers per entry
    invokeRtInit->AddArgument(new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"@staticClassArray"), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"CBegin"))));
    invokeRtInit->AddArgument(new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"GlobalInitCompileUnits"));
    rtInitCall = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), invokeRtInit);
    mainFunctionBody->AddStatement(rtInitCall);
    ConstructionStatementNode* argc = new ConstructionStatementNode(Span(), boost::uuids::nil_uuid(), new IntNode(Span(), boost::uuids::nil_uuid()), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"argc"));
    argc->AddArgument(new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RtArgc")));
    mainFunctionBody->AddStatement(argc);
    ConstructionStatementNode* argv = new ConstructionStatementNode(Span(), boost::uuids::nil_uuid(), new ConstNode(Span(), boost::uuids::nil_uuid(), new PointerNode(Span(), boost::uuids::nil_uuid(), 
        new PointerNode(Span(), boost::uuids::nil_uuid(), new CharNode(Span(), boost::uuids::nil_uuid())))), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"argv"));
    argv->AddArgument(new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RtArgv")));
    mainFunctionBody->AddStatement(argv);
    if (!module.GetSymbolTable().JsonClasses().empty())
    {
        ExpressionStatementNode* registerJsonClassesCall = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), new InvokeNode(Span(), boost::uuids::nil_uuid(), 
            new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RegisterJsonClasses")));
        mainFunctionBody->AddStatement(registerJsonClassesCall);
    }
    CompoundStatementNode* tryBlock = new CompoundStatementNode(Span(), boost::uuids::nil_uuid());
    if (!module.GetSymbolTable().JsonClasses().empty())
    {
        ExpressionStatementNode* registerJsonClassesCall = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), new InvokeNode(Span(), boost::uuids::nil_uuid(), 
            new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RegisterJsonClasses")));
        tryBlock->AddStatement(registerJsonClassesCall);
    }
    FunctionSymbol* userMain = module.GetSymbolTable().MainFunctionSymbol();
    InvokeNode* invokeMain = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), userMain->GroupName()));
    if (!userMain->Parameters().empty())
    {
        invokeMain->AddArgument(new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"argc"));
        invokeMain->AddArgument(new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"argv"));
    }
    StatementNode* callMainStatement = nullptr;
    if (!userMain->ReturnType() || userMain->ReturnType()->IsVoidType())
    {
        callMainStatement = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), invokeMain);
    }
    else
    {
        callMainStatement = new AssignmentStatementNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exitCode"), invokeMain);
    }
    InvokeNode* invokeInitialize = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"Initialize"));
    StatementNode* callInitializeStatement = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), invokeInitialize);
    tryBlock->AddStatement(callInitializeStatement);
    InvokeNode* invokeSetInstance = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"WinSetInstance"));
    ExpressionStatementNode* setInstanceStatement = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), invokeSetInstance);
    tryBlock->AddStatement(setInstanceStatement);
    tryBlock->AddStatement(callMainStatement);
    TryStatementNode* tryStatement = new TryStatementNode(Span(), boost::uuids::nil_uuid(), tryBlock);
    CompoundStatementNode* catchBlock = new CompoundStatementNode(Span(), boost::uuids::nil_uuid());
    CatchNode* catchAll = new CatchNode(Span(), boost::uuids::nil_uuid(), new ConstNode(Span(), boost::uuids::nil_uuid(), new LValueRefNode(Span(), boost::uuids::nil_uuid(), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"System.Exception"))), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"ex"), catchBlock);
    tryStatement->AddCatch(catchAll);
    InvokeNode* invokeWinShowMessageBox = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"WinShowMessageBoxWithType"));
    InvokeNode* exToString = new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"ex"), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"ToString")));
    ConstructionStatementNode* constructExStr = new ConstructionStatementNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"string"), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exStr"));
    constructExStr->AddArgument(exToString);
    catchBlock->AddStatement(constructExStr);
    ConstructionStatementNode* constructExCharPtr = new ConstructionStatementNode(Span(), boost::uuids::nil_uuid(), new PointerNode(Span(), boost::uuids::nil_uuid(), 
        new CharNode(Span(), boost::uuids::nil_uuid())), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exCharPtr"));
    catchBlock->AddStatement(constructExCharPtr);
    InvokeNode* invokeExChars = new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exStr"),  
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"Chars")));
    constructExCharPtr->AddArgument(invokeExChars);
    invokeWinShowMessageBox->AddArgument(new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exCharPtr"));
    invokeWinShowMessageBox->AddArgument(new NullLiteralNode(Span(), boost::uuids::nil_uuid()));
    invokeWinShowMessageBox->AddArgument(new NullLiteralNode(Span(), boost::uuids::nil_uuid()));
    invokeWinShowMessageBox->AddArgument(new UIntLiteralNode(Span(), boost::uuids::nil_uuid(), 0x00000010 | 0x00000000)); // MB_ICONSTOP | MB_OK
    ExpressionStatementNode* showMessageBoxStatement = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), invokeWinShowMessageBox);
    catchBlock->AddStatement(showMessageBoxStatement);
    AssignmentStatementNode* assignExitCodeStatement = new AssignmentStatementNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exitCode"), 
        new IntLiteralNode(Span(), boost::uuids::nil_uuid(), 1));
    catchBlock->AddStatement(assignExitCodeStatement);
    mainFunctionBody->AddStatement(tryStatement);
    ExpressionStatementNode* winDoneCall = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), new InvokeNode(Span(), boost::uuids::nil_uuid(), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"WinDone")));
    mainFunctionBody->AddStatement(winDoneCall);
    ExpressionStatementNode* rtDoneCall = nullptr;
    rtDoneCall = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RtDone")));
    mainFunctionBody->AddStatement(rtDoneCall);
    InvokeNode* exitCall = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RtExit"));
    exitCall->AddArgument(new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exitCode"));
    ExpressionStatementNode* rtExitCall = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), exitCall);
    mainFunctionBody->AddStatement(rtExitCall);
    ReturnStatementNode* returnStatement = new ReturnStatementNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exitCode"));
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
    cmajor::codegen::GenerateCode(emittingContext, boundMainCompileUnit);
    mainObjectFilePath = boundMainCompileUnit.ObjectFilePath();
}

void CreateMainUnitCppWindowsGUI(std::vector<std::string>& objectFilePaths, Module& module, cmajor::codegen::EmittingContext& emittingContext, AttributeBinder* attributeBinder,
    std::string& mainObjectFilePath, std::string& mainSourceFilePath)
{
    CompileUnitNode mainCompileUnit(Span(), boost::uuids::nil_uuid(), boost::filesystem::path(module.OriginalFilePath()).parent_path().append("__main__.cm").generic_string());
    mainCompileUnit.SetSynthesizedUnit();
    mainCompileUnit.SetProgramMainUnit();
    mainCompileUnit.GlobalNs()->AddMember(new NamespaceImportNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"System")));
    mainCompileUnit.GlobalNs()->AddMember(MakePolymorphicClassArray(module.GetSymbolTable().PolymorphicClasses(), U"__polymorphicClassArray"));
    mainCompileUnit.GlobalNs()->AddMember(MakeStaticClassArray(module.GetSymbolTable().ClassesHavingStaticConstructor(), U"__staticClassArray"));
    FunctionNode* mainFunction(new FunctionNode(Span(), boost::uuids::nil_uuid(), Specifiers::public_ | Specifiers::winapi, new IntNode(Span(), boost::uuids::nil_uuid()), U"wWinMain", nullptr));
    mainFunction->AddParameter(new ParameterNode(Span(), boost::uuids::nil_uuid(), new PointerNode(Span(), boost::uuids::nil_uuid(), new VoidNode(Span(), boost::uuids::nil_uuid())), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"instance")));
    mainFunction->AddParameter(new ParameterNode(Span(), boost::uuids::nil_uuid(), new PointerNode(Span(), boost::uuids::nil_uuid(), new VoidNode(Span(), boost::uuids::nil_uuid())), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"prevInstance")));
    mainFunction->AddParameter(new ParameterNode(Span(), boost::uuids::nil_uuid(), new PointerNode(Span(), boost::uuids::nil_uuid(), new CharNode(Span(), boost::uuids::nil_uuid())), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"commandLine")));
    mainFunction->AddParameter(new ParameterNode(Span(), boost::uuids::nil_uuid(), new IntNode(Span(), boost::uuids::nil_uuid()), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"cmdShow")));
    mainFunction->SetProgramMain();
    CompoundStatementNode* mainFunctionBody = new CompoundStatementNode(Span(), boost::uuids::nil_uuid());
    ConstructionStatementNode* constructExitCode = new ConstructionStatementNode(Span(), boost::uuids::nil_uuid(), new IntNode(Span(), boost::uuids::nil_uuid()), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exitCode"));
    mainFunctionBody->AddStatement(constructExitCode);
    ExpressionStatementNode* rtInitCall = nullptr;
    InvokeNode* invokeRtInit = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RtInit"));
    invokeRtInit->AddArgument(new DivNode(Span(), boost::uuids::nil_uuid(),
        new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"__polymorphicClassArray"), 
            new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"Length"))),
        new LongLiteralNode(Span(), boost::uuids::nil_uuid(), 4))); // 4 64-bit integers per entry
    invokeRtInit->AddArgument(new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), 
        U"__polymorphicClassArray"), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"CBegin"))));
    invokeRtInit->AddArgument(new DivNode(Span(), boost::uuids::nil_uuid(),
        new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"__staticClassArray"), 
            new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"Length"))),
        new LongLiteralNode(Span(), boost::uuids::nil_uuid(), 2))); // 2 64-bit integers per entry
    invokeRtInit->AddArgument(new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"__staticClassArray"), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"CBegin"))));
    invokeRtInit->AddArgument(new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"GlobalInitCompileUnits"));
    rtInitCall = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), invokeRtInit);
    mainFunctionBody->AddStatement(rtInitCall);
    ConstructionStatementNode* argc = new ConstructionStatementNode(Span(), boost::uuids::nil_uuid(), new IntNode(Span(), boost::uuids::nil_uuid()), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"argc"));
    argc->AddArgument(new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RtArgc")));
    mainFunctionBody->AddStatement(argc);
    ConstructionStatementNode* argv = new ConstructionStatementNode(Span(), boost::uuids::nil_uuid(), new ConstNode(Span(), boost::uuids::nil_uuid(), new PointerNode(Span(), boost::uuids::nil_uuid(), 
        new PointerNode(Span(), boost::uuids::nil_uuid(), new CharNode(Span(), boost::uuids::nil_uuid())))), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"argv"));
    argv->AddArgument(new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RtArgv")));
    mainFunctionBody->AddStatement(argv);
    if (!module.GetSymbolTable().JsonClasses().empty())
    {
        ExpressionStatementNode* registerJsonClassesCall = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), new InvokeNode(Span(), boost::uuids::nil_uuid(), 
            new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RegisterJsonClasses")));
        mainFunctionBody->AddStatement(registerJsonClassesCall);
    }
    CompoundStatementNode* tryBlock = new CompoundStatementNode(Span(), boost::uuids::nil_uuid());
    if (!module.GetSymbolTable().JsonClasses().empty())
    {
        ExpressionStatementNode* registerJsonClassesCall = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), new InvokeNode(Span(), boost::uuids::nil_uuid(), 
            new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RegisterJsonClasses")));
        tryBlock->AddStatement(registerJsonClassesCall);
    }
    FunctionSymbol* userMain = module.GetSymbolTable().MainFunctionSymbol();
    InvokeNode* invokeMain = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), userMain->GroupName()));
    if (!userMain->Parameters().empty())
    {
        invokeMain->AddArgument(new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"argc"));
        invokeMain->AddArgument(new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"argv"));
    }
    StatementNode* callMainStatement = nullptr;
    if (!userMain->ReturnType() || userMain->ReturnType()->IsVoidType())
    {
        callMainStatement = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), invokeMain);
    }
    else
    {
        callMainStatement = new AssignmentStatementNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exitCode"), invokeMain);
    }
    InvokeNode* invokeInitialize = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"Initialize"));
    StatementNode* callInitializeStatement = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), invokeInitialize);
    tryBlock->AddStatement(callInitializeStatement);
    InvokeNode* invokeSetInstance = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"WinSetInstance"));
    ExpressionStatementNode* setInstanceStatement = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), invokeSetInstance);
    tryBlock->AddStatement(setInstanceStatement);
    tryBlock->AddStatement(callMainStatement);
    TryStatementNode* tryStatement = new TryStatementNode(Span(), boost::uuids::nil_uuid(), tryBlock);
    CompoundStatementNode* catchBlock = new CompoundStatementNode(Span(), boost::uuids::nil_uuid());
    CatchNode* catchAll = new CatchNode(Span(), boost::uuids::nil_uuid(), new ConstNode(Span(), boost::uuids::nil_uuid(), new LValueRefNode(Span(), boost::uuids::nil_uuid(), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"System.Exception"))), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"ex"), catchBlock);
    tryStatement->AddCatch(catchAll);
    InvokeNode* invokeWinShowMessageBox = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"WinShowMessageBoxWithType"));
    InvokeNode* exToString = new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"ex"), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"ToString")));
    ConstructionStatementNode* constructExStr = new ConstructionStatementNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"string"), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exStr"));
    constructExStr->AddArgument(exToString);
    catchBlock->AddStatement(constructExStr);
    ConstructionStatementNode* constructExCharPtr = new ConstructionStatementNode(Span(), boost::uuids::nil_uuid(), new PointerNode(Span(), boost::uuids::nil_uuid(), 
        new CharNode(Span(), boost::uuids::nil_uuid())), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exCharPtr"));
    catchBlock->AddStatement(constructExCharPtr);
    InvokeNode* invokeExChars = new InvokeNode(Span(), boost::uuids::nil_uuid(), new DotNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exStr"), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"Chars")));
    constructExCharPtr->AddArgument(invokeExChars);
    invokeWinShowMessageBox->AddArgument(new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exCharPtr"));
    invokeWinShowMessageBox->AddArgument(new NullLiteralNode(Span(), boost::uuids::nil_uuid()));
    invokeWinShowMessageBox->AddArgument(new NullLiteralNode(Span(), boost::uuids::nil_uuid()));
    invokeWinShowMessageBox->AddArgument(new UIntLiteralNode(Span(), boost::uuids::nil_uuid(), 0x00000010 | 0x00000000)); // MB_ICONSTOP | MB_OK
    ExpressionStatementNode* showMessageBoxStatement = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), invokeWinShowMessageBox);
    catchBlock->AddStatement(showMessageBoxStatement);
    AssignmentStatementNode* assignExitCodeStatement = new AssignmentStatementNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exitCode"), 
        new IntLiteralNode(Span(), boost::uuids::nil_uuid(), 1));
    catchBlock->AddStatement(assignExitCodeStatement);
    mainFunctionBody->AddStatement(tryStatement);
    ExpressionStatementNode* winDoneCall = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), new InvokeNode(Span(), boost::uuids::nil_uuid(), 
        new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"WinDone")));
    mainFunctionBody->AddStatement(winDoneCall);
    ExpressionStatementNode* rtDoneCall = nullptr;
    rtDoneCall = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RtDone")));
    mainFunctionBody->AddStatement(rtDoneCall);
    InvokeNode* exitCall = new InvokeNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"RtExit"));
    exitCall->AddArgument(new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exitCode"));
    ExpressionStatementNode* rtExitCall = new ExpressionStatementNode(Span(), boost::uuids::nil_uuid(), exitCall);
    mainFunctionBody->AddStatement(rtExitCall);
    ReturnStatementNode* returnStatement = new ReturnStatementNode(Span(), boost::uuids::nil_uuid(), new IdentifierNode(Span(), boost::uuids::nil_uuid(), U"exitCode"));
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
    cmajor::codegen::GenerateCode(emittingContext, boundMainCompileUnit);
    mainObjectFilePath = boundMainCompileUnit.ObjectFilePath();
    mainSourceFilePath = Path::ChangeExtension(boundMainCompileUnit.LLFilePath(), ".cpp");
}

void SetDefines(Module* module, const std::string& definesFilePath)
{
    module->ClearDefines();
    if (GetConfig() == "debug")
    {
        module->DefineSymbol(U"DEBUG");
    }
    else if (GetConfig() == "release")
    {
        module->DefineSymbol(U"RELEASE");
    }
    else if (GetConfig() == "profile")
    {
        module->DefineSymbol(U"RELEASE");
        module->DefineSymbol(U"PROFILE");
    }
#ifdef _WIN32
    module->DefineSymbol(U"WINDOWS");
#else
    module->DefineSymbol(U"LINUX");
#endif
    std::ifstream definesFile(definesFilePath);
    if (definesFile)
    {
        std::string line;
        while (std::getline(definesFile, line))
        {
            module->DefineSymbol(ToUtf32(line));
        }
    }
}

void InstallSystemLibraries(Module* systemInstallModule)
{
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(systemInstallModule->LogStreamId(), "Installing system libraries...");
    }
    sngcm::ast::BackEnd backend = sngcm::ast::BackEnd::llvm;
    if (GetBackEnd() == cmajor::symbols::BackEnd::cmsx)
    {
        backend = sngcm::ast::BackEnd::cmsx;
    }
    else if (GetBackEnd() == cmajor::symbols::BackEnd::cmcpp)
    {
        backend = sngcm::ast::BackEnd::cppcm;
    }
    SystemDirKind systemDirKind = SystemDirKind::regular;
    if (GetGlobalFlag(GlobalFlags::repository))
    {
        systemDirKind = SystemDirKind::repository;
    }
    boost::filesystem::path systemLibDir = CmajorSystemLibDir(GetConfig(), backend, GetToolChain(), systemDirKind);
    boost::filesystem::create_directories(systemLibDir);
    for (Module* systemModule : systemInstallModule->AllReferencedModules())
    {
        boost::filesystem::path from = systemModule->OriginalFilePath();
        boost::filesystem::path to = systemLibDir / from.filename();
        if (boost::filesystem::exists(to))
        {
            boost::filesystem::remove(to);
        }
        boost::filesystem::copy(from, to);
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            LogMessage(systemInstallModule->LogStreamId(), from.generic_string() + " -> " + to.generic_string());
        }
        if (!systemModule->LibraryFilePath().empty() && !GetGlobalFlag(GlobalFlags::disableCodeGen))
        {
            from = systemModule->LibraryFilePath();
            to = systemLibDir / from.filename();
            if (boost::filesystem::exists(to))
            {
                boost::filesystem::remove(to);
            }
            boost::filesystem::copy(from, to);
            if (GetGlobalFlag(GlobalFlags::verbose))
            {
                LogMessage(systemInstallModule->LogStreamId(), from.generic_string() + " -> " + to.generic_string());
            }
        }
    }
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(systemInstallModule->LogStreamId(), "System libraries installed.");
    }
}

void InstallSystemWindowsLibraries(Module* systemInstallWindowsModule)
{
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(systemInstallWindowsModule->LogStreamId(), "Installing system libraries (Windows)...");
    }
    sngcm::ast::BackEnd backend = sngcm::ast::BackEnd::llvm;
    if (GetBackEnd() == cmajor::symbols::BackEnd::cmsx)
    {
        backend = sngcm::ast::BackEnd::cmsx;
    }
    else if (GetBackEnd() == cmajor::symbols::BackEnd::cmcpp)
    {
        backend = sngcm::ast::BackEnd::cppcm;
    }
    SystemDirKind systemDirKind = SystemDirKind::regular;
    if (GetGlobalFlag(GlobalFlags::repository))
    {
        systemDirKind = SystemDirKind::repository;
    }
    boost::filesystem::path systemLibDir = CmajorSystemLibDir(GetConfig(), backend, GetToolChain(), systemDirKind);
    boost::filesystem::create_directories(systemLibDir);
    for (Module* systemModule : systemInstallWindowsModule->AllReferencedModules())
    {
        boost::filesystem::path from = systemModule->OriginalFilePath();
        boost::filesystem::path to = systemLibDir / from.filename();
        if (boost::filesystem::exists(to))
        {
            boost::filesystem::remove(to);
        }
        boost::filesystem::copy(from, to);
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            LogMessage(systemInstallWindowsModule->LogStreamId(), from.generic_string() + " -> " + to.generic_string());
        }
        if (!systemModule->LibraryFilePath().empty() && !GetGlobalFlag(GlobalFlags::disableCodeGen))
        {
            from = systemModule->LibraryFilePath();
            to = systemLibDir / from.filename();
            if (boost::filesystem::exists(to))
            {
                boost::filesystem::remove(to);
            }
            boost::filesystem::copy(from, to);
            if (GetGlobalFlag(GlobalFlags::verbose))
            {
                LogMessage(systemInstallWindowsModule->LogStreamId(), from.generic_string() + " -> " + to.generic_string());
            }
        }
    }
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(systemInstallWindowsModule->LogStreamId(), "System libraries installed.");
    }
}

void CompileSingleThreaded(Project* project, Module* rootModule, std::vector<std::unique_ptr<BoundCompileUnit>>& boundCompileUnits, cmajor::codegen::EmittingContext& emittingContext,
    std::vector<std::string>& objectFilePaths, std::unordered_map<int, cmdoclib::File>& docFileMap, bool& stop) 
{
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(project->LogStreamId(), "Compiling...");
    }
    rootModule->StartBuild();
    int maxFileIndex = 0;
    for (std::unique_ptr<BoundCompileUnit>& boundCompileUnit : boundCompileUnits)
    {
        if (stop)
        {
            return;
        }
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            LogMessage(project->LogStreamId(), "> " + boost::filesystem::path(boundCompileUnit->GetCompileUnitNode()->FilePath()).filename().generic_string());
        }
        BindStatements(*boundCompileUnit);
        if (boundCompileUnit->HasGotos())
        {
            AnalyzeControlFlow(*boundCompileUnit);
        }
        if (GetGlobalFlag(GlobalFlags::bdt2xml))
        {
            std::unique_ptr<sngxml::dom::Document> bdtDoc = cmajor::bdt2dom::GenerateBdtDocument(boundCompileUnit.get());
            std::string bdtXmlFilePath = Path::ChangeExtension(boundCompileUnit->GetCompileUnitNode()->FilePath(), ".bdt.xml");
            std::ofstream bdtXmlFile(bdtXmlFilePath);
            CodeFormatter formatter(bdtXmlFile);
            formatter.SetIndentSize(1);
            bdtDoc->Write(formatter);
        }
        if (GetGlobalFlag(GlobalFlags::cmdoc))
        {
            cmdoclib::GenerateSourceCode(project, boundCompileUnit.get(), docFileMap);
            maxFileIndex = std::max(maxFileIndex, boundCompileUnit->GetCompileUnitNode()->GetSpan().fileIndex);
        }
        else
        {
            cmajor::codegen::GenerateCode(emittingContext, *boundCompileUnit);
            objectFilePaths.push_back(boundCompileUnit->ObjectFilePath());
        }
    }
    if (GetGlobalFlag(GlobalFlags::cmdoc))
    {
        cmdoclib::GenerateLexerAndParserHtmlSources(project, maxFileIndex, docFileMap);
    }
    rootModule->StopBuild();
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(rootModule->LogStreamId(), ToUtf8(rootModule->Name()) + " compilation time: " + FormatTimeMs(rootModule->GetBuildTimeMs()));
    }
}

int64_t compileDebugStart = 0;

std::string CurrentCompileDebugMsStr()
{
    return FormatTimeMs(static_cast<int32_t>(CurrentMs() - compileDebugStart));
}

int compileGetTimeoutSecs = 3;

class CompileQueue
{
public:
    CompileQueue(std::mutex* mtx_, const std::string& name_, bool& stop_, bool& ready_, int logStreamId_);
    void Put(int compileUnitIndex);
    int Get();
    void NotifyAll();
private:
    std::mutex* mtx;
    std::string name;
    std::list<int> queue;
    std::condition_variable cond;
    bool& stop;
    bool& ready;
    int logStreamId;
};

CompileQueue::CompileQueue(std::mutex* mtx_, const std::string& name_, bool& stop_, bool& ready_, int logStreamId_) :
    mtx(mtx_), name(name_), stop(stop_), ready(ready_), logStreamId(logStreamId_)
{
}

void CompileQueue::Put(int compileUnitIndex)
{
    std::lock_guard<std::mutex> lock(*mtx);
    queue.push_back(compileUnitIndex);
    cond.notify_one();
}

int CompileQueue::Get()
{
    while (!stop && !ready)
    {
        std::unique_lock<std::mutex> lock(*mtx);
        cond.wait(lock, [this] { return stop || ready || !queue.empty(); });
        if (stop || ready) return -1;
        int compileUnitIndex = queue.front();
        queue.pop_front();
        return compileUnitIndex;
    }
    return -1;
}

void CompileQueue::NotifyAll()
{
    cond.notify_all();
}

struct CompileData
{
    CompileData(std::mutex* mtx_, Module* rootModule_, std::vector<std::unique_ptr<BoundCompileUnit>>& boundCompileUnits_, std::vector<std::string>& objectFilePaths_, bool& stop_, bool& ready_,
        int numThreads_, CompileQueue& input_, CompileQueue& output_) :
        mtx(mtx_), rootModule(rootModule_), boundCompileUnits(boundCompileUnits_), objectFilePaths(objectFilePaths_), stop(stop_), ready(ready_), numThreads(numThreads_),
        input(input_), output(output_)
    {
        exceptions.resize(numThreads);
        sourceFileFilePaths.resize(boundCompileUnits.size());
        for (int i = 0; i < boundCompileUnits.size(); ++i)
        {
            sourceFileFilePaths[i] = boundCompileUnits[i]->GetCompileUnitNode()->FilePath();
        }
    }
    std::mutex* mtx;
    Module* rootModule;
    std::vector<std::string> sourceFileFilePaths;
    std::vector<std::unique_ptr<BoundCompileUnit>>& boundCompileUnits;
    std::vector<std::string>& objectFilePaths;
    bool& stop;
    bool& ready;
    int numThreads;
    CompileQueue& input;
    CompileQueue& output;
    std::vector<std::exception_ptr> exceptions;
};

void GenerateCode(CompileData* data, int threadId)
{
    try
    {
        SetRootModuleForCurrentThread(data->rootModule);
        cmajor::codegen::EmittingContext emittingContext(GetOptimizationLevel());
        while (!data->stop && !data->ready)
        {
            int compileUnitIndex = data->input.Get();
            if (compileUnitIndex >= 0 && compileUnitIndex < data->boundCompileUnits.size())
            {
                if (GetGlobalFlag(GlobalFlags::debugCompile))
                {
                    LogMessage(-1, data->sourceFileFilePaths[compileUnitIndex] + " " + std::to_string(compileUnitIndex) + " : GET INPUT " + std::to_string(compileUnitIndex));
                }
                BoundCompileUnit* compileUnit = data->boundCompileUnits[compileUnitIndex].get();
                cmajor::codegen::GenerateCode(emittingContext, *compileUnit);
                if (GetGlobalFlag(GlobalFlags::debugCompile))
                {
                    LogMessage(-1, data->sourceFileFilePaths[compileUnitIndex] + " " + std::to_string(compileUnitIndex) + " : PUT OUTPUT " + std::to_string(compileUnitIndex));
                }
                {
                    std::lock_guard<std::mutex> lock(*data->mtx);
                    data->objectFilePaths.push_back(compileUnit->ObjectFilePath());
                    data->boundCompileUnits[compileUnitIndex].reset();
                }
                data->output.Put(compileUnitIndex);
            }
        }
    }
    catch (...)
    {
        std::lock_guard<std::mutex> lock(*data->mtx);
        std::exception_ptr exception = std::current_exception();
        if (threadId >= 0 && threadId < data->exceptions.size())
        {
            data->exceptions[threadId] = exception;
        }
        data->stop = true;
        data->output.NotifyAll();
    }
}

void CompileMultiThreaded(Project* project, Module* rootModule, std::vector<std::unique_ptr<BoundCompileUnit>>& boundCompileUnits, std::vector<std::string>& objectFilePaths, 
    bool& stop)
{
    int numThreads = std::min(static_cast<int>(std::thread::hardware_concurrency()), static_cast<int>(boundCompileUnits.size()));
    if (numThreads <= 0)
    {
        numThreads = 1;
    }
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(project->LogStreamId(), "Compiling using " + std::to_string(numThreads) + " threads...");
    }
    compileDebugStart = CurrentMs();
    rootModule->StartBuild();
    bool ready = false;
    std::mutex mtx;
    CompileQueue input(&mtx, "input", stop, ready, rootModule->LogStreamId());
    CompileQueue output(&mtx, "output", stop, ready, rootModule->LogStreamId());
    CompileData compileData(&mtx, rootModule, boundCompileUnits, objectFilePaths, stop, ready, numThreads, input, output);
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i)
    {
        threads.push_back(std::thread{ GenerateCode, &compileData, i });
    }
    int n = boundCompileUnits.size();
    for (int i = 0; i < n; ++i)
    {
        BoundCompileUnit* compileUnit = boundCompileUnits[i].get();
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            LogMessage(rootModule->LogStreamId(), "> " + compileUnit->GetCompileUnitNode()->FilePath());
        }
        try
        {
            BindStatements(*compileUnit);
            if (compileUnit->HasGotos())
            {
                AnalyzeControlFlow(*compileUnit);
            }
            compileUnit->SetImmutable();
        }
        catch (...)
        {
            stop = true;
            input.NotifyAll();
            for (int i = 0; i < numThreads; ++i)
            {
                if (threads[i].joinable())
                {
                    threads[i].join();
                }
            }
            throw;
        }
        if (GetGlobalFlag(GlobalFlags::debugCompile))
        {
            LogMessage(rootModule->LogStreamId(), compileData.sourceFileFilePaths[i] + " : PUT INPUT " + std::to_string(i));
        }
        input.Put(i);
    }
    int numOutputsReceived = 0;
    while (numOutputsReceived < n && !stop)
    {
        int compileUnitIndex = output.Get();
        if (compileUnitIndex != -1)
        {
            if (GetGlobalFlag(GlobalFlags::debugCompile))
            {
                LogMessage(rootModule->LogStreamId(), compileData.sourceFileFilePaths[compileUnitIndex] + " : GET OUTPUT " + std::to_string(compileUnitIndex));
            }
        }
        if (compileUnitIndex != -1)
        {
            ++numOutputsReceived;
        }
    }
    if (GetGlobalFlag(GlobalFlags::debugCompile))
    {
        LogMessage(rootModule->LogStreamId(), ToUtf8(rootModule->Name()) + " > BEGIN READY");
    }
    {
        std::lock_guard<std::mutex> lock(mtx);
        ready = true;
        compileData.input.NotifyAll();
    }
    if (GetGlobalFlag(GlobalFlags::debugCompile))
    {
        LogMessage(rootModule->LogStreamId(), ToUtf8(rootModule->Name()) + " < END READY");
    }
    for (int i = 0; i < numThreads; ++i)
    {
        if (threads[i].joinable())
        {
            threads[i].join();
        }
    }
    for (int i = 0; i < numThreads; ++i)
    {
        if (compileData.exceptions[i])
        {
            std::rethrow_exception(compileData.exceptions[i]);
        }
    }
    rootModule->StopBuild();
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(rootModule->LogStreamId(), ToUtf8(rootModule->Name()) + " compilation time: " + FormatTimeMs(rootModule->GetBuildTimeMs()));
    }
}

std::unique_ptr<Project> ReadProject(const std::string& projectFilePath)
{
    SystemDirKind systemDirKind = SystemDirKind::regular;
    if (GetGlobalFlag(GlobalFlags::repository))
    {
        systemDirKind = SystemDirKind::repository;
    }
    std::string config = GetConfig();
    MappedInputFile projectFile(projectFilePath);
    std::u32string p(ToUtf32(std::string(projectFile.Begin(), projectFile.End())));
    sngcm::ast::BackEnd backend = sngcm::ast::BackEnd::llvm;
    if (GetBackEnd() == cmajor::symbols::BackEnd::cmsx)
    {
        backend = sngcm::ast::BackEnd::cmsx;
    }
    else if (GetBackEnd() == cmajor::symbols::BackEnd::cmcpp)
    {
        backend = sngcm::ast::BackEnd::cppcm;
    }
    ContainerFileLexer containerFileLexer(p, projectFilePath, 0);
    std::unique_ptr<Project> project = ProjectFileParser::Parse(containerFileLexer, config, backend, GetToolChain(), systemDirKind);
    project->ResolveDeclarations();
    return project;
}

void WriteProjectInfo(Project* project, std::vector<std::unique_ptr<CompileUnitNode>>& compileUnits)
{
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(project->LogStreamId(), "Writing project info file...");
    }
    Preprocess(project, compileUnits);
    ProjectInfo projectInfo;
    projectInfo.projectFilePath = project->FilePath();
    projectInfo.projectId = project->Id();
    projectInfo.projectHash = project->Hash();
    projectInfo.projectName = ToUtf8(project->Name());
    projectInfo.target = TargetStr(project->GetTarget());
    for (const std::string& dependsOnId : project->DependsOnIds())
    {
        projectInfo.dependsOnProjects.push_back(dependsOnId);
    }
    for (const auto& compileUnit : compileUnits)
    {
        SourceFileInfo sourceFileInfo;
        sourceFileInfo.filePath = compileUnit->FilePath();
        sourceFileInfo.fileId = compileUnit->Id();
        sourceFileInfo.fileHash = compileUnit->Hash();
        projectInfo.fileInfos.push_back(sourceFileInfo);
    }
    std::string projectInfoFilePath = Path::ChangeExtension(project->FilePath(), ".json");
    std::ofstream projectInfoFile(projectInfoFilePath);
    CodeFormatter formatter(projectInfoFile);
    std::unique_ptr<JsonValue> jsonValue =  projectInfo.ToJson();
    jsonValue->Write(formatter);
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(project->LogStreamId(), "==> " + projectInfoFilePath);
    }
}

bool IsProjectInfoUpToDate(Project* project, const std::string& projectInfoFilePath)
{
    if (!boost::filesystem::exists(projectInfoFilePath))
    {
        return false;
    }
    time_t projectInfoFileWriteTime = boost::filesystem::last_write_time(projectInfoFilePath);
    time_t projectFileWriteTime = boost::filesystem::last_write_time(project->FilePath());
    if (projectFileWriteTime > projectInfoFileWriteTime)
    {
        return false;
    }
    for (const std::string& sourceFilePath : project->SourceFilePaths())
    {
        time_t sourceFileWriteTime = boost::filesystem::last_write_time(sourceFilePath);
        if (sourceFileWriteTime > projectInfoFileWriteTime)
        {
            return false;
        }
    }
    return true;
}

void GenerateProjectInfoFile(Project* project)
{
    std::unique_ptr<Module> module(new Module());
    bool stop = false;
    for (const std::string& referencedProjectFilePath : project->ReferencedProjectFilePaths())
    {
        std::unique_ptr<Project> referencedProject = ReadProject(referencedProjectFilePath);
        project->AddDependsOnId(referencedProject->Id());
    }
    std::vector<std::unique_ptr<CompileUnitNode>> compileUnits = ParseSources(module.get(), project->SourceFilePaths(), stop);
    WriteProjectInfo(project, compileUnits);
}

ProjectInfo ReadPojectInfo(Project* project, const std::string& projectInfoFilePath)
{
    if (project != nullptr)
    {
        bool projectInfoUpToDate = IsProjectInfoUpToDate(project, projectInfoFilePath);
        if (!projectInfoUpToDate)
        {
            GenerateProjectInfoFile(project);
        }
    }
    ProjectInfo projectInfo;
    std::string projectInfoFile = ReadFile(projectInfoFilePath);
    std::u32string projectInfoFileContent = ToUtf32(projectInfoFile);
    JsonLexer lexer(projectInfoFileContent, projectInfoFilePath, 0);
    std::unique_ptr<JsonValue> projectInfoJsonValue = JsonParser::Parse(lexer);
    if (projectInfoJsonValue->Type() == JsonValueType::object)
    {
        JsonObject* projectFileInfo = static_cast<JsonObject*>(projectInfoJsonValue.get());
        projectInfo = ProjectInfo(projectFileInfo);
    }
    else
    {
        throw std::runtime_error("buildclient: JSON object expected");
    }
    return projectInfo;
}

void AddResourcesSystemX(Project* project, std::vector<std::string>& objectFilePaths)
{
    for (const auto& resourceFilePath : project->ResourceFilePaths())
    {
        std::string objectFilePath = cmsx::object::GenerateResourceUnit(resourceFilePath, project->LibraryDir());
        objectFilePaths.push_back(objectFilePath);
    }
}

void BuildProject(Project* project, std::unique_ptr<Module>& rootModule, bool& stop, bool resetRootModule, std::set<std::string>& builtProjects)
{
    try
    {
        Variables variables;
        std::string outDir = project->OutDir();
        if (!outDir.empty())
        {
            variables.AddVariable(new Variable("OUT_DIR", outDir));
        }
        variables.AddVariable(new Variable("PROJECT_DIR", project->ProjectDir()));
        variables.AddVariable(new Variable("LIBRARY_DIR", project->LibraryDir()));
        if (!GetGlobalFlag(GlobalFlags::msbuild))
        {
            if (builtProjects.find(project->FilePath()) != builtProjects.cend()) return;
            builtProjects.insert(project->FilePath());
            for (const std::string& referencedProjectFilePath : project->ReferencedProjectFilePaths())
            {
                SystemDirKind systemDirKind = SystemDirKind::regular;
                if (GetGlobalFlag(GlobalFlags::repository))
                {
                    systemDirKind = SystemDirKind::repository;
                }
                std::unique_ptr<Project> referencedProject = ReadProject(referencedProjectFilePath);
                project->AddDependsOnId(referencedProject->Id());
                if (GetGlobalFlag(GlobalFlags::buildAll))
                {
                    if (builtProjects.find(referencedProjectFilePath) == builtProjects.cend())
                    {
                        std::unique_ptr<Module> module;
                        try
                        {
                            BuildProject(referencedProject.get(), module, stop, resetRootModule, builtProjects);
                        }
                        catch (...)
                        {
                            rootModule.reset(module.release());
                            throw;
                        }
                    }
                }
            }
        }
        std::string config = GetConfig();
        bool isSystemModule = IsSystemModule(project->Name());
        if (isSystemModule)
        {
            project->SetSystemProject();
        }
        bool upToDate = false;
        sngcm::ast::BackEnd astBackEnd = sngcm::ast::BackEnd::llvm;
        if (GetBackEnd() == cmajor::symbols::BackEnd::cmsx)
        {
            astBackEnd = sngcm::ast::BackEnd::cmsx;
        }
        else if (GetBackEnd() == cmajor::symbols::BackEnd::cmcpp)
        {
            astBackEnd = sngcm::ast::BackEnd::cppcm;
        }
        if (!GetGlobalFlag(GlobalFlags::rebuild))
        {
            SystemDirKind systemDirKind = SystemDirKind::regular;
            if (GetGlobalFlag(GlobalFlags::repository))
            {
                systemDirKind = SystemDirKind::repository;
            }
            upToDate = project->IsUpToDate(CmajorSystemModuleFilePath(config, astBackEnd, GetToolChain(), systemDirKind));
        }
        if (upToDate)
        {
            if (GetGlobalFlag(GlobalFlags::verbose))
            {
                LogMessage(project->LogStreamId(), "===== Project '" + ToUtf8(project->Name()) + "' (" + project->FilePath() + ") is up-to-date.");
            }
            return;
        }
        bool systemLibraryInstalled = false;
        if (project->GetTarget() == Target::unitTest)
        {
            throw std::runtime_error("cannot build unit test project '" + ToUtf8(project->Name()) + "' using cmc, use cmunit.");
        }
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            LogMessage(project->LogStreamId(), "===== Building project '" + ToUtf8(project->Name()) + "' (" + project->FilePath() + ") using " + config + " configuration.");
        }
        if (GetGlobalFlag(GlobalFlags::cmdoc))
        {
            cmdoclib::SetEmptyLibraryPrefix(project->Name());
            if (GetGlobalFlag(GlobalFlags::optimizeCmDoc))
            {
                if (cmdoclib::HtmlSourceFilePathsUpToDate(project) && cmdoclib::SymbolTableXmlFilesUpToDate(project))
                {
                    if (GetGlobalFlag(GlobalFlags::verbose))
                    {
                        LogMessage(project->LogStreamId(), "Project '" + ToUtf8(project->Name()) + " xml is up-to-date.");
                    }
                    return;
                }
            }
        }
        rootModule.reset(new Module(project->Name(), project->ModuleFilePath(), project->GetTarget()));
        rootModule->SetRootModule();
        SetRootModuleForCurrentThread(rootModule.get());
        {
            rootModule->SetLogStreamId(project->LogStreamId());
            rootModule->SetCurrentProjectName(project->Name());
            rootModule->SetCurrentToolName(U"cmc");
            boost::filesystem::path libraryFilePath = project->LibraryFilePath();
            boost::filesystem::path libDir = libraryFilePath.remove_filename();
            std::string definesFilePath = GetFullPath((libDir / boost::filesystem::path("defines.txt")).generic_string());
            SetDefines(rootModule.get(), definesFilePath);
            rootModule->SetFlag(cmajor::symbols::ModuleFlags::compiling);
            std::vector<std::unique_ptr<CompileUnitNode>> compileUnits = ParseSources(rootModule.get(), project->SourceFilePaths(), stop);
            AttributeBinder attributeBinder(rootModule.get());
            std::vector<ClassTypeSymbol*> classTypes;
            std::vector<ClassTemplateSpecializationSymbol*> classTemplateSpecializations;
            bool prevPreparing = rootModule->Preparing();
            rootModule->SetPreparing(true);
            PrepareModuleForCompilation(rootModule.get(), project->References(), project->GetTarget());
            Preprocess(project, compileUnits);
            CreateSymbols(rootModule->GetSymbolTable(), compileUnits, stop);
            if (GetGlobalFlag(GlobalFlags::sym2xml))
            {
                std::unique_ptr<sngxml::dom::Document> symbolTableDoc = rootModule->GetSymbolTable().ToDomDocument();
                std::string symbolTableXmlFilePath = Path::ChangeExtension(project->FilePath(), ".sym0.xml");
                std::ofstream symbolTableXmlFile(symbolTableXmlFilePath);
                CodeFormatter formatter(symbolTableXmlFile);
                formatter.SetIndentSize(1);
                symbolTableDoc->Write(formatter);
            }
            CompileUnitNode* compileUnit0 = nullptr;
            if (!compileUnits.empty())
            {
                compileUnit0 = compileUnits[0].get();
            }
            if (GetGlobalFlag(GlobalFlags::verbose))
            {
                LogMessage(project->LogStreamId(), "Binding types...");
            }
            std::vector<std::unique_ptr<BoundCompileUnit>> boundCompileUnits = BindTypes(*rootModule, compileUnits, &attributeBinder, stop);
            if (stop)
            {
                return;
            }
            rootModule->SetPreparing(prevPreparing);
            if (GetGlobalFlag(GlobalFlags::sym2xml))
            {
                std::unique_ptr<sngxml::dom::Document> symbolTableDoc = rootModule->GetSymbolTable().ToDomDocument();
                std::string symbolTableXmlFilePath = Path::ChangeExtension(project->FilePath(), ".sym1.xml");
                std::ofstream symbolTableXmlFile(symbolTableXmlFilePath);
                CodeFormatter formatter(symbolTableXmlFile);
                formatter.SetIndentSize(1);
                symbolTableDoc->Write(formatter);
            }
            std::unordered_map<int, cmdoclib::File> docFileMap;
            cmajor::codegen::EmittingContext emittingContext(GetOptimizationLevel());
            std::vector<std::string> objectFilePaths;
            if (GetGlobalFlag(GlobalFlags::singleThreadedCompile))
            {
                CompileSingleThreaded(project, rootModule.get(), boundCompileUnits, emittingContext, objectFilePaths, docFileMap, stop);
            }
            else
            {
                CompileMultiThreaded(project, rootModule.get(), boundCompileUnits, objectFilePaths, stop);
            }
            if (GetGlobalFlag(GlobalFlags::cmdoc))
            {
                cmdoclib::GenerateSymbolTableXml(rootModule.get(), docFileMap);
            }
            else
            {
                if (GetGlobalFlag(GlobalFlags::sym2xml))
                {
                    std::unique_ptr<sngxml::dom::Document> symbolTableDoc = rootModule->GetSymbolTable().ToDomDocument();
                    std::string symbolTableXmlFilePath = Path::ChangeExtension(project->FilePath(), ".sym2.xml");
                    std::ofstream symbolTableXmlFile(symbolTableXmlFilePath);
                    CodeFormatter formatter(symbolTableXmlFile);
                    formatter.SetIndentSize(1);
                    symbolTableDoc->Write(formatter);
                }
                std::string mainObjectFilePath;
                std::string mainSourceFilePath;
                if (project->GetTarget() == Target::program || project->GetTarget() == Target::winguiapp || project->GetTarget() == Target::winapp)
                {
                    CheckMainFunctionSymbol(*rootModule);
                    rootModule->SetFlag(ModuleFlags::programModule);
                    if (!rootModule->GetSymbolTable().JsonClasses().empty())
                    {
                        CreateJsonRegistrationUnit(objectFilePaths, *rootModule, emittingContext, &attributeBinder);
                    }
                    if (GetBackEnd() == cmajor::symbols::BackEnd::llvm)
                    {
                        if (project->GetTarget() == Target::winguiapp)
                        {
                            CreateMainUnitLlvmWindowsGUI(objectFilePaths, *rootModule, emittingContext, &attributeBinder, mainObjectFilePath);
                        }
                        else
                        {
                            CreateMainUnitLlvm(objectFilePaths, *rootModule, emittingContext, &attributeBinder, mainObjectFilePath);
                        }
                    }
                    else if (GetBackEnd() == cmajor::symbols::BackEnd::cmcpp)
                    {
                        if (project->GetTarget() == Target::winguiapp)
                        {
                            CreateMainUnitCppWindowsGUI(objectFilePaths, *rootModule, emittingContext, &attributeBinder, mainObjectFilePath, mainSourceFilePath);
                        }
                        else
                        {
                            CreateMainUnitCpp(objectFilePaths, *rootModule, emittingContext, &attributeBinder, mainObjectFilePath, mainSourceFilePath);
                        }
                    }
                    else if (GetBackEnd() == cmajor::symbols::BackEnd::cmsx)
                    {
                        CreateMainUnitSystemX(objectFilePaths, *rootModule, emittingContext, &attributeBinder, mainObjectFilePath);
                    }
                }
                if (GetBackEnd() == cmajor::symbols::BackEnd::cmsx)
                {
                    AddResourcesSystemX(project, objectFilePaths);
                }
                if (!objectFilePaths.empty())
                {
                    GenerateLibrary(rootModule.get(), objectFilePaths, project->LibraryFilePath());
                }
#ifdef _WIN32
                cmajor::resources::ProcessResourcesInProject(*project, *rootModule, astBackEnd);
#endif
                if (GetBackEnd() == BackEnd::cmcpp)
                {
                    CreateCppProjectFile(project, *rootModule, mainSourceFilePath, project->LibraryFilePath(), rootModule->LibraryFilePaths());
                }
                else if (GetBackEnd() == BackEnd::cmsx)
                {
                    if (GetGlobalFlag(GlobalFlags::cpp))
                    {
                        CreateCppProjectFileSystemX(project, *rootModule, mainObjectFilePath);
                    }
                }
                if (project->GetTarget() == Target::program || project->GetTarget() == Target::winguiapp || project->GetTarget() == Target::winapp)
                {
                    Link(project->GetTarget(), project->ExecutableFilePath(), project->LibraryFilePath(), rootModule->LibraryFilePaths(),
                        mainObjectFilePath, *rootModule);
                }
                if (GetGlobalFlag(GlobalFlags::verbose))
                {
                    LogMessage(project->LogStreamId(), "Writing module file...");
                }
                SymbolWriter writer(project->ModuleFilePath());
                writer.SetLexers(rootModule->GetLexers());
                writer.SetSpanConversionModuleId(rootModule->Id());
                rootModule->Write(writer);
                rootModule->ResetFlag(cmajor::symbols::ModuleFlags::compiling);
                if (cmajor::symbols::GetGlobalFlag(cmajor::symbols::GlobalFlags::updateSourceFileModuleMap))
                {
                    rootModule->UpdateSourceFileModuleMap();
                }
                if (GetGlobalFlag(GlobalFlags::verbose))
                {
                    LogMessage(project->LogStreamId(), "==> " + project->ModuleFilePath());
                }
                if (GetBackEnd() == BackEnd::cmcpp)
                {
                    if (GetGlobalFlag(GlobalFlags::verbose))
                    {
                        LogMessage(project->LogStreamId(), "Writing project debug info file...");
                    }
                    std::string pdiFilePath = Path::ChangeExtension(project->ModuleFilePath(), ".pdi");
                    rootModule->WriteProjectDebugInfoFile(pdiFilePath);
                    if (GetGlobalFlag(GlobalFlags::verbose))
                    {
                        LogMessage(project->LogStreamId(), "==> " + pdiFilePath);
                    }
                }
                if (!GetGlobalFlag(GlobalFlags::msbuild))
                {
                    WriteProjectInfo(project, compileUnits);
                }
                if (GetBackEnd() == BackEnd::cmcpp)
                {
                    if (project->GetTarget() == Target::program || project->GetTarget() == Target::winguiapp || project->GetTarget() == Target::winapp)
                    {
                        if (GetGlobalFlag(GlobalFlags::verbose))
                        {
                            LogMessage(project->LogStreamId(), "Writing debug information file...");
                        }
#ifdef _WIN32
                        std::string cmdbFilePath = Path::ChangeExtension(project->ExecutableFilePath(), ".cmdb");
#else
                        std::string cmdbFilePath = project->ExecutableFilePath() + ".cmdb";
#endif
                        rootModule->WriteCmdbFile(cmdbFilePath);
                        if (GetGlobalFlag(GlobalFlags::verbose))
                        {
                            LogMessage(project->LogStreamId(), "==> " + cmdbFilePath);
                        }
                    }
                }
                RunBuildActions(*project, variables);
                if (GetGlobalFlag(GlobalFlags::verbose))
                {
                    LogMessage(project->LogStreamId(), std::to_string(rootModule->GetSymbolTable().NumSpecializations()) + " class template specializations, " +
                        std::to_string(rootModule->GetSymbolTable().NumSpecializationsNew()) + " new, " +
                        std::to_string(rootModule->GetSymbolTable().NumSpecializationsCopied()) + " copied.");
                    LogMessage(project->LogStreamId(), "Project '" + ToUtf8(project->Name()) + "' built successfully.");
                }
                project->SetModuleFilePath(rootModule->OriginalFilePath());
                project->SetLibraryFilePath(rootModule->LibraryFilePath());
                if (rootModule->IsSystemModule())
                {
                    project->SetSystemProject();
                }
                if (rootModule->Name() == U"System.Install")
                {
                    InstallSystemLibraries(rootModule.get());
                    systemLibraryInstalled = true;
                }
                else if (rootModule->Name() == U"System.Windows.Install")
                {
                    InstallSystemWindowsLibraries(rootModule.get());
                    systemLibraryInstalled = true;
                }
            }
        }
        if (resetRootModule)
        {
            PutModuleToModuleCache(std::move(rootModule));
            rootModule.reset();
        }
        if (systemLibraryInstalled)
        {
            ResetModuleCache();
        }
    }
    catch (const cmajor::symbols::Error& ex) 
    {
        LogMessage(-1, "project: " + ToUtf8(project->Name()) + ": " + ex.Message());
        throw;
    }
    catch (const std::exception& ex)
    {
        LogMessage(-1, "project: " + ToUtf8(project->Name()) + ": " + PlatformStringToUtf8(ex.what())); 
        throw;
    }
}

void BuildProject(const std::string& projectFilePath, std::unique_ptr<Module>& rootModule, std::set<std::string>& builtProjects)
{
    std::unique_ptr<Project> project = ReadProject(projectFilePath);
    if (GetGlobalFlag(GlobalFlags::clean))
    {
        if (!GetGlobalFlag(GlobalFlags::msbuild))
        {
            for (const std::string& referencedProjectFilePath : project->ReferencedProjectFilePaths())
            {
                SystemDirKind systemDirKind = SystemDirKind::regular;
                if (GetGlobalFlag(GlobalFlags::repository))
                {
                    systemDirKind = SystemDirKind::repository;
                }
                std::unique_ptr<Project> referencedProject = ReadProject(referencedProjectFilePath);
                project->AddDependsOnId(referencedProject->Id());
                if (currentSolution == nullptr && GetGlobalFlag(GlobalFlags::buildAll))
                {
                    BuildProject(referencedProjectFilePath, rootModule, builtProjects);
                }
            }
        }
        CleanProject(project.get());
    }
    else
    {
        stopBuild = false;
        BuildProject(project.get(), rootModule, stopBuild, true, builtProjects);
    }
}

bool buildDebug = false;
const int buildGetTimeoutSecs = 3;
int64_t buildDebugStart = 0;

std::string CurrentProjectDebugMsStr()
{
    return FormatTimeMs(static_cast<int32_t>(CurrentMs() - buildDebugStart));
}

class ProjectQueue
{
public:
    ProjectQueue(bool& stop_, const std::string& name_, std::mutex* mtx_);
    void Put(Project* project);
    Project* Get();
    void Stop();
private:
    std::string name;
    std::list<Project*> queue;
    std::mutex* mtx;
    std::condition_variable cond;
    bool& stop;
};


ProjectQueue::ProjectQueue(bool& stop_, const std::string& name_, std::mutex* mtx_) : stop(stop_), name(name_), mtx(mtx_)
{
}

void ProjectQueue::Put(Project* project)
{
    if (buildDebug)
    {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        LogMessage(-1, CurrentProjectDebugMsStr() + ">ProjectQueue(" + name + ")::Put: " + CurrentThreadIdStr() + " " + ToUtf8(project->Name())); 
    }
    std::lock_guard<std::mutex> lock(*mtx);
    queue.push_back(project);
    cond.notify_one();
    if (buildDebug)
    {
        LogMessage(-1, CurrentProjectDebugMsStr() + "<ProjectQueue(" + name + ")::Put: " + CurrentThreadIdStr() + " " + ToUtf8(project->Name()));
    }
}

Project* ProjectQueue::Get()
{
    while (!stop)
    {
        if (buildDebug)
        {
            LogMessage(-1, CurrentProjectDebugMsStr() + ">ProjectQueue(" + name + ")::Get: " + CurrentThreadIdStr());
        }
        std::unique_lock<std::mutex> lock(*mtx);
        cond.wait(lock, [this] { return stop || !queue.empty(); });
        if (stop) return nullptr;
        Project* project = queue.front();
        queue.pop_front();
        if (buildDebug)
        {
            LogMessage(-1, CurrentProjectDebugMsStr() + "<ProjectQueue(" + name + ")::Get: " + CurrentThreadIdStr() + " got project " + ToUtf8(project->Name()));
        }
        return project;
    }
    if (buildDebug)
    {
        LogMessage(-1, CurrentProjectDebugMsStr() + "<ProjectQueue(" + name + ")::Get: " + CurrentThreadIdStr() + " stop");
    }
    return nullptr;
}

void ProjectQueue::Stop()
{
    stop = true;
    cond.notify_all();
}

struct BuildData
{
    BuildData(std::mutex* mtx_, bool& stop_, 
        ProjectQueue& buildQueue_, ProjectQueue& readyQueue_, std::vector<std::unique_ptr<Module>>& rootModules_, bool& isSystemSolution_, 
        std::set<std::string>& builtProjects_) :
        mtx(mtx_), stop(stop_), buildQueue(buildQueue_), readyQueue(readyQueue_), rootModules(rootModules_), isSystemSolution(isSystemSolution_), builtProjects(builtProjects_)
    {
    }
    std::mutex* mtx;
    bool& stop;
    ProjectQueue& buildQueue;
    ProjectQueue& readyQueue;
    std::vector<std::unique_ptr<Module>>& rootModules;
    std::vector<std::exception_ptr> exceptions;
    bool& isSystemSolution;
    std::set<std::string>& builtProjects;
};

void ProjectBuilder(BuildData* buildData)
{
    try
    {
        Project* toBuild = buildData->buildQueue.Get();
        while (!buildData->stop)
        {
            if (toBuild)
            {
                BuildProject(toBuild, buildData->rootModules[toBuild->Index()], buildData->stop, true, buildData->builtProjects);
                if (toBuild->IsSystemProject())
                {
                    buildData->isSystemSolution = true;
                }
                buildData->readyQueue.Put(toBuild);
            }
            toBuild = buildData->buildQueue.Get();
        }
    }
    catch (...)
    {
        if (buildDebug)
        {
            LogMessage(-1, CurrentProjectDebugMsStr() + ">ProjectBuilder()::catch " + CurrentThreadIdStr());
        }
        std::lock_guard<std::mutex> lock(*buildData->mtx);
        buildData->exceptions.push_back(std::current_exception());
        buildData->buildQueue.Stop();
        buildData->readyQueue.Stop();
        if (buildDebug)
        {
            LogMessage(-1, CurrentProjectDebugMsStr() + "<ProjectBuilder()::catch " + CurrentThreadIdStr());
        }
    }
}

struct CurrentSolutionGuard
{
    CurrentSolutionGuard(Solution* solution) { currentSolution = solution; }
    ~CurrentSolutionGuard() { currentSolution = nullptr; }
};

bool SolutionContainsSystemModule(const std::string& solutionFilePath)
{
    MappedInputFile solutionFile(solutionFilePath);
    std::u32string s(ToUtf32(std::string(solutionFile.Begin(), solutionFile.End())));
    ContainerFileLexer containerFileLexer(s, solutionFilePath, 0);
    std::unique_ptr<Solution> solution = SolutionFileParser::Parse(containerFileLexer);
    solution->ResolveDeclarations();
    int np = solution->ProjectFilePaths().size();
    for (int i = 0; i < np; ++i)
    {
        const std::string& projectFilePath = solution->ProjectFilePaths()[i];
        std::unique_ptr<Project> project = ReadProject(projectFilePath);
        if (cmajor::symbols::IsSystemModule(project->Name())) return true;
    }
    return false;
}

void BuildSolution(const std::string& solutionFilePath, std::vector<std::unique_ptr<Module>>& rootModules)
{
    std::u32string solutionName;
    std::vector<std::u32string> moduleNames;
    BuildSolution(solutionFilePath, rootModules, solutionName, moduleNames);
}

void BuildSolution(const std::string& solutionFilePath, std::vector<std::unique_ptr<Module>>& rootModules, std::u32string& solutionName, std::vector<std::u32string>& moduleNames)
{
    if (buildLogWriter)
    {
        buildLogWriter->WriteCurrentDateTime();
        buildLogWriter->WriteLine("Building solution '" + solutionFilePath + "'");
    }
    std::set<std::string> builtProjects;
    MappedInputFile solutionFile(solutionFilePath);
    std::u32string s(ToUtf32(std::string(solutionFile.Begin(), solutionFile.End())));
    ContainerFileLexer containerFileLexer(s, solutionFilePath, 0);
    std::unique_ptr<Solution> solution = SolutionFileParser::Parse(containerFileLexer);
    solution->ResolveDeclarations();
    CurrentSolutionGuard currentSolutionGuard(solution.get());
    solutionName = solution->Name();
    std::string config = GetConfig();
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        if (GetGlobalFlag(GlobalFlags::clean))
        {
            LogMessage(-1, "Cleaning solution '" + ToUtf8(solution->Name()) + "' (" + solution->FilePath() + ") using " + config + " configuration.");
        }
        else
        {
            LogMessage(-1, "Building solution '" + ToUtf8(solution->Name()) + "' (" + solution->FilePath() + ") using " + config + " configuration.");
        }
    }
    int np = solution->ProjectFilePaths().size();
    for (int i = 0; i < np; ++i)
    {
        const std::string& projectFilePath = solution->ProjectFilePaths()[i];
        const std::string& relativeProjectFilePath = solution->RelativeProjectFilePaths()[i];
        std::unique_ptr<Project> project = ReadProject(projectFilePath);
        project->SetRelativeFilePath(relativeProjectFilePath);
        solution->AddProject(std::move(project));
    }
    std::vector<Project*> buildOrder = solution->CreateBuildOrder();
    std::vector<Project*> projectsToBuild;
    bool isSystemSolution = false;
    int n = buildOrder.size();
    for (int i = 0; i < n; ++i)
    {
        Project* project = buildOrder[i];
        project->SetLogStreamId(i);
        project->SetIndex(i);
        if (project->GetTarget() == Target::unitTest)
        {
            if (GetGlobalFlag(GlobalFlags::verbose))
            {
                LogMessage(-1, "skipping unit test project '" + ToUtf8(project->Name()) + "'");
            }
            continue;
        }
        if (GetGlobalFlag(GlobalFlags::clean))
        {
            CleanProject(project);
        }
        else
        {
            projectsToBuild.push_back(project);
        }
    }
    if (!projectsToBuild.empty())
    {
        for (Project* project : projectsToBuild)
        {
            moduleNames.push_back(project->Name());
        }
        if (GetGlobalFlag(GlobalFlags::cmdoc) && GetGlobalFlag(GlobalFlags::optimizeCmDoc))
        {
            cmdoclib::ReadGlobals(moduleNames);
        }
        int numProjectsToBuild = projectsToBuild.size();
        int numThreads = std::min(numProjectsToBuild, int(std::thread::hardware_concurrency() * 2));
        int n = GetNumBuildThreads();
        if (n != -1)
        {
            numThreads = std::max(1, n);
        }
        if (buildLogWriter)
        {
            buildLogWriter->WriteLine("building " + std::to_string(numProjectsToBuild) + " projects using " + std::to_string(numThreads) + " threads...");
        }
        rootModules.resize(numProjectsToBuild);
        if (numThreads <= 1)
        {
            if (GetGlobalFlag(GlobalFlags::verbose))
            {
                LogMessage(-1, "Building " + std::to_string(numProjectsToBuild) + " projects in main thread...");
            }
            for (int i = 0; i < numProjectsToBuild; ++i)
            {
                Project* project = projectsToBuild[i];
                stopBuild = false;
                BuildProject(project, rootModules[i], stopBuild, true, builtProjects);
            }
        }
        else
        {
            if (GetGlobalFlag(GlobalFlags::verbose))
            {
                LogMessage(-1, "Building " + std::to_string(numProjectsToBuild) + " projects using " + std::to_string(numThreads) + " threads...");
            }
            stopBuild = false;
            const char* buildDebugEnvVarDefined = std::getenv("CMAJOR_BUILD_DEBUG");
            if (buildDebugEnvVarDefined != nullptr && std::string(buildDebugEnvVarDefined) != "")
            {
                buildDebugStart = CurrentMs();
                buildDebug = true;
            }
            else
            {
                buildDebug = false;
            }
            std::mutex mtx;
            ProjectQueue buildQueue(stopBuild, "build", &mtx);
            ProjectQueue readyQueue(stopBuild, "ready", &mtx);
            BuildData buildData(&mtx, stopBuild, buildQueue, readyQueue, rootModules, isSystemSolution, builtProjects);
            std::vector<std::thread> threads;
            for (int i = 0; i < numThreads; ++i)
            {
                threads.push_back(std::thread(ProjectBuilder, &buildData));
                if (buildData.stop) break;
            }
            while (numProjectsToBuild > 0 && !stopBuild)
            {
                std::vector<Project*> building;
                for (Project* project : projectsToBuild)
                {
                    if (project->Ready())
                    {
                        if (buildLogWriter)
                        {
                            buildLogWriter->WriteCurrentDateTime();
                            buildLogWriter->WriteLine("project '" + ToUtf8(project->Name()) + "' is ready to build");
                        }
                        building.push_back(project);
                        buildQueue.Put(project);
                    }
                }
                for (Project* project : building)
                {
                    projectsToBuild.erase(std::remove(projectsToBuild.begin(), projectsToBuild.end(), project), projectsToBuild.end());
                }
                Project* ready = readyQueue.Get();
                if (ready)
                {
                    ready->SetBuilt();      
                    --numProjectsToBuild;
                    if (buildLogWriter)
                    {
                        buildLogWriter->WriteCurrentDateTime();
                        buildLogWriter->WriteLine("project '" + ToUtf8(ready->Name()) + "' built");
                    }
                }
            }
            if (stopBuild)
            {
                LogMessage(-1, "Build stopped.");
            }
            stopBuild = true;
            int numStartedThreads = threads.size();
            for (int i = 0; i < numStartedThreads; ++i)
            {
                buildQueue.Put(nullptr);
            }
            for (int i = 0; i < numStartedThreads; ++i)
            {
                if (threads[i].joinable())
                {
                    threads[i].join();
                }
            }
            if (!buildData.exceptions.empty())
            {
                std::rethrow_exception(buildData.exceptions.front());
            }
        }
    }
    if (!GetGlobalFlag(GlobalFlags::clean))
    {
        if (GetBackEnd() == BackEnd::cmcpp)
        {
            CreateCppSolutionFile(solution.get(), projectsToBuild);
        }
    }
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        if (GetGlobalFlag(GlobalFlags::clean))
        {
            LogMessage(-1, "Solution '" + ToUtf8(solution->Name()) + "' cleaned successfully.");
        }
        else
        {
            LogMessage(-1, "Solution '" + ToUtf8(solution->Name()) + "' built successfully.");
        }
    }
}

void BuildMsBuildProject(const std::string& projectName, const std::string& projectDirectory, const std::string& target, 
    const std::vector<std::string>& sourceFiles, const std::vector<std::string>& resourceFiles, const std::vector<std::string>& referenceFiles,
    std::unique_ptr<Module>& rootModule)
{
    std::set<std::string> builtProjects;
    std::string projectFilePath = GetFullPath(Path::Combine(projectDirectory, projectName + ".cmproj"));
    std::unique_ptr<Project> project(new Project(ToUtf32(projectName), projectFilePath, GetConfig(), sngcm::ast::BackEnd::llvm, "", SystemDirKind::regular));
    if (target == "program")
    {
        project->AddDeclaration(new TargetDeclaration(Target::program));
    }
    else if (target == "winguiapp")
    {
        project->AddDeclaration(new TargetDeclaration(Target::winguiapp));
    }
    else if (target == "winapp")
    {
        project->AddDeclaration(new TargetDeclaration(Target::winapp));
    }
    else if (target == "library")
    {
        project->AddDeclaration(new TargetDeclaration(Target::library));
    }
    else if (target == "winlib")
    {
        project->AddDeclaration(new TargetDeclaration(Target::winlib));
    }
    else if (target == "unitTest")
    {
        project->AddDeclaration(new TargetDeclaration(Target::unitTest));
    }
    else
    {
        throw std::runtime_error("unsupported target '" + target + "'");
    }
    for (const std::string& sourceFile : sourceFiles)
    {
        project->AddDeclaration(new SourceFileDeclaration(sourceFile));
    }
    for (const std::string& resourceFile : resourceFiles)
    {
        project->AddDeclaration(new ResourceFileDeclaration(resourceFile));
    }
    for (const std::string& referenceFile : referenceFiles)
    {
        project->AddDeclaration(new ReferenceDeclaration(referenceFile));
    }
    project->ResolveDeclarations();
    project->SetLogStreamId(-1);
    stopBuild = false;
    BuildProject(project.get(), rootModule, stopBuild, false, builtProjects);
}

} } // namespace cmajor::build
