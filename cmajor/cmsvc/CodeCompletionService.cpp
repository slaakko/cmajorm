// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmsvc/CodeCompletionService.hpp>
#include <cmajor/cmsvc/PortMapService.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/Parser.hpp>
#include <soulng/util/Process.hpp>
#include <soulng/util/Socket.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <thread>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <sstream>

namespace cmajor { namespace service {

using namespace soulng::util;
using namespace soulng::unicode;

const int keepAliveIntervalSeconds = 60;
const int defaultCodeCompletionServerPort = 54327;
const int defaultKeepAliveServerPort = 54328;

CodeCompletionServiceStartParams::CodeCompletionServiceStartParams() : debugServer(false), wait(false), pid(0)
{
}

CodeCompletionServiceStartParams& CodeCompletionServiceStartParams::ProcessName(const std::string& processName_)
{
    processName = processName_;
    return *this;
}

CodeCompletionServiceStartParams& CodeCompletionServiceStartParams::Pid(int pid_)
{
    pid = pid_;
    return *this;
}

CodeCompletionServiceStartParams& CodeCompletionServiceStartParams::DebugServer(bool debugServer_)
{
    debugServer = debugServer_;
    return *this;
}

CodeCompletionServiceStartParams& CodeCompletionServiceStartParams::Wait(bool wait_)
{
    wait = wait_;
    return *this;
}

class CodeCompletionService
{
public:
    static void Init();
    static void Done();
    static CodeCompletionService& Instance() { return *instance; }
    void Start(CodeCompletionServiceStartParams& startParams);
    void Stop(bool log);
    void Quit();
    void Run();
    bool Running();
    void KeepAlive();
    void PutRequest(CodeCompletionServiceRequest* request);
    void LoadEditModule(const std::string& projectFilePath, const std::string& backend, const std::string& config);
    void ResetEditModuleCache();
    void ParseSource(const std::string& projectFilePath, const std::string& backend, const std::string& config, const std::string& sourceFilePath, std::u32string&& sourceCode);
    void GetCCList(const std::string& projectFilePath, const std::string& backend, const std::string& config, const std::string& sourceFilePath, const std::string& ccText_);
private:
    static std::unique_ptr<CodeCompletionService> instance;
    CodeCompletionService();
    CodeCompletionServiceStartParams startParams;
    std::string MakeCodeCompletionServerStartCommand(std::string& startStatus);
    void StartKeepAliveThread();
    void StopKeepAliveThread();
    void SendReceiveKeepAlive();
    void ProcessRequests();
    void ExecuteRequest(CodeCompletionServiceRequest* request);
    void WriteMessage(sngxml::dom::Element* messageElement);
    std::unique_ptr<sngxml::dom::Document> ReadReply(const std::string& replyMessageName);
    TcpSocket socket;
    bool running;
    bool exiting;
    bool keepAliveThreadStarted;
    bool keepAliveThreadRunning;
    bool stopKeepAlives;
    bool serviceThreadStarted;
    bool requestInProgress;
    bool codeCompletionServerProcessTerminated;
    int serverPort;
    int keepAliveServerPort;
    std::thread serviceThread;
    std::thread keepAliveThread;
    std::unique_ptr<Process> codeCompletionServerProcess;
    std::mutex keepAliveMutex;
    std::condition_variable stopKeepAliveVar;
    std::mutex requestMutex;
    std::mutex queueMutex;
    std::condition_variable requestAvailableOrExiting;
    std::list<std::unique_ptr<CodeCompletionServiceRequest>> requestQueue;
};

std::unique_ptr<CodeCompletionService> CodeCompletionService::instance;

void CodeCompletionService::Init()
{
    instance.reset(new CodeCompletionService());
}

void CodeCompletionService::Done()
{
    instance.reset();
}

CodeCompletionService::CodeCompletionService() : 
    running(false),
    exiting(false),
    requestInProgress(false),
    keepAliveThreadStarted(false),
    keepAliveThreadRunning(false),
    stopKeepAlives(false),
    serviceThreadStarted(false),
    codeCompletionServerProcessTerminated(false),
    serverPort(-1),
    keepAliveServerPort(-1)
{
}

std::string CodeCompletionService::MakeCodeCompletionServerStartCommand(std::string& startStatus)
{
    startStatus.clear();
    std::string startCommand = "cmccs";
    startStatus = "starting code completion server (cmccs):";
    if (startParams.debugServer)
    {
        startCommand = "cmccsd";
        startStatus = "starting code completion server (cmccsd):";
    }
    serverPort = GetFreePortNumber(startParams.processName);
    keepAliveServerPort = -1;
    if (serverPort == -1)
    {
        serverPort = defaultCodeCompletionServerPort;
        startStatus.append(" port=" + std::to_string(serverPort) + " (default)");
        keepAliveServerPort = defaultKeepAliveServerPort;
        startStatus.append(", keep alive port=" + std::to_string(keepAliveServerPort) + " (default)");
    }
    else
    {
        startStatus.append(" port=" + std::to_string(serverPort));
        keepAliveServerPort = GetFreePortNumber(startParams.processName);
    }
    if (keepAliveServerPort == -1)
    {
        keepAliveServerPort = defaultKeepAliveServerPort;
        startStatus.append(", keep alive port=" + std::to_string(keepAliveServerPort) + " (default)");
    }
    else
    {
        startStatus.append(", keep alive port=" + std::to_string(keepAliveServerPort));
    }
    startCommand.append(" --port=" + std::to_string(serverPort));
    startCommand.append(" --keepAliveServerPort=" + std::to_string(keepAliveServerPort));
    int portMapServicePort = GetPortMapServicePortNumberFromConfig();
    if (portMapServicePort != -1)
    {
        startCommand.append(" --portMapServicePort=" + std::to_string(portMapServicePort));
    }
    if (startParams.wait)
    {
        startCommand.append(" --wait");
    }
    startStatus.append("...");
    return startCommand;
}

void RunCCKeepAliveThread()
{
    CodeCompletionService::Instance().KeepAlive();
}

void CodeCompletionService::StartKeepAliveThread()
{
    if (!keepAliveThreadStarted)
    {
        keepAliveThreadStarted = true;
        stopKeepAlives = false;
        keepAliveThread = std::thread(RunCCKeepAliveThread);
    }
}

void CodeCompletionService::StopKeepAliveThread()
{
    try
    {
        if (keepAliveThreadRunning)
        {
            stopKeepAlives = true;
            stopKeepAliveVar.notify_one();
        }
        if (keepAliveThreadStarted)
        {
            keepAliveThread.join();
            keepAliveThreadStarted = false;
        }
    }
    catch (...)
    {
    }
}

void CodeCompletionService::KeepAlive()
{
    try
    {
        keepAliveThreadRunning = true;
        while (!stopKeepAlives)
        {
            std::unique_lock<std::mutex> lock(keepAliveMutex);
            if (stopKeepAliveVar.wait_for(lock, std::chrono::seconds(keepAliveIntervalSeconds), [this] { return stopKeepAlives; }))
            {
                keepAliveThreadRunning = false;
                return;
            }
            SendReceiveKeepAlive();
        }
    }
    catch (const std::exception& ex)
    {
        keepAliveThreadRunning = false;
        PutOutputServiceMessage("error: code completion service keep alive: " + std::string(ex.what()));
    }
}

void CodeCompletionService::SendReceiveKeepAlive()
{
    try
    {
        TcpSocket socket("localhost", std::to_string(keepAliveServerPort));
        KeepAliveCCRequest keepAliveRequest;
        std::unique_ptr<sngxml::dom::Element> requestElement = keepAliveRequest.ToXml("keepAliveCCRequest");
        sngxml::dom::Document requestDoc;
        requestDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(requestElement.release()));
        std::stringstream strStream;
        CodeFormatter formatter(strStream);
        requestDoc.Write(formatter);
        std::string requestStr = strStream.str();
        Write(socket, requestStr);
        std::string replyStr = ReadStr(socket);
        std::unique_ptr<sngxml::dom::Document> replyDoc = sngxml::dom::ParseDocument(ToUtf32(replyStr), "socket"); 
        std::string message = ToUtf8(replyDoc->DocumentElement()->Name());
        if (message != "keepAliveCCReply")
        {
            throw std::runtime_error("keepAliveCCReply expected");
        }
        KeepAliveCCReply reply(replyDoc->DocumentElement());
    }
    catch (const std::exception& ex)
    {
        PutOutputServiceMessage("error: code completion service send/receive keep alive: " + std::string(ex.what()));
    }
}

void CodeCompletionService::ProcessRequests()
{
    try
    {
        while (!exiting)
        {
            std::unique_ptr<CodeCompletionServiceRequest> request;
            {
                std::unique_lock<std::mutex> lock(requestMutex);
                requestAvailableOrExiting.wait(lock, [this] { return exiting || !requestQueue.empty(); });
                if (exiting)
                {
                    running = false;
                    return;
                }
                request = std::move(requestQueue.front());
                requestQueue.pop_front();
            }
            if (request)
            {
                ExecuteRequest(request.get());
            }
        }
    }
    catch (const std::exception& ex)
    {
        PutOutputServiceMessage("error: code completion service process requests: " + std::string(ex.what()));
        running = false;
    }
}

struct CodeCompletionServiceRequestGuard
{
    CodeCompletionServiceRequestGuard(bool& requestInProgress_) : requestInProgress(requestInProgress_)
    {
        requestInProgress = true;
    }
    ~CodeCompletionServiceRequestGuard()
    {
        requestInProgress = false;
    }
    bool& requestInProgress;
};

void CodeCompletionService::PutRequest(CodeCompletionServiceRequest* request)
{
    std::lock_guard<std::mutex> lock(queueMutex);
    requestQueue.push_back(std::unique_ptr<CodeCompletionServiceRequest>(request));
    requestAvailableOrExiting.notify_one();
}

void CodeCompletionService::ExecuteRequest(CodeCompletionServiceRequest* request)
{
    CodeCompletionServiceRequestGuard requestGuard(requestInProgress);
    try
    {
        request->Execute();
    }
    catch (const std::exception& ex)
    {
        PutOutputServiceMessage("code completion service: error executing " + request->Name() + ": " + std::string(ex.what()));
        request->Failed(ex.what());
        throw;
    }
}

void CodeCompletionService::WriteMessage(sngxml::dom::Element* messageElement)
{
    sngxml::dom::Document messageDoc;
    messageDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(messageElement));
    std::stringstream strStream;
    CodeFormatter formatter(strStream);
    messageDoc.Write(formatter);
    Write(socket, strStream.str());
}

std::unique_ptr<sngxml::dom::Document> CodeCompletionService::ReadReply(const std::string& replyMessageName)
{
    std::string replyStr = ReadStr(socket);
    std::unique_ptr<sngxml::dom::Document> replyDoc = sngxml::dom::ParseDocument(ToUtf32(replyStr), "socket");
    std::string message = ToUtf8(replyDoc->DocumentElement()->Name());
    if (message != replyMessageName)
    {
        throw std::runtime_error("'" + replyMessageName + "' message expected");
    }
    return replyDoc;
}

void RunCodeCompletionService()
{
    CodeCompletionService::Instance().Run();
}

void CodeCompletionService::Start(CodeCompletionServiceStartParams& startParams_)
{
    startParams = startParams_;
    exiting = false;
    running = false;
    serviceThread = std::thread(RunCodeCompletionService);
    serviceThreadStarted = true;
}

void CodeCompletionService::Stop(bool log)
{
    try
    {
        if (log)
        {
            PutOutputServiceMessage("stopping code completion server...");
        }
        exiting = true;
        if (serviceThreadStarted)
        {
            if (running)
            {
                if (keepAliveThreadStarted)
                {
                    StopKeepAliveThread();
                }
                Quit();
                requestAvailableOrExiting.notify_one();
                running = false;
            }
            serviceThread.join();
            serviceThreadStarted = false;
        }
        else
        {
            running = false;
        }
        if (log)
        {
            PutOutputServiceMessage("code completion server stopped");
        }
    }
    catch (...)
    {
    }
}

void CodeCompletionService::Quit()
{
    try
    {
        if (!codeCompletionServerProcess) return;
        if (codeCompletionServerProcessTerminated) return;
        codeCompletionServerProcessTerminated = true;
        codeCompletionServerProcess->Terminate();
        running = false;
    }
    catch (...)
    {
    }
}

void CodeCompletionService::Run()
{
    try
    {
        running = true;
        StartKeepAliveThread();
        std::string startStatus;
        std::string serverStartCommand = MakeCodeCompletionServerStartCommand(startStatus);
        PutOutputServiceMessage(startStatus);
        codeCompletionServerProcessTerminated = false;
        codeCompletionServerProcess.reset(new Process(serverStartCommand, Process::Redirections::processStdIn | Process::Redirections::processStdOut | Process::Redirections::processStdErr));
        try
        {
            if (!exiting)
            {
                std::string status = Trim(codeCompletionServerProcess->ReadLine(Process::StdHandle::stdOut));
                if (status == "code-completion-server-ready")
                {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    PutOutputServiceMessage(status);
                    PutOutputServiceMessage("code completion server started");
                    socket.Connect("localhost", std::to_string(serverPort));
                    ProcessRequests();
                }
                else
                {
                    std::string errorMessage = codeCompletionServerProcess->ReadLine(Process::StdHandle::stdOut);
                    PutOutputServiceMessage("error: code completion server status is: " + status + ": " + errorMessage);
                }
            }
            if (!codeCompletionServerProcessTerminated)
            {
                codeCompletionServerProcessTerminated = true;
                codeCompletionServerProcess->Terminate();
            }
            codeCompletionServerProcess.reset();
        }
        catch (const std::exception& ex)
        {
            if (!codeCompletionServerProcessTerminated)
            {
                codeCompletionServerProcessTerminated = true;
                codeCompletionServerProcess->Terminate();
            }
            codeCompletionServerProcess.reset();
            PutOutputServiceMessage("error: code completion service run: " + std::string(ex.what()));
            StopKeepAliveThread();
            running = false;
        }
    }
    catch (const std::exception& ex)
    {
        PutOutputServiceMessage("error: could not start code completion server: " + std::string(ex.what()));
        StopKeepAliveThread();
        running = false;
    }
}

bool CodeCompletionService::Running()
{
    running = codeCompletionServerProcess && codeCompletionServerProcess->Running();
    return running;
}

void CodeCompletionService::LoadEditModule(const std::string& projectFilePath, const std::string& backend, const std::string& config)
{
    LoadEditModuleRequest request;
    request.projectFilePath = projectFilePath;
    request.backend = backend;
    request.config = config;
    std::unique_ptr<sngxml::dom::Element> requestElement = request.ToXml("loadEditModuleRequest");
    WriteMessage(requestElement.release());
    std::unique_ptr<sngxml::dom::Document> replyDoc = ReadReply("loadEditModuleReply");
    LoadEditModuleReply reply(replyDoc->DocumentElement());
    PutServiceMessage(new LoadEditModuleReplyServiceMessage(reply));
}

void CodeCompletionService::ResetEditModuleCache()
{
    ResetEditModuleCacheRequest request;
    std::unique_ptr<sngxml::dom::Element> requestElement = request.ToXml("resetEditModuleCacheRequest");
    WriteMessage(requestElement.release());
    std::unique_ptr<sngxml::dom::Document> replyDoc = ReadReply("resetEditModuleCacheReply");
    ResetEditModuleCacheReply reply(replyDoc->DocumentElement());
    PutServiceMessage(new ResetEditModuleCacheReplyServiceMessage(reply));
}

void CodeCompletionService::ParseSource(const std::string& projectFilePath, const std::string& backend, const std::string& config, const std::string& sourceFilePath, std::u32string&& sourceCode)
{
    ParseSourceRequest request;
    request.projectFilePath = projectFilePath;
    request.backend = backend;
    request.config = config;
    request.sourceFilePath = sourceFilePath;
    request.sourceCode = std::move(sourceCode);
    std::unique_ptr<sngxml::dom::Element> requestElement = request.ToXml("parseSourceRequest");
    WriteMessage(requestElement.release());
    std::unique_ptr<sngxml::dom::Document> replyDoc = ReadReply("parseSourceReply");
    ParseSourceReply reply(replyDoc->DocumentElement());
    PutServiceMessage(new ParseSourceReplyServiceMessage(reply));
}

void CodeCompletionService::GetCCList(const std::string& projectFilePath, const std::string& backend, const std::string& config, const std::string& sourceFilePath, const std::string& ccText)
{
    GetCCListRequest request;
    request.projectFilePath = projectFilePath;
    request.backend = backend;
    request.config = config;
    request.sourceFilePath = sourceFilePath;
    request.ccText = ccText;
    std::unique_ptr<sngxml::dom::Element> requestElement = request.ToXml("getCCListRequest");
    WriteMessage(requestElement.release());
    std::unique_ptr<sngxml::dom::Document> replyDoc = ReadReply("getCCListReply");
    GetCCListReply reply(replyDoc->DocumentElement());
    PutServiceMessage(new GetCCListReplyServiceMessage(reply));
}

CodeCompletionServiceRequest::~CodeCompletionServiceRequest()
{
}

RunLoadEditModuleServiceRequest::RunLoadEditModuleServiceRequest(const std::string& projectFilePath_, const std::string& backend_, const std::string& config_) : 
    projectFilePath(projectFilePath_), backend(backend_), config(config_)
{
}

void RunLoadEditModuleServiceRequest::Execute()
{
    CodeCompletionService::Instance().LoadEditModule(projectFilePath, backend, config);
}

void RunLoadEditModuleServiceRequest::Failed(const std::string& error)
{
    PutServiceMessage(new LoadEditModuleErrorServiceMessage(error));
}

LoadEditModuleReplyServiceMessage::LoadEditModuleReplyServiceMessage(const LoadEditModuleReply& reply_) : ServiceMessage(ServiceMessageKind::loadEditModuleReply), reply(reply_)
{
}

LoadEditModuleErrorServiceMessage::LoadEditModuleErrorServiceMessage(const std::string& error_) : ServiceMessage(ServiceMessageKind::loadEditModuleError), error(error_)
{
}

void RunResetEditModuleCacheServiceRequest::Execute()
{
    CodeCompletionService::Instance().ResetEditModuleCache();
}

void RunResetEditModuleCacheServiceRequest::Failed(const std::string& error)
{
    PutServiceMessage(new ResetEditModuleCacheErrorServiceMessage(error));
}

ResetEditModuleCacheReplyServiceMessage::ResetEditModuleCacheReplyServiceMessage(const ResetEditModuleCacheReply& reply_) : ServiceMessage(ServiceMessageKind::resetEditModuleCacheReply), reply(reply_)
{
}

ResetEditModuleCacheErrorServiceMessage::ResetEditModuleCacheErrorServiceMessage(const std::string& error_) : ServiceMessage(ServiceMessageKind::resetEditModuleCacheError), error(error_)
{
}

RunParseSourceServiceRequest::RunParseSourceServiceRequest(const std::string& projectFilePath_, const std::string& backend_, const std::string& config_, const std::string& sourceFilePath_, std::u32string&& sourceCode_) : 
    projectFilePath(projectFilePath_), backend(backend_), config(config_), sourceFilePath(sourceFilePath_), sourceCode(std::move(sourceCode_))
{
}

void RunParseSourceServiceRequest::Execute()
{
    CodeCompletionService::Instance().ParseSource(projectFilePath, backend, config, sourceFilePath, std::move(sourceCode));
}

void RunParseSourceServiceRequest::Failed(const std::string& error)
{
    PutServiceMessage(new ParseSourceErrorServiceMessage(error));
}

ParseSourceReplyServiceMessage::ParseSourceReplyServiceMessage(const ParseSourceReply& reply_) : ServiceMessage(ServiceMessageKind::parseSourceReply), reply(reply_)
{
}

ParseSourceErrorServiceMessage::ParseSourceErrorServiceMessage(const std::string& error_) : ServiceMessage(ServiceMessageKind::parseSourceError), error(error_)
{
}

RunGetCCListServiceRequest::RunGetCCListServiceRequest(const std::string& projectFilePath_, const std::string& backend_, const std::string& config_, const std::string& sourceFilePath_, const std::string& ccText_) : 
    projectFilePath(projectFilePath_), backend(backend_), config(config_), sourceFilePath(sourceFilePath_), ccText(ccText_)
{
}

void RunGetCCListServiceRequest::Execute()
{
    CodeCompletionService::Instance().GetCCList(projectFilePath, backend, config, sourceFilePath, ccText);
}

void RunGetCCListServiceRequest::Failed(const std::string& error)
{
    PutServiceMessage(new GetCCListErrorServiceMessage(error));
}

GetCCListReplyServiceMessage::GetCCListReplyServiceMessage(const GetCCListReply& reply_) : ServiceMessage(ServiceMessageKind::getCCListReply), reply(reply_)
{
}

GetCCListErrorServiceMessage::GetCCListErrorServiceMessage(const std::string& error_) : ServiceMessage(ServiceMessageKind::getCCListError), error(error_)
{
}

void StartCodeCompletionService(CodeCompletionServiceStartParams& startParams)
{
    CodeCompletionService::Instance().Start(startParams);
}

void StopCodeCompletionService(bool log)
{
    CodeCompletionService::Instance().Stop(log);
}

bool CodeCompletionServiceRunning()
{
    return CodeCompletionService::Instance().Running();
}

void LoadEditModule(const std::string& projectFilePath, const std::string& backend, const std::string& config)
{
    CodeCompletionService::Instance().PutRequest(new RunLoadEditModuleServiceRequest(projectFilePath, backend, config));
}

void ResetEditModuleCache()
{
    CodeCompletionService::Instance().PutRequest(new RunResetEditModuleCacheServiceRequest());
}

void ParseSource(const std::string& projectFilePath, const std::string& backend, const std::string& config, const std::string& sourceFilePath, std::u32string&& sourceCode)
{
    CodeCompletionService::Instance().PutRequest(new RunParseSourceServiceRequest(projectFilePath, backend, config, sourceFilePath, std::move(sourceCode)));
}

void GetCCList(const std::string& projectFilePath, const std::string& backend, const std::string& config, const std::string& sourceFilePath, const std::string& ccText)
{
    CodeCompletionService::Instance().PutRequest(new RunGetCCListServiceRequest(projectFilePath, backend, config, sourceFilePath, ccText));
}

void InitCodeCompletionService()
{
    CodeCompletionService::Init();
}

void DoneCodeCompletionService()
{
    CodeCompletionService::Done();
}

} } // namespace cmajor::service
