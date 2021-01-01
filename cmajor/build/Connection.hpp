// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_CONNECTION_INCLUDED
#define CMAJOR_BUILD_CONNECTION_INCLUDED
#include <cmajor/build/Log.hpp>

namespace cmajor { namespace build {

class MessageBase;
class BuildServer;
class Host;

class BUILD_API Connection
{
public:
    Connection(Log* log_);
    virtual ~Connection();
    void Send(JsonObject* messageObject);
    virtual void DoSend(JsonObject* messageObject) = 0;
    std::unique_ptr<MessageBase> Receive();
    virtual std::unique_ptr<JsonObject> DoReceive() = 0;
    void Close();
    virtual void DoClose() = 0;
    Log* GetLog() const { return log; }
    virtual const std::string& GetActor() const = 0;
    bool ServerAlive() const { return serverAlive; }
    void SetServerAlive(bool serverAlive_);
    void SetServer(BuildServer* server_) { server = server_; }
    virtual Host* GetHost() const = 0;
private:
    Log* log;
    BuildServer* server;
    bool serverAlive;
};

} } // namespace cmajor::build

#endif // CMAJOR_BUILD_CONNECTION_INCLUDED
