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

CodeCompletionServiceRequest::~CodeCompletionServiceRequest()
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

void InitCodeCompletionService()
{
    CodeCompletionService::Init();
}

void DoneCodeCompletionService()
{
    CodeCompletionService::Done();
}

} } // namespace cmajor::service
