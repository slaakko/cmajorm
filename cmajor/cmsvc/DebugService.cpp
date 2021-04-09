// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsvc/DebugService.hpp>
#include <cmajor/cmsvc/PortMapService.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/Parser.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Process.hpp>
#include <soulng/util/Socket.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <condition_variable>
#include <list>
#include <thread>
#include <mutex>
#include <sstream>

#undef min
#undef max

namespace cmajor { namespace service {

using namespace sngxml::dom;
using namespace soulng::unicode;
using namespace soulng::util;

const int defaultDebugServerPort = 54326;
const int defaultSessionPort = 54322;

enum class DebugMessageKind : int
{
    startDebugRequest, startDebugReply, stopDebugRequest, stopDebugReply, genericDebugErrorReply, 
    continueRequest, continueReply, nextRequest, nextReply, stepRequest, stepReply, finishRequest, finishReply, untilRequest, untilReply, breakRequest, breakReply, deleteRequest, deleteReply, 
    depthRequest, depthReply, framesRequest, framesReply, evaluateRequest, evaluateReply, countRequest, countReply, evaluateChildRequest, evaluateChildReply,
    targetRunningRequest, targetRunningReply, targetInputRequest, targetInputReply, targetOutputRequest, targetOutputReply, logDebugMessageRequest, logDebugMessageReply
};

DebugServiceStartParams::DebugServiceStartParams() : pid(0), debugServer(false), log(false), wait(false), verbose(false), toolChain("gcc")
{
}

DebugServiceStartParams& DebugServiceStartParams::ProcessName(const std::string& processName_)
{
    processName = processName_;
    return *this;
}

DebugServiceStartParams& DebugServiceStartParams::Pid(int pid_)
{
    pid = pid_;
    return *this;
}

DebugServiceStartParams& DebugServiceStartParams::Backend(const std::string& backend_)
{
    backend = backend_;
    return *this;
}

DebugServiceStartParams& DebugServiceStartParams::ToolChain(const std::string& toolChain_)
{
    toolChain = toolChain_;
    return *this;
}

DebugServiceStartParams& DebugServiceStartParams::Config(const std::string& config_)
{
    config = config_;
    return *this;
}

DebugServiceStartParams& DebugServiceStartParams::ProjectFilePath(const std::string& projectFilePath_)
{
    projectFilePath = projectFilePath_;
    return *this;
}

DebugServiceStartParams& DebugServiceStartParams::ExecutableName(const std::string& executableName_)
{
    executableName = executableName_;
    return *this;
}

DebugServiceStartParams& DebugServiceStartParams::ProgramArguments(const std::string& programArguments_)
{
    programArguments = programArguments_;
    return *this;
}

DebugServiceStartParams& DebugServiceStartParams::DebugServer(bool debugServer_)
{
    debugServer = debugServer_;
    return *this;
}

DebugServiceStartParams& DebugServiceStartParams::Log(bool log_)
{
    log = log_;
    return *this;
}

DebugServiceStartParams& DebugServiceStartParams::Wait(bool wait_)
{
    wait = wait_;
    return *this;
}

DebugServiceStartParams& DebugServiceStartParams::Verbose(bool verbose_)
{
    verbose = verbose_;
    return *this;
}

class DebugService
{
public:
    static void Init();
    static void Done();
    static DebugService& Instance() { return *instance; }
    void Start(const DebugServiceStartParams& startParams, const std::vector<Breakpoint*>& breakpoints_);
    void Run();
    void Stop();
    void PutRequest(DebugServiceRequest* request);
    void WriteMessage(sngxml::dom::Element* messageElement);
    std::unique_ptr<sngxml::dom::Document> ReadReply(DebugMessageKind messageKind);
    void RunStartRequest();
    void RunStopRequest();
    void SetTargetInputEof();
    void PutTargetInputLine(const std::string& targetInputLine);
    void Terminate();
    void RunContinueRequest();
    void RunStepRequest();
    void RunNextRequest();
    void RunFinishRequest();
    void RunUntilRequest(const SourceLoc& sourceLocation);
    void RunBreakRequest(Breakpoint* breakpoint);
    void RunDeleteRequest(const std::string& breakpointId);
    void RunDepthRequest();
    void RunFramesRequest(int lowFrame, int highFrame);
    void RunEvaluateRequest(const std::string& expression, int requestId);
    void RunCountRequest(const std::string& expression);
    void RunEvaluateChildRequest(const std::string& expression, int start, int count);
    void SetRequestInProgress(const std::string& requestName);
    void ResetRequestInProgress();
    bool RequestInProgress(std::string& requestName);
    void ClearRequestQueue();
    void SendKillRequest();
private:
    DebugService();
    DebugMessageKind GetDebugMessageKind(const std::u32string& messageName) const;
    void ProcessMessage(sngxml::dom::Element* messageElement, DebugMessageKind messageKind);
    void ProcessTargetRunningRequest(const TargetRunningRequest& targetRunningRequest);
    void ProcessTargetInputRequest(const TargetInputRequest& targetInputRequest);
    void ProcessTargetOutputRequest(const TargetOutputRequest& targetOutputRequest);
    void ProcessLogDebugMessageRequest(const LogDebugMessageRequest& logDebugMessageRequest);
    void ProcessGenericDebugErrorReply(const GenericDebugErrorReply& genericDebugErrorReply);
    void ProcessUnexpectedDebugMessage(const std::string& messageName);
    void MakeDebugServiceStartCommand(const DebugServiceStartParams& startParams);
    void ExecuteRequest(DebugServiceRequest* request);
    static std::unique_ptr<DebugService> instance;
    std::string startCommand;
    std::string startStatus;
    std::vector<Breakpoint*> breakpoints;
    std::thread serviceThread;
    std::unique_ptr<soulng::util::Process> serverProcess;
    std::condition_variable requestAvailableOrStopping;
    std::condition_variable targetInputLineAvailableOrEof;
    std::condition_variable stopped;
    std::mutex queueMutex;
    std::mutex inputMutex;
    std::list<std::unique_ptr<DebugServiceRequest>> requestQueue;
    std::list<std::string> targetInputLines;
    bool waitingForTargetInput;
    bool targetInputEof;
    int serverPort;
    int killPort;
    TcpSocket socket;
    bool started;
    bool running;
    bool stop;
    bool requestInProgress;
    bool terminated;
    std::string runningRequestName;
    std::map<std::u32string, DebugMessageKind> messageKindMap;
};

std::unique_ptr<DebugService> DebugService::instance;

DebugService::DebugService() : serverPort(0), killPort(0), waitingForTargetInput(false), targetInputEof(false), started(false), running(false), stop(false), terminated(false), requestInProgress(false)
{
    messageKindMap[U"startDebugRequest"] = DebugMessageKind::startDebugRequest;
    messageKindMap[U"startDebugReply"] = DebugMessageKind::startDebugReply;
    messageKindMap[U"stopDebugRequest"] = DebugMessageKind::stopDebugRequest;
    messageKindMap[U"stopDebugReply"] = DebugMessageKind::stopDebugReply;
    messageKindMap[U"genericDebugErrorReply"] = DebugMessageKind::genericDebugErrorReply;
    messageKindMap[U"continueRequest"] = DebugMessageKind::continueRequest;
    messageKindMap[U"continueReply"] = DebugMessageKind::continueReply;
    messageKindMap[U"nextRequest"] = DebugMessageKind::nextRequest;
    messageKindMap[U"nextReply"] = DebugMessageKind::nextReply;
    messageKindMap[U"stepRequest"] = DebugMessageKind::stepRequest;
    messageKindMap[U"stepReply"] = DebugMessageKind::stepReply;
    messageKindMap[U"finishRequest"] = DebugMessageKind::finishRequest;
    messageKindMap[U"finishReply"] = DebugMessageKind::finishReply;
    messageKindMap[U"untilRequest"] = DebugMessageKind::untilRequest;
    messageKindMap[U"untilReply"] = DebugMessageKind::untilReply;
    messageKindMap[U"breakRequest"] = DebugMessageKind::breakRequest;
    messageKindMap[U"breakReply"] = DebugMessageKind::breakReply;
    messageKindMap[U"deleteRequest"] = DebugMessageKind::deleteRequest;
    messageKindMap[U"deleteReply"] = DebugMessageKind::deleteReply;
    messageKindMap[U"depthRequest"] = DebugMessageKind::depthRequest;
    messageKindMap[U"depthReply"] = DebugMessageKind::depthReply;
    messageKindMap[U"framesRequest"] = DebugMessageKind::framesRequest;
    messageKindMap[U"framesReply"] = DebugMessageKind::framesReply;
    messageKindMap[U"evaluateRequest"] = DebugMessageKind::evaluateRequest;
    messageKindMap[U"evaluateReply"] = DebugMessageKind::evaluateReply;
    messageKindMap[U"countRequest"] = DebugMessageKind::countRequest;
    messageKindMap[U"countReply"] = DebugMessageKind::countReply;
    messageKindMap[U"evaluateChildRequest"] = DebugMessageKind::evaluateChildRequest;
    messageKindMap[U"evaluateChildReply"] = DebugMessageKind::evaluateChildReply;
    messageKindMap[U"targetRunningRequest"] = DebugMessageKind::targetRunningRequest;
    messageKindMap[U"targetRunningReply"] = DebugMessageKind::targetRunningReply;
    messageKindMap[U"targetInputRequest"] = DebugMessageKind::targetInputRequest;
    messageKindMap[U"targetInputReply"] = DebugMessageKind::targetInputReply;
    messageKindMap[U"targetOutputRequest"] = DebugMessageKind::targetOutputRequest;
    messageKindMap[U"targetOutputReply"] = DebugMessageKind::targetOutputReply;
    messageKindMap[U"logDebugMessageRequest"] = DebugMessageKind::logDebugMessageRequest;
    messageKindMap[U"logDebugMessageReply"] = DebugMessageKind::logDebugMessageReply;
}

DebugMessageKind DebugService::GetDebugMessageKind(const std::u32string& messageName) const
{
    auto it = messageKindMap.find(messageName);
    if (it != messageKindMap.cend())
    {
        return it->second;
    }
    throw std::runtime_error("unknown debug message '" + ToUtf8(messageName) + "' received");
}

void DebugService::ProcessMessage(sngxml::dom::Element* messageElement, DebugMessageKind messageKind)
{
    switch (messageKind)
    {
        case DebugMessageKind::targetRunningRequest:
        {
            TargetRunningRequest targetRunningRequest(messageElement);
            ProcessTargetRunningRequest(targetRunningRequest);
            break;
        }
        case DebugMessageKind::targetInputRequest:
        {
            TargetInputRequest targetInputRequest(messageElement);
            ProcessTargetInputRequest(targetInputRequest);
            break;
        }
        case DebugMessageKind::targetOutputRequest:
        {
            TargetOutputRequest targetOutputRequest(messageElement);
            ProcessTargetOutputRequest(targetOutputRequest);
            break;
        }
        case DebugMessageKind::logDebugMessageRequest:
        {
            LogDebugMessageRequest logDebugMessageRequest(messageElement);
            ProcessLogDebugMessageRequest(logDebugMessageRequest);
            break;
        }
        case DebugMessageKind::genericDebugErrorReply:
        {
            GenericDebugErrorReply genericDebugErrorReply(messageElement);
            ProcessGenericDebugErrorReply(genericDebugErrorReply);
            break;
        }
        default:
        {
            ProcessUnexpectedDebugMessage(ToUtf8(messageElement->Name()));
            break;
        }
    }
}

void DebugService::ProcessTargetRunningRequest(const TargetRunningRequest& targetRunningRequest)
{
    PutServiceMessage(new TargetRunningServiceMessage());
    TargetRunningReply targetRunningReply;
    std::unique_ptr<sngxml::dom::Element> replyElement = targetRunningReply.ToXml("targetRunningReply");
    WriteMessage(replyElement.release());
}

void DebugService::ProcessTargetInputRequest(const TargetInputRequest& targetInputRequest)
{
    std::unique_lock<std::mutex> lock(inputMutex);
    if (!targetInputEof && targetInputLines.empty())
    {
        waitingForTargetInput = true;
        PutServiceMessage(new TargetInputServiceMessage());
        targetInputLineAvailableOrEof.wait(lock, [this]{ return targetInputEof || !targetInputLines.empty(); });
    }
    waitingForTargetInput = false;
    if (targetInputEof)
    {
        TargetInputReply targetInputReply;
        targetInputReply.eof = true;
        std::unique_ptr<sngxml::dom::Element> replyElement = targetInputReply.ToXml("targetInputReply");
        WriteMessage(replyElement.release());
    }
    else
    {
        std::string targetInputLine = std::move(targetInputLines.front());
        targetInputLines.pop_front();
        TargetInputReply targetInputReply;
        targetInputReply.eof = false;
        targetInputReply.line = targetInputLine;
        std::unique_ptr<sngxml::dom::Element> replyElement = targetInputReply.ToXml("targetInputReply");
        WriteMessage(replyElement.release());
    }
}

void DebugService::ProcessTargetOutputRequest(const TargetOutputRequest& targetOutputRequest)
{
    PutServiceMessage(new TargetOutputServiceMessage(targetOutputRequest));
    TargetOutputReply targetOutputReply;
    std::unique_ptr<sngxml::dom::Element> replyElement = targetOutputReply.ToXml("targetOutputReply");
    WriteMessage(replyElement.release());
}

void DebugService::ProcessLogDebugMessageRequest(const LogDebugMessageRequest& logDebugMessageRequest)
{
    PutOutputServiceMessage(logDebugMessageRequest.logMessage);
    LogDebugMessageReply logDebugMessageReply;
    std::unique_ptr<sngxml::dom::Element> replyElement = logDebugMessageReply.ToXml("logDebugMessageReply");
    WriteMessage(replyElement.release());
}

void DebugService::ProcessGenericDebugErrorReply(const GenericDebugErrorReply& genericDebugErrorReply)
{
    PutOutputServiceMessage("error from debug server: " + genericDebugErrorReply.errorMessage);
}

void DebugService::ProcessUnexpectedDebugMessage(const std::string& messageName)
{
    PutOutputServiceMessage("error: unexpected message '" + messageName + "' received from debug server");
}

void DebugService::Init()
{
    instance.reset(new DebugService());
}

void DebugService::Done()
{
    instance.reset();
}

void DebugService::PutRequest(DebugServiceRequest* request)
{
    std::lock_guard<std::mutex> lock(queueMutex);
    requestQueue.push_back(std::unique_ptr<DebugServiceRequest>(request));
    requestAvailableOrStopping.notify_one();
}

void DebugService::WriteMessage(sngxml::dom::Element* messageElement)
{
    sngxml::dom::Document messageDoc;
    messageDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(messageElement));
    std::stringstream strStream; 
    CodeFormatter formatter(strStream);
    messageDoc.Write(formatter);
    Write(socket, strStream.str());
}

std::unique_ptr<sngxml::dom::Document> DebugService::ReadReply(DebugMessageKind replyKind)
{
    std::string replyStr = ReadStr(socket);
    std::unique_ptr<sngxml::dom::Document> replyDoc = ParseDocument(ToUtf32(replyStr), "socket");
    DebugMessageKind messageKind = GetDebugMessageKind(replyDoc->DocumentElement()->Name());
    while (messageKind != replyKind)
    {
        ProcessMessage(replyDoc->DocumentElement(), messageKind);
        replyStr = ReadStr(socket);
        replyDoc = ParseDocument(ToUtf32(replyStr), "socket");
        messageKind = GetDebugMessageKind(replyDoc->DocumentElement()->Name());
    }
    return replyDoc;
}

std::string MakeExecutablePath(const DebugServiceStartParams& startParams)
{
    std::string executablePath = Path::Combine(Path::Combine(Path::Combine(Path::Combine(Path::Combine(Path::GetDirectoryName(startParams.projectFilePath), "bin"), startParams.backend),
        startParams.toolChain), startParams.config), startParams.executableName);
    return executablePath;
}

void DebugService::MakeDebugServiceStartCommand(const DebugServiceStartParams& startParams)
{
    startCommand.clear();
    startStatus.clear();
    serverPort = 0;
    killPort = 0;
    if (startParams.debugServer)
    {
        startCommand.append("cmdbd");
        startStatus = "starting debug server (cmdbd)";
    }
    else
    {
        startCommand.append("cmdb");
        startStatus = "starting debug server (cmdb)";
    }
    startCommand.append(" --server");
    serverPort = GetFreePortNumber(startParams.processName);
    if (serverPort == -1)
    {
        serverPort = defaultDebugServerPort;
    }
    startCommand.append(" --port=").append(std::to_string(serverPort));
    int sessionPort = GetFreePortNumber(startParams.processName);
    if (sessionPort == -1)
    {
        sessionPort = defaultSessionPort;
    }
    startCommand.append(" --sessionPort=").append(std::to_string(sessionPort));
    killPort = GetFreePortNumber(startParams.processName);
    if (killPort != -1)
    {
        startCommand.append(" --killPort=").append(std::to_string(killPort));
    }
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
    if (startParams.verbose)
    {
        startCommand.append(" --verbose");
    }
    startCommand.append(" \"").append(MakeExecutablePath(startParams)).append("\"");
    if (!startParams.programArguments.empty())
    {
        startCommand.append(" ").append(startParams.programArguments);
    }
    startStatus.append("...");
}

void RunService(DebugService* service)
{
    service->Run();
}

void DebugService::Start(const DebugServiceStartParams& startParams, const std::vector<Breakpoint*>& breakpoints_)
{
    terminated = false;
    requestInProgress = false;
    targetInputEof = false;
    running = false;
    stop = false;
    MakeDebugServiceStartCommand(startParams);
    breakpoints = breakpoints_;
    serviceThread = std::thread(RunService, this);
    started = true;
}

void DebugService::Run()
{
    try
    {
        running = true;
        PutOutputServiceMessage(startStatus);
        serverProcess.reset(new Process(startCommand, Process::Redirections::processStdIn | Process::Redirections::processStdOut | Process::Redirections::processStdErr)); 
        std::string status = Trim(serverProcess->ReadLine(Process::StdHandle::stdOut)); 
        if (status == "debug-server-ready")
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            PutOutputServiceMessage(status);
            PutOutputServiceMessage("debug server started");
            socket.Connect("localhost", std::to_string(serverPort));
            while (!stop)
            {
                std::unique_ptr<DebugServiceRequest> request;
                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    requestAvailableOrStopping.wait(lock, [this] { return stop || !requestQueue.empty(); });
                    if (stop)
                    {
                        break;
                    }
                    request = std::move(requestQueue.front());
                    requestQueue.pop_front();
                }
                ExecuteRequest(request.get());
            }
            terminated = true;
            stopped.notify_all();
        }
        else
        {
            std::string errorMessage = serverProcess->ReadLine(Process::StdHandle::stdOut);
            PutOutputServiceMessage("error: debug server status is: " + status + ": " + errorMessage);
            serverProcess->WaitForExit();
        }
    }
    catch (const std::exception& ex)
    {
        running = false;
        PutOutputServiceMessage("debug service: error: " + std::string(ex.what()));
    }
}

struct RequestGuard
{
    RequestGuard(DebugService* service_, DebugServiceRequest* request_) : service(service_), request(request_) { service->SetRequestInProgress(request->Name());  }
    ~RequestGuard() { service->ResetRequestInProgress(); }
    DebugService* service;
    DebugServiceRequest* request;
};

void DebugService::ExecuteRequest(DebugServiceRequest* request)
{
    try
    {
        RequestGuard requestGuard(this, request);
        request->Execute();
    }
    catch (const std::exception& ex)
    {
        PutOutputServiceMessage("debug service: error executing " + request->Name() + ": " + ex.what());
        request->Failed(ex.what());
    }
}

void DebugService::Stop()
{
    try
    {
        if (running)
        {
            if (waitingForTargetInput)
            {
                SetTargetInputEof();
            }
            if (!requestInProgress)
            {
                PutRequest(new RunStopDebugServiceRequest());
                std::cv_status status = std::cv_status::no_timeout;
                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    status = stopped.wait_for(lock, std::chrono::seconds(3));
                }
                if (status == std::cv_status::timeout)
                {
                    SendKillRequest();
                }
                requestInProgress = false;
            }
            stop = true;
            requestAvailableOrStopping.notify_one();
            if (requestInProgress)
            {
                std::cv_status status = std::cv_status::no_timeout;
                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    status = stopped.wait_for(lock, std::chrono::seconds(3));
                }
                if (status == std::cv_status::timeout)
                {
                    if (requestInProgress)
                    {
                        SendKillRequest();
                    }
                }
            }
            DebugService::Terminate();
        }
        if (started)
        {
            started = false;
            serviceThread.join();
        }
        ClearRequestQueue();
        PutServiceMessage(new DebugServiceStoppedServiceMessage());
    }
    catch (const std::exception& ex)
    {
        PutOutputServiceMessage("error stopping debug service: " + std::string(ex.what()));
    }
}

void DebugService::RunStartRequest()
{
    StartDebugRequest request;
    for (Breakpoint* breakpoint : breakpoints)
    {
        SourceLoc breakpointLocation;
        if (!breakpoint->list)
        {
            throw std::runtime_error("source file path for the breakpoint not set");
        }
        breakpointLocation.path = breakpoint->list->FilePath();
        breakpointLocation.line = breakpoint->line;
        request.breakpointLocations.push_back(breakpointLocation);
    }
    std::unique_ptr<Element> requestElement = request.ToXml("startDebugRequest");
    WriteMessage(requestElement.release());
    std::unique_ptr<sngxml::dom::Document> replyDoc = ReadReply(DebugMessageKind::startDebugReply);
    StartDebugReply reply(replyDoc->DocumentElement());
    int n = std::min(reply.breakpointInfos.size(), breakpoints.size());
    for (int i = 0; i < n; ++i)
    {
        Breakpoint* breakpoint = breakpoints[i];
        breakpoint->info = reply.breakpointInfos[i];
    }
    PutServiceMessage(new StartReplyServiceMessage(reply));
}

void DebugService::RunStopRequest()
{
    StopDebugRequest request;
    std::unique_ptr<Element> requestElement = request.ToXml("stopDebugRequest");
    WriteMessage(requestElement.release());
    std::unique_ptr<sngxml::dom::Document> replyDoc = ReadReply(DebugMessageKind::stopDebugReply);
    StopDebugReply reply(replyDoc->DocumentElement());
    serverProcess->WaitForExit();
    stopped.notify_all();
    stop = true;
}

void DebugService::SetTargetInputEof()
{
    std::unique_lock<std::mutex> lock(inputMutex);
    targetInputEof = true;
    if (waitingForTargetInput)
    {
        targetInputLineAvailableOrEof.notify_one();
    }
}

void DebugService::PutTargetInputLine(const std::string& targetInputLine)
{
    std::unique_lock<std::mutex> lock(inputMutex);
    targetInputLines.push_back(targetInputLine);
    if (waitingForTargetInput)
    {
        targetInputLineAvailableOrEof.notify_one();
    }
}

void DebugService::Terminate()
{
    if (!terminated)
    {
        serverProcess->Terminate();
    }
}

void DebugService::RunContinueRequest()
{
    PutServiceMessage(new TargetRunningServiceMessage());
    ContinueRequest request;
    std::unique_ptr<Element> requestElement = request.ToXml("continueRequest");
    WriteMessage(requestElement.release());
    std::unique_ptr<sngxml::dom::Document> replyDoc = ReadReply(DebugMessageKind::continueReply);
    ContinueReply reply(replyDoc->DocumentElement());
    PutServiceMessage(new ContinueReplyServiceMessage(reply));
}

void DebugService::RunStepRequest()
{
    PutServiceMessage(new TargetRunningServiceMessage());
    StepRequest request;
    std::unique_ptr<Element> requestElement = request.ToXml("stepRequest");
    WriteMessage(requestElement.release());
    std::unique_ptr<sngxml::dom::Document> replyDoc = ReadReply(DebugMessageKind::stepReply);
    StepReply reply(replyDoc->DocumentElement());
    PutServiceMessage(new StepReplyServiceMessage(reply));
}

void DebugService::RunNextRequest()
{
    PutServiceMessage(new TargetRunningServiceMessage());
    NextRequest request;
    std::unique_ptr<Element> requestElement = request.ToXml("nextRequest");
    WriteMessage(requestElement.release());
    std::unique_ptr<sngxml::dom::Document> replyDoc = ReadReply(DebugMessageKind::nextReply);
    NextReply reply(replyDoc->DocumentElement());
    PutServiceMessage(new NextReplyServiceMessage(reply));
}

void DebugService::RunFinishRequest()
{
    PutServiceMessage(new TargetRunningServiceMessage());
    FinishRequest request;
    std::unique_ptr<Element> requestElement = request.ToXml("finishRequest");
    WriteMessage(requestElement.release());
    std::unique_ptr<sngxml::dom::Document> replyDoc = ReadReply(DebugMessageKind::finishReply);
    FinishReply reply(replyDoc->DocumentElement());
    PutServiceMessage(new FinishReplyServiceMessage(reply));
}

void DebugService::RunUntilRequest(const SourceLoc& sourceLocation)
{
    PutServiceMessage(new TargetRunningServiceMessage());
    UntilRequest request;
    request.sourceLoc = sourceLocation;
    std::unique_ptr<Element> requestElement = request.ToXml("untilRequest");
    WriteMessage(requestElement.release());
    std::unique_ptr<sngxml::dom::Document> replyDoc = ReadReply(DebugMessageKind::untilReply);
    UntilReply reply(replyDoc->DocumentElement());
    PutServiceMessage(new UntilReplyServiceMessage(reply));
}

void DebugService::RunBreakRequest(Breakpoint* breakpoint)
{
    BreakRequest request;
    if (!breakpoint->list)
    {
        throw std::runtime_error("source file path for the breakpoint not set");
    }
    request.breakpointLocation.path = breakpoint->list->FilePath();
    request.breakpointLocation.line = breakpoint->line;
    std::unique_ptr<Element> requestElement = request.ToXml("breakRequest");
    WriteMessage(requestElement.release());
    std::unique_ptr<sngxml::dom::Document> replyDoc = ReadReply(DebugMessageKind::breakReply);
    BreakReply reply(replyDoc->DocumentElement());
    breakpoint->info = reply.breakpointInfo;
    PutServiceMessage(new BreakReplyServiceMessage(reply));
}

void DebugService::RunDeleteRequest(const std::string& breakpointId)
{
    DeleteRequest request;
    request.breakpointId = breakpointId;
    std::unique_ptr<Element> requestElement = request.ToXml("deleteRequest");
    WriteMessage(requestElement.release());
    std::unique_ptr<sngxml::dom::Document> replyDoc = ReadReply(DebugMessageKind::deleteReply);
    DeleteReply reply(replyDoc->DocumentElement());
    PutServiceMessage(new DeleteReplyServiceMessage(reply));
}

void DebugService::RunDepthRequest()
{
    DepthRequest request;
    std::unique_ptr<Element> requestElement = request.ToXml("depthRequest");
    WriteMessage(requestElement.release());
    std::unique_ptr<sngxml::dom::Document> replyDoc = ReadReply(DebugMessageKind::depthReply);
    DepthReply reply(replyDoc->DocumentElement());
    PutServiceMessage(new DepthReplyServiceMessage(reply));
}

void DebugService::RunFramesRequest(int lowFrame, int highFrame)
{
    FramesRequest request;
    request.lowFrame = lowFrame;
    request.highFrame = highFrame;
    std::unique_ptr<Element> requestElement = request.ToXml("framesRequest");
    WriteMessage(requestElement.release());
    std::unique_ptr<sngxml::dom::Document> replyDoc = ReadReply(DebugMessageKind::framesReply);
    FramesReply reply(replyDoc->DocumentElement());
    PutServiceMessage(new FramesReplyServiceMessage(reply));
}

void DebugService::RunEvaluateRequest(const std::string& expression, int requestId)
{
    EvaluateRequest request;
    request.expression = expression;
    std::unique_ptr<Element> requestElement = request.ToXml("evaluateRequest");
    WriteMessage(requestElement.release());
    std::unique_ptr<sngxml::dom::Document> replyDoc = ReadReply(DebugMessageKind::evaluateReply);
    EvaluateReply reply(replyDoc->DocumentElement());
    PutServiceMessage(new EvaluateReplyServiceMessage(reply, requestId));
}

void DebugService::RunCountRequest(const std::string& expression)
{
    CountRequest request;
    request.expression = expression;
    std::unique_ptr<Element> requestElement = request.ToXml("countRequest");
    WriteMessage(requestElement.release());
    std::unique_ptr<sngxml::dom::Document> replyDoc = ReadReply(DebugMessageKind::countReply);
    CountReply reply(replyDoc->DocumentElement());
    PutServiceMessage(new CountReplyServiceMessage(reply));
}

void DebugService::RunEvaluateChildRequest(const std::string& expression, int start, int count)
{
    EvaluateChildRequest request;
    request.expression = expression;
    request.start = start;
    request.count = count;
    std::unique_ptr<Element> requestElement = request.ToXml("evaluateChildRequest");
    WriteMessage(requestElement.release());
    std::unique_ptr<sngxml::dom::Document> replyDoc = ReadReply(DebugMessageKind::evaluateChildReply);
    EvaluateChildReply reply(replyDoc->DocumentElement());
    PutServiceMessage(new EvaluateChildReplyServiceMessage(reply));
}

void DebugService::SetRequestInProgress(const std::string& requestName)
{
    requestInProgress = true;
    runningRequestName = requestName;
}

void DebugService::ResetRequestInProgress()
{
    requestInProgress = false;
}

bool DebugService::RequestInProgress(std::string& requestName)
{
    if (requestInProgress)
    {
        requestName = runningRequestName;
        return true;
    }
    else
    {
        return false;
    }
}

void DebugService::ClearRequestQueue()
{
    requestQueue.clear();
}

void DebugService::SendKillRequest()
{
    try
    {
        if (killPort == -1)
        {
            PutOutputServiceMessage("debug service: error: no kill port set, use Task Manager to terminate GDB.");
        }
        KillRequest killRequest;
        std::unique_ptr<sngxml::dom::Element> requestElement = killRequest.ToXml("killRequest");
        sngxml::dom::Document requestDoc;
        requestDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(requestElement.release()));
        std::stringstream strStream;
        CodeFormatter formatter(strStream);
        requestDoc.Write(formatter);
        std::string requestStr = strStream.str();
        TcpSocket socket("localhost", std::to_string(killPort));
        Write(socket, requestStr);
        std::string replyStr = ReadStr(socket);
        std::unique_ptr<sngxml::dom::Document> replyDoc = sngxml::dom::ParseDocument(ToUtf32(replyStr), "socket");
        if (replyDoc->DocumentElement()->Name() == U"killReply")
        {
            KillReply killReply(replyDoc->DocumentElement());
            PutOutputServiceMessage("debug service: kill request sent to cmdb");
        }
    }
    catch (const std::exception& ex)
    {
        PutOutputServiceMessage("debug service: could not send kill request to cmdb: " + std::string(ex.what()));
    }
    catch (...)
    {
        PutOutputServiceMessage("debug service: could not send kill request to cmdb: unknown exception occurred");
    }
}

DebugServiceRequest::~DebugServiceRequest()
{
}

RunStartDebugServiceRequest::RunStartDebugServiceRequest()
{
}

void RunStartDebugServiceRequest::Execute()
{
    DebugService::Instance().RunStartRequest();
}

std::string RunStartDebugServiceRequest::Name() const
{
    return "runStartDebugServiceRequest";
}

void RunStartDebugServiceRequest::Failed(const std::string& error)
{
    PutServiceMessage(new StartErrorServiceMessage(error));
}

StartReplyServiceMessage::StartReplyServiceMessage(const StartDebugReply& startReply_) : ServiceMessage(ServiceMessageKind::startDebugReply), startReply(startReply_)
{
}

StartErrorServiceMessage::StartErrorServiceMessage(const std::string& error_) : ServiceMessage(ServiceMessageKind::startError), error(error_)
{
}

RunStopDebugServiceRequest::RunStopDebugServiceRequest()
{
}

void RunStopDebugServiceRequest::Execute()
{
    DebugService::Instance().RunStopRequest();
}

std::string RunStopDebugServiceRequest::Name() const
{
    return "runStopDebugServiceRequest";
}

void RunStopDebugServiceRequest::Failed(const std::string& error)
{
    DebugService::Instance().ClearRequestQueue();
    PutServiceMessage(new DebugServiceStoppedServiceMessage());
}

TargetRunningServiceMessage::TargetRunningServiceMessage() : ServiceMessage(ServiceMessageKind::targetRunning)
{
}

TargetInputServiceMessage::TargetInputServiceMessage() : ServiceMessage(ServiceMessageKind::targetInput)
{
}

TargetOutputServiceMessage::TargetOutputServiceMessage(const TargetOutputRequest& targetOutputRequest_) : ServiceMessage(ServiceMessageKind::targetOutput), targetOutputRequest(targetOutputRequest_)
{
}

DebugServiceStoppedServiceMessage::DebugServiceStoppedServiceMessage() : ServiceMessage(ServiceMessageKind::debugServiceStopped)
{
}

RunContinueDebugServiceRequest::RunContinueDebugServiceRequest()
{
}

void RunContinueDebugServiceRequest::Execute()
{
    DebugService::Instance().RunContinueRequest();
}

std::string RunContinueDebugServiceRequest::Name() const
{
    return "runContinueDebugServiceRequest";
}

void RunContinueDebugServiceRequest::Failed(const std::string& error)
{
}

ContinueReplyServiceMessage::ContinueReplyServiceMessage(const ContinueReply& continueReply_) : ServiceMessage(ServiceMessageKind::continueReply), continueReply(continueReply_)
{
}

RunNextDebugServiceRequest::RunNextDebugServiceRequest()
{
}

void RunNextDebugServiceRequest::Execute()
{
    DebugService::Instance().RunNextRequest();
}

std::string RunNextDebugServiceRequest::Name() const
{
    return "runNextDebugServiceRequest";
}

void RunNextDebugServiceRequest::Failed(const std::string& error)
{
}

NextReplyServiceMessage::NextReplyServiceMessage(const NextReply& nextReply_) : ServiceMessage(ServiceMessageKind::nextReply), nextReply(nextReply_)
{
}

RunStepDebugServiceRequest::RunStepDebugServiceRequest()
{
}

void RunStepDebugServiceRequest::Execute()
{
    DebugService::Instance().RunStepRequest();
}

std::string RunStepDebugServiceRequest::Name() const
{
    return "runStepDebugServiceRequest";
}

void RunStepDebugServiceRequest::Failed(const std::string& error)
{
}

StepReplyServiceMessage::StepReplyServiceMessage(const StepReply& stepReply_) : ServiceMessage(ServiceMessageKind::stepReply), stepReply(stepReply_)
{
}

RunFinishDebugServiceRequest::RunFinishDebugServiceRequest()
{
}

void RunFinishDebugServiceRequest::Execute()
{
    DebugService::Instance().RunFinishRequest();
}

std::string RunFinishDebugServiceRequest::Name() const
{
    return "runFinishDebugServiceRequest";
}

void RunFinishDebugServiceRequest::Failed(const std::string& error)
{
}

FinishReplyServiceMessage::FinishReplyServiceMessage(const FinishReply& finishReply_) : ServiceMessage(ServiceMessageKind::finishReply), finishReply(finishReply_)
{
}

RunUntilDebugServiceRequest::RunUntilDebugServiceRequest(const SourceLoc& sourceLocation_) : sourceLocation(sourceLocation_)
{
}

void RunUntilDebugServiceRequest::Execute()
{
    DebugService::Instance().RunUntilRequest(sourceLocation);
}

std::string RunUntilDebugServiceRequest::Name() const
{
    return "runUntilDebugServiceRequest";
}

void RunUntilDebugServiceRequest::Failed(const std::string& error)
{
}

UntilReplyServiceMessage::UntilReplyServiceMessage(const UntilReply& untilReply_) : ServiceMessage(ServiceMessageKind::untilReply), untilReply(untilReply_)
{
}

RunBreakDebugServiceRequest::RunBreakDebugServiceRequest(Breakpoint* breakpoint_) : breakpoint(breakpoint_)
{
}

void RunBreakDebugServiceRequest::Execute()
{
    DebugService::Instance().RunBreakRequest(breakpoint);
}

std::string RunBreakDebugServiceRequest::Name() const
{
    return "runBreakDebugServiceRequest";
}

void RunBreakDebugServiceRequest::Failed(const std::string& error)
{
}

BreakReplyServiceMessage::BreakReplyServiceMessage(const BreakReply& breakReply_) : ServiceMessage(ServiceMessageKind::breakReply), breakReply(breakReply_)
{
}

RunDeleteDebugServiceRequest::RunDeleteDebugServiceRequest(const std::string& breakpointId_) : breakpointId(breakpointId_)
{
}

void RunDeleteDebugServiceRequest::Execute()
{
    DebugService::Instance().RunDeleteRequest(breakpointId);
}

std::string RunDeleteDebugServiceRequest::Name() const
{
    return "runDeleteDebugServiceRequest";
}

void RunDeleteDebugServiceRequest::Failed(const std::string& error)
{
}

DeleteReplyServiceMessage::DeleteReplyServiceMessage(const DeleteReply& deleteReply_) : ServiceMessage(ServiceMessageKind::deleteReply), deleteReply(deleteReply_)
{
}

RunDepthDebugServiceRequest::RunDepthDebugServiceRequest()
{
}

void RunDepthDebugServiceRequest::Execute()
{
    DebugService::Instance().RunDepthRequest();
}

std::string RunDepthDebugServiceRequest::Name() const
{
    return "runDepthDebugServiceRequest";
}

void RunDepthDebugServiceRequest::Failed(const std::string& error)
{
}

DepthReplyServiceMessage::DepthReplyServiceMessage(const DepthReply& depthReply_) : ServiceMessage(ServiceMessageKind::depthReply), depthReply(depthReply_)
{
}

RunFramesDebugServiceRequest::RunFramesDebugServiceRequest(int lowFrame_, int highFrame_) : lowFrame(lowFrame_), highFrame(highFrame_)
{
}

void RunFramesDebugServiceRequest::Execute()
{
    DebugService::Instance().RunFramesRequest(lowFrame, highFrame);
}

std::string RunFramesDebugServiceRequest::Name() const
{
    return "runFrameDebugServiceRequest";
}

void RunFramesDebugServiceRequest::Failed(const std::string& error)
{
}

FramesReplyServiceMessage::FramesReplyServiceMessage(const FramesReply& framesReply_) : ServiceMessage(ServiceMessageKind::framesReply), framesReply(framesReply_)
{
}

RunEvaluateDebugServiceRequest::RunEvaluateDebugServiceRequest(const std::string& expression_, int requestId_) : expression(expression_), requestId(requestId_)
{
}

void RunEvaluateDebugServiceRequest::Execute()
{
    DebugService::Instance().RunEvaluateRequest(expression, requestId);
}

std::string RunEvaluateDebugServiceRequest::Name() const
{
    return "runEvaluateDebugServiceRequest";
}

void RunEvaluateDebugServiceRequest::Failed(const std::string& error)
{
}

EvaluateReplyServiceMessage::EvaluateReplyServiceMessage(const EvaluateReply& evaluateReply_, int requestId_) : 
    ServiceMessage(ServiceMessageKind::evaluateReply), evaluateReply(evaluateReply_), requestId(requestId_)
{
}

RunCountDebugServiceRequest::RunCountDebugServiceRequest(const std::string& expression_) : expression(expression_)
{
}

void RunCountDebugServiceRequest::Execute()
{
    DebugService::Instance().RunCountRequest(expression);
}

std::string RunCountDebugServiceRequest::Name() const
{
    return "runCountDebugServiceRequest";
}

void RunCountDebugServiceRequest::Failed(const std::string& error) 
{
}

CountReplyServiceMessage::CountReplyServiceMessage(const CountReply& countReply_) : ServiceMessage(ServiceMessageKind::countReply), countReply(countReply_)
{
}

RunEvaluateChildRequest::RunEvaluateChildRequest(const std::string& expression_, int start_, int count_) : expression(expression_), start(start_), count(count_)
{
}

void RunEvaluateChildRequest::Execute()
{
    DebugService::Instance().RunEvaluateChildRequest(expression, start, count);
}

std::string RunEvaluateChildRequest::Name() const
{
    return "runEvaluateChildRequest";
}

void RunEvaluateChildRequest::Failed(const std::string& error)
{
}

EvaluateChildReplyServiceMessage::EvaluateChildReplyServiceMessage(const EvaluateChildReply& evaluateChildReply_) : 
    ServiceMessage(ServiceMessageKind::evaluateChildReply), evaluateChildReply(evaluateChildReply_)
{
}

void InitDebugService()
{
    DebugService::Init();
}

void DoneDebugService()
{
    DebugService::Done();
}

void StartDebugService(DebugServiceStartParams& startParams, const std::vector<Breakpoint*>& breakpoints)
{
    DebugService::Instance().Start(startParams, breakpoints);
    DebugService::Instance().PutRequest(new RunStartDebugServiceRequest());
}

void StopDebugService()
{
    DebugService::Instance().Stop();
}

void Continue()
{
    DebugService::Instance().PutRequest(new RunContinueDebugServiceRequest());
}

void Next()
{
    DebugService::Instance().PutRequest(new RunNextDebugServiceRequest());
}

void Step()
{
    DebugService::Instance().PutRequest(new RunStepDebugServiceRequest());
}

void Finish()
{
    DebugService::Instance().PutRequest(new RunFinishDebugServiceRequest());
}

void Until(const SourceLoc& sourceLocation)
{
    DebugService::Instance().PutRequest(new RunUntilDebugServiceRequest(sourceLocation));
}

void Break(Breakpoint* breakpoint)
{
    DebugService::Instance().PutRequest(new RunBreakDebugServiceRequest(breakpoint));
}

void Delete(const std::string& breakpointId)
{
    DebugService::Instance().PutRequest(new RunDeleteDebugServiceRequest(breakpointId));
}

void Depth()
{
    DebugService::Instance().PutRequest(new RunDepthDebugServiceRequest());
}

void Frames(int lowFrame, int highFrame)
{
    DebugService::Instance().PutRequest(new RunFramesDebugServiceRequest(lowFrame, highFrame));
}

void Evaluate(const std::string& expression, int requestId)
{
    DebugService::Instance().PutRequest(new RunEvaluateDebugServiceRequest(expression, requestId));
}

void Count(const std::string& expression)
{
    DebugService::Instance().PutRequest(new RunCountDebugServiceRequest(expression));
}

void EvaluateChild(const std::string& expression, int start, int count)
{
    DebugService::Instance().PutRequest(new RunEvaluateChildRequest(expression, start, count));
}

void SetTargetInputEof()
{
    DebugService::Instance().SetTargetInputEof();
}

void PutTargetInputLine(const std::string& targetInputLine)
{
    DebugService::Instance().PutTargetInputLine(targetInputLine);
}

bool DebugRequestInProgress(std::string& requestName)
{
    return DebugService::Instance().RequestInProgress(requestName);
}

} } // namespace cmajor::service
