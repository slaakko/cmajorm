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
    void WaitForStoppedOrKill();
    void RunContinueRequest();
    void RunStepRequest();
    void RunNextRequest();
    void RunFinishRequest();
    void RunUntilRequest(const SourceLoc& sourceLocation);
    void RunBreakRequest(Breakpoint* breakpoint);
    void RunDeleteRequest(const std::string& breakpointId);
    void SetRequestInProgress(const std::string& requestName);
    void ResetRequestInProgress();
    bool RequestInProgress(std::string& requestName);
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
    std::condition_variable_any requestAvailableOrStopping;
    std::condition_variable targetInputLineAvailableOrEof;
    std::condition_variable_any stopped;
    std::recursive_mutex mtx;
    std::mutex inputMutex;
    std::list<std::unique_ptr<DebugServiceRequest>> requestQueue;
    std::list<std::string> targetInputLines;
    bool waitingForTargetInput;
    bool targetInputEof;
    int serverPort;
    TcpSocket socket;
    bool started;
    bool running;
    bool stop;
    bool requestInProgress;
    std::string runningRequestName;
    std::map<std::u32string, DebugMessageKind> messageKindMap;
};

std::unique_ptr<DebugService> DebugService::instance;

DebugService::DebugService() : serverPort(0), waitingForTargetInput(false), targetInputEof(false), started(false), running(false), stop(false), requestInProgress(false)
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
    std::lock_guard<std::recursive_mutex> lock(mtx);
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
                std::unique_lock<std::recursive_mutex> lock(mtx);
                requestAvailableOrStopping.wait(lock, [this]{ return stop || !requestQueue.empty(); });
                if (stop) return;
                std::unique_ptr<DebugServiceRequest> request = std::move(requestQueue.front());
                requestQueue.pop_front();
                ExecuteRequest(request.get());
            }
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
            DebugService::PutRequest(new RunStopDebugServiceRequest());
            DebugService::WaitForStoppedOrKill();
            stop = true;
            requestAvailableOrStopping.notify_one();
        }
        if (started)
        {
            started = false;
            serviceThread.join();
        }
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
    stopped.notify_one();
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

void DebugService::WaitForStoppedOrKill()
{
    std::unique_lock<std::recursive_mutex> lock(mtx);
    std::cv_status status = stopped.wait_for(lock, std::chrono::seconds(3));
    if (status == std::cv_status::timeout)
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
