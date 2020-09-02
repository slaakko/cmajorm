// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmbs/BuildClient.hpp>
#include <cmajor/cmbs/BuildServerMessage.hpp>
#include <sngjson/json/JsonLexer.hpp>
#include <sngjson/json/JsonParser.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/Socket.hpp>
#include <soulng/util/Unicode.hpp>
#include <iostream>

namespace cmbs {

using namespace soulng::util;
using namespace soulng::unicode;

std::string GetMessageKind(JsonValue* message) 
{
    if (message->Type() == JsonValueType::object)
    {
        JsonObject* messageObject = static_cast<JsonObject*>(message);
        return messageObject->GetStringField(U"messageKind");
    }
    return std::string();
}

std::unique_ptr<JsonValue> ReadMessage(TcpSocket& socket)
{
    std::string message = ReadStr(socket);
    std::u32string content = ToUtf32(message);
    JsonLexer lexer(content, "", 0);
    std::unique_ptr<JsonValue> messageValue(JsonParser::Parse(lexer));
    return messageValue;
}

void ProcessBuildReply(const BuildReply& buildReply)
{
    if (!buildReply.requestValid)
    {
        std::cout << "buildReply.requestValid=false:\n" << buildReply.requestErrorMessage << std::endl;;
    }
    if (!buildReply.success)
    {
        std::cout << "buildReply.success=false:\n" << buildReply.error << std::endl;
    }
    if (!buildReply.logException.empty())
    {
        std::cout << "buildReply.logException:\n" << buildReply.logException << std::endl;
    }
    if (buildReply.success)
    {
        std::cout << "buildReply.success=true" << std::endl;
    }
}

void RunRequest(const std::string& requestFilePath, int port)
{
    std::string request = ReadFile(requestFilePath);
    TcpSocket socket("localhost", std::to_string(port));
    Write(socket, request);
    std::unique_ptr<JsonValue> message = ReadMessage(socket);
    std::string messageKind = GetMessageKind(message.get());
    while (messageKind == "logMessageRequest")
    {
        LogMessageRequest request(message.get());
        std::cout << request.message << std::endl;
        LogMessageReply replyMessage;
        replyMessage.messageKind = "logMessageReply";
        replyMessage.ok = true;
        std::unique_ptr<JsonValue> replyMessageValue(replyMessage.ToJson());
        std::string reply = replyMessageValue->ToString();
        Write(socket, reply);
        message = ReadMessage(socket);
        messageKind = GetMessageKind(message.get());
    }
    if (messageKind == "buildReply")
    {
        BuildReply reply(message.get());
        ProcessBuildReply(reply);
    }
    else if (messageKind == "genericErrorReply")
    {
        GenericErrorReply reply(message.get());
        throw std::runtime_error("generic error received: " + reply.error);
    }
    else
    {
        throw std::runtime_error("unknown reply received: 'messageKind' field value is '" + messageKind + "'");
    }
}

} // namespace cmbs
