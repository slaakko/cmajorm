// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/FiberConnection.hpp>
#include <cmajor/build/BuildServer.hpp>
#include <soulng/util/Fiber.hpp>
#include <sngjson/json/JsonParser.hpp>
#include <sngjson/json/JsonLexer.hpp>

namespace cmajor { namespace build {

FiberConnection::FiberConnection(Log* log) : Connection(log), serverFiber(nullptr), clientFiber(nullptr), currentFiber(nullptr), client("client"), server("server")
{
}

FiberConnection::~FiberConnection()
{
    if (clientFiber)
    {
        SwitchToFiber(clientFiber);
    }
}

void FiberConnection::SetFibers(void* serverFiber, void* clientFiber)
{
    this->serverFiber = serverFiber;
    this->clientFiber = clientFiber;
    currentFiber = serverFiber;
}

void FiberConnection::Switch()
{
    if (currentFiber == clientFiber)
    {
        currentFiber = serverFiber;
        SwitchToFiber(serverFiber);
    }
    else
    {
        currentFiber = clientFiber;
        SwitchToFiber(clientFiber);
    }
}

const std::string& FiberConnection::GetActor() const
{
    if (currentFiber == clientFiber)
    {
        return client;
    }
    else
    {
        return server;
    }
}

void FiberConnection::DoSend(JsonObject* messageObject)
{
    if (messageObject)
    {
        messageStr = messageObject->ToString();
    }
    else
    {
        messageStr = "{ }";
    }
    Switch();
}

std::unique_ptr<JsonObject> FiberConnection::DoReceive()
{
    Switch();
    JsonLexer lexer(ToUtf32(messageStr), "", 0);
    std::unique_ptr<JsonValue> jsonValue = JsonParser::Parse(lexer);
    if (jsonValue->Type() == JsonValueType::object)
    {
        std::unique_ptr<JsonObject> messageObject(static_cast<JsonObject*>(jsonValue.release()));
        return messageObject;
    }
    else
    {
        throw std::runtime_error("SingleServerConnection: JSON object expected");
    }
}

void FiberConnection::DoClose()
{
}

std::unique_ptr<FiberConnection> CreateFiberConnection(Log* log)
{
    return std::unique_ptr<FiberConnection>(new FiberConnection(log));
}

} } // namespace cmajor::buil
