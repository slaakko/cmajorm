// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmbs/BuildServer.hpp>
#include <cmajor/cmmsg/BuildServerMessage.hpp>
#include <cmajor/cmbs/Error.hpp>
#include <cmajor/build/Build.hpp>
#include <cmajor/symbols/Module.hpp>
#include <sngcm/ast/Project.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/ModuleCache.hpp>
#include <cmajor/symbols/SourceFileModuleMap.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/cmmid/InitDone.hpp>
#ifdef TRACE
#include <cmajor/cmbs_trace/TraceFunctions.hpp>
#include <soulng/util/Trace.hpp>
#endif
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/Log.hpp>
#include <soulng/util/LogFileWriter.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Socket.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/lexer/ParsingException.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/Document.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <thread>
#include <mutex>
#include <atomic>
#include <iostream>
#include <stdexcept>
#include <time.h>

namespace cmbs {

using namespace sngcm::ast;
using namespace soulng::util;
using namespace soulng::unicode;
using namespace sngxml::dom;
using namespace cmajor::mid;
using namespace cmajor::symbols;

struct BackendSelector
{
    BackendSelector(cmajor::symbols::BackEnd backend)
    {
#ifdef TRACE
        soulng::util::Tracer tracer(BackendSelector_BackendSelector);
#endif // TRACE
        switch (backend)
        {
            case cmajor::symbols::BackEnd::llvm:
            {
                CmmInit(cmajor::mid::BackEndKind::llvmBackEnd);
                break;
            }
            case cmajor::symbols::BackEnd::cmsx:
            {
                CmmInit(cmajor::mid::BackEndKind::cmsxBackEnd);
                break;
            }
            case cmajor::symbols::BackEnd::cmcpp:
            {
                CmmInit(cmajor::mid::BackEndKind::cmcppBackEnd);
                break;
            }
        }
    }
    ~BackendSelector()
    {
#ifdef TRACE
        soulng::util::Tracer tracer(BackendSelector_BackendSelector_dtor);
#endif // TRACE
        CmmDone();
    }
};

std::string CmbsLogFilePath()
{
#ifdef TRACE
    soulng::util::Tracer tracer(CmbsLogFilePath_f);
#endif // TRACE
    return Path::Combine(CmajorLogFileDir(), "cmbs.log");
}

class BuildServer
{
public:
    BuildServer(bool log_, bool progress_);
    ~BuildServer();
    void Start(int port, const std::string& version, std::condition_variable* exitVar, bool* exiting);
    void Stop();
    void Run();
    BuildReply ProcessBuildRequest(const BuildRequest& buildRequest, LogFileWriter* writer);
    CacheModuleReply ProcessCacheModuleRequest(const CacheModuleRequest& cacheModuleRequest);
    GetDefinitionReply ProcessGetDefinitionRequest(const GetDefinitionRequest& getDefinitionRequest);
    void WriteGenericErrorReply(const std::string& messageKind);
    void ProcessCppBackendRequest(const BuildRequest& cppBuildRequest, BuildReply& reply, LogFileWriter* logWriter);
    void ProcessLlvmBackendRequest(const BuildRequest& llvmBuildRequest, BuildReply& reply, LogFileWriter* logWriter);
    void BuildSolution(const std::string& solutionFilePath, std::vector<std::unique_ptr<cmajor::symbols::Module>>& rootModules, BuildReply& reply, LogFileWriter* logWriter);
    void BuildProject(const std::string& projectFilePath, std::unique_ptr<cmajor::symbols::Module>& rootModule,
        std::set<std::string>& builtProjects, BuildReply& reply, LogFileWriter* logWriter);
    void SetRunException(const std::exception_ptr& runException_);
    void SetLogException(const std::exception_ptr& logException_);
    void SetLogExceptionToReply(BuildReply& reply);
    void StartLogging();
    void EndLogging();
    void StartLogThread();
    void RunLog();
    void StopLogThread();
    std::string GetMessageKind(Element* element) const;
    std::string DocumentToString(Document& doc) const;
    std::string ElementToString(Element* element) const;
    void SetRequestInProgress();
    void ResetRequestInProgress();
    bool StopRequested() const { return stopRequested; }
    const std::string& LogFilePath() const { return logFilePath; }
    bool Log() const { return log; }
private:
    int port;
    bool log;
    bool progress;
    std::string version;
    std::atomic_bool exit;
    bool running;
    std::thread serverThread;
    std::thread logThread;
    TcpSocket listenSocket;
    TcpSocket socket;
    std::exception_ptr runException;
    std::exception_ptr logException;
    bool requestInProgress;
    time_t lastActionTime;
    bool stopRequested;
    std::string logFilePath;
    int progressIntervalMs;
    std::condition_variable* exitVar;
    bool* exiting;
};

struct RequestGuard
{
    RequestGuard(BuildServer* server_) : server(server_)
    {
#ifdef TRACE
        soulng::util::Tracer tracer(RequestGuard_RequestGuard);
#endif // TRACE
        server->SetRequestInProgress();
    }
    ~RequestGuard()
    {
#ifdef TRACE
        soulng::util::Tracer tracer(RequestGuard_RequestGuard_dtor);
#endif // TRACE
        server->ResetRequestInProgress();
    }
    BuildServer* server;
};

BuildServer::BuildServer(bool log_, bool progress_) :
    port(54325), log(log_), progress(progress_), version(), exit(false), listenSocket(), socket(), requestInProgress(false),
    lastActionTime(), stopRequested(false), logFilePath(CmbsLogFilePath()), running(false), progressIntervalMs(0), exitVar(nullptr), exiting(nullptr)
{
    if (progress)
    {
        progressIntervalMs = 250;
    }
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServer_BuildServer);
#endif // TRACE
    if (log)
    {
        LogFileWriter writer(logFilePath);
        writer.WriteLine("================================================================================");
        writer.WriteCurrentDateTime();
        writer << "build server created" << std::endl;
        writer << "log=" << log << std::endl;
    }
}

BuildServer::~BuildServer()
{
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServer_BuildServer_dtor);
#endif // TRACE
}

void BuildServer::Run()
{
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServer_Run);
#endif // TRACE
    try
    {
        running = true;
        LogFileWriter writer(logFilePath, log);
        if (log)
        {
            writer.WriteLine("================================================================================");
            writer.WriteCurrentDateTime();
            writer << "build server run: port=" << port << std::endl;
        }
        listenSocket.Bind(port);
        listenSocket.Listen(10);
        while (!exit)
        {
            std::cout << "build-server-ready" << std::endl;
            socket = listenSocket.Accept();
            if (exit)
            {
                if (log)
                {
                    writer << "exiting" << std::endl;
                }
                return;
            }
            RequestGuard requestGuard(this);
            std::string request = ReadStr(socket);
            std::u32string content = ToUtf32(request);
            std::unique_ptr<Document> requestDoc = ParseDocument(content, "socket");
            std::string messageKind = GetMessageKind(requestDoc->DocumentElement());
            if (messageKind == "stopBuildRequest")
            {
                StopBuildReply reply;
                std::unique_ptr<Element> replyElement = reply.ToXml("stopBuildReply");
                std::string replyStr = ElementToString(replyElement.release());
                Write(socket, replyStr);
                if (log)
                {
                    LogFileWriter writer(logFilePath);
                    writer.WriteLine("================================================================================");
                    writer.WriteCurrentDateTime();
                    writer << "stop build request received:" << std::endl;
                    CodeFormatter formatter(writer.LogFile());
                    requestDoc->Write(formatter);
                }
                stopRequested = true;
                if (exitVar && exiting)
                {
                    *exiting = true;
                    exitVar->notify_one();
                }
            }
            else if (messageKind == "buildRequest")
            {
                LogFileWriter writer(logFilePath, log);
                if (log)
                {
                    writer.WriteLine("================================================================================");
                    writer.WriteCurrentDateTime();
                    writer << "build request received:" << std::endl;
                    CodeFormatter formatter(writer.LogFile());
                    requestDoc->Write(formatter);
                }
                BuildRequest buildRequest;
                buildRequest.FromXml(requestDoc->DocumentElement());
                BuildReply buildReply = ProcessBuildRequest(buildRequest, &writer);
                std::unique_ptr<Element> replyElement = buildReply.ToXml("buildReply");
                Document replyDoc; 
                replyDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(replyElement.release()));
                if (log)
                {
                    writer << "build reply:" << std::endl;
                    CodeFormatter formatter(writer.LogFile());
                    replyDoc.Write(formatter);
                }
                std::string reply = DocumentToString(replyDoc);
                Write(socket, reply);
            }
            else if (messageKind == "cacheModuleRequest")
            {
                LogFileWriter writer(logFilePath, log);
                if (log)
                {
                    writer.WriteLine("================================================================================");
                    writer.WriteCurrentDateTime();
                    writer << "cache module request received:" << std::endl;
                    CodeFormatter formatter(writer.LogFile());
                    requestDoc->Write(formatter);
                }
                CacheModuleRequest cacheModuleRequest;
                cacheModuleRequest.FromXml(requestDoc->DocumentElement());
                CacheModuleReply cacheModuleReply = ProcessCacheModuleRequest(cacheModuleRequest);
                std::unique_ptr<Element> replyElement = cacheModuleReply.ToXml("cacheModuleReply");
                Document replyDoc;
                replyDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(replyElement.release()));
                if (log)
                {
                    writer << "cache module reply:" << std::endl;
                    CodeFormatter formatter(writer.LogFile());
                    replyDoc.Write(formatter);
                }
                std::string reply = DocumentToString(replyDoc);
                Write(socket, reply);
            }
            else if (messageKind == "getDefinitionRequest")
            {
                LogFileWriter writer(logFilePath, log);
                if (log)
                {
                    writer.WriteLine("================================================================================");
                    writer.WriteCurrentDateTime();
                    writer << "get definition request received:" << std::endl;
                    CodeFormatter formatter(writer.LogFile());
                    requestDoc->Write(formatter);
                }
                GetDefinitionRequest getDefinitionRequest;
                getDefinitionRequest.FromXml(requestDoc->DocumentElement());
                GetDefinitionReply getDefinitionReply = ProcessGetDefinitionRequest(getDefinitionRequest);
                std::unique_ptr<Element> replyElement = getDefinitionReply.ToXml("getDefinitionReply");
                Document replyDoc;
                replyDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(replyElement.release()));
                if (log)
                {
                    writer << "get definition reply:" << std::endl;
                    CodeFormatter formatter(writer.LogFile());
                    replyDoc.Write(formatter);
                }
                std::string reply = DocumentToString(replyDoc);
                Write(socket, reply);
            }
            else
            {
                if (log)
                {
                    LogFileWriter writer(logFilePath);
                    writer.WriteLine("================================================================================");
                    writer.WriteCurrentDateTime();
                    writer << "error: unknown message kind received: " << messageKind << std::endl;
                }
                WriteGenericErrorReply(messageKind);
            }
        }
    }
    catch (const std::exception& ex)
    {
        running = false;
        std::cout << "build-server-error" << std::endl;
        std::cout << ex.what() << std::endl;
        std::cerr << "BuildServer::Run: " << ex.what() << std::endl;
        if (log)
        {
            LogFileWriter writer(logFilePath);
            writer.WriteLine("================================================================================");
            writer.WriteCurrentDateTime();
            writer << "error: BuildServer::Run got exception: " << ex.what() << std::endl;
        }
        runException = std::current_exception();
    }
}

void BuildServer::WriteGenericErrorReply(const std::string& messageKind)
{
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServer_WriteGenericErrorReply);
#endif // TRACE
    GenericBuildErrorReply genericBuildErrorReply;
    genericBuildErrorReply.error = "unknown request message: '" + messageKind + "'";
    std::unique_ptr<Element> replyElement = genericBuildErrorReply.ToXml("genericBuildErrorReply");
    std::string reply = ElementToString(replyElement.release());
    Write(socket, reply);
}

BuildReply BuildServer::ProcessBuildRequest(const BuildRequest& buildRequest, LogFileWriter* logWriter)
{
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServer_ProcessBuildRequest);
#endif // TRACE
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    BuildReply buildReply;
    try
    {
        cmajor::symbols::ResetGlobalFlags();
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::updateSourceFileModuleMap);
        cmajor::build::ResetStopBuild();
        StartLogging();
        cmajor::symbols::SetCompilerVersion(version);
        if (buildRequest.backend == "cpp")
        {
            ProcessCppBackendRequest(buildRequest, buildReply, logWriter);
        }
        else if (buildRequest.backend == "llvm")
        {
            ProcessLlvmBackendRequest(buildRequest, buildReply, logWriter);
        }
        else
        {
            throw std::runtime_error("buildserver: error processing build request: 'backend' field required");
        }
        if (buildRequest.filePath.empty())
        {
            throw std::runtime_error("buildserver: error processing build request: 'filePath' field required");
        }
    }
    catch (const std::exception& ex)
    {
        buildReply.requestValid = false;
        buildReply.requestErrorMessage = ex.what();
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto dur = end - start;
    long long totalSecs = std::chrono::duration_cast<std::chrono::seconds>(dur).count() + 1;
    int hours = static_cast<int>(totalSecs / 3600);
    int mins = static_cast<int>((totalSecs / 60) % 60);
    int secs = static_cast<int>(totalSecs % 60);
    std::string time = 
        (hours > 0 ? std::to_string(hours) + " hour" + ((hours != 1) ? "s " : " ") : "") +
        (mins > 0 ? std::to_string(mins) + " minute" + ((mins != 1) ? "s " : " ") : "") +
        std::to_string(secs) + " second" + ((secs != 1) ? "s" : "");
    buildReply.time = time;
    EndLogging();
    SetLogExceptionToReply(buildReply);
    return buildReply;
}

CacheModuleReply BuildServer::ProcessCacheModuleRequest(const CacheModuleRequest& cacheModuleRequest)
{
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServer_ProcessCacheModuleRequest);
#endif // TRACE
    CacheModuleReply reply;
    try
    {
        sngcm::ast::BackEnd backend = sngcm::ast::BackEnd::llvm;
        if (cacheModuleRequest.backend == "cpp")
        {
            backend = sngcm::ast::BackEnd::cppcm;
        }
        else if (cacheModuleRequest.backend == "llvm")
        {
            backend = sngcm::ast::BackEnd::llvm;
        }
        std::u32string moduleName = ToUtf32(cacheModuleRequest.projectName);
        Project project(moduleName, cacheModuleRequest.projectFilePath, cacheModuleRequest.config, backend, "gcc", sngcm::ast::SystemDirKind::regular);
        std::string moduleFilePath = project.ModuleFilePath();
        if (!IsModuleCached(moduleFilePath))
        {
            std::unique_ptr<ModuleCache> prevCache;
            try
            {
                prevCache = ReleaseModuleCache();
                if (IsSystemModule(moduleName))
                {
                    InitModuleCache();
                }
                else
                {
                    MoveNonSystemModulesTo(prevCache);
                    UpdateModuleCache();
                }
                std::unique_ptr<Module> module(new Module(moduleFilePath, true));
                SetCacheModule(moduleFilePath, std::move(module));
                RestoreModulesFrom(prevCache.get());
            }
            catch (...)
            {
                RestoreModulesFrom(prevCache.get());
                throw;
            }
        }
    }
    catch (const std::exception& ex)
    {
        reply.error = ex.what();
    }
    return reply;
}

GetDefinitionReply BuildServer::ProcessGetDefinitionRequest(const GetDefinitionRequest& getDefinitionRequest)
{
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServer_ProcessGetDefinitionRequest);
#endif // TRACE
    cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::updateSourceFileModuleMap);
    GetDefinitionReply reply;
    try
    {
        sngcm::ast::BackEnd backend = sngcm::ast::BackEnd::llvm;
        if (getDefinitionRequest.backend == "cpp")
        {
            backend = sngcm::ast::BackEnd::cppcm;
            SetBackEnd(cmajor::symbols::BackEnd::cmcpp);
        }
        else if (getDefinitionRequest.backend == "llvm")
        {
            backend = sngcm::ast::BackEnd::llvm;
            SetBackEnd(cmajor::symbols::BackEnd::llvm);
        }
        sngcm::ast::Config config = sngcm::ast::Config::debug;
        if (getDefinitionRequest.config == "release")
        {
            cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::release);
            config = sngcm::ast::Config::release;
        }
        else if (!getDefinitionRequest.config.empty() && getDefinitionRequest.config != "debug")
        {
            throw std::runtime_error("buildserver: error processing get definition request: unknown configuration '" + getDefinitionRequest.config + "'");
        }
        Module* module = GetModuleBySourceFile(backend, config, getDefinitionRequest.identifierLocation.file);
        if (!module)
        {
            ReadToolChains(cmajor::symbols::GetGlobalFlag(cmajor::symbols::GlobalFlags::verbose));
            ResetToolChain();
            BackendSelector backendSelector(cmajor::symbols::GetBackEnd());
            std::u32string moduleName = ToUtf32(getDefinitionRequest.projectName);
            Project project(moduleName, getDefinitionRequest.projectFilePath, getDefinitionRequest.config, backend, GetToolChain(), sngcm::ast::SystemDirKind::regular);
            std::string moduleFilePath = project.ModuleFilePath();
            if (!IsModuleCached(moduleFilePath))
            {
                std::unique_ptr<ModuleCache> prevCache;
                try
                {
                    if (IsSystemModule(moduleName))
                    {
                        prevCache = ReleaseModuleCache();
                        InitModuleCache();
                    }
                    else
                    {
                        MoveNonSystemModulesTo(prevCache);
                        UpdateModuleCache();
                    }
                    std::unique_ptr<Module> module(new Module(moduleFilePath, true));
                    SetCacheModule(moduleFilePath, std::move(module));
                    RestoreModulesFrom(prevCache.get());
                }
                catch (...)
                {
                    RestoreModulesFrom(prevCache.get());
                    throw;
                }
            }
            module = GetModuleBySourceFile(backend, config, getDefinitionRequest.identifierLocation.file);
        }
        if (module)
        {
            int32_t fileIndex = module->GetFileIndexForFilePath(getDefinitionRequest.identifierLocation.file);
            if (fileIndex != -1)
            {
                int32_t line = boost::lexical_cast<int32_t>(getDefinitionRequest.identifierLocation.line);
                int32_t scol = boost::lexical_cast<int32_t>(getDefinitionRequest.identifierLocation.scol);
                int32_t ecol = boost::lexical_cast<int32_t>(getDefinitionRequest.identifierLocation.ecol);
                SymbolLocation identifierLocation(module->Id(), fileIndex, line, scol, ecol);
                SymbolLocation* definitionLocation = module->GetSymbolTable().GetDefinitionLocation(identifierLocation);
                if (definitionLocation)
                {
                    std::string filePath = GetSourceFilePath(definitionLocation->fileIndex, definitionLocation->moduleId);
                    if (filePath.empty())
                    {
                        std::string moduleName = "<unknown>";
                        Module* m = GetModuleById(definitionLocation->moduleId);
                        if (m)
                        {
                            moduleName = ToUtf8(m->Name());
                        }
                        throw std::runtime_error("file path for file index " + std::to_string(definitionLocation->fileIndex) + " not found from module '" + moduleName + "'");
                    }
                    reply.definitionLocation.file = filePath;
                    reply.definitionLocation.line = definitionLocation->line;
                    reply.definitionLocation.scol = definitionLocation->scol;
                    reply.definitionLocation.ecol = definitionLocation->ecol;
                    reply.ok = true;
                }
                else
                {
                    throw std::runtime_error("definition location of identifier '" + getDefinitionRequest.identifier  + "' not found");
                }
            }
            else
            {
                throw std::runtime_error("source file '" + getDefinitionRequest.identifierLocation.file + "' not included by module '" + ToUtf8(module->Name()) + "' or any of its referenced modules");
            }
        }
        else
        {
            throw std::runtime_error("module for source file '" + getDefinitionRequest.identifierLocation.file + "' not in cache");
        }
    }
    catch (const std::exception& ex)
    {
        reply.error = ex.what();
        reply.ok = false;
    }
    return reply;
}

void BuildServer::ProcessCppBackendRequest(const BuildRequest& cppBuildRequest, BuildReply& reply, LogFileWriter* logWriter)
{
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServer_ProcessCppBackendRequest);
#endif // TRACE
    LogMessage(-1, "Cmajor with C++ backend compiler version " + version + " for Windows x64");
    SetBackEnd(cmajor::symbols::BackEnd::cmcpp);
    ResetToolChain();
    std::unique_ptr<cmajor::symbols::Module> rootModule;
    std::vector<std::unique_ptr<cmajor::symbols::Module>> rootModules;
    std::set<std::string> builtProjects;
    bool forceNoDebugInfo = false;
    bool useModuleCache = true;
    if (cppBuildRequest.verbose)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::verbose);
    }
    if (cppBuildRequest.quiet)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::quiet);
    }
    if (cppBuildRequest.clean)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::clean);
    }
    if (cppBuildRequest.strictNothrow)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::strictNothrow);
    }
    if (cppBuildRequest.rebuild)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::rebuild);
    }
    if (cppBuildRequest.buildAllDependencies)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::buildAll);
    }
    if (cppBuildRequest.forceDebugInfo)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::generateDebugInfo);
    }
    if (cppBuildRequest.forceNoDebugInfo)
    {
        forceNoDebugInfo = true;
    }
    if (cppBuildRequest.disableModuleCache)
    {
        useModuleCache = false;
    }
    if (cppBuildRequest.singleThreadedCompile)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::singleThreadedCompile);
    }
    if (cppBuildRequest.disableCodeGen)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::disableCodeGen);
    }
    if (cppBuildRequest.emitIR)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::emitLlvm);
    }
    if (cppBuildRequest.justMyCode)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::justMyCodeDebugging);
    }
    if (cppBuildRequest.config == "release")
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::release);
    }
    else if (!cppBuildRequest.config.empty() && cppBuildRequest.config != "debug")
    {
        throw std::runtime_error("buildserver: error processing build request: unknown configuration '" + cppBuildRequest.config + "'");
    }
    for (const std::string& define : cppBuildRequest.defines)
    {
        cmajor::symbols::DefineCommandLineConditionalSymbol(ToUtf32(define));
    }
    if (!cppBuildRequest.numBuildThreads.empty() && cppBuildRequest.numBuildThreads != "default")
    {
        try
        {
            int numBuildThreads = boost::lexical_cast<int>(cppBuildRequest.numBuildThreads);
            if (numBuildThreads < 0 || numBuildThreads > 256)
            {
                throw std::runtime_error("number of build threads out of range");
            }
            cmajor::symbols::SetNumBuildThreads(numBuildThreads);
        }
        catch (const std::exception& ex)
        {
            throw std::runtime_error("buildserver: error processing build request: invalid number of build threads '" + cppBuildRequest.numBuildThreads + "': " +
                ex.what() + ", accepted values are 0-256");
        }
    }
    else
    {
        cmajor::symbols::SetNumBuildThreads(-1);
    }
    if (!cppBuildRequest.optimizationLevel.empty() && cppBuildRequest.optimizationLevel != "default")
    {
        try
        {
            int optimizationLevel = boost::lexical_cast<int>(cppBuildRequest.optimizationLevel);
            if (optimizationLevel < 0 || optimizationLevel > 3)
            {
                throw std::runtime_error("level out of range");
            }
            cmajor::symbols::SetOptimizationLevel(optimizationLevel);
        }
        catch (const std::exception& ex)
        {
            throw std::runtime_error("buildserver: error processing build request: invalid optimization level '" + cppBuildRequest.optimizationLevel + "': " +
                ex.what() + ", accepted values are 0-3");
        }
    }
    if (!cppBuildRequest.toolChain.empty() && cppBuildRequest.toolChain != "default")
    {
        SetToolChain(cppBuildRequest.toolChain);
    }
    ReadToolChains(cmajor::symbols::GetGlobalFlag(cmajor::symbols::GlobalFlags::verbose));
    if (cmajor::symbols::GetGlobalFlag(cmajor::symbols::GlobalFlags::verbose))
    {
        LogMessage(-1, "current platform is '" + GetPlatform() + "'");
        LogMessage(-1, "current tool chain is '" + GetToolChain() + "'");
    }
    if (GetToolChain() == "vs")
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::disableCodeGen);
        if (cmajor::symbols::GetGlobalFlag(cmajor::symbols::GlobalFlags::verbose))
        {
            LogMessage(-1, "Note: Native code generation for 'vs' tool chain is disabled. Just generating C++ source files and Visual Studio C++ project and solution files. "
                "You can generate native code by compiling those project and solution files using Visual Studio or msbuild.");
        }
    }
    cmajor::symbols::SetUseModuleCache(useModuleCache);
    BackendSelector backend(cmajor::symbols::GetBackEnd());
    if (!cmajor::symbols::GetGlobalFlag(cmajor::symbols::GlobalFlags::release) && !forceNoDebugInfo)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::generateDebugInfo);
    }
    std::string filePath = cppBuildRequest.filePath;
    boost::filesystem::path fp(filePath);
    if (fp.extension() == ".cms")
    {
        if (!boost::filesystem::exists(fp))
        {
            throw std::runtime_error("buildserver: error processing build request: solution file '" + filePath + "' does not exist");
        }
        reply.requestValid = true;
        BuildSolution(GetFullPath(fp.generic_string()), rootModules, reply, logWriter);
    }
    else if (fp.extension() == ".cmp")
    {
        if (!boost::filesystem::exists(fp))
        {
            throw std::runtime_error("buildserver: error processing build request: project file '" + filePath + "' does not exist");
        }
        reply.requestValid = true;
        BuildProject(GetFullPath(fp.generic_string()), rootModule, builtProjects, reply, logWriter);
    }
    else
    {
        throw std::runtime_error("buildserver: error processing build request: filePath '" + filePath + "' has unknown extension (not .cms or .cmp)");
    }
}

void BuildServer::ProcessLlvmBackendRequest(const BuildRequest& llvmBuildRequest, BuildReply& reply, LogFileWriter* logWriter)
{
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServer_ProcessLlvmBackendRequest);
#endif // TRACE
    LogMessage(-1, "Cmajor with LLVM backend compiler version " + version + " for Windows x64");
    SetBackEnd(cmajor::symbols::BackEnd::llvm);
    std::unique_ptr<cmajor::symbols::Module> rootModule;
    std::vector<std::unique_ptr<cmajor::symbols::Module>> rootModules;
    std::set<std::string> builtProjects;
    bool forceNoDebugInfo = false;
    bool useModuleCache = true;
    if (llvmBuildRequest.verbose)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::verbose);
    }
    if (llvmBuildRequest.quiet)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::quiet);
    }
    if (llvmBuildRequest.clean)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::clean);
    }
    if (llvmBuildRequest.strictNothrow)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::strictNothrow);
    }
    if (llvmBuildRequest.emitIR)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::emitLlvm);
    }
    if (llvmBuildRequest.emitOptIR)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::emitLlvm);
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::emitOptLlvm);
    }
    if (llvmBuildRequest.linkWithDebugRuntime)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::linkWithDebugRuntime);
    }
    if (llvmBuildRequest.linkUsingMsLink)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::linkUsingMsLink);
    }
    if (llvmBuildRequest.rebuild)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::rebuild);
    }
    if (llvmBuildRequest.buildAllDependencies)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::buildAll);
    }
    if (llvmBuildRequest.forceDebugInfo)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::generateDebugInfo);
    }
    if (llvmBuildRequest.forceNoDebugInfo)
    {
        forceNoDebugInfo = true;
    }
    if (llvmBuildRequest.disableModuleCache)
    {
        useModuleCache = false;
    }
    if (llvmBuildRequest.singleThreadedCompile)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::singleThreadedCompile);
    }
    if (llvmBuildRequest.config == "release")
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::release);
    }
    else if (!llvmBuildRequest.config.empty() && llvmBuildRequest.config != "debug")
    {
        throw std::runtime_error("buildserver: error processing build request: unknown configuration '" + llvmBuildRequest.config + "'");
    }
    for (const std::string& define : llvmBuildRequest.defines)
    {
        cmajor::symbols::DefineCommandLineConditionalSymbol(ToUtf32(define));
    }
    if (!llvmBuildRequest.numBuildThreads.empty() && llvmBuildRequest.numBuildThreads != "default")
    {
        try
        {
            int numBuildThreads = boost::lexical_cast<int>(llvmBuildRequest.numBuildThreads);
            if (numBuildThreads < 0 || numBuildThreads > 256)
            {
                throw std::runtime_error("number of build threads out of range");
            }
            cmajor::symbols::SetNumBuildThreads(numBuildThreads);
        }
        catch (const std::exception& ex)
        {
            throw std::runtime_error("buildserver: error processing build request: invalid number of build threads '" + llvmBuildRequest.numBuildThreads + "': " +
                ex.what() + ", accepted values are 0-256");
        }
    }
    else
    {
        cmajor::symbols::SetNumBuildThreads(-1);
    }
    if (!llvmBuildRequest.optimizationLevel.empty() && llvmBuildRequest.optimizationLevel != "default")
    {
        try
        {
            int optimizationLevel = boost::lexical_cast<int>(llvmBuildRequest.optimizationLevel);
            if (optimizationLevel < 0 || optimizationLevel > 3)
            {
                throw std::runtime_error("level out of range");
            }
            cmajor::symbols::SetOptimizationLevel(optimizationLevel);
        }
        catch (const std::exception& ex)
        {
            throw std::runtime_error("buildserver: error processing build request: invalid optimization level '" + llvmBuildRequest.optimizationLevel + "': " +
                ex.what() + ", accepted values are 0-3");
        }
    }
    cmajor::symbols::SetUseModuleCache(useModuleCache);
    BackendSelector backend(cmajor::symbols::GetBackEnd());
    if (!cmajor::symbols::GetGlobalFlag(cmajor::symbols::GlobalFlags::release) && !forceNoDebugInfo)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::generateDebugInfo);
    }
    std::string filePath = llvmBuildRequest.filePath;
    boost::filesystem::path fp(filePath);
    if (fp.extension() == ".cms")
    {
        if (!boost::filesystem::exists(fp))
        {
            throw std::runtime_error("buildserver: error processing build request: solution file '" + filePath + "' does not exist");
        }
        reply.requestValid = true;
        BuildSolution(GetFullPath(fp.generic_string()), rootModules, reply, logWriter);
    }
    else if (fp.extension() == ".cmp")
    {
        if (!boost::filesystem::exists(fp))
        {
            throw std::runtime_error("buildserver: error processing build request: project file '" + filePath + "' does not exist");
        }
        reply.requestValid = true;
        BuildProject(GetFullPath(fp.generic_string()), rootModule, builtProjects, reply, logWriter);
    }
    else
    {
        throw std::runtime_error("buildserver: error processing build request: filePath '" + filePath + "' has unknown extension (not .cms or .cmp)");
    }
}

void BuildServer::BuildSolution(const std::string& solutionFilePath, std::vector<std::unique_ptr<cmajor::symbols::Module>>& rootModules, BuildReply& reply, LogFileWriter* logWriter)
{
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServer_BuildSolution);
#endif // TRACE
    std::unique_ptr<ModuleCache> prevCache;
    try
    {
        if (!GetGlobalFlag(GlobalFlags::clean))
        {
            if (cmajor::build::SolutionContainsSystemModule(solutionFilePath))
            {
                prevCache = ReleaseModuleCache();
                InitModuleCache();
            }
            else
            {
                MoveNonSystemModulesTo(prevCache);
                UpdateModuleCache();
            }
        }
        if (log)
        {
            cmajor::build::SetBuildLogWriter(logWriter);
        }
        else
        {
            cmajor::build::SetBuildLogWriter(nullptr);
        }
        cmajor::build::BuildSolution(solutionFilePath, rootModules);
        reply.success = true;
    }
    catch (const soulng::lexer::ParsingException& ex)
    {
        reply.success = false;
        reply.errors.push_back(ParsingExceptionToError(ex));
        LogMessage(-1, ex.what());
    }
    catch (const cmajor::symbols::Exception& ex)
    {
        reply.success = false;
        reply.errors = SymbolsExceptionToErrors(ex);
        LogMessage(-1, ex.What());
    }
    catch (const std::exception& ex)
    {
        reply.success = false;
        CompileError error;
        error.message = ex.what();
        reply.errors.push_back(error);
        LogMessage(-1, ex.what());
    }
    RestoreModulesFrom(prevCache.get());
}

void BuildServer::BuildProject(const std::string& projectFilePath, std::unique_ptr<cmajor::symbols::Module>& rootModule,
    std::set<std::string>& builtProjects, BuildReply& reply, LogFileWriter* logWriter)
{
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServer_BuildProject);
#endif // TRACE
    std::unique_ptr<ModuleCache> prevCache;
    try
    {
        std::unique_ptr<Project> project = cmajor::build::ReadProject(projectFilePath);
        if (!GetGlobalFlag(GlobalFlags::clean))
        {
            if (IsSystemModule(project->Name()))
            {
                prevCache = ReleaseModuleCache();
                InitModuleCache();
            }
            else
            {
                MoveNonSystemModulesTo(prevCache);
                UpdateModuleCache();
            }
        }
        if (log)
        {
            cmajor::build::SetBuildLogWriter(logWriter);
        }
        else
        {
            cmajor::build::SetBuildLogWriter(nullptr);
        }
        cmajor::build::BuildProject(projectFilePath, rootModule, builtProjects);
        reply.success = true;
    }
    catch (const soulng::lexer::ParsingException& ex)
    {
        reply.success = false;
        reply.errors.push_back(ParsingExceptionToError(ex));
        LogMessage(-1, ex.what());
    }
    catch (const cmajor::symbols::Exception& ex)
    {
        reply.success = false;
        reply.errors = SymbolsExceptionToErrors(ex);
        LogMessage(-1, ex.What());
    }
    catch (const std::exception& ex)
    {
        reply.success = false;
        CompileError error;
        error.message = ex.what();
        reply.errors.push_back(error);
        LogMessage(-1, ex.what());
    }
    RestoreModulesFrom(prevCache.get());
}

void BuildServer::SetRunException(const std::exception_ptr& runException_)
{
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServer_SetRunException);
#endif // TRACE
    runException = runException_;
}

void BuildServer::SetLogException(const std::exception_ptr& logException_)
{
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServer_SetLogException);
#endif // TRACE
    logException = logException_;
}

void BuildServer::SetLogExceptionToReply(BuildReply& reply)
{
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServer_SetLogExceptionToReply);
#endif // TRACE
    if (!logException)
    {
        return;
    }
    try
    {
        std::rethrow_exception(logException);
    }
    catch (const std::exception& ex)
    {
        reply.logException = ex.what();
    }
}

void BuildServer::StartLogging()
{
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServer_StartLogging);
#endif // TRACE
    if (log)
    {
        LogFileWriter writer(logFilePath);
        writer.WriteLine("================================================================================");
        writer.WriteCurrentDateTime();
        writer << "start logging" << std::endl;
    }
    soulng::util::SetLogMode(soulng::util::LogMode::queue);
    soulng::util::StartLog();
    StartLogThread();
}

void BuildServer::EndLogging()
{
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServer_EndLogging);
#endif // TRACE
    if (log)
    {
        LogFileWriter writer(logFilePath);
        writer.WriteLine("================================================================================");
        writer.WriteCurrentDateTime();
        writer << "end logging" << std::endl;
    }
    EndLog();
    StopLogThread();
}

void BuildServer::RunLog()
{
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServer_RunLog);
#endif // TRACE
    while (true)
    {
        bool endOfLog = false;
        bool timeout = false;
        std::string message = soulng::util::FetchLogMessage(endOfLog, progressIntervalMs, timeout);
        if (endOfLog) return;
        if (timeout)
        {
            BuildProgressMessage buildProgressMessage;
            std::unique_ptr<Element> progress = buildProgressMessage.ToXml("buildProgressMessage");
            std::string message = ElementToString(progress.release());
            Write(socket, message);
            continue;
        }
        LogBuildMessageRequest logBuildMessageRequest;
        logBuildMessageRequest.message = message;
        std::unique_ptr<Element> logRequest = logBuildMessageRequest.ToXml("logBuildMessageRequest");
        std::string request = ElementToString(logRequest.release());
        Write(socket, request);
        std::string reply = ReadStr(socket);
        std::u32string content = ToUtf32(reply);
        std::unique_ptr<Document> logReply = ParseDocument(content, "socket");
        std::string messageKind = GetMessageKind(logReply->DocumentElement());
        if (messageKind == "logBuildMessageReply")
        {
            LogBuildMessageReply logBuildMessageReply;
            logBuildMessageReply.FromXml(logReply->DocumentElement());
            if (!logBuildMessageReply.ok)
            {
                throw std::runtime_error("log message refused by client");
            }
        }
        else
        {
            std::string error;
            if (messageKind.empty())
            {
                error = "'messageKind' field of reply is empty";
            }
            else
            {
                error = "unknown 'messageKind' field value '" + messageKind + "'";
            }
            throw std::runtime_error("invalid logBuildMessageReply received: " + error);
        }
    }
}

void RunLogThread(BuildServer* server)
{
#ifdef TRACE
    soulng::util::SetThreadId('L');
    soulng::util::Tracer tracer(RunLogThread_f);
#endif // TRACE
    try
    {
        server->RunLog();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "RunLogThread: " << ex.what() << std::endl;
        server->SetLogException(std::current_exception());
    }
}

void BuildServer::StartLogThread()
{
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServer_StartLogThread);
#endif // TRACE
    try
    {
        logThread = std::thread{ RunLogThread, this };
    }
    catch (const std::exception& ex)
    {
        std::cerr << "BuildServer::StartLogThread: " << ex.what() << std::endl;
        if (log)
        {
            LogFileWriter writer(logFilePath);
            writer.WriteLine("================================================================================");
            writer.WriteCurrentDateTime();
            writer << "StartLogThread got exception: " << ex.what() << std::endl;
        }
        logException = std::current_exception();
    }
}

void BuildServer::StopLogThread()
{
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServer_StopLogThread);
#endif // TRACE
    try
    {
        logThread.join();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "BuildServer::StopLogThread: " << ex.what() << std::endl;
        if (log)
        {
            LogFileWriter writer(logFilePath);
            writer.WriteLine("================================================================================");
            writer.WriteCurrentDateTime();
            writer << "StopLogThread got exception: " << ex.what() << std::endl;
        }
    }
}

std::string BuildServer::GetMessageKind(Element* element) const
{
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServer_GetMessageKind);
#endif // TRACE
    return ToUtf8(element->Name());
}

std::string BuildServer::DocumentToString(Document& doc) const
{
    std::stringstream strStream;
    CodeFormatter formatter(strStream);
    doc.Write(formatter);
    return strStream.str();
}

std::string BuildServer::ElementToString(Element* element) const
{
    Document doc;
    doc.AppendChild(std::unique_ptr<sngxml::dom::Node>(element));
    return DocumentToString(doc);
}

void RunServer(BuildServer* server)
{
#ifdef TRACE
    soulng::util::SetThreadId('B');
    soulng::util::Tracer tracer(RunServer_BuildServer_f);
#endif // TRACE
    try
    {
        server->Run();
    }
    catch (const std::exception& ex)
    {
        std::cout << "build-server-error" << std::endl;
        std::cout << ex.what() << std::endl;
        std::cerr << "RunServer: exception from BuildServer::Run: " << ex.what() << std::endl;
        if (server->Log())
        {
            LogFileWriter writer(server->LogFilePath());
            writer.WriteLine("================================================================================");
            writer.WriteCurrentDateTime();
            writer << "RunServer got exception: " << ex.what() << std::endl;
        }
        server->SetRunException(std::current_exception());
    }
}

void BuildServer::Start(int port, const std::string& version, std::condition_variable* exitVar, bool* exiting)
{
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServer_Start);
#endif // TRACE
    try
    {
        if (log)
        {
            LogFileWriter writer(logFilePath);
            writer.WriteLine("================================================================================");
            writer.WriteCurrentDateTime();
            writer << "build server start: port=" << port << ", version=" << version << std::endl;
        }
        this->port = port;
        this->version = version;
        this->exitVar = exitVar;
        this->exiting = exiting;
        serverThread = std::thread{ RunServer, this };
        while (!running)
        {
            std::this_thread::sleep_for(std::chrono::seconds{ 1 });
        }
    }
    catch (const std::exception& ex)
    {
        std::cout << "build-server-error" << std::endl;
        std::cout << ex.what() << std::endl;
        std::cerr << "exception from BuildServer::Start: " << ex.what() << std::endl;
        throw;
    }
}

void BuildServer::Stop()
{
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServer_Stop);
#endif // TRACE
    try
    {
        if (log)
        {
            LogFileWriter writer(logFilePath);
            writer.WriteLine("================================================================================");
            writer.WriteCurrentDateTime();
            writer << "build server stop" << std::endl;
        }
        exit = true;
        if (running)
        {
            cmajor::build::StopBuild();
            if (runException)
            {
                serverThread.join();
                std::rethrow_exception(runException);
            }
            else
            {
                TcpSocket socket("localhost", std::to_string(port));
            }
        }
        serverThread.join();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "exception from BuildServer::Stop: " << ex.what() << std::endl;
        throw;
    }
}

void BuildServer::SetRequestInProgress()
{
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServer_SetRequestInProgress);
#endif // TRACE
    requestInProgress = true;
}

void BuildServer::ResetRequestInProgress()
{
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServer_ResetRequestInProgress);
#endif // TRACE
    requestInProgress = false;
}

BuildServer* buildServer = nullptr;

void StartBuildServer(int port, const std::string& version, bool log, bool progress, std::condition_variable* exitVar, bool* exiting)
{
#ifdef TRACE
    soulng::util::Tracer tracer(StartBuildServer_f);
#endif // TRACE
    if (!buildServer)
    {
        buildServer = new BuildServer(log, progress);
    }
    buildServer->Start(port, version, exitVar, exiting);
}

void StopBuildServer()
{
#ifdef TRACE
    soulng::util::Tracer tracer(StopBuildServer_f);
#endif // TRACE
    buildServer->Stop();
}

bool BuildServerStopRequested()
{
#ifdef TRACE
    soulng::util::Tracer tracer(BuildServerStopRequested_f);
#endif // TRACE
    return buildServer->StopRequested();
}

} // namespace cmbs
