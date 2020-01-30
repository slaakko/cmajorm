// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/Build.hpp>
#include <cmajor/codegen/EmittingContext.hpp>
#include <cmajor/codegen/Interface.hpp>
#include <soulng/lexer/XmlParsingLog.hpp>
#include <sngcm/cmlexer/ContainerFileLexer.hpp>
#include <sngcm/cmlexer/CmajorLexer.hpp>
#include <sngcm/cmparser/ProjectFile.hpp>
#include <sngcm/cmparser/SolutionFile.hpp>
#include <sngcm/cmparser/CompileUnit.hpp>
#include <soulng/lexer/ParsingException.hpp>
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
#include <cmajor/ast2dom/Ast2Dom.hpp>
#include <cmajor/bdt2dom/Bdt2Dom.hpp>
#include <cmajor/cmdoclib/Input.hpp>
#include <cmajor/cmdoclib/Global.hpp>
#include <cmajor/cmdoclib/ParserDoc.hpp>
#include <cmajor/cmdoclib/SourceCodePrinter.hpp>
#include <cmajor/cmdoclib/SymbolTableXml.hpp>
#include <cmajor/cmdoclib/File.hpp>
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

bool stopBuild = false;

void StopBuild()
{
    stopBuild = true;
}

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
    for (const std::string& sourceFilePath : sourceFilePaths)
    {
        if (stop)
        {
            return std::vector<std::unique_ptr<CompileUnitNode>>();
        }
        if (boost::filesystem::file_size(sourceFilePath) == 0)
        {
            std::unique_ptr<CompileUnitNode> compileUnit(new CompileUnitNode(Span(), sourceFilePath));
            compileUnits.push_back(std::move(compileUnit));
        }
        else
        {
            MappedInputFile sourceFile(sourceFilePath);
            int32_t fileIndex = module->GetFileTable().RegisterFilePath(sourceFilePath);
            ParsingContext parsingContext;
            std::u32string s(ToUtf32(std::string(sourceFile.Begin(), sourceFile.End())));
            std::unique_ptr<CmajorLexer> lexer(new CmajorLexer(s, sourceFilePath, fileIndex));
            soulng::lexer::XmlParsingLog log(std::cout);
            if (GetGlobalFlag(GlobalFlags::debugParsing))
            {
                lexer->SetLog(&log);
            }
            std::unique_ptr<CompileUnitNode> compileUnit = CompileUnitParser::Parse(*lexer, &parsingContext);
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
    module->SetLexers(std::move(lexers));
    return compileUnits;
}

struct ParserData
{
    ParserData(const std::vector<std::string>& sourceFilePaths_, std::vector<std::unique_ptr<CompileUnitNode>>& compileUnits_, std::vector<std::unique_ptr<CmajorLexer>>& lexers_,
        const std::vector<uint32_t>& fileIndeces_, std::vector<std::exception_ptr>& exceptions_, bool& stop_) :
        sourceFilePaths(sourceFilePaths_), compileUnits(compileUnits_), lexers(lexers_), fileIndeces(fileIndeces_), stop(stop_), exceptions(exceptions_)
    {
    }
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
                std::unique_ptr<CompileUnitNode> compileUnit(new CompileUnitNode(Span(), sourceFilePath));
                parserData->compileUnits[index].reset(compileUnit.release());
            }
            else
            {
                MappedInputFile sourceFile(sourceFilePath);
                ParsingContext parsingContext;
                int fileIndex = parserData->fileIndeces[index];
                std::u32string s(ToUtf32(std::string(sourceFile.Begin(), sourceFile.End())));
                std::unique_ptr<CmajorLexer> lexer(new CmajorLexer(s, sourceFilePath, fileIndex));
                std::unique_ptr<CompileUnitNode> compileUnit = CompileUnitParser::Parse(*lexer, &parsingContext);
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
    ParserData parserData(sourceFilePaths, compileUnits, lexers, fileIndeces, exceptions, stop);
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
    module->SetLexers(std::move(lexers));
    return compileUnits;
}

std::vector<std::unique_ptr<CompileUnitNode>> ParseSources(Module* module, const std::vector<std::string>& sourceFilePaths, bool& stop)
{
    try
    {
        int numCores = std::thread::hardware_concurrency();
        if (numCores == 0 || sourceFilePaths.size() < numCores || GetGlobalFlag(GlobalFlags::debugParsing))
        {
            return ParseSourcesInMainThread(module, sourceFilePaths, stop);
        }
        else
        {
            return ParseSourcesConcurrently(module, sourceFilePaths, numCores, stop);
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
        throw Exception(module, ex.what(), ex.GetSpan(), ex.PrevSpan());
    }
}

void Preprocess(std::vector<std::unique_ptr<CompileUnitNode>>& compileUnits)
{
    for (std::unique_ptr<CompileUnitNode>& compileUnit : compileUnits)
    {
        if (compileUnit->GlobalNs()->HasUnnamedNs())
        {
            sngcm::ast::AddNamespaceImportsForUnnamedNamespaces(*compileUnit);
        }
    }
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
    std::string libErrorFilePath = Path::Combine(Path::GetDirectoryName(libraryFilePath), "llvm-lib.error");
    std::string libCommandLine = "cmfileredirector -2 " + libErrorFilePath + " llvm-lib";
    for (const std::string& arg : args)
    {
        libCommandLine.append(1, ' ').append(arg);
    }
    try
    {
        System(libCommandLine);
        boost::filesystem::remove(boost::filesystem::path(libErrorFilePath));
    }
    catch (const std::exception& ex)
    {
        std::string errors = ReadFile(libErrorFilePath);
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
    module->SetCurrentToolName(U"cmsxar");
    std::vector<std::string> args;
    args.push_back("-o=" + QuotedPath(libraryFilePath));
    int n = objectFilePaths.size();
    for (int i = 0; i < n; ++i)
    {
        args.push_back(QuotedPath(objectFilePaths[i]));
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            LogMessage(module->LogStreamId(), "> " + QuotedPath(objectFilePaths[i]));
        }
    }
    std::string libErrorFilePath = Path::Combine(Path::GetDirectoryName(libraryFilePath), "cmsxar.error");
    std::string libCommandLine = "cmfileredirector -2 " + libErrorFilePath + " cmsxar";
    for (const std::string& arg : args)
    {
        libCommandLine.append(1, ' ').append(arg);
    }
    try
    {
        System(libCommandLine);
        boost::filesystem::remove(boost::filesystem::path(libErrorFilePath));
    }
    catch (const std::exception& ex)
    {
        std::string errors = ReadFile(libErrorFilePath);
        throw std::runtime_error("generating library '" + libraryFilePath + "' failed: " + ex.what() + ":\nerrors:\n" + errors);
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
    std::string libErrorFilePath = Path::Combine(Path::GetDirectoryName(libraryFilePath), "ar.error");
    std::string libCommandLine = "cmfileredirector -2 " + libErrorFilePath + " ar q";
    for (const std::string& arg : args)
    {
        libCommandLine.append(1, ' ').append(arg);
    }
    try
    {
        System(libCommandLine);
        boost::filesystem::remove(boost::filesystem::path(libErrorFilePath));
    }
    catch (const std::exception& ex)
    {
        std::string errors = ReadFile(libErrorFilePath);
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
#ifdef _WIN32
    else if (GetBackEnd() == cmajor::symbols::BackEnd::cmsx)
    {
        GenerateLibrarySystemX(module, objectFilePaths, libraryFilePath);
    }
#endif
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

void LinkLlvm(const std::string& executableFilePath, const std::string& libraryFilePath, const std::vector<std::string>& libraryFilePaths, Module& module)
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
    args.push_back("/entry:main");
    args.push_back("/debug");
    args.push_back("/out:" + QuotedPath(executableFilePath));
    args.push_back("/stack:16777216");
    std::string cmrtLibName = "cmrt330.lib";
    if (GetGlobalFlag(GlobalFlags::linkWithDebugRuntime))
    {
        cmrtLibName = "cmrt330d.lib";
    }
    args.push_back(QuotedPath(Path::Combine(Path::Combine(CmajorRootDir(), "lib"), cmrtLibName)));
    int n = libraryFilePaths.size();
    for (int i = 0; i < n; ++i)
    {
        args.push_back(QuotedPath(libraryFilePaths[i]));
    }
    std::string linkCommandLine;
    std::string linkErrorFilePath;
    if (GetGlobalFlag(GlobalFlags::linkUsingMsLink))
    {
        linkErrorFilePath = Path::Combine(Path::GetDirectoryName(executableFilePath), "link.error");
        linkCommandLine = "cmfileredirector -2 " + linkErrorFilePath + " link";
    }
    else
    {
        linkErrorFilePath = Path::Combine(Path::GetDirectoryName(executableFilePath), "lld-link.error");
        linkCommandLine = "cmfileredirector -2 " + linkErrorFilePath + " lld-link";
    }
    for (const std::string& arg : args)
    {
        linkCommandLine.append(1, ' ').append(arg);
    }
    try
    {
        System(linkCommandLine);
        boost::filesystem::remove(boost::filesystem::path(linkErrorFilePath));
    }
    catch (const std::exception& ex)
    {
        std::string errors = ReadFile(linkErrorFilePath);
        throw std::runtime_error("linking executable '" + executableFilePath + "' failed: " + ex.what() + ":\nerrors:\n" + errors);
    }
    if (GetGlobalFlag(GlobalFlags::verbose) && !GetGlobalFlag(GlobalFlags::unitTest))
    {
        LogMessage(module.LogStreamId(), "==> " + executableFilePath);
    }
}

void LinkSystemX(const std::string& executableFilePath, const std::string& libraryFilePath, const std::vector<std::string>& libraryFilePaths, Module& module)
{
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(module.LogStreamId(), "Linking...");
    }
    std::string classIdFileName = Path::ChangeExtension(libraryFilePath, ".clsid");
    MakeClassIdFile(module.GetSymbolTable().PolymorphicClasses(), classIdFileName);
    module.SetCurrentToolName(U"cmsxlink");
    boost::filesystem::path bdp = executableFilePath;
    bdp.remove_filename();
    boost::filesystem::create_directories(bdp);
    std::vector<std::string> args;
    args.push_back("--clsid=" + QuotedPath(classIdFileName));
    args.push_back("--out=" + QuotedPath(executableFilePath));
    int n = libraryFilePaths.size();
    for (int i = n - 1; i >= 0; --i)
    {
        args.push_back(QuotedPath(libraryFilePaths[i]));
    }
    std::string linkCommandLine;
    std::string linkErrorFilePath;
    linkErrorFilePath = Path::Combine(Path::GetDirectoryName(executableFilePath), "cmsxlink.error");
    linkCommandLine = "cmfileredirector -2 " + linkErrorFilePath + " cmsxlink";
    for (const std::string& arg : args)
    {
        linkCommandLine.append(1, ' ').append(arg);
    }
    try
    {
        System(linkCommandLine);
        boost::filesystem::remove(boost::filesystem::path(linkErrorFilePath));
    }
    catch (const std::exception& ex)
    {
        std::string errors = ReadFile(linkErrorFilePath);
        throw std::runtime_error("linking executable '" + executableFilePath + "' failed: " + ex.what() + ":\nerrors:\n" + errors);
    }
    if (GetGlobalFlag(GlobalFlags::verbose) && !GetGlobalFlag(GlobalFlags::unitTest))
    {
        LogMessage(module.LogStreamId(), "==> " + executableFilePath);
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

void LinkLlvm(const std::string& executableFilePath, const std::string& libraryFilePath, const std::vector<std::string>& libraryFilePaths, Module& module)
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
        args.push_back("-lgmpintf");
    }
    else
    {
        std::string cmrtLibName = "cmrt";
        args.push_back("-l" + cmrtLibName);
        args.push_back("-lgmpintf");
    }
    args.push_back("-lboost_filesystem -lboost_iostreams -lboost_system -lgmp -lbz2 -lz");
    args.push_back("-Xlinker --end-group");
    args.push_back("-o " + QuotedPath(executableFilePath));
    std::string linkCommandLine;
    std::string linkErrorFilePath;
    linkErrorFilePath = Path::Combine(Path::GetDirectoryName(executableFilePath), "clang++.error");
    linkCommandLine = "cmfileredirector -2 " + linkErrorFilePath + " clang++";
    for (const std::string& arg : args)
    {
        linkCommandLine.append(1, ' ').append(arg);
    }
    try
    {
        System(linkCommandLine);
        boost::filesystem::remove(boost::filesystem::path(linkErrorFilePath));
    }
    catch (const std::exception& ex)
    {
        std::string errors = ReadFile(linkErrorFilePath);
        throw std::runtime_error("linking executable '" + executableFilePath + "' failed: " + ex.what() + ":\nerrors:\n" + errors);
    }
    if (GetGlobalFlag(GlobalFlags::verbose) && !GetGlobalFlag(GlobalFlags::unitTest))
    {
        LogMessage(module.LogStreamId(), "==> " + executableFilePath);
    }
}

#endif

void Link(const std::string& executableFilePath, const std::string& libraryFilePath, const std::vector<std::string>& libraryFilePaths, Module& module)
{
    if (GetBackEnd() == cmajor::symbols::BackEnd::llvm)
    {
        LinkLlvm(executableFilePath, libraryFilePath, libraryFilePaths, module);
    }
#ifdef _WIN32
    else if (GetBackEnd() == cmajor::symbols::BackEnd::cmsx)
    {
        LinkSystemX(executableFilePath, libraryFilePath, libraryFilePaths, module);
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
    mfp.remove_filename();
    boost::filesystem::remove_all(mfp);
    if (project->GetTarget() == Target::program || project->GetTarget() == Target::winapp)
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
        throw Exception(&module, "program has no main function", Span());
    }
    if (!userMain->Parameters().empty())
    {
        if (userMain->Parameters().size() != 2)
        {
            throw Exception(&module, "main function must either take no parameters or take two parameters", userMain->GetSpan());
        }
        if (!TypesEqual(userMain->Parameters()[0]->GetType(), module.GetSymbolTable().GetTypeByName(U"int")))
        {
            throw Exception(&module, "first parameter of main function must be of int type", userMain->GetSpan());
        }
        if (!TypesEqual(userMain->Parameters()[1]->GetType(), module.GetSymbolTable().GetTypeByName(U"char")->AddConst(userMain->GetSpan())->AddPointer(userMain->GetSpan())->AddPointer(userMain->GetSpan())))
        {
            throw Exception(&module, "second parameter of main function must be of 'const char**' type", userMain->GetSpan());
        }
    }
    if (userMain->ReturnType() && !userMain->ReturnType()->IsVoidType())
    {
        if (!TypesEqual(userMain->ReturnType(), module.GetSymbolTable().GetTypeByName(U"int")))
        {
            throw Exception(&module, "main function must either be void function or return an int", userMain->GetSpan());
        }
    }
}

void CreateJsonRegistrationUnit(std::vector<std::string>& objectFilePaths, Module& module, cmajor::codegen::EmittingContext& emittingContext, AttributeBinder* attributeBinder)
{
    CompileUnitNode jsonRegistrationCompileUnit(Span(), boost::filesystem::path(module.OriginalFilePath()).parent_path().append("__json__.cm").generic_string());
    jsonRegistrationCompileUnit.SetSynthesizedUnit();
    jsonRegistrationCompileUnit.GlobalNs()->AddMember(new NamespaceImportNode(Span(), new IdentifierNode(Span(), U"System")));
    jsonRegistrationCompileUnit.GlobalNs()->AddMember(new NamespaceImportNode(Span(), new IdentifierNode(Span(), U"System.Json")));
    FunctionNode* jsonRegistrationFunction(new FunctionNode(Span(), Specifiers::public_, new IntNode(Span()), U"RegisterJsonClasses", nullptr));
    jsonRegistrationFunction->SetReturnTypeExpr(new VoidNode(Span()));
    CompoundStatementNode* jsonRegistrationFunctionBody = new CompoundStatementNode(Span());
    const std::unordered_set<std::u32string>& jsonClasses = module.GetSymbolTable().JsonClasses();
    for (const std::u32string& jsonClass : jsonClasses)
    {
        InvokeNode* invokeRegisterJsonClass = new InvokeNode(Span(), new IdentifierNode(Span(), U"RegisterJsonClass"));
        invokeRegisterJsonClass->AddArgument(new TypeNameNode(Span(), new IdentifierNode(Span(), jsonClass)));
        invokeRegisterJsonClass->AddArgument(new DotNode(Span(), new IdentifierNode(Span(), jsonClass), new IdentifierNode(Span(), U"Create")));
        ExpressionStatementNode* registerStatement = new ExpressionStatementNode(Span(), invokeRegisterJsonClass);
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

void CreateMainUnitLlvm(std::vector<std::string>& objectFilePaths, Module& module, cmajor::codegen::EmittingContext& emittingContext, AttributeBinder* attributeBinder)
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
    ExpressionStatementNode* rtInitCall = nullptr;
    if (GetGlobalFlag(GlobalFlags::profile))
    {
        InvokeNode* invokeRtInit = new InvokeNode(Span(), new IdentifierNode(Span(), U"RtStartProfiling"));
        invokeRtInit->AddArgument(new DivNode(Span(),
            new InvokeNode(Span(), new DotNode(Span(), new IdentifierNode(Span(), U"@polymorphicClassArray"), new IdentifierNode(Span(), U"Length"))),
            new LongLiteralNode(Span(), 3))); // 3 64-bit integers per entry
        invokeRtInit->AddArgument(new InvokeNode(Span(), new DotNode(Span(), new IdentifierNode(Span(), U"@polymorphicClassArray"), new IdentifierNode(Span(), U"CBegin"))));
        invokeRtInit->AddArgument(new DivNode(Span(),
            new InvokeNode(Span(), new DotNode(Span(), new IdentifierNode(Span(), U"@staticClassArray"), new IdentifierNode(Span(), U"Length"))),
            new LongLiteralNode(Span(), 2))); // 2 64-bit integers per entry
        invokeRtInit->AddArgument(new InvokeNode(Span(), new DotNode(Span(), new IdentifierNode(Span(), U"@staticClassArray"), new IdentifierNode(Span(), U"CBegin"))));
        rtInitCall = new ExpressionStatementNode(Span(), invokeRtInit);
    }
    else
    {
        InvokeNode* invokeRtInit = new InvokeNode(Span(), new IdentifierNode(Span(), U"RtInit"));
        invokeRtInit->AddArgument(new DivNode(Span(), 
            new InvokeNode(Span(), new DotNode(Span(), new IdentifierNode(Span(), U"@polymorphicClassArray"), new IdentifierNode(Span(), U"Length"))),
            new LongLiteralNode(Span(), 3))); // 3 64-bit integers per entry
        invokeRtInit->AddArgument(new InvokeNode(Span(), new DotNode(Span(), new IdentifierNode(Span(), U"@polymorphicClassArray"), new IdentifierNode(Span(), U"CBegin"))));
        invokeRtInit->AddArgument(new DivNode(Span(),
            new InvokeNode(Span(), new DotNode(Span(), new IdentifierNode(Span(), U"@staticClassArray"), new IdentifierNode(Span(), U"Length"))),
            new LongLiteralNode(Span(), 2))); // 2 64-bit integers per entry
        invokeRtInit->AddArgument(new InvokeNode(Span(), new DotNode(Span(), new IdentifierNode(Span(), U"@staticClassArray"), new IdentifierNode(Span(), U"CBegin"))));
        rtInitCall = new ExpressionStatementNode(Span(), invokeRtInit);
    }
    mainFunctionBody->AddStatement(rtInitCall);
#ifdef _WIN32
    ConstructionStatementNode* argc = new ConstructionStatementNode(Span(), new IntNode(Span()), new IdentifierNode(Span(), U"argc"));
    argc->AddArgument(new InvokeNode(Span(), new IdentifierNode(Span(), U"RtArgc")));
    mainFunctionBody->AddStatement(argc);
    ConstructionStatementNode* argv = new ConstructionStatementNode(Span(), new ConstNode(Span(), new PointerNode(Span(), new PointerNode(Span(), new CharNode(Span())))), new IdentifierNode(Span(), U"argv"));
    argv->AddArgument(new InvokeNode(Span(), new IdentifierNode(Span(), U"RtArgv")));
    mainFunctionBody->AddStatement(argv);
#endif
    CompoundStatementNode* tryBlock = new CompoundStatementNode(Span());
    if (!module.GetSymbolTable().JsonClasses().empty())
    {
        ExpressionStatementNode* registerJsonClassesCall = new ExpressionStatementNode(Span(), new InvokeNode(Span(), new IdentifierNode(Span(), U"RegisterJsonClasses")));
        tryBlock->AddStatement(registerJsonClassesCall);
    }
    FunctionSymbol* userMain = module.GetSymbolTable().MainFunctionSymbol();
    InvokeNode* invokeMain = new InvokeNode(Span(), new IdentifierNode(Span(), userMain->GroupName()));
    if (!userMain->Parameters().empty())
    {
        invokeMain->AddArgument(new IdentifierNode(Span(), U"argc"));
        invokeMain->AddArgument(new IdentifierNode(Span(), U"argv"));
    }
    StatementNode* callMainStatement = nullptr;
    if (!userMain->ReturnType() || userMain->ReturnType()->IsVoidType())
    {
        callMainStatement = new ExpressionStatementNode(Span(), invokeMain);
    }
    else
    {
        callMainStatement = new AssignmentStatementNode(Span(), new IdentifierNode(Span(), U"exitCode"), invokeMain);
    }
    InvokeNode* invokeInitialize = new InvokeNode(Span(), new IdentifierNode(Span(), U"Initialize"));
    StatementNode* callInitializeStatement = new ExpressionStatementNode(Span(), invokeInitialize);
    tryBlock->AddStatement(callInitializeStatement);
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
    AssignmentStatementNode* assignExitCodeStatement = new AssignmentStatementNode(Span(), new IdentifierNode(Span(), U"exitCode"), new IntLiteralNode(Span(), 1));
    catchBlock->AddStatement(assignExitCodeStatement);
    CatchNode* catchAll = new CatchNode(Span(), new ConstNode(Span(), new LValueRefNode(Span(), new IdentifierNode(Span(), U"System.Exception"))), new IdentifierNode(Span(), U"ex"), catchBlock);
    tryStatement->AddCatch(catchAll);
    mainFunctionBody->AddStatement(tryStatement);
    ExpressionStatementNode* rtDoneCall = nullptr;
    if (GetGlobalFlag(GlobalFlags::profile))
    {
        rtDoneCall = new ExpressionStatementNode(Span(), new InvokeNode(Span(), new IdentifierNode(Span(), U"RtEndProfiling")));
    }
    else
    {
        rtDoneCall = new ExpressionStatementNode(Span(), new InvokeNode(Span(), new IdentifierNode(Span(), U"RtDone")));
    }
    mainFunctionBody->AddStatement(rtDoneCall);
    InvokeNode* exitCall = new InvokeNode(Span(), new IdentifierNode(Span(), U"RtExit"));
    exitCall->AddArgument(new IdentifierNode(Span(), U"exitCode"));
    ExpressionStatementNode* rtExitCall = new ExpressionStatementNode(Span(), exitCall);
    mainFunctionBody->AddStatement(rtExitCall);
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
    cmajor::codegen::GenerateCode(emittingContext, boundMainCompileUnit);
    objectFilePaths.push_back(boundMainCompileUnit.ObjectFilePath());
}

void CreateMainUnitSystemX(std::vector<std::string>& objectFilePaths, Module& module, cmajor::codegen::EmittingContext& emittingContext, AttributeBinder* attributeBinder)
{
    CompileUnitNode mainCompileUnit(Span(), boost::filesystem::path(module.OriginalFilePath()).parent_path().append("__main__.cm").generic_string());
    mainCompileUnit.SetSynthesizedUnit();
    mainCompileUnit.GlobalNs()->AddMember(new NamespaceImportNode(Span(), new IdentifierNode(Span(), U"System")));
    FunctionNode* mainFunction(new FunctionNode(Span(), Specifiers::public_, new IntNode(Span()), U"main", nullptr));
    mainFunction->AddParameter(new ParameterNode(Span(), new IntNode(Span()), new IdentifierNode(Span(), U"argc")));
    mainFunction->AddParameter(new ParameterNode(Span(), new PointerNode(Span(), new PointerNode(Span(), new CharNode(Span()))), new IdentifierNode(Span(), U"argv")));
    mainFunction->AddParameter(new ParameterNode(Span(), new PointerNode(Span(), new PointerNode(Span(), new CharNode(Span()))), new IdentifierNode(Span(), U"envp")));
    mainFunction->SetProgramMain();
    CompoundStatementNode* mainFunctionBody = new CompoundStatementNode(Span());
    ConstructionStatementNode* constructExitCode = new ConstructionStatementNode(Span(), new IntNode(Span()), new IdentifierNode(Span(), U"exitCode"));
    mainFunctionBody->AddStatement(constructExitCode);
    CompoundStatementNode* tryBlock = new CompoundStatementNode(Span());
    InvokeNode* invokeSetupEnvironment = new InvokeNode(Span(), new IdentifierNode(Span(), U"StartupSetupEnvironment"));
    invokeSetupEnvironment->AddArgument(new IdentifierNode(Span(), U"envp"));
    StatementNode* callSetEnvironmentStatement = new ExpressionStatementNode(Span(), invokeSetupEnvironment);
    tryBlock->AddStatement(callSetEnvironmentStatement);
    FunctionSymbol* userMain = module.GetSymbolTable().MainFunctionSymbol();
    InvokeNode* invokeMain = new InvokeNode(Span(), new IdentifierNode(Span(), userMain->GroupName()));
    if (!userMain->Parameters().empty())
    {
        invokeMain->AddArgument(new IdentifierNode(Span(), U"argc"));
        invokeMain->AddArgument(new IdentifierNode(Span(), U"argv"));
    }
    StatementNode* callMainStatement = nullptr;
    if (!userMain->ReturnType() || userMain->ReturnType()->IsVoidType())
    {
        callMainStatement = new ExpressionStatementNode(Span(), invokeMain);
    }
    else
    {
        callMainStatement = new AssignmentStatementNode(Span(), new IdentifierNode(Span(), U"exitCode"), invokeMain);
    }
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
    AssignmentStatementNode* assignExitCodeStatement = new AssignmentStatementNode(Span(), new IdentifierNode(Span(), U"exitCode"), new IntLiteralNode(Span(), 1));
    catchBlock->AddStatement(assignExitCodeStatement);
    CatchNode* catchAll = new CatchNode(Span(), new ConstNode(Span(), new LValueRefNode(Span(), new IdentifierNode(Span(), U"System.Exception"))), new IdentifierNode(Span(), U"ex"), catchBlock);
    tryStatement->AddCatch(catchAll);
    mainFunctionBody->AddStatement(tryStatement);
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
    cmajor::codegen::GenerateCode(emittingContext, boundMainCompileUnit);
    objectFilePaths.insert(objectFilePaths.begin(), boundMainCompileUnit.ObjectFilePath());
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
    boost::filesystem::path systemLibDir = CmajorSystemLibDir(GetConfig(), backend);
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
        if (!systemModule->LibraryFilePath().empty())
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
    boost::filesystem::path systemLibDir = CmajorSystemLibDir(GetConfig(), backend);
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
        if (!systemModule->LibraryFilePath().empty())
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
    CompileQueue(const std::string& name_, bool& stop_, std::atomic<bool>& ready_, int logStreamId_);
    void Put(int compileUnitIndex);
    int Get();
private:
    std::string name;
    std::list<int> queue;
    std::mutex mtx;
    std::condition_variable cond;
    bool& stop;
    std::atomic<bool>& ready;
    int logStreamId;
};

CompileQueue::CompileQueue(const std::string& name_, bool& stop_, std::atomic<bool>& ready_, int logStreamId_) : name(name_), stop(stop_), ready(ready_), logStreamId(logStreamId_)
{
}

void CompileQueue::Put(int compileUnitIndex)
{
    std::lock_guard<std::mutex> lock(mtx);
    queue.push_back(compileUnitIndex);
    cond.notify_one();
}

int CompileQueue::Get()
{
    while (!stop && !ready)
    {
        std::unique_lock<std::mutex> lock(mtx);
        if (cond.wait_for(lock, std::chrono::duration<std::uint64_t>(std::chrono::seconds(compileGetTimeoutSecs)), [this] { return stop || ready || !queue.empty(); }))
        {
            if (stop || ready) return -1;
            int compileUnitIndex = queue.front();
            queue.pop_front();
            return compileUnitIndex;
        }
        else
        {
            return -1;
        }
    }
    return -1;
}

struct CompileData
{
    CompileData(Module* rootModule_, std::vector<std::unique_ptr<BoundCompileUnit>>& boundCompileUnits_, std::vector<std::string>& objectFilePaths_, bool& stop_, std::atomic<bool>& ready_,
        int numThreads_, CompileQueue& input_, CompileQueue& output_) :
        rootModule(rootModule_), boundCompileUnits(boundCompileUnits_), objectFilePaths(objectFilePaths_), stop(stop_), ready(ready_), numThreads(numThreads_),
        input(input_), output(output_)
    {
        exceptions.resize(numThreads);
        sourceFileFilePaths.resize(boundCompileUnits.size());
        for (int i = 0; i < boundCompileUnits.size(); ++i)
        {
            sourceFileFilePaths[i] = boundCompileUnits[i]->GetCompileUnitNode()->FilePath();
        }
    }
    Module* rootModule;
    std::vector<std::string> sourceFileFilePaths;
    std::vector<std::unique_ptr<BoundCompileUnit>>& boundCompileUnits;
    std::vector<std::string>& objectFilePaths;
    bool& stop;
    std::atomic<bool>& ready;
    int numThreads;
    CompileQueue& input;
    CompileQueue& output;
    std::vector<std::exception_ptr> exceptions;
    std::mutex mtx;
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
                std::lock_guard<std::mutex> lock(data->mtx);
                data->objectFilePaths.push_back(compileUnit->ObjectFilePath());
                data->boundCompileUnits[compileUnitIndex].reset();
                data->output.Put(compileUnitIndex);
            }
        }
    }
    catch (...)
    {
        std::exception_ptr exception = std::current_exception();
        if (threadId >= 0 && threadId < data->exceptions.size())
        {
            data->exceptions[threadId] = exception;
        }
        data->stop = true;
    }
}

void CompileMultiThreaded(Project* project, Module* rootModule, std::vector<std::unique_ptr<BoundCompileUnit>>& boundCompileUnits, std::vector<std::string>& objectFilePaths, 
    bool& stop)
{
    int numThreads = std::thread::hardware_concurrency();
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
    std::atomic<bool> ready(false);
    CompileQueue input("input", stop, ready, rootModule->LogStreamId());
    CompileQueue output("output", stop, ready, rootModule->LogStreamId());
    CompileData compileData(rootModule, boundCompileUnits, objectFilePaths, stop, ready, numThreads, input, output);
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
            for (int i = 0; i < numThreads; ++i)
            {
                input.Put(-1);
            }
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
        std::lock_guard<std::mutex> lock(compileData.mtx);
        ready = true;
    }
    if (GetGlobalFlag(GlobalFlags::debugCompile))
    {
        LogMessage(rootModule->LogStreamId(), ToUtf8(rootModule->Name()) + " < END READY");
    }
    for (int i = 0; i < numThreads; ++i)
    {
        input.Put(-1);
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

void BuildProject(Project* project, std::unique_ptr<Module>& rootModule, bool& stop, bool resetRootModule)
{
    try
    {
        std::string config = GetConfig();
        bool isSystemModule = IsSystemModule(project->Name());
        if (isSystemModule)
        {
            project->SetSystemProject();
        }
        bool upToDate = false;
        if (!GetGlobalFlag(GlobalFlags::rebuild))
        {
            sngcm::ast::BackEnd astBackEnd = sngcm::ast::BackEnd::llvm;
            if (GetBackEnd() == cmajor::symbols::BackEnd::cmsx)
            {
                astBackEnd = sngcm::ast::BackEnd::cmsx;
            }
            upToDate = project->IsUpToDate(CmajorSystemModuleFilePath(config, astBackEnd));
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
            Preprocess(compileUnits);
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
                //cmdoclib::GeneratePPXml(project); 
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
                if (project->GetTarget() == Target::program || project->GetTarget() == Target::winapp)
                {
                    CheckMainFunctionSymbol(*rootModule);
                    if (!rootModule->GetSymbolTable().JsonClasses().empty())
                    {
                        CreateJsonRegistrationUnit(objectFilePaths, *rootModule, emittingContext, &attributeBinder);
                    }
                    if (GetBackEnd() == cmajor::symbols::BackEnd::llvm)
                    {
                        CreateMainUnitLlvm(objectFilePaths, *rootModule, emittingContext, &attributeBinder);
                    }
                    else if (GetBackEnd() == cmajor::symbols::BackEnd::cmsx)
                    {
                        CreateMainUnitSystemX(objectFilePaths, *rootModule, emittingContext, &attributeBinder);
                    }
                }
                if (!objectFilePaths.empty())
                {
                    GenerateLibrary(rootModule.get(), objectFilePaths, project->LibraryFilePath());
                }
                if (project->GetTarget() == Target::program || project->GetTarget() == Target::winapp)
                {
                    Link(project->ExecutableFilePath(), project->LibraryFilePath(), rootModule->LibraryFilePaths(), *rootModule);
                }
                if (GetGlobalFlag(GlobalFlags::verbose))
                {
                    LogMessage(project->LogStreamId(), "Writing module file...");
                }
                SymbolWriter writer(project->ModuleFilePath());
                writer.SetLexers(rootModule->GetLexers());
                rootModule->ResetFlag(cmajor::symbols::ModuleFlags::compiling);
                rootModule->Write(writer);
                if (GetGlobalFlag(GlobalFlags::verbose))
                {
                    LogMessage(project->LogStreamId(), "==> " + project->ModuleFilePath());
                }
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
    catch (const std::exception& ex)
    {
        LogMessage(-1, "project: " + ToUtf8(project->Name()) + ": " + ex.what());
        throw;
    }
}

void BuildProject(const std::string& projectFilePath, std::unique_ptr<Module>& rootModule)
{
    std::string config = GetConfig();
    MappedInputFile projectFile(projectFilePath);
    std::u32string p(ToUtf32(std::string(projectFile.Begin(), projectFile.End())));
    sngcm::ast::BackEnd backend = sngcm::ast::BackEnd::llvm;
    if (GetBackEnd() == cmajor::symbols::BackEnd::cmsx)
    {
        backend = sngcm::ast::BackEnd::cmsx;
    }
    ContainerFileLexer containerFileLexer(p, projectFilePath, 0);
    std::unique_ptr<Project> project = ProjectFileParser::Parse(containerFileLexer, config, backend);
    project->ResolveDeclarations();
    if (GetGlobalFlag(GlobalFlags::clean))
    {
        CleanProject(project.get());
    }
    else
    {
        stopBuild = false;
        BuildProject(project.get(), rootModule, stopBuild, true);
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
    ProjectQueue(bool& stop_, const std::string& name_);
    void Put(Project* project);
    Project* Get();
private:
    std::string name;
    std::list<Project*> queue;
    std::mutex mtx;
    std::condition_variable cond;
    bool& stop;
};

ProjectQueue::ProjectQueue(bool& stop_, const std::string& name_) : stop(stop_), name(name_)
{
}

void ProjectQueue::Put(Project* project)
{
    if (buildDebug)
    {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        LogMessage(-1, CurrentProjectDebugMsStr() + ">ProjectQueue(" + name + ")::Put: " + CurrentThreadIdStr() + " " + ToUtf8(project->Name())); 
    }
    std::lock_guard<std::mutex> lock(mtx);
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
        std::unique_lock<std::mutex> lock(mtx);
        if (cond.wait_for(lock, std::chrono::duration<std::uint64_t>(std::chrono::seconds(buildGetTimeoutSecs)), [this]{ return stop || !queue.empty(); }))
        {
            if (stop) return nullptr;
            Project* project = queue.front();
            queue.pop_front();
            if (buildDebug)
            {
                LogMessage(-1, CurrentProjectDebugMsStr() + "<ProjectQueue(" + name + ")::Get: " + CurrentThreadIdStr() + " got project " + ToUtf8(project->Name()));
            }
            return project;
        }
        else
        {
            if (buildDebug)
            {
                LogMessage(-1, CurrentProjectDebugMsStr() + "<ProjectQueue(" + name + ")::Get: " + CurrentThreadIdStr() + " timeout");
            }
            return nullptr;
        }
    }
    if (buildDebug)
    {
        LogMessage(-1, CurrentProjectDebugMsStr() + "<ProjectQueue(" + name + ")::Get: " + CurrentThreadIdStr() + " stop");
    }
    return nullptr;
}

struct BuildData
{
    BuildData(bool& stop_, ProjectQueue& buildQueue_, ProjectQueue& readyQueue_, std::vector<std::unique_ptr<Module>>& rootModules_, bool& isSystemSolution_) :
        stop(stop_), buildQueue(buildQueue_), readyQueue(readyQueue_), rootModules(rootModules_), isSystemSolution(isSystemSolution_)
    {
    }
    bool& stop;
    ProjectQueue& buildQueue;
    ProjectQueue& readyQueue;
    std::vector<std::unique_ptr<Module>>& rootModules;
    std::mutex exceptionMutex;
    std::vector<std::exception_ptr> exceptions;
    bool& isSystemSolution;
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
                BuildProject(toBuild, buildData->rootModules[toBuild->Index()], buildData->stop, true);
                if (toBuild->IsSystemProject())
                {
                    buildData->isSystemSolution = true;
                }
                toBuild->SetBuilt();
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
        std::lock_guard<std::mutex> lock(buildData->exceptionMutex);
        buildData->exceptions.push_back(std::current_exception());
        buildData->stop = true;
        if (buildDebug)
        {
            LogMessage(-1, CurrentProjectDebugMsStr() + "<ProjectBuilder()::catch " + CurrentThreadIdStr());
        }
    }
}

void BuildSolution(const std::string& solutionFilePath, std::vector<std::unique_ptr<Module>>& rootModules)
{
    std::u32string solutionName;
    std::vector<std::u32string> moduleNames;
    BuildSolution(solutionFilePath, rootModules, solutionName, moduleNames);
}

void BuildSolution(const std::string& solutionFilePath, std::vector<std::unique_ptr<Module>>& rootModules, std::u32string& solutionName, std::vector<std::u32string>& moduleNames)
{
    MappedInputFile solutionFile(solutionFilePath);
    std::u32string s(ToUtf32(std::string(solutionFile.Begin(), solutionFile.End())));
    ContainerFileLexer containerFileLexer(s, solutionFilePath, 0);
    std::unique_ptr<Solution> solution = SolutionFileParser::Parse(containerFileLexer);
    solution->ResolveDeclarations();
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
        MappedInputFile projectFile(projectFilePath);
        std::u32string p(ToUtf32(std::string(projectFile.Begin(), projectFile.End())));
        sngcm::ast::BackEnd backend = sngcm::ast::BackEnd::llvm;
        if (GetBackEnd() == cmajor::symbols::BackEnd::cmsx)
        {
            backend = sngcm::ast::BackEnd::cmsx;
        }
        ContainerFileLexer containerFileLexer(p, projectFilePath, 0);
        std::unique_ptr<Project> project = ProjectFileParser::Parse(containerFileLexer, config, backend);
        project->SetRelativeFilePath(relativeProjectFilePath);
        project->ResolveDeclarations();
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
                BuildProject(project, rootModules[i], stopBuild, true);
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
            ProjectQueue buildQueue(stopBuild, "build");
            ProjectQueue readyQueue(stopBuild, "ready");
            BuildData buildData(stopBuild, buildQueue, readyQueue, rootModules, isSystemSolution);
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
                    --numProjectsToBuild;
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
    const std::vector<std::string>& sourceFiles, const std::vector<std::string>& referenceFiles, std::unique_ptr<Module>& rootModule)
{
    std::string projectFilePath = GetFullPath(Path::Combine(projectDirectory, projectName + ".cmproj"));
    std::unique_ptr<Project> project(new Project(ToUtf32(projectName), projectFilePath, GetConfig(), sngcm::ast::BackEnd::llvm));
    if (target == "program")
    {
        project->AddDeclaration(new TargetDeclaration(Target::program));
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
    for (const std::string& referenceFile : referenceFiles)
    {
        project->AddDeclaration(new ReferenceDeclaration(referenceFile));
    }
    project->ResolveDeclarations();
    project->SetLogStreamId(-1);
    stopBuild = false;
    BuildProject(project.get(), rootModule, stopBuild, false);
}

} } // namespace cmajor::build
