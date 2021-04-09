// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_CLIENT_CHANNEL_INCLUDED
#define CMAJOR_DEBUG_CLIENT_CHANNEL_INCLUDED
#include <cmajor/cmdebug/DebugApi.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/Document.hpp>
#include <soulng/util/Socket.hpp>
#include <condition_variable>
#include <mutex>
#include <list>
#include <thread>

namespace cmajor { namespace debug {

using namespace sngxml::dom;
using namespace soulng::util;

class ClientChannelUser
{
public:
    virtual ~ClientChannelUser();
    virtual std::unique_ptr<Element> GetIdleClientChannelMessage() = 0;
    virtual bool SendIdleChannelMessages() const = 0;
    virtual bool IsIdleChannelMessage(Element* message) const = 0;
    virtual void ClientChannelError(const std::string& error) = 0;
    virtual void ProcessReceivedClientChannelMessage(Element* message) = 0;
};

class ClientChannel
{
public:
    ClientChannel(ClientChannelUser* user_, TcpSocket& socket_, int timeoutMs_);
    ~ClientChannel();
    void SendMessage(Element* message);
    void StartSendingIdleMessages();
    void StopSendingIdleMessages();
    void Run();
private:
    std::unique_ptr<Document> GetMessage();
    ClientChannelUser* user;
    TcpSocket& socket;
    int timeoutMs;
    std::thread thread;
    std::mutex messageQueueMtx;
    std::list<std::unique_ptr<Document>> messageQueue;
    std::condition_variable messageEnqueued;
    bool sendIdleMessages;
};

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_CLIENT_CHANNEL_INCLUDED
