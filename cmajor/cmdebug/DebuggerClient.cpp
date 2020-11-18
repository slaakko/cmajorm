// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/DebuggerClient.hpp>
#include <cmajor/cmdebug/CmdbMessage.hpp>
#include <cmajor/cmdebug/CmdbMessageMap.hpp>
#include <cmajor/cmdebug/DebugInfo.hpp>
#include <sngjson/json/JsonLexer.hpp>
#include <sngjson/json/JsonParser.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Socket.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>

namespace cmajor { namespace debug {

using namespace soulng::util;
using namespace soulng::unicode;

class DebuggerClient;

class ClientCommand
{
public:
    virtual ~ClientCommand();
    virtual void Execute(DebuggerClient& client) = 0;
};

class StartCommand : public ClientCommand
{
public:
    void Execute(DebuggerClient& client) override;
};

class StopCommand : public ClientCommand
{
public:
    void Execute(DebuggerClient& client) override;
};

class ContinueCommand : public ClientCommand
{
public:
    void Execute(DebuggerClient& client) override;
};

class NextCommand : public ClientCommand
{
public:
    void Execute(DebuggerClient& client) override;
};

class StepCommand : public ClientCommand
{
public:
    void Execute(DebuggerClient& client) override;
};

class LocalsCommand : public ClientCommand
{
public:
    void Execute(DebuggerClient& client) override;
};

class ChildrenCommand : public ClientCommand
{
public:
    ChildrenCommand(const std::string& expr_, const std::string& start_, const std::string& count_);
    void Execute(DebuggerClient& client) override;
private:
    std::string expr;
    std::string start;
    std::string count;
};

class EvaluateCommand : public ClientCommand
{
public:
    EvaluateCommand(const std::string& expr_);
    void Execute(DebuggerClient& client) override;
private:
    std::string expr;
};

class BreakCommand : public ClientCommand
{
public:
    BreakCommand(const SourceLoc& location_);
    void Execute(DebuggerClient& client) override;
private:
    SourceLoc location;
};

std::unique_ptr<ClientCommand> ParseCommand(const std::string& line)
{
    if (line == "start")
    {
        return std::unique_ptr<ClientCommand>(new StartCommand());
    }
    else if (line == "stop")
    {
        return std::unique_ptr<ClientCommand>(new StopCommand());
    }
    else if (line == "continue")
    {
        return std::unique_ptr<ClientCommand>(new ContinueCommand());
    }
    else if (line == "next")
    {
        return std::unique_ptr<ClientCommand>(new NextCommand());
    }
    else if (line == "step")
    {
        return std::unique_ptr<ClientCommand>(new StepCommand());
    }
    else if (line == "locals")
    {
        return std::unique_ptr<ClientCommand>(new LocalsCommand());
    }
    else if (StartsWith(line, "children"))
    {
        std::string::size_type spacePos = line.find(' ');
        if (spacePos != std::string::npos)
        {
            std::string params = line.substr(spacePos + 1);
            std::vector<std::string> paramVec = Split(params, ',');
            if (paramVec.size() == 3)
            {
                return std::unique_ptr<ClientCommand>(new ChildrenCommand(paramVec[0], paramVec[1], paramVec[2]));
            }
            else
            {
                throw std::runtime_error("invalid children params");
            }
        }
        else
        {
            throw std::runtime_error("invalid children params");
        }
    }
    else if (StartsWith(line, "evaluate"))
    {
        std::string::size_type spacePos = line.find(' ');
        if (spacePos != std::string::npos)
        {
            std::string expr = line.substr(spacePos + 1);
            return std::unique_ptr<ClientCommand>(new EvaluateCommand(expr));
        }
        else
        {
            throw std::runtime_error("invalid evaluate params");
        }
    }
    else if (StartsWith(line, "break"))
    {
        std::string::size_type spacePos = line.find(' ');
        if (spacePos != std::string::npos)
        {
            std::string params = line.substr(spacePos + 1);
            std::vector<std::string> paramVec = Split(params, ':');
            if (paramVec.size() == 1)
            {
                SourceLoc sourceLoc;
                sourceLoc.path = "";
                sourceLoc.line = params[0];
                return std::unique_ptr<ClientCommand>(new BreakCommand(sourceLoc));
            }
            else if (paramVec.size() == 2)
            {
                SourceLoc sourceLoc;
                sourceLoc.path = paramVec[0];
                sourceLoc.line = paramVec[1];
                return std::unique_ptr<ClientCommand>(new BreakCommand(sourceLoc));
            }
            else
            {
                throw std::runtime_error("invalid break params");
            }
        }
        else
        {
            throw std::runtime_error("invalid break params");
        }
    }
    else
    {
        throw std::runtime_error("unknown command");
    }
}

class DebuggerClient
{
public:
    DebuggerClient(int port);
    MessageKind GetMessageKind(JsonValue* message, std::string& messageKindStr);
    void Start();
    void Stop();
    void Continue();
    void Next();
    void Step();
    void Locals();
    void Children(const std::string& expr, const std::string& start, const std::string& count);
    void Evaluate(const std::string& expr);
    void Break(const SourceLoc& location);
    void WriteRequest(JsonValue* request);
    void WriteReply(JsonValue* reply);
    std::unique_ptr<JsonValue> ReadReply(MessageKind replyMessageKind);
    void ProcessMessage(JsonValue* message, MessageKind messageKind, const std::string& messageKindStr);
    void ProcessTargetRunningRequest(const TargetRunningRequest& targetRunningRequest);
    void ProcessTargetInputRequest(const TargetInputRequest& targetInputRequest);
    void ProcessTargetOutputRequest(const TargetOutputRequest& targetOutputRequest);
    void ProcessLogMessageRequest(const LogMessageRequest& logMessageRequest);
    void ProcessErrorReply(const GenericErrorReply& errorReply);
    void ProcessStartReply(JsonValue* reply);
    void ProcessStopReply(JsonValue* reply);
    void ProcessContinueReply(JsonValue* reply);
    void ProcessNextReply(JsonValue* reply);
    void ProcessStepReply(JsonValue* reply);
    int ProcessCountReply(JsonValue* reply);
    void ProcessEvaluateChildReply(JsonValue* reply);
    void ProcessEvaluateReply(JsonValue* reply);
    void ProcessBreakReply(JsonValue* reply);
    bool Stopped() const { return stopped; }
private:
    MessageMap messageMap;
    int port;
    TcpSocket socket;
    bool stopped;
};

DebuggerClient::DebuggerClient(int port_) : port(port_), stopped(false)
{
}

MessageKind DebuggerClient::GetMessageKind(JsonValue* message, std::string& messageKindStr)
{
    if (message->Type() == JsonValueType::object)
    {
        JsonObject* messageObject = static_cast<JsonObject*>(message);
        messageKindStr = messageObject->GetStringField(U"messageKind");
        return messageMap.GetMessageKind(messageKindStr);
    }
    return MessageKind::none;
}

void DebuggerClient::ProcessMessage(JsonValue* message, MessageKind messageKind, const std::string& messageKindStr)
{
    switch (messageKind)
    {
        case MessageKind::targetRunningRequest:
        {
            TargetRunningRequest targetRunningRequest(message);
            ProcessTargetRunningRequest(targetRunningRequest);
            break;
        }
        case MessageKind::targetInputRequest:
        {
            TargetInputRequest targetInputRequest(message);
            ProcessTargetInputRequest(targetInputRequest);
            break;
        }
        case MessageKind::targetOutputRequest:
        {
            TargetOutputRequest targetOutputRequest(message);
            ProcessTargetOutputRequest(targetOutputRequest);
            break;
        }
        case MessageKind::logMessageRequest:
        {
            LogMessageRequest logMessageRequest(message);
            ProcessLogMessageRequest(logMessageRequest);
            break;
        }
        case MessageKind::genericErrorReply:
        {
            GenericErrorReply errorReply(message);
            ProcessErrorReply(errorReply);
            break;
        }
    }
}

void DebuggerClient::ProcessTargetRunningRequest(const TargetRunningRequest& targetRunningRequest)
{
    TargetRunningReply targetRunningReply;
    targetRunningReply.messageKind = "targetRunningReply";
    std::unique_ptr<JsonValue> reply = targetRunningReply.ToJson();
    WriteReply(reply.get());
}

void DebuggerClient::ProcessTargetInputRequest(const TargetInputRequest& targetInputRequest)
{
    TargetInputReply targetInputReply;
    targetInputReply.messageKind = "targetInputReply";
    std::string line;
    if (std::getline(std::cin, line))
    {
        targetInputReply.line = line;
    }
    else
    {
        targetInputReply.eof = true;
    }
    std::unique_ptr<JsonValue> reply = targetInputReply.ToJson();
    WriteReply(reply.get());
}

void DebuggerClient::ProcessTargetOutputRequest(const TargetOutputRequest& targetOutputRequest)
{
    int handle = boost::lexical_cast<int>(targetOutputRequest.handle);
    if (handle == 1)
    {
        std::cout << targetOutputRequest.output;
    }
    else if (handle == 2)
    {
        std::cerr << targetOutputRequest.output;
    }
    TargetOutputReply targetOutputReply;
    targetOutputReply.messageKind = "targetOutputReply";
    std::unique_ptr<JsonValue> reply = targetOutputReply.ToJson();
    WriteReply(reply.get());
}

void DebuggerClient::ProcessLogMessageRequest(const LogMessageRequest& logMessageRequest)
{
    std::cout << logMessageRequest.logMessage << std::endl;
    LogMessageReply logMessageReply;
    logMessageReply.messageKind = "logMessageReply";
    std::unique_ptr<JsonValue> reply = logMessageReply.ToJson();
    WriteReply(reply.get());
}

void DebuggerClient::ProcessErrorReply(const GenericErrorReply& errorReply)
{
    std::cerr << errorReply.errorMessage << std::endl;
}

void DebuggerClient::WriteRequest(JsonValue* request)
{
    std::string requestStr = request->ToString();
    Write(socket, requestStr);
}

void DebuggerClient::WriteReply(JsonValue* reply)
{
    std::string replyStr = reply->ToString();
    Write(socket, replyStr);
}

std::unique_ptr<JsonValue> DebuggerClient::ReadReply(MessageKind replyMessageKind)
{
    std::string replyStr = ReadStr(socket);
    JsonLexer lexer(ToUtf32(replyStr), "", 0);
    std::unique_ptr<JsonValue> replyValue = JsonParser::Parse(lexer);
    std::string messageKindStr;
    MessageKind messageKind = GetMessageKind(replyValue.get(), messageKindStr);
    while (messageKind != replyMessageKind)
    {
        ProcessMessage(replyValue.get(), messageKind, messageKindStr);
        replyStr = ReadStr(socket);
        JsonLexer lexer(ToUtf32(replyStr), "", 0);
        replyValue = JsonParser::Parse(lexer);
        messageKind = GetMessageKind(replyValue.get(), messageKindStr);
    }
    return replyValue;
}

void DebuggerClient::Start()
{
    socket.Connect("localhost", std::to_string(port));
    StartRequest startRequest;
    startRequest.messageKind = "startRequest";
    std::unique_ptr<JsonValue> request = startRequest.ToJson();
    WriteRequest(request.get());
    std::unique_ptr<JsonValue> replyValue = ReadReply(MessageKind::startReply);
    ProcessStartReply(replyValue.get());
}

void DebuggerClient::ProcessStartReply(JsonValue* reply)
{
    StartReply startReply(reply);
}

void DebuggerClient::Stop()
{
    StopRequest stopRequest;
    stopRequest.messageKind = "stopRequest";
    std::unique_ptr<JsonValue> request = stopRequest.ToJson();
    WriteRequest(request.get());
    std::unique_ptr<JsonValue> replyValue = ReadReply(MessageKind::stopReply);
    ProcessStopReply(replyValue.get());
}

void DebuggerClient::ProcessStopReply(JsonValue* reply)
{
    StopReply stopReply(reply);
    stopped = true;
}

void DebuggerClient::Continue()
{
    ContinueRequest continueRequest;
    continueRequest.messageKind = "continueRequest";
    std::unique_ptr<JsonValue> request = continueRequest.ToJson();
    WriteRequest(request.get());
    std::unique_ptr<JsonValue> replyValue = ReadReply(MessageKind::continueReply);
    ProcessContinueReply(replyValue.get());
}

void DebuggerClient::ProcessContinueReply(JsonValue* reply)
{
    ContinueReply continueReply(reply);
}

void DebuggerClient::Next()
{
    NextRequest nextRequest;
    nextRequest.messageKind = "nextRequest";
    std::unique_ptr<JsonValue> request = nextRequest.ToJson();
    WriteRequest(request.get());
    std::unique_ptr<JsonValue> replyValue = ReadReply(MessageKind::nextReply);
    ProcessNextReply(replyValue.get());
}

void DebuggerClient::ProcessNextReply(JsonValue* reply)
{
    NextReply nextReply(reply);
}

void DebuggerClient::Step()
{
    StepRequest stepRequest;
    stepRequest.messageKind = "stepRequest";
    std::unique_ptr<JsonValue> request = stepRequest.ToJson();
    WriteRequest(request.get());
    std::unique_ptr<JsonValue> replyValue = ReadReply(MessageKind::stepReply);
    ProcessStepReply(replyValue.get());
}

void DebuggerClient::ProcessStepReply(JsonValue* reply)
{
    StepReply stepReply(reply);
}

void DebuggerClient::Locals()
{
    CountRequest countRequest;
    countRequest.messageKind = "countRequest";
    countRequest.expression = "@locals";
    std::unique_ptr<JsonValue> request = countRequest.ToJson();
    WriteRequest(request.get());
    std::unique_ptr<JsonValue> replyValue = ReadReply(MessageKind::countReply);
    int numLocals = ProcessCountReply(replyValue.get());
    EvaluateChildRequest evaluateChildRequest;
    evaluateChildRequest.messageKind = "evaluateChildRequest";
    evaluateChildRequest.expression = "@locals";
    evaluateChildRequest.start = std::to_string(0);
    evaluateChildRequest.count = std::to_string(numLocals);
    std::unique_ptr<JsonValue> req = evaluateChildRequest.ToJson();
    WriteRequest(req.get());
    std::unique_ptr<JsonValue> replyVal = ReadReply(MessageKind::evaluateChildReply);
    ProcessEvaluateChildReply(replyVal.get());
}

int DebuggerClient::ProcessCountReply(JsonValue* reply)
{
    CountReply countReply(reply);
    int numLocals = boost::lexical_cast<int>(countReply.count);
    return numLocals;
}

void DebuggerClient::ProcessEvaluateChildReply(JsonValue* reply)
{
    EvaluateChildReply evaluateChildReply(reply);
    int n = evaluateChildReply.results.size();
    for (int i = 0; i < n; ++i)
    {
        const ChildResult& childResult = evaluateChildReply.results[i];
        std::string s = childResult.name;
        if (!childResult.value.empty())
        {
            s.append(" = ").append(childResult.value);
        }
        std::cout << s << " : [" << childResult.expr << ", " << childResult.type << ", " << childResult.count << "]" << std::endl;;
    }
}

void DebuggerClient::ProcessEvaluateReply(JsonValue* reply)
{
    EvaluateReply evaluateReply(reply);
    if (evaluateReply.success)
    {
        std::cout << evaluateReply.result.value << std::endl;
    }
    else
    {
        std::cerr << evaluateReply.error << std::endl;
    }
}

void DebuggerClient::ProcessBreakReply(JsonValue* reply)
{
    BreakReply breakReply(reply);
}

void DebuggerClient::Children(const std::string& expr, const std::string& start, const std::string& count)
{
    EvaluateChildRequest evaluateChildRequest;
    evaluateChildRequest.messageKind = "evaluateChildRequest";
    evaluateChildRequest.expression = expr;
    evaluateChildRequest.start = start;
    evaluateChildRequest.count = count;
    std::unique_ptr<JsonValue> req = evaluateChildRequest.ToJson();
    WriteRequest(req.get());
    std::unique_ptr<JsonValue> replyVal = ReadReply(MessageKind::evaluateChildReply);
    ProcessEvaluateChildReply(replyVal.get());
}

void DebuggerClient::Evaluate(const std::string& expr)
{
    EvaluateRequest evaluateRequest;
    evaluateRequest.messageKind = "evaluateRequest";
    evaluateRequest.expression = expr;
    std::unique_ptr<JsonValue> request = evaluateRequest.ToJson();
    WriteRequest(request.get());
    std::unique_ptr<JsonValue> reply = ReadReply(MessageKind::evaluateReply);
    ProcessEvaluateReply(reply.get());
}

void DebuggerClient::Break(const SourceLoc& location)
{
    BreakRequest breakRequest;
    breakRequest.messageKind = "breakRequest";
    breakRequest.breakpointLocation = location;
    std::unique_ptr<JsonValue> request = breakRequest.ToJson();
    WriteRequest(request.get());
    std::unique_ptr<JsonValue> reply = ReadReply(MessageKind::breakReply);
    ProcessBreakReply(reply.get());
}

ClientCommand::~ClientCommand()
{
}

void StartCommand::Execute(DebuggerClient& client)
{
    client.Start();
}

void StopCommand::Execute(DebuggerClient& client)
{
    client.Stop();
}

void ContinueCommand::Execute(DebuggerClient& client)
{
    client.Continue();
}

void NextCommand::Execute(DebuggerClient& client)
{
    client.Next();
}

void StepCommand::Execute(DebuggerClient& client)
{
    client.Step();
}

void LocalsCommand::Execute(DebuggerClient& client)
{
    client.Locals();
}

ChildrenCommand::ChildrenCommand(const std::string& expr_, const std::string& start_, const std::string& count_) : expr(expr_), start(start_), count(count_)
{
}

void ChildrenCommand::Execute(DebuggerClient& client)
{
    client.Children(expr, start, count);
}

EvaluateCommand::EvaluateCommand(const std::string& expr_) : expr(expr_)
{
}

void EvaluateCommand::Execute(DebuggerClient& client)
{
    client.Evaluate(expr);
}

BreakCommand::BreakCommand(const SourceLoc& location_) : location(location_)
{
}

void BreakCommand::Execute(DebuggerClient& client)
{
    client.Break(location);
}

void RunClient(int port)
{
    DebuggerClient client(port);
    while (!client.Stopped())
    {
        std::string line;
        std::cout << "> ";
        std::getline(std::cin, line);
        std::unique_ptr<ClientCommand> command = ParseCommand(line);
        command->Execute(client);
    }
}

} } // namespace cmajor::debug
