// =================================
// Copyright (c) 2020 Seppo Laakko
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
        int32_t size = messageStr.length();
        uint8_t buffer[sizeof(size)];
        MemoryWriter writer(&buffer[0], sizeof(size));
        writer.Write(size);
        socket.Send(&buffer[0], sizeof(size));
        socket.Send(reinterpret_cast<const uint8_t*>(messageStr.c_str()), size);
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
        int32_t size = 0;
        uint8_t buffer[sizeof(size)];
        int offset = 0;
        int bytesToReceive = sizeof(size);
        int bytesReceived = socket.Receive(&buffer[offset], bytesToReceive);
        if (bytesReceived == 0)
        {
            return std::unique_ptr<JsonObject>();
        }
        bytesToReceive = bytesToReceive - bytesReceived;
        offset = offset + bytesReceived;
        while (bytesToReceive > 0)
        {
            bytesReceived = socket.Receive(&buffer[offset], bytesToReceive);
            if (bytesReceived == 0)
            {
                return std::unique_ptr<JsonObject>();
            }
            bytesToReceive = bytesToReceive - bytesReceived;
            offset = offset + bytesReceived;
        }
        MemoryReader reader(&buffer[0], sizeof(size));
        size = reader.ReadInt();
        std::unique_ptr<uint8_t[]> mem(new uint8_t[size]);
        offset = 0;
        bytesToReceive = size;
        bytesReceived = socket.Receive(mem.get() + offset, bytesToReceive);
        bytesToReceive = bytesToReceive - bytesReceived;
        offset = offset + bytesReceived;
        while (bytesToReceive > 0)
        {
            bytesReceived = socket.Receive(mem.get() + offset, bytesToReceive);
            bytesToReceive = bytesToReceive - bytesReceived;
            offset = offset + bytesReceived;
        }
        std::string messageStr(reinterpret_cast<const char*>(mem.get()), size);
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
