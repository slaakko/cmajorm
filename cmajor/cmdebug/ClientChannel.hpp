// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_CLIENT_CHANNEL_INCLUDED
#define CMAJOR_DEBUG_CLIENT_CHANNEL_INCLUDED
#include <cmajor/cmdebug/DebugApi.hpp>
#include <soulng/util/Json.hpp>
#include <soulng/util/Socket.hpp>
#include <condition_variable>
#include <mutex>
#include <list>
#include <thread>

namespace cmajor { namespace debug {

using namespace soulng::util;

class ClientChannelUser
{
public:
    virtual ~ClientChannelUser();
    virtual std::unique_ptr<JsonValue> GetIdleClientChannelMessage() = 0;
    virtual bool IsIdleChannelMessage(JsonValue* message) const = 0;
    virtual void ClientChannelError(const std::string& error) = 0;
    virtual void ProcessReceivedClientChannelMessage(JsonValue* message) = 0;
};

class ClientChannel
{
public:
    ClientChannel(ClientChannelUser* user_, TcpSocket& socket_, int timeoutMs_);
    ~ClientChannel();
    void SendMessage(JsonValue* message);
    void StartSendingIdleMessages();
    void StopSendingIdleMessages();
    void Run();
private:
    std::unique_ptr<JsonValue> GetMessage();
    ClientChannelUser* user;
    TcpSocket& socket;
    int timeoutMs;
    std::thread thread;
    std::mutex messageQueueMtx;
    std::list<std::unique_ptr<JsonValue>> messageQueue;
    std::condition_variable messageEnqueued;
    bool sendIdleMessages;
};

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_CLIENT_CHANNEL_INCLUDED
