// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/ServerDebugger.hpp>
#include <cmajor/cmdebug/ClientChannel.hpp>
#include <cmajor/cmdebug/CmdbMessage.hpp>
#include <cmajor/cmdebug/CmdbMessageMap.hpp>
#include <cmajor/cmdebug/Debugger.hpp>
#include <sngjson/json/JsonLexer.hpp>
#include <sngjson/json/JsonParser.hpp>
#include <sngcm/ast/Project.hpp>
#include <soulng/util/LogFileWriter.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Socket.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <condition_variable>
#include <iostream>
#include <thread>

namespace cmajor { namespace debug {

using namespace soulng::util;
using namespace soulng::unicode;
using namespace sngcm::ast;

std::string CmdbLogFilePath()
{
    return Path::Combine(CmajorLogFileDir(), "cmdb.log");
}

Location ToLocation(const Frame& frame, bool includeLevel)
{
    Location loc;
    loc.func = frame.func;
    loc.file = frame.file;
    loc.line = std::to_string(frame.line);
    loc.scol = std::to_string(frame.scol);
    loc.ecol = std::to_string(frame.ecol);
    if (includeLevel)
    {
        loc.level = std::to_string(frame.level);
    }
    return loc;
}

Location ToLocation(const Frame& frame)
{
    return ToLocation(frame, false);
}

SourceLocation ToSourceLocation(const SourceLoc& breakpointLocation)
{
    SourceLocation sourceLocation(breakpointLocation.path, boost::lexical_cast<int>(breakpointLocation.line));
    return sourceLocation;
}

class ServerDebugger;

class ServerDebuggerOutputWriter : public DebuggerOutputWriter
{
public:
    ServerDebuggerOutputWriter(ServerDebugger* debugger_, bool log_);
    void WriteLogMessage(const std::string& message) override;
    void WriteJsonValue(soulng::util::JsonValue* jsonValue) override;
    void WriteWarning(const std::string& warning) override;
private:
    ServerDebugger* serverDebugger;
    bool log;
    std::string logFilePath;
};

ServerDebuggerOutputWriter::ServerDebuggerOutputWriter(ServerDebugger* serverDebugger_, bool log_) :
    serverDebugger(serverDebugger_), log(log_), logFilePath(CmdbLogFilePath())
{
}

void ServerDebuggerOutputWriter::WriteJsonValue(soulng::util::JsonValue* jsonValue)
{
    if (log)
    {
        LogFileWriter writer(logFilePath);
        writer.WriteLine("================================================================================");
        writer.WriteCurrentDateTime();
        CodeFormatter formatter(writer.LogFile());
        jsonValue->Write(formatter);
    }
}

void ServerDebuggerOutputWriter::WriteWarning(const std::string& warning)
{
    if (log)
    {
        LogFileWriter writer(logFilePath);
        writer.WriteLine("================================================================================");
        writer.WriteCurrentDateTime();
        writer.WriteLine(warning);
    }
}

class ServerDebugger : public Debugger, public ClientChannelUser
{
public:
    ServerDebugger(const std::string& executable, const std::vector<std::string>& args, bool verbose_, bool breakOnThrow_, const std::string& version_, int port_, bool log_);
    ~ServerDebugger();
    void StartServer();
    void StopServer();
    void RunServer();
    void LogRequest(JsonValue* request);
    void LogReply(JsonValue* reply);
    std::unique_ptr<JsonValue> ProcessRequest(JsonValue* requestMessage);
    std::unique_ptr<JsonValue> GetIdleClientChannelMessage() override;
    void ClientChannelError(const std::string& error) override;
    void ProcessReceivedClientChannelMessage(JsonValue* message) override;
    void ProcessTargetRunningReply(JsonValue* message);
    void ProcessTargetInputReply(JsonValue* message);
    void ProcessTargetOutputReply(JsonValue* message);
    std::unique_ptr<JsonValue> ProcessStartRequest(const StartRequest& startRequest);
    std::vector<BreakpointInfo> SetBreakpoints(const std::vector<SourceLoc>& breakpointLocations);
    BreakpointInfo SetBreakpoint(const SourceLocation& sourceLocation);
    std::unique_ptr<JsonValue> ProcessStopRequest(const StopRequest& stopRequest);
    std::unique_ptr<JsonValue> ProcessContinueRequest(const ContinueRequest& continueRequest);
    std::unique_ptr<JsonValue> ProcessNextRequest(const NextRequest& stopRequest);
    std::unique_ptr<JsonValue> ProcessStepRequest(const StepRequest& stepRequest);
    std::unique_ptr<JsonValue> ProcessFinishRequest(const FinishRequest& finishRequst);
    std::unique_ptr<JsonValue> ProcessUntilRequest(const UntilRequest& untilRequest);
    std::unique_ptr<JsonValue> ProcessBreakRequest(const BreakRequest& breakRequest);
    std::unique_ptr<JsonValue> ProcessDeleteRequest(const DeleteRequest& deleteRequest);
    std::unique_ptr<JsonValue> ProcessDepthRequest(const DepthRequest& depthRequest);
    std::unique_ptr<JsonValue> ProcessFramesRequest(const FramesRequest& framesRequest);
    std::unique_ptr<JsonValue> ProcessLocalCountRequest(const LocalCountRequest& localCountRequest);
    std::unique_ptr<JsonValue> ProcessNameRequest(const NameRequest& nameRequest);
    std::unique_ptr<JsonValue> ProcessEvaluateRequest(const EvaluateRequest& evaluateRequest);
    std::string GetValue(DIVariable* variable);
    std::string GetEnumeratedTypeValue(uint64_t value, DIEnumType* enumType);
    EvaluateReply DoEvaluate(const std::string& expression);
    void GetLocationResult(bool& success, std::string& error, Location& loc, TargetState& targetState);
    MessageKind GetMessageKind(JsonValue* message, std::string& messageKindStr);
    void AddStopResultToResult() override; 
    void Proceed() override;
    void ResetConsole() override;
    void Prompt() override;
    void TargetInputPrompt() override;
    void Error(const std::string& msg) override;
    void WriteResult(soulng::util::JsonValue* result, Instruction* stoppedInstruction) override;
    void WriteSourceFile(SourceFile& sourceFile, int line, Instruction* stoppedInstruction) override;
    void SetConsoleActive() override;
    void SetTargetRunning() override;
    void ProcessConsoleOutput(GdbConsoleOutputRecord* record) override;
    void ProcessTargetOutput(GdbTargetOutputRecord* record) override;
    void ProcessLogOutput(GdbLogOutputRecord* record) override;
    std::string GetTargetInputBytes() override;
    void WriteTargetOuput(int handle, const std::string& s) override;
    void SetClientChannel(ClientChannel* channel);
    void WriteLogMessage(const std::string& logMessage);
private:
    MessageMap messageMap;
    std::thread mainThread;
    std::string version;
    int port;
    bool log;
    bool exiting;
    bool listening;
    TcpSocket listenSocket;
    TcpSocket socket;
    std::string logFilePath;
    Location location;
    TargetState state;
    ClientChannel* clientChannel;
    std::mutex targetIOMutex;
    TargetRunningReply targetRunningReply;
    bool targetInputReplyReceived;
    std::condition_variable targetInputReplyReceivedVar;
    TargetInputReply targetInputReply;
    bool targetOutputReplyReceived;
    std::condition_variable targetOutputReplyReceivedVar;
    TargetOutputReply targetOutputReply;
    int targetRunningIntervalMs;
};

void ServerDebuggerOutputWriter::WriteLogMessage(const std::string& message)
{
    if (log)
    {
        LogFileWriter writer(logFilePath);
        writer.WriteLine("================================================================================");
        writer.WriteCurrentDateTime();
        writer << message << std::endl;
    }
    serverDebugger->WriteLogMessage(message);
}

struct ClientChannelGuard
{
    ClientChannelGuard(ServerDebugger* debugger_, ClientChannel* channel) : debugger(debugger_)
    {
        debugger->SetClientChannel(channel);
    }
    ~ClientChannelGuard()
    {
        debugger->SetClientChannel(nullptr);
    }
    ServerDebugger* debugger;
};

ServerDebugger::ServerDebugger(const std::string& executable, const std::vector<std::string>& args, bool verbose_, bool breakOnThrow_, const std::string& version_,
    int port_, bool log_) :
    Debugger(verbose_, breakOnThrow_, new ServerDebuggerOutputWriter(this, log), executable, args), version(version_), port(port_), log(log_), exiting(false),
    listening(false), logFilePath(CmdbLogFilePath()), clientChannel(nullptr), targetInputReplyReceived(false), targetOutputReplyReceived(false), targetRunningIntervalMs(250)
{
}

void ServerDebugger::SetClientChannel(ClientChannel* channel)
{
    clientChannel = channel;
}

void ServerDebugger::WriteLogMessage(const std::string& logMessage)
{
    if (clientChannel)
    {
        LogMessageRequest logMessageRequest;
        logMessageRequest.messageKind = "logMessageRequest";
        logMessageRequest.logMessage = logMessage;
        std::unique_ptr<JsonValue> request = logMessageRequest.ToJson();
        clientChannel->SendMessage(request.release());
    }
}

ServerDebugger::~ServerDebugger()
{
    try
    {
        StopGDB(*this);
    }
    catch (...)
    {
    }
    try
    {
        StopCmdbSession();
    }
    catch (...)
    {
    }
}

void ServerDebugger::RunServer()
{
    try
    {
        listening = true;
        listenSocket.Bind(port);
        listenSocket.Listen(10);
        std::cout << "debug-server-ready" << std::endl;
        std::cout << "Cmajor debugger version " + version << std::endl;
        LogFileWriter writer(logFilePath, log);
        if (log)
        {
            writer.WriteLine("================================================================================");
            writer.WriteCurrentDateTime();
            writer << "debug server run: port=" << port << std::endl;
        }
        socket = listenSocket.Accept();
        listening = false;
        while (!exiting)
        {
            std::string request = ReadStr(socket);
            std::u32string content = ToUtf32(request);
            if (content.empty()) return;
            JsonLexer lexer(content, "", 0);
            std::unique_ptr<JsonValue> requestJsonValue(JsonParser::Parse(lexer));
            LogRequest(requestJsonValue.get());
            std::unique_ptr<JsonValue> replyJsonValue = ProcessRequest(requestJsonValue.get());
            LogReply(replyJsonValue.get());
            std::string reply = replyJsonValue->ToString();
            Write(socket, reply);
        }
    }
    catch (const std::exception& ex)
    {
        std::cout << "debug-server-error" << std::endl;
        std::cout << ex.what() << std::endl;
        std::cerr << "exception from ServerDebugger::RunServer: " << ex.what() << std::endl;
        throw;
    }
}

void ServerDebugger::LogRequest(JsonValue* request)
{
    if (log)
    {
        LogFileWriter writer(logFilePath);
        writer.WriteLine("================================================================================");
        writer.WriteCurrentDateTime();
        writer << "request:" << std::endl;
        CodeFormatter formatter(writer.LogFile());
        request->Write(formatter);
    }
}

void ServerDebugger::LogReply(JsonValue* reply)
{
    if (log)
    {
        LogFileWriter writer(logFilePath);
        writer.WriteLine("================================================================================");
        writer.WriteCurrentDateTime();
        writer << "reply:" << std::endl;
        CodeFormatter formatter(writer.LogFile());
        reply->Write(formatter);
    }
}

std::unique_ptr<JsonValue> ServerDebugger::ProcessRequest(JsonValue* requestMessage)
{
    ClientChannel channel(this, socket, targetRunningIntervalMs);
    ClientChannelGuard channelGuard(this, &channel);
    std::string messageKindStr;
    MessageKind messageKind = GetMessageKind(requestMessage, messageKindStr);
    switch (messageKind)
    {
        case MessageKind::startRequest:
        {
            StartRequest startRequest(requestMessage);
            return ProcessStartRequest(startRequest);
        }
        case MessageKind::stopRequest:
        {
            StopRequest stopRequest(requestMessage);
            return ProcessStopRequest(stopRequest);
        }
        case MessageKind::continueRequest:
        {
            ContinueRequest continueRequest(requestMessage);
            return ProcessContinueRequest(continueRequest);
        }
        case MessageKind::nextRequest:
        {
            NextRequest nextRequest(requestMessage);
            return ProcessNextRequest(nextRequest);
        }
        case MessageKind::stepRequest:
        {
            StepRequest stepRequest(requestMessage);
            return ProcessStepRequest(stepRequest);
        }
        case MessageKind::finishRequest:
        {
            FinishRequest finishRequest(requestMessage);
            return ProcessFinishRequest(finishRequest);
        }
        case MessageKind::untilRequest:
        {
            UntilRequest untilRequest(requestMessage);
            return ProcessUntilRequest(untilRequest);
        }
        case MessageKind::breakRequest:
        {
            BreakRequest breakRequest(requestMessage);
            return ProcessBreakRequest(breakRequest);
        }
        case MessageKind::deleteRequest:
        {
            DeleteRequest deleteRequest(requestMessage);
            return ProcessDeleteRequest(deleteRequest);
        }
        case MessageKind::depthRequest:
        {
            DepthRequest depthRequest(requestMessage);
            return ProcessDepthRequest(depthRequest);
        }
        case MessageKind::framesRequest:
        {
            FramesRequest framesRequest(requestMessage);
            return ProcessFramesRequest(framesRequest);
        }
        case MessageKind::localCountRequest:
        {
            LocalCountRequest localCountRequest(requestMessage);
            return ProcessLocalCountRequest(localCountRequest);
        }
        case MessageKind::nameRequest:
        {
            NameRequest nameRequest(requestMessage);
            return ProcessNameRequest(nameRequest);
        }
        case MessageKind::evaluateRequest:
        {
            EvaluateRequest evaluateRequest(requestMessage);
            return ProcessEvaluateRequest(evaluateRequest);
        }
        default:
        {
            GenericErrorReply genericErrorReply;
            genericErrorReply.messageKind = "genericErrorReply";
            if (messageKindStr.empty())
            {
                genericErrorReply.errorMessage = "request has no 'messageKind' field";
            }
            else
            {
                genericErrorReply.errorMessage = "unknown request 'messageKind' value: '" + messageKindStr + "'";
            }
            return genericErrorReply.ToJson();
        }
    }
}

std::unique_ptr<JsonValue> ServerDebugger::GetIdleClientChannelMessage()
{
    TargetRunningRequest targetRunningRequest;
    targetRunningRequest.messageKind = "targetRunningRequest";
    return targetRunningRequest.ToJson();
}

void ServerDebugger::ClientChannelError(const std::string& error)
{
    std::cerr << error << std::endl;
}

void ServerDebugger::ProcessReceivedClientChannelMessage(JsonValue* message)
{
    std::string messageKindStr;
    MessageKind messageKind = GetMessageKind(message, messageKindStr);
    switch (messageKind)
    {
        case MessageKind::targetRunningReply:
        {
            ProcessTargetRunningReply(message);
            break;
        }
        case MessageKind::targetInputReply:
        {
            ProcessTargetInputReply(message);
            break;
        }
        case MessageKind::targetOutputReply:
        {
            ProcessTargetOutputReply(message);
            break;
        }
    }
}

void ServerDebugger::ProcessTargetRunningReply(JsonValue* message)
{
    std::lock_guard<std::mutex> lock(targetIOMutex);
    targetRunningReply = TargetRunningReply(message);
}

void ServerDebugger::ProcessTargetInputReply(JsonValue* message)
{
    std::lock_guard<std::mutex> lock(targetIOMutex);
    targetInputReply = TargetInputReply(message);
    targetInputReplyReceived = true;
    targetInputReplyReceivedVar.notify_one();
}

void ServerDebugger::ProcessTargetOutputReply(JsonValue* message)
{
    std::lock_guard<std::mutex> lock(targetIOMutex);
    targetOutputReply = TargetOutputReply(message);
    targetOutputReplyReceived = true;
    targetOutputReplyReceivedVar.notify_one();
}

std::unique_ptr<JsonValue> ServerDebugger::ProcessStartRequest(const StartRequest& startRequest)
{
    StartReply startReply;
    startReply.messageKind = "startReply";
    try
    {
        StartDebugging();
        startReply.location = location;
        startReply.state = state;
        OutputWriter()->WriteLogMessage("setting breakpoints...");
        std::vector<BreakpointInfo> breakpointInfos = SetBreakpoints(startRequest.breakpointLocations);
        startReply.breakpointInfos = breakpointInfos;
        startReply.success = true;
        OutputWriter()->WriteLogMessage("breakpoints set");
    }
    catch (const std::exception& ex)
    {
        startReply.success = false;
        startReply.error = ex.what();
    }
    return startReply.ToJson();
}

std::vector<BreakpointInfo> ServerDebugger::SetBreakpoints(const std::vector<SourceLoc>& breakpointLocations)
{
    std::vector<BreakpointInfo> breakpointInfos;
    int n = breakpointLocations.size();
    for (int i = 0; i < n; ++i)
    {
        SourceLocation sourceLocation = ToSourceLocation(breakpointLocations[i]);
        breakpointInfos.push_back(SetBreakpoint(sourceLocation));
    }
    return breakpointInfos;
}

BreakpointInfo ServerDebugger::SetBreakpoint(const SourceLocation& location)
{
    BreakpointInfo info;
    try
    {
        Break(location);
        JsonValue* resultValue = GetResult();
        if (resultValue && resultValue->Type() == JsonValueType::object)
        {
            JsonObject* resultObject = static_cast<JsonObject*>(resultValue);
            JsonValue* successField = resultObject->GetField(U"success");
            if (successField && successField->Type() == JsonValueType::boolean)
            {
                info.success = static_cast<JsonBool*>(successField)->Value();
            }
            JsonValue* breakpointField = resultObject->GetField(U"breakpoint");
            if (breakpointField && breakpointField->Type() == JsonValueType::object)
            {
                JsonObject* breakpointObject = static_cast<JsonObject*>(breakpointField);
                JsonValue* idField = breakpointObject->GetField(U"id");
                if (idField && idField->Type() == JsonValueType::string)
                {
                    info.breakpointId = ToUtf8(static_cast<JsonString*>(idField)->Value());
                }
                JsonValue* numInstsField = breakpointObject->GetField(U"numInsts");
                if (numInstsField && numInstsField->Type() == JsonValueType::string)
                {
                    info.numInsts = ToUtf8(static_cast<JsonString*>(numInstsField)->Value());
                }
                JsonValue* locationField = breakpointObject->GetField(U"location");
                if (locationField && locationField->Type() == JsonValueType::object)
                {
                    JsonObject* locationObject = static_cast<JsonObject*>(locationField);
                    JsonValue* funcField = locationObject->GetField(U"func");
                    if (funcField && funcField->Type() == JsonValueType::string)
                    {
                        info.location.func = ToUtf8(static_cast<JsonString*>(funcField)->Value());
                    }
                    JsonValue* fileField = locationObject->GetField(U"file");
                    if (fileField && fileField->Type() == JsonValueType::string)
                    {
                        info.location.file = ToUtf8(static_cast<JsonString*>(fileField)->Value());
                    }
                    JsonValue* lineField = locationObject->GetField(U"line");
                    if (lineField && lineField->Type() == JsonValueType::string)
                    {
                        info.location.line = ToUtf8(static_cast<JsonString*>(lineField)->Value());
                    }
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        info.success = false;
        info.error = ex.what();
    }
    return info;
}

std::unique_ptr<JsonValue> ServerDebugger::ProcessStopRequest(const StopRequest& stopRequest)
{
    StopReply stopReply;
    stopReply.messageKind = "stopReply";
    exiting = true;
    return stopReply.ToJson();
}

std::unique_ptr<JsonValue> ServerDebugger::ProcessContinueRequest(const ContinueRequest& continueRequest)
{
    ContinueReply continueReply;
    continueReply.messageKind = "continueReply";
    try
    {
        Continue();
        GetLocationResult(continueReply.success, continueReply.error, continueReply.location, continueReply.state);
    }
    catch (const std::exception& ex)
    {
        continueReply.success = false;
        continueReply.error = ex.what();
    }
    return continueReply.ToJson();
}

std::unique_ptr<JsonValue> ServerDebugger::ProcessNextRequest(const NextRequest& stopRequest)
{
    NextReply nextReply;
    nextReply.messageKind = "nextReply";
    try
    {
        Next();
        GetLocationResult(nextReply.success, nextReply.error, nextReply.location, nextReply.state);
    }
    catch (const std::exception& ex)
    {
        nextReply.success = false;
        nextReply.error = ex.what();
    }
    return nextReply.ToJson();
}

std::unique_ptr<JsonValue> ServerDebugger::ProcessStepRequest(const StepRequest& stepRequest)
{
    StepReply stepReply;
    stepReply.messageKind = "stepReply";
    try
    {
        Step();
        GetLocationResult(stepReply.success, stepReply.error, stepReply.location, stepReply.state);
    }
    catch (const std::exception& ex)
    {
        stepReply.success = false;
        stepReply.error = ex.what();
    }
    return stepReply.ToJson();
}

std::unique_ptr<JsonValue> ServerDebugger::ProcessFinishRequest(const FinishRequest& finishRequest)
{
    FinishReply finishReply;
    finishReply.messageKind = "finishReply";
    try
    {
        Finish();
        GetLocationResult(finishReply.success, finishReply.error, finishReply.location, finishReply.state);
    }
    catch (const std::exception& ex)
    {
        finishReply.success = false;
        finishReply.error = ex.what();
    }
    return finishReply.ToJson();
}

std::unique_ptr<JsonValue> ServerDebugger::ProcessUntilRequest(const UntilRequest& untilRequest)
{
    UntilReply untilReply;
    untilReply.messageKind = "untilReply";
    try
    {
        SourceLocation sourceLocation = ToSourceLocation(untilRequest.sourceLoc);
        Until(sourceLocation);
        GetLocationResult(untilReply.success, untilReply.error, untilReply.location, untilReply.state);
    }
    catch (const std::exception& ex)
    {
        untilReply.success = false;
        untilReply.error = ex.what();
    }
    return untilReply.ToJson();
}

std::unique_ptr<JsonValue> ServerDebugger::ProcessBreakRequest(const BreakRequest& breakRequest)
{
    BreakReply breakReply;
    breakReply.messageKind = "breakReply";
    try
    {
        SourceLocation location = ToSourceLocation(breakRequest.breakpointLocation);
        BreakpointInfo breakpointInfo = SetBreakpoint(location);
        breakReply.breakpointInfo = breakpointInfo;
    }
    catch (const std::exception& ex)
    {
        BreakpointInfo breakpointInfo;
        breakpointInfo.success = false;
        breakpointInfo.error = ex.what();
        breakReply.breakpointInfo = breakpointInfo;
    }
    return breakReply.ToJson();
}

std::unique_ptr<JsonValue> ServerDebugger::ProcessDeleteRequest(const DeleteRequest& deleteRequest)
{
    DeleteReply deleteReply;
    deleteReply.messageKind = "deleteReply";
    try
    {
        Delete(deleteRequest.breakpointId);
        JsonValue* result = GetResult();
        if (result && result->Type() == JsonValueType::object)
        {
            JsonObject* resultObject = static_cast<JsonObject*>(result);
            JsonValue* successField = resultObject->GetField(U"success");
            if (successField && successField->Type() == JsonValueType::boolean)
            {
                deleteReply.success = static_cast<JsonBool*>(successField)->Value();
            }
            JsonValue* errorField = resultObject->GetField(U"error");
            if (errorField && errorField->Type() == JsonValueType::string)
            {
                deleteReply.error = ToUtf8(static_cast<JsonString*>(errorField)->Value());
            }
        }
    }
    catch (const std::exception& ex)
    {
        deleteReply.success = false;
        deleteReply.error = ex.what();
    }
    return deleteReply.ToJson();
}

std::unique_ptr<JsonValue> ServerDebugger::ProcessDepthRequest(const DepthRequest& depthRequest)
{
    DepthReply depthReply;
    depthReply.messageKind = "depthReply";
    try
    {
        Depth();
        JsonValue* result = GetResult();
        if (result && result->Type() == JsonValueType::object)
        {
            JsonObject* resultObject = static_cast<JsonObject*>(result);
            JsonValue* successField = resultObject->GetField(U"success");
            if (successField && successField->Type() == JsonValueType::boolean)
            {
                depthReply.success = static_cast<JsonBool*>(successField)->Value();
            }
            JsonValue* depthField = resultObject->GetField(U"depth");
            if (depthField && depthField->Type() == JsonValueType::string)
            {
                depthReply.depth = ToUtf8(static_cast<JsonString*>(depthField)->Value());
            }
            JsonValue* errorField = resultObject->GetField(U"error");
            if (errorField && errorField->Type() == JsonValueType::string)
            {
                depthReply.error = ToUtf8(static_cast<JsonString*>(errorField)->Value());
            }
        }
    }
    catch (const std::exception& ex)
    {
        depthReply.success = false;
        depthReply.error = ex.what();
    }
    return depthReply.ToJson();
}

std::unique_ptr<JsonValue> ServerDebugger::ProcessFramesRequest(const FramesRequest& framesRequest)
{
    FramesReply framesReply;
    framesReply.messageKind = "framesReply";
    try
    {
        Frames(boost::lexical_cast<int>(framesRequest.lowFrame), boost::lexical_cast<int>(framesRequest.highFrame));
        JsonValue* result = GetResult();
        if (result && result->Type() == JsonValueType::object)
        {
            JsonObject* resultObject = static_cast<JsonObject*>(result);
            JsonValue* successField = resultObject->GetField(U"success");
            if (successField && successField->Type() == JsonValueType::boolean)
            {
                framesReply.success = static_cast<JsonBool*>(successField)->Value();
            }
            JsonValue* errorField = resultObject->GetField(U"error");
            if (errorField && errorField->Type() == JsonValueType::string)
            {
                framesReply.error = ToUtf8(static_cast<JsonString*>(errorField)->Value());
            }
        }
        if (framesReply.success)
        {
            const std::vector<Frame>& frames = Frames();
            int n = frames.size();
            for (int i = 0; i < n; ++i)
            {
                const Frame& frame = frames[i];
                Location loc = ToLocation(frame);
                framesReply.frames.push_back(std::move(loc));
            }
        }
    }
    catch (const std::exception& ex)
    {
        framesReply.success = false;
        framesReply.error = ex.what();
    }
    return framesReply.ToJson();
}

std::unique_ptr<JsonValue> ServerDebugger::ProcessLocalCountRequest(const LocalCountRequest& localCountRequest)
{
    LocalCountReply localCountReply;
    localCountReply.messageKind = "localCountReply";
    Instruction* stoppedInstruction = StoppedInstruction();
    if (stoppedInstruction)
    {
        CompileUnitFunction* function = stoppedInstruction->GetCompileUnitFunction();
        int localVariableCount = function->LocalVariables().size();
        localCountReply.success = true;
        localCountReply.count = std::to_string(localVariableCount);
    }
    else
    {
        localCountReply.success = false;
        localCountReply.error = "not stopped";
    }
    return localCountReply.ToJson();
}

std::unique_ptr<JsonValue> ServerDebugger::ProcessNameRequest(const NameRequest& nameRequest)
{
    NameReply nameReply;
    nameReply.messageKind = "nameReply";
    try
    {
        int start = boost::lexical_cast<int>(nameRequest.start);
        int count = boost::lexical_cast<int>(nameRequest.count);
        Instruction* stoppedInstruction = StoppedInstruction();
        if (stoppedInstruction)
        {
            CompileUnitFunction* function = stoppedInstruction->GetCompileUnitFunction();
            int localVariableCount = function->LocalVariables().size();
            if (start >= 0 && start < localVariableCount)
            {
                if (start + count >= 0 && start + count <= localVariableCount)
                {
                    nameReply.success = true;
                    for (int i = start; i < start + count; ++i)
                    {
                        DIVariable* localVariable = function->LocalVariables()[i];
                        nameReply.names.push_back(localVariable->Name());
                        nameReply.values.push_back(GetValue(localVariable));
                    }
                }
                else
                {
                    nameReply.success = false;
                    nameReply.error = "count not valid";
                }
            }
            else
            {
                nameReply.success = false;
                nameReply.error = "start not valid";
            }
        }
        else
        {
            nameReply.success = false;
            nameReply.error = "not stopped";
        }
    }
    catch (const std::exception& ex)
    {
        nameReply.success = false;
        nameReply.error = ex.what();
    }
    return nameReply.ToJson();
}

std::string ServerDebugger::GetValue(DIVariable* variable)
{
    EvaluateReply evaluateReply = DoEvaluate(variable->Name());
    if (evaluateReply.success)
    {
        DIType* type = variable->GetType();
        switch (type->GetKind())
        {
            case DIType::Kind::primitiveType:
            {
                return evaluateReply.result.value;
            }
            case DIType::Kind::enumType:
            {
                if (evaluateReply.result.initialized)
                {
                    try
                    {
                        uint64_t value = boost::lexical_cast<uint64_t>(evaluateReply.result.value);
                        return GetEnumeratedTypeValue(value, static_cast<DIEnumType*>(type));
                        
                    }
                    catch (...)
                    {
                        return evaluateReply.result.value;
                    }
                }
                else
                {
                    return evaluateReply.result.value;
                }
            }
        }
    }
    return std::string();
}

std::string ServerDebugger::GetEnumeratedTypeValue(uint64_t value, DIEnumType* enumType)
{
    uint64_t wholeValue = value;
    std::string strValue;
    bool first = true;
    while (true)
    {
        DIEnumConstant* enumConstant = enumType->GetEnumConstant(value);
        if (enumConstant)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                strValue.append(" | ");
            }
            strValue.append(enumType->Name()).append(".").append(enumConstant->Name());
            if (value == 0)
            {
                break;
            }
        }
        else
        {
            return std::to_string(wholeValue);
        }
    }
    return strValue;
}

std::unique_ptr<JsonValue> ServerDebugger::ProcessEvaluateRequest(const EvaluateRequest& evaluateRequest)
{
    EvaluateReply evaluateReply = DoEvaluate(evaluateRequest.expression);
    return evaluateReply.ToJson();
}

EvaluateReply ServerDebugger::DoEvaluate(const std::string& expression)
{
    EvaluateReply evaluateReply;
    evaluateReply.messageKind = "evaluateReply";
    try
    {
        Print(expression);
        JsonValue* result = GetResult();
        if (result && result->Type() == JsonValueType::object)
        {
            JsonObject* resultObject = static_cast<JsonObject*>(result);
            JsonValue* success = resultObject->GetField(U"success");
            if (success && success->Type() == JsonValueType::boolean)
            {
                evaluateReply.success = static_cast<JsonBool*>(success)->Value();
            }
            JsonValue* error = resultObject->GetField(U"error");
            if (error && error->Type() == JsonValueType::string)
            {
                evaluateReply.error = ToUtf8(static_cast<JsonString*>(error)->Value());
            }
            JsonValue* staticType = resultObject->GetField(U"static_type");
            if (staticType && staticType->Type() == JsonValueType::object)
            {
                JsonObject* staticTypeObject = static_cast<JsonObject*>(staticType);
                JsonValue* id = staticTypeObject->GetField(U"id");
                if (id && id->Type() == JsonValueType::string)
                {
                    evaluateReply.result.staticType.id = ToUtf8(static_cast<JsonString*>(id)->Value());
                }
                JsonValue* name = staticTypeObject->GetField(U"name");
                if (name && name->Type() == JsonValueType::string)
                {
                    evaluateReply.result.staticType.name = ToUtf8(static_cast<JsonString*>(name)->Value());
                }
            }
            JsonValue* dynamicType = resultObject->GetField(U"dynamic_type");
            if (dynamicType && dynamicType->Type() == JsonValueType::object)
            {
                JsonObject* dynamicTypeObject = static_cast<JsonObject*>(dynamicType);
                JsonValue* id = dynamicTypeObject->GetField(U"id");
                if (id && id->Type() == JsonValueType::string)
                {
                    evaluateReply.result.dynamicType.id = ToUtf8(static_cast<JsonString*>(id)->Value());
                }
                JsonValue* name = dynamicTypeObject->GetField(U"name");
                if (name && name->Type() == JsonValueType::string)
                {
                    evaluateReply.result.dynamicType.name = ToUtf8(static_cast<JsonString*>(name)->Value());
                }
            }
            JsonValue* status = resultObject->GetField(U"status");
            if (status && status->Type() == JsonValueType::string)
            {
                std::string s = ToUtf8(static_cast<JsonString*>(status)->Value());
                if (s == "initialized")
                {
                    evaluateReply.result.initialized = true;
                    JsonValue* value = resultObject->GetField(U"value");
                    if (value && value->Type() == JsonValueType::string)
                    {
                        evaluateReply.result.value = ToUtf8(static_cast<JsonString*>(value)->Value());
                    }
                }
                else
                {
                    evaluateReply.result.initialized = false;
                    evaluateReply.result.value = "<uninitialized>";
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        evaluateReply.success = false;
        evaluateReply.error = ex.what();
    }
    return evaluateReply;
}

void ServerDebugger::GetLocationResult(bool& success, std::string& error, Location& loc, TargetState& targetState)
{
    JsonValue* result = GetResult();
    if (result && result->Type() == JsonValueType::object)
    {
        JsonObject* resultObject = static_cast<JsonObject*>(result);
        JsonValue* successField = resultObject->GetField(U"success");
        if (successField && successField->Type() == JsonValueType::boolean)
        {
            success = static_cast<JsonBool*>(successField)->Value();
        }
        JsonValue* errorField = resultObject->GetField(U"error");
        if (errorField && errorField->Type() == JsonValueType::string)
        {
            error = ToUtf8(static_cast<JsonString*>(errorField)->Value());
        }
    }
    loc = location;
    targetState = state;
}

MessageKind ServerDebugger::GetMessageKind(JsonValue* message, std::string& messageKindStr)
{
    if (message->Type() == JsonValueType::object)
    {
        JsonObject* messageObject = static_cast<JsonObject*>(message);
        messageKindStr = messageObject->GetStringField(U"messageKind");
        return messageMap.GetMessageKind(messageKindStr);
    }
    return MessageKind::none;
}

void ServerDebugger::AddStopResultToResult()
{
    Debugger::AddStopResultToResult();
    Instruction* stoppedInstruction = StoppedInstruction();
    if (stoppedInstruction)
    {
        Frame frame = stoppedInstruction->GetCmajorFrame();
        location = ToLocation(frame);
    }
    else
    {
        location = Location();
    }
    JsonValue* result = GetResult();
    if (result && result->Type() == JsonValueType::object)
    {
        JsonObject* resultObject = static_cast<JsonObject*>(result);
        JsonValue* stoppedResult = resultObject->GetField(U"stopped");
        if (stoppedResult && stoppedResult->Type() == JsonValueType::object)
        {
            JsonObject* stoppedObject = static_cast<JsonObject*>(stoppedResult);
            JsonValue* reasonValue = stoppedObject->GetField(U"reason");
            if (reasonValue && reasonValue->Type() == JsonValueType::string)
            {
                state.stopReason = ToUtf8(static_cast<JsonString*>(reasonValue)->Value());
            }
            if (state.stopReason == "exited")
            {
                JsonValue* exitCodeValue = stoppedObject->GetField(U"exitCode");
                if (exitCodeValue && exitCodeValue->Type() == JsonValueType::string)
                {
                    state.exitCode = ToUtf8(static_cast<JsonString*>(exitCodeValue)->Value());
                }
            }
            if (state.stopReason == "breakpoint-hit")
            {
                JsonValue* breakpointIdValue = stoppedObject->GetField(U"breakpointId");
                if (breakpointIdValue && breakpointIdValue->Type() == JsonValueType::string)
                {
                    state.breakpointId = ToUtf8(static_cast<JsonString*>(breakpointIdValue)->Value());
                }
            }
            if (state.stopReason == "signal-received")
            {
                JsonValue* signalNameValue = stoppedObject->GetField(U"signalName");
                if (signalNameValue && signalNameValue->Type() == JsonValueType::string)
                {
                    state.signalName = ToUtf8(static_cast<JsonString*>(signalNameValue)->Value());
                }
                JsonValue* signalMeaningValue = stoppedObject->GetField(U"signalMeaning");
                if (signalMeaningValue && signalMeaningValue->Type() == JsonValueType::string)
                {
                    state.signalMeaning = ToUtf8(static_cast<JsonString*>(signalMeaningValue)->Value());
                }
            }
        }
    }
}

void DoRunServer(ServerDebugger* server)
{
    try
    {
        server->RunServer();
    }
    catch (const std::exception& ex)
    {
        std::cout << "debug-server-error" << std::endl;
        std::cout << ex.what() << std::endl;
        std::cerr << "exception from DoRunServer: " << ex.what() << std::endl;
    }
}

void ServerDebugger::StartServer()
{
    try
    {
        mainThread = std::thread{ DoRunServer, this };
    }
    catch (const std::exception& ex)
    {
        std::cout << "debug-server-error" << std::endl;
        std::cout << ex.what() << std::endl;
        std::cerr << "exception from ServerDebugger::StartServer: " << ex.what() << std::endl;
        throw;
    }
}

void ServerDebugger::StopServer()
{
    try
    {
        exiting = true;
        if (listening)
        {
            TcpSocket socket("localhost", std::to_string(port));
        }
        mainThread.join();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "exception from ServerDebugger::StopServer: " << ex.what() << std::endl;
        throw;
    }
}

void ServerDebugger::Proceed()
{
}

void ServerDebugger::ResetConsole()
{
}

void ServerDebugger::Prompt()
{
}

void ServerDebugger::TargetInputPrompt()
{
}

void ServerDebugger::Error(const std::string& msg)
{
}

void ServerDebugger::WriteResult(soulng::util::JsonValue* result, Instruction* stoppedInstruction)
{
}

void ServerDebugger::WriteSourceFile(SourceFile& sourceFile, int line, Instruction* stoppedInstruction)
{
}

void ServerDebugger::SetConsoleActive()
{
}

void ServerDebugger::SetTargetRunning()
{
}

void ServerDebugger::ProcessConsoleOutput(GdbConsoleOutputRecord* record)
{
}

void ServerDebugger::ProcessTargetOutput(GdbTargetOutputRecord* record)
{
}

void ServerDebugger::ProcessLogOutput(GdbLogOutputRecord* record)
{
}

std::string ServerDebugger::GetTargetInputBytes()
{
    targetInputReplyReceived = false;
    TargetInputRequest targetInputRequest;
    targetInputRequest.messageKind = "targetInputRequest";
    std::unique_ptr<JsonValue> request = targetInputRequest.ToJson();
    clientChannel->SendMessage(request.release());
    std::unique_lock<std::mutex> lock(targetIOMutex);
    targetInputReplyReceivedVar.wait(lock, [this] { return targetInputReplyReceived; });
    if (targetInputReply.eof)
    {
        return std::string();
    }
    else
    {
        std::string targetInputBytes;
        for (unsigned char c : targetInputReply.line)
        {
            targetInputBytes.append(ToHexString(static_cast<uint8_t>(c)));
        }
        targetInputBytes.append(ToHexString(static_cast<uint8_t>('\n')));
        return targetInputBytes;
    }
}

void ServerDebugger::WriteTargetOuput(int handle, const std::string& s)
{
    TargetOutputRequest targetOutputRequest;
    targetOutputRequest.messageKind = "targetOutputRequest";
    targetOutputRequest.handle = std::to_string(handle);
    targetOutputRequest.output = s;
    std::unique_ptr<JsonValue> request = targetOutputRequest.ToJson();
    clientChannel->SendMessage(request.release());
    std::unique_lock<std::mutex> lock(targetIOMutex);
    targetOutputReplyReceivedVar.wait(lock, [this] { return targetOutputReplyReceived; });
}

ServerDebugger* serverDebugger = nullptr;

void StartDebuggerServer(const std::string& executable, const std::vector<std::string>& args, bool verbose, bool breakOnThrow, const std::string& version,
    int port, bool log)
{
    if (!serverDebugger)
    {
        serverDebugger = new ServerDebugger(executable, args, verbose, breakOnThrow, version, port, log);
    }
    serverDebugger->StartServer();
}

void StopDebuggerServer()
{
    serverDebugger->StopServer();
    delete serverDebugger;
}

void RunDebuggerServer(const std::string& executable, const std::vector<std::string>& args, bool verbose, bool breakOnThrow, const std::string& version, int port,
    bool log)
{
    if (!serverDebugger)
    {
        serverDebugger = new ServerDebugger(executable, args, verbose, breakOnThrow, version, port, log);
    }
    serverDebugger->RunServer();
    delete serverDebugger;
}

} } // namespace cmajor::debug
