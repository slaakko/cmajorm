// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsvc/BuildService.hpp>
#include <cmajor/cmsvc/PortMapService.hpp>
#include <cmajor/cmsvc/Message.hpp>
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
const int defaultBuildServerPort = 54325;
const int defaultKeepAliveServerPort = 54329;

BuildServiceStartParams::BuildServiceStartParams() : debugServer(false), log(false), wait(false)
{
}

BuildServiceStartParams& BuildServiceStartParams::ProcessName(const std::string& processName_)
{
    processName = processName_;
    return *this;
}

BuildServiceStartParams& BuildServiceStartParams::DebugServer(bool debugServer_)
{
    debugServer = debugServer_;
    return *this;
}

BuildServiceStartParams& BuildServiceStartParams::Log(bool log_)
{
    log = log_;
    return *this;
}

BuildServiceStartParams& BuildServiceStartParams::Wait(bool wait_)
{
    wait = wait_;
    return *this;
}

class BuildService
{
public:
    static void Init();
    static void Done();
    static BuildService& Instance() { return *instance; }
    void Start(BuildServiceStartParams& startParams_);
    bool Running();
    void Stop();
    void Stop(bool log);
    void Run();
    void Quit();
    void KeepAlive();
    void Put(BuildServiceRequest* request);
    void ProcessRequests();
    void ExecuteRequest(BuildServiceRequest* request);
    void ProcessBuildRequest(BuildRequest& buildRequest);
    ~BuildService();
private:
    BuildService();
    BuildServiceStartParams startParams;
    std::string MakeBuildServerStartCommand(std::string& startStatus);
    void StartKeepAliveThread();
    void StopKeepAliveThread();
    void SendReceiveKeepAlive();
    static std::unique_ptr<BuildService> instance;
    bool running;
    bool exiting;
    bool keepAliveThreadStarted;
    bool keepAliveThreadRunning;
    bool stopKeepAlives;
    bool serviceThreadStarted;
    bool requestInProgress;
    bool buildServerProcessTerminated;
    int serverPort;
    int keepAliveServerPort;
    std::thread serviceThread;
    std::thread keepAliveThread;
    std::unique_ptr<Process> buildServerProcess;
    std::mutex keepAliveMutex;
    std::condition_variable stopKeepAliveVar;
    std::mutex requestMutex;
    std::condition_variable requestAvailableOrExiting;
    std::list<std::unique_ptr<BuildServiceRequest>> requestQueue;
};

std::unique_ptr<BuildService> BuildService::instance;

void BuildService::Init()
{
    instance.reset(new BuildService());
}

void BuildService::Done()
{
    instance.reset();
}

BuildService::BuildService() : 
    running(false), 
    exiting(false), 
    requestInProgress(false), 
    keepAliveThreadStarted(false), 
    keepAliveThreadRunning(false), 
    stopKeepAlives(false), 
    serviceThreadStarted(false), 
    buildServerProcessTerminated(false),
    serverPort(-1),
    keepAliveServerPort(-1)
{
}

BuildService::~BuildService()
{
    Stop();
}

std::string BuildService::MakeBuildServerStartCommand(std::string& startStatus)
{
    startStatus.clear();
    std::string startCommand = "cmbs";
    startStatus = "starting build server (cmbs):";
    if (startParams.debugServer)
    {
        startCommand = "cmbsd";
        startStatus = "starting build server (cmbsd):";
    }
    serverPort = GetFreePortNumber(startParams.processName);
    keepAliveServerPort = -1;
    if (serverPort == -1)
    {
        serverPort = defaultBuildServerPort;
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
    if (startParams.log)
    {
        startCommand.append(" --log");
    }
    if (startParams.wait)
    {
        startCommand.append(" --wait");
    }
    startStatus.append("...");
    return startCommand;
}

void BuildService::KeepAlive()
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
        PutOutputServiceMessage("error: build service keep alive: " + std::string(ex.what()));
    }
}

void BuildService::SendReceiveKeepAlive()
{
    try
    {
        TcpSocket socket("localhost", std::to_string(keepAliveServerPort));
        KeepAliveBuildRequest keepAliveRequest;
        std::unique_ptr<sngxml::dom::Element> requestElement = keepAliveRequest.ToXml("keepAliveBuildRequest");
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
        if (message != "keepAliveBuildReply")
        {
            throw std::runtime_error("keepAliveBuildReply expected");
        }
        KeepAliveBuildReply reply(replyDoc->DocumentElement());
    }
    catch (const std::exception& ex)
    {
        PutOutputServiceMessage("error: build service send/receive keep alive: " + std::string(ex.what()));
    }
}

void RunKeepAliveThread()
{
    BuildService::Instance().KeepAlive();
}

void BuildService::StartKeepAliveThread()
{
    keepAliveThreadStarted = true;
    stopKeepAlives = false;
    keepAliveThread = std::thread(RunKeepAliveThread);
}

void BuildService::StopKeepAliveThread()
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

void BuildService::Put(BuildServiceRequest* request)
{
    std::unique_lock<std::mutex> lock(requestMutex);
    requestQueue.push_back(std::unique_ptr<BuildServiceRequest>(request));
    requestAvailableOrExiting.notify_one();
}

void BuildService::ProcessRequests()
{
    try
    {
        while (!exiting)
        {
            std::unique_ptr<BuildServiceRequest> request;
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
        PutOutputServiceMessage("error: build service process requests: " + std::string(ex.what()));
        running = false;
    }
}

struct BuildServiceRequestGuard
{
    BuildServiceRequestGuard(bool& requestInProgress_) : requestInProgress(requestInProgress_)
    {
        requestInProgress = true;
    }
    ~BuildServiceRequestGuard()
    {
        requestInProgress = false;
    }
    bool& requestInProgress;
};

void BuildService::ExecuteRequest(BuildServiceRequest* request)
{
    BuildServiceRequestGuard requestGuard(requestInProgress);
    try
    {
        request->Execute();
    }
    catch (const std::exception& ex)
    {
        PutOutputServiceMessage("build service: error executing " + request->Name() + ": " + std::string(ex.what()));
        request->Failed(ex.what());
    }
}

void BuildService::ProcessBuildRequest(BuildRequest& buildRequest)
{
    std::unique_ptr<sngxml::dom::Element> requestElement = buildRequest.ToXml("buildRequest");
    sngxml::dom::Document requestDoc;
    requestDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(requestElement.release()));
    std::stringstream strStream;
    CodeFormatter formatter(strStream);
    requestDoc.Write(formatter);
    std::string requestStr = strStream.str();
    TcpSocket socket("localhost", std::to_string(serverPort));
    Write(socket, requestStr);
    std::string messageStr = ReadStr(socket);
    std::unique_ptr<sngxml::dom::Document> messageDoc = sngxml::dom::ParseDocument(ToUtf32(messageStr), "socket");
    std::string message = ToUtf8(messageDoc->DocumentElement()->Name());
    while (message == "logBuildMessageRequest")
    {
        LogBuildMessageRequest logBuildMessageRequest(messageDoc->DocumentElement());
        PutOutputServiceMessage(logBuildMessageRequest.message);
        LogBuildMessageReply logBuildMessageReply;
        logBuildMessageReply.ok = true;
        std::unique_ptr<sngxml::dom::Element> logMessageReplyElement = logBuildMessageReply.ToXml("logBuildMessageReply");
        sngxml::dom::Document logMessageReplyDoc;
        logMessageReplyDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(logMessageReplyElement.release()));
        std::stringstream strStream;
        CodeFormatter formatter(strStream);
        logMessageReplyDoc.Write(formatter);
        std::string logMessageReplyStr = strStream.str();
        Write(socket, logMessageReplyStr);
        messageStr = ReadStr(socket);
        messageDoc = sngxml::dom::ParseDocument(ToUtf32(messageStr), "socket");
        message = ToUtf8(messageDoc->DocumentElement()->Name());
    }
    if (message == "buildReply")
    {
        BuildReply buildReply(messageDoc->DocumentElement());
        PutServiceMessage(new BuildReplyServiceMessage(buildReply));
    }
    else if (message == "genericBuildErrorReply")
    {
        GenericBuildErrorReply errorReply(messageDoc->DocumentElement());
        throw std::runtime_error("generic build error received: " + errorReply.error);
    }
    else
    {
        throw std::runtime_error("unknown build message '" + message + "' received");
    }
}

void BuildService::Run()
{
    try
    {
        running = true;
        StartKeepAliveThread();
        std::string startStatus;
        std::string buildServerStartCommand = MakeBuildServerStartCommand(startStatus);
        PutOutputServiceMessage(startStatus);
        buildServerProcessTerminated = false;
        buildServerProcess.reset(new Process(buildServerStartCommand, Process::Redirections::processStdIn | Process::Redirections::processStdOut | Process::Redirections::processStdErr));
        try
        {
            if (!exiting)
            {
                std::string status = Trim(buildServerProcess->ReadLine(Process::StdHandle::stdOut));
                if (status == "build-server-ready")
                {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    PutOutputServiceMessage(status);
                    PutOutputServiceMessage("build server started");
                    ProcessRequests();
                }
                else
                {
                    std::string errorMessage = buildServerProcess->ReadLine(Process::StdHandle::stdOut);
                    PutOutputServiceMessage("error: build server status is: " + status + ": " + errorMessage);
                }
            }
            if (!buildServerProcessTerminated)
            {
                buildServerProcessTerminated = true;
                buildServerProcess->Terminate();
            }
            buildServerProcess.reset();
        }
        catch (const std::exception& ex)
        {
            if (!buildServerProcessTerminated)
            {
                buildServerProcessTerminated = true;
                buildServerProcess->Terminate();
            }
            buildServerProcess.reset();
            PutOutputServiceMessage("error: build service run: " + std::string(ex.what()));
            StopKeepAliveThread();
            running = false;
        }
    }
    catch (const std::exception& ex)
    {
        PutOutputServiceMessage("error: could not start build server: " + std::string(ex.what()));
        StopKeepAliveThread();
        running = false;
    }
}

void RunBuildService()
{
    BuildService::Instance().Run();
}

void BuildService::Start(BuildServiceStartParams& startParams_)
{
    startParams = startParams_;
    exiting = false;
    running = false;
    serviceThread = std::thread(RunBuildService);
    serviceThreadStarted = true;
}

void BuildService::Quit()
{
    try
    {
        if (!buildServerProcess) return;
        if (buildServerProcessTerminated) return;
        buildServerProcessTerminated = true;
        buildServerProcess->Terminate();
        running = false;
    }
    catch (...)
    {
    }
}

void BuildService::Stop()
{
    Stop(false);
}

bool BuildService::Running() 
{
    running = buildServerProcess && buildServerProcess->Running();
    return running;
}

void BuildService::Stop(bool log)
{
    try
    {
        if (log)
        {
            PutOutputServiceMessage("stopping build server...");
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
                if (requestInProgress)
                {
                    Quit();
                }
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
            PutOutputServiceMessage("build server stopped");
        }
    }
    catch (...)
    {
    }
}

BuildServiceRequest::~BuildServiceRequest()
{
}

RunBuildRequest::RunBuildRequest(const BuildRequest& buildRequest_) : buildRequest(buildRequest_)
{
}

void RunBuildRequest::Execute()
{
    BuildService::Instance().ProcessBuildRequest(buildRequest);
}

void RunBuildRequest::Failed(const std::string& error)
{
    PutServiceMessage(new BuildErrorServiceMessage(error));
}

BuildReplyServiceMessage::BuildReplyServiceMessage(const BuildReply& buildReply_) : ServiceMessage(ServiceMessageKind::buildReply), buildReply(buildReply_)
{
}

BuildErrorServiceMessage::BuildErrorServiceMessage(const std::string& error_) : ServiceMessage(ServiceMessageKind::buildError), error(error_)
{
}

StopBuildServiceMessage::StopBuildServiceMessage() : ServiceMessage(ServiceMessageKind::stopBuild)
{
}

void StartBuildService(BuildServiceStartParams& startParams)
{
    BuildService::Instance().Start(startParams);
}

void EnqueueBuildServiceRequest(BuildServiceRequest* request)
{
    BuildService::Instance().Put(request);
}

bool BuildServiceRunning()
{
    return BuildService::Instance().Running();
}

void StopBuildService(bool log)
{
    BuildService::Instance().Stop(log);
}

void InitBuildService()
{
    BuildService::Init();
}

void DoneBuildService()
{
    BuildService::Done();
}

} } // namespace cmajor::service
