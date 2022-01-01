// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/SocketConnection.hpp>
#include <cmajor/build/Host.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <sngjson/json/JsonParser.hpp>
#include <sngjson/json/JsonLexer.hpp>
#include <soulng/util/Log.hpp>
#include <soulng/util/MemoryReader.hpp>
#include <soulng/util/MemoryWriter.hpp>

namespace cmajor { namespace build {

using namespace cmajor::symbols;

SocketConnection::SocketConnection(Log* log, Host* host_, TcpSocket&& socket_) : Connection(log), host(host_), socket(std::move(socket_))
{
}

const std::string& SocketConnection::GetActor() const
{
    return host->Name();
}

void SocketConnection::DoSend(JsonObject* messageObject)
{
    try
    {
        std::string messageStr;
        if (messageObject)
        {
            messageStr = messageObject->ToString();
        }
        else
        {
            messageStr = "{ }";
        }
        Write(socket, messageStr);
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("socket connection: " + host->Name() + ": send: " + ex.what());
    }
}

std::unique_ptr<JsonObject> SocketConnection::DoReceive()
{
    try
    {
        std::string messageStr = ReadStr(socket);
        if (messageStr.empty())
        {
            return std::unique_ptr<JsonObject>();
        }
        JsonLexer lexer(ToUtf32(messageStr), "", 0);
        std::unique_ptr<JsonValue> jsonValue = JsonParser::Parse(lexer);
        if (jsonValue->Type() == JsonValueType::object)
        {
            std::unique_ptr<JsonObject> messageObject(static_cast<JsonObject*>(jsonValue.release()));
            return messageObject;
        }
        else
        {
            throw std::runtime_error("JSON object expected");
        }
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("socket connection: " + host->Name() + ": receive: " + ex.what());
    }
}

void SocketConnection::DoClose()
{
    try
    {
        socket.Close();
    }
    catch (const std::exception& ex)
    {
        if (GetGlobalFlag(GlobalFlags::printDebugMessages))
        {
            LogMessage(-1, "socket connection: " + host->Name() + ": socket close failed: " + std::string(ex.what()));
        }
    }
    host->Exit();
}

} } // namespace cmajor::build
