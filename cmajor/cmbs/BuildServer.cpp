// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmbs/BuildServer.hpp>
#include <cmajor/cmbs/BuildServerMessage.hpp>
#include <cmajor/build/Build.hpp>
#include <sngcm/ast/Project.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/ModuleCache.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/cmmid/InitDone.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/Log.hpp>
#include <soulng/util/LogFileWriter.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Socket.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/lexer/ParsingException.hpp>
#include <sngjson/json/JsonImport.hpp>
#include <sngjson/json/JsonLexer.hpp>
#include <sngjson/json/JsonParser.hpp>
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
using namespace sngjson::json;
using namespace cmajor::mid;

struct BackendSelector
{
    BackendSelector(cmajor::symbols::BackEnd backend)
    {
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
        CmmDone();
    }
};

std::string CmbsLogFilePath()
{
    return Path::Combine(CmajorLogFileDir(), "cmbs.log");
}

class BuildServer
{
public:
    BuildServer(int timeOutSecs_, bool log_);
    ~BuildServer();
    void Start(int port, const std::string& version);
    void Stop();
    void Run();
    BuildReply ProcessRequest(const BuildRequest& buildRequest);
    void WriteGenericErrorReply(const std::string& messageKind);
    void ProcessCppBackendRequest(const BuildRequest& cppBuildRequest, BuildReply& reply);
    void ProcessLlvmBackendRequest(const BuildRequest& llvmBuildRequest, BuildReply& reply);
    void BuildSolution(const std::string& solutionFilePath, std::vector<std::unique_ptr<cmajor::symbols::Module>>& rootModules, BuildReply& reply);
    void BuildProject(const std::string& projectFilePath, std::unique_ptr<cmajor::symbols::Module>& rootModule,
        std::set<std::string>& builtProjects, BuildReply& reply);
    void SetRunException(const std::exception_ptr& runException_);
    void SetLogException(const std::exception_ptr& logException_);
    void SetLogExceptionToReply(BuildReply& reply);
    void StartLogging();
    void EndLogging();
    void StartLogThread();
    void RunLog();
    void StopLogThread();
    std::string GetMessageKind(JsonValue* message) const;
    void SetLastActionTime();
    void SetRequestInProgress();
    void ResetRequestInProgress();
    bool TimeOut() const;
    bool StopRequested() const { return stopRequested; }
    const std::string& LogFilePath() const { return logFilePath; }
    bool Log() const { return log; }
private:
    int port;
    bool log;
    std::string version;
    std::atomic_bool exiting;
    bool running;
    std::thread serverThread;
    std::thread logThread;
    TcpSocket listenSocket;
    TcpSocket socket;
    std::exception_ptr runException;
    std::exception_ptr logException;
    bool requestInProgress;
    int timeoutSecs;
    time_t lastActionTime;
    bool stopRequested;
    std::string logFilePath;
    int progressIntervalMs;
};

struct RequestGuard
{
    RequestGuard(BuildServer* server_) : server(server_)
    {
        server->SetRequestInProgress();
    }
    ~RequestGuard()
    {
        server->ResetRequestInProgress();
    }
    BuildServer* server;
};

BuildServer::BuildServer(int timeoutSecs_, bool log_) :
    port(54325), log(log_), version(), exiting(false), listenSocket(), socket(), requestInProgress(false), timeoutSecs(timeoutSecs_),
    lastActionTime(), stopRequested(false), logFilePath(CmbsLogFilePath()), running(false), progressIntervalMs(250)
{
    SetLastActionTime();
    if (log)
    {
        LogFileWriter writer(logFilePath);
        writer.WriteLine("================================================================================");
        writer.WriteCurrentDateTime();
        writer << "build server created" << std::endl;
        writer << "timeout=" << timeoutSecs << ", log=" << log << std::endl;
    }
}

BuildServer::~BuildServer()
{
}

void BuildServer::Run()
{
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
        while (!exiting)
        {
            std::cout << "build-server-ready" << std::endl;
            socket = listenSocket.Accept();
            if (exiting)
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
            JsonLexer lexer(content, "", 0);
            std::unique_ptr<JsonValue> requestJsonValue(JsonParser::Parse(lexer));
            std::string messageKind = GetMessageKind(requestJsonValue.get());
            if (messageKind == "stopRequest")
            {
                stopRequested = true;
                if (log)
                {
                    LogFileWriter writer(logFilePath);
                    writer.WriteLine("================================================================================");
                    writer.WriteCurrentDateTime();
                    writer << "stop request received:" << std::endl;
                    CodeFormatter formatter(writer.LogFile());
                    requestJsonValue->Write(formatter);
                }
            }
            else if (messageKind == "keepAliveRequest")
            {
                SetLastActionTime();
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
                    requestJsonValue->Write(formatter);
                }
                BuildRequest buildRequest(requestJsonValue.get());
                BuildReply buildReply = ProcessRequest(buildRequest);
                std::unique_ptr<JsonValue> replyJsonValue = buildReply.ToJson();
                if (log)
                {
                    writer << "build reply:" << std::endl;
                    CodeFormatter formatter(writer.LogFile());
                    replyJsonValue->Write(formatter);
                }
                std::string reply = replyJsonValue->ToString();
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
    GenericErrorReply genericErrorReply;
    genericErrorReply.messageKind = "genericErrorReply";
    if (messageKind.empty())
    {
        genericErrorReply.error = "request message has no 'messageKind' field";
    }
    else
    {
        genericErrorReply.error = "request message has unknown 'messageKind' field value '" + messageKind + "'";
    }
    std::unique_ptr<JsonValue> replyJsonValue = genericErrorReply.ToJson();
    std::string reply = replyJsonValue->ToString();
    Write(socket, reply);
}

BuildReply BuildServer::ProcessRequest(const BuildRequest& buildRequest)
{
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    BuildReply buildReply;
    buildReply.messageKind = "buildReply";
    try
    {
        cmajor::symbols::ResetGlobalFlags();
        cmajor::build::ResetStopBuild();
        StartLogging();
        cmajor::symbols::SetCompilerVersion(version);
        if (buildRequest.backend == "cpp")
        {
            ProcessCppBackendRequest(buildRequest, buildReply);
        }
        else if (buildRequest.backend == "llvm")
        {
            ProcessLlvmBackendRequest(buildRequest, buildReply);
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

void BuildServer::ProcessCppBackendRequest(const BuildRequest& cppBuildRequest, BuildReply& reply)
{
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
        cmajor::symbols::SetNumBuildThreads(1);
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
        BuildSolution(GetFullPath(fp.generic_string()), rootModules, reply);
    }
    else if (fp.extension() == ".cmp")
    {
        if (!boost::filesystem::exists(fp))
        {
            throw std::runtime_error("buildserver: error processing build request: project file '" + filePath + "' does not exist");
        }
        reply.requestValid = true;
        BuildProject(GetFullPath(fp.generic_string()), rootModule, builtProjects, reply);
    }
    else
    {
        throw std::runtime_error("buildserver: error processing build request: filePath '" + filePath + "' has unknown extension (not .cms or .cmp)");
    }
}

void BuildServer::ProcessLlvmBackendRequest(const BuildRequest& llvmBuildRequest, BuildReply& reply)
{
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
        cmajor::symbols::SetNumBuildThreads(1);
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
        BuildSolution(GetFullPath(fp.generic_string()), rootModules, reply);
    }
    else if (fp.extension() == ".cmp")
    {
        if (!boost::filesystem::exists(fp))
        {
            throw std::runtime_error("buildserver: error processing build request: project file '" + filePath + "' does not exist");
        }
        reply.requestValid = true;
        BuildProject(GetFullPath(fp.generic_string()), rootModule, builtProjects, reply);
    }
    else
    {
        throw std::runtime_error("buildserver: error processing build request: filePath '" + filePath + "' has unknown extension (not .cms or .cmp)");
    }
}

void BuildServer::BuildSolution(const std::string& solutionFilePath, std::vector<std::unique_ptr<cmajor::symbols::Module>>& rootModules, BuildReply& reply)
{
    try
    {
        cmajor::build::BuildSolution(solutionFilePath, rootModules);
        reply.success = true;
    }
    catch (const soulng::lexer::ParsingException& ex)
    {
        reply.success = false;
        reply.error = ex.what();
        LogMessage(-1, ex.what());
    }
    catch (const cmajor::symbols::Exception& ex)
    {
        reply.success = false;
        reply.error = ex.What();
        LogMessage(-1, ex.What());
    }
    catch (const std::exception& ex)
    {
        reply.success = false;
        reply.error = ex.what();
        LogMessage(-1, ex.what());
    }
}

void BuildServer::BuildProject(const std::string& projectFilePath, std::unique_ptr<cmajor::symbols::Module>& rootModule,
    std::set<std::string>& builtProjects, BuildReply& reply)
{
    try
    {
        cmajor::build::BuildProject(projectFilePath, rootModule, builtProjects);
        reply.success = true;
    }
    catch (const soulng::lexer::ParsingException& ex)
    {
        reply.success = false;
        reply.error = ex.what();
        LogMessage(-1, ex.what());
    }
    catch (const cmajor::symbols::Exception& ex)
    {
        reply.success = false;
        reply.error = ex.What();
        LogMessage(-1, ex.What());
    }
    catch (const std::exception& ex)
    {
        reply.success = false;
        reply.error = ex.what();
        LogMessage(-1, ex.what());
    }
}

void BuildServer::SetRunException(const std::exception_ptr& runException_)
{
    runException = runException_;
}

void BuildServer::SetLogException(const std::exception_ptr& logException_)
{
    logException = logException_;
}

void BuildServer::SetLogExceptionToReply(BuildReply& reply)
{
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
    while (true)
    {
        bool endOfLog = false;
        bool timeout = false;
        std::string message = soulng::util::FetchLogMessage(endOfLog, progressIntervalMs, timeout);
        if (endOfLog) return;
        if (timeout)
        {
            ProgressMessage progressMessage;
            progressMessage.messageKind = "progressMessage";
            std::unique_ptr<JsonValue> progress = progressMessage.ToJson();
            std::string message = progress->ToString();
            Write(socket, message);
            continue;
        }
        LogMessageRequest logMessageRequest;
        logMessageRequest.messageKind = "logMessageRequest";
        logMessageRequest.message = message;
        std::unique_ptr<JsonValue> logRequest = logMessageRequest.ToJson();
        std::string request = logRequest->ToString();
        Write(socket, request);
        std::string reply = ReadStr(socket);
        std::u32string content = ToUtf32(reply);
        JsonLexer lexer(content, "", 0);
        std::unique_ptr<JsonValue> logReply = JsonParser::Parse(lexer);
        std::string messageKind = GetMessageKind(logReply.get());
        if (messageKind == "logMessageReply")
        {
            LogMessageReply logMessageReply(logReply.get());
            if (!logMessageReply.ok)
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
            throw std::runtime_error("invalid logMessageReply received: " + error);
        }
    }
}

void RunLogThread(BuildServer* server)
{
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

std::string BuildServer::GetMessageKind(JsonValue* message) const
{
    if (message->Type() == JsonValueType::object)
    {
        JsonObject* messageObject = static_cast<JsonObject*>(message);
        return messageObject->GetStringField(U"messageKind");
    }
    return std::string();
}

void RunServer(BuildServer* server)
{
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

void BuildServer::Start(int port, const std::string& version)
{
    try
    {
        SetLastActionTime();
        if (log)
        {
            LogFileWriter writer(logFilePath);
            writer.WriteLine("================================================================================");
            writer.WriteCurrentDateTime();
            writer << "build server start: port=" << port << ", version=" << version << std::endl;
        }
        this->port = port;
        this->version = version;
        serverThread = std::thread{ RunServer, this };
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
    try
    {
        if (log)
        {
            LogFileWriter writer(logFilePath);
            writer.WriteLine("================================================================================");
            writer.WriteCurrentDateTime();
            writer << "build server stop" << std::endl;
        }
        exiting = true;
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

void BuildServer::SetLastActionTime()
{
    time(&lastActionTime);
}

void BuildServer::SetRequestInProgress()
{
    requestInProgress = true;
    SetLastActionTime();
}

void BuildServer::ResetRequestInProgress()
{
    requestInProgress = false;
    SetLastActionTime();
}

bool BuildServer::TimeOut() const
{
    if (requestInProgress)
    {
        return false;
    }
    time_t now;
    time(&now);
    if (now - lastActionTime > timeoutSecs)
    {
        if (log)
        {
            LogFileWriter writer(logFilePath);
            writer.WriteLine("================================================================================");
            writer.WriteCurrentDateTime();
            writer << "build server timeout" << std::endl;
        }
        return true;
    }
    return false;
}

BuildServer* buildServer = nullptr;

void StartBuildServer(int port, const std::string& version, int timeoutSecs, bool log)
{
    if (!buildServer)
    {
        buildServer = new BuildServer(timeoutSecs, log);
    }
    buildServer->Start(port, version);
}

void StopBuildServer()
{
    buildServer->Stop();
}

bool BuildServerTimeOut()
{
    return buildServer->TimeOut();
}

bool BuildServerStopRequested()
{
    return buildServer->StopRequested();
}

} // namespace cmbs