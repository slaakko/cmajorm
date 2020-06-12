// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/Connection.hpp>
#include <cmajor/build/BuildServerMessage.hpp>
#include <cmajor/build/Message.hpp>
#include <cmajor/build/BuildOption.hpp>
#include <cmajor/build/BuildServer.hpp>
#include <sngjson/json/JsonParser.hpp>
#include <sngjson/json/JsonLexer.hpp>

namespace cmajor { namespace build {

Connection::Connection(Log* log_) : log(log_), serverAlive(false), server(nullptr)
{
}

Connection::~Connection()
{
}

void Connection::Send(soulng::util::JsonObject* messageObject)
{
    if (log)
    {
        const std::string& actor = GetActor();
        log->Write(actor + ": send: ", messageObject);
    }
    DoSend(messageObject);
}

std::unique_ptr<MessageBase> Connection::Receive()
{
    std::unique_ptr<soulng::util::JsonObject> messageObject = DoReceive();
    if (messageObject)
    {
        if (log)
        {
            const std::string& actor = GetActor();
            log->Write(actor + ": receive: ", messageObject.get());
        }
        std::unique_ptr<MessageBase> message = MessageFactory::Instance().CreateMessage(messageObject.get());
        return message;
    }
    else
    {
        return std::unique_ptr<MessageBase>(new CloseConnectionRequest());
    }
}

void Connection::Close()
{
    if (log)
    {
        const std::string& actor = GetActor();
        log->Write(actor +  ": closing connection\n", nullptr);
    }
    DoClose();
    if (server)
    {
        server->Exit();
    }
}

void Connection::SetServerAlive(bool alive)
{
    serverAlive = alive;
    if (log)
    {
        const std::string& actor = GetActor();
        log->Write(actor + ": alive = " + BoolStr(alive) + "\n", nullptr);
    }
}

} } // namespace cmajor::build
