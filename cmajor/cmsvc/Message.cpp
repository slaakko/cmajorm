// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsvc/Message.hpp>
#include <cmajor/cmsvc/Request.hpp>
#include <list>
#include <mutex>
#include <memory>

namespace cmajor { namespace service {

wing::Window* serviceMessageHandlerView = nullptr;

class ServiceMessageQueue
{
public:
    static void Init();
    static void Done();
    static ServiceMessageQueue& Instance() { return *instance; }
    bool IsEmpty();
    void Put(ServiceMessage* message);
    std::unique_ptr<ServiceMessage> Get();
private:
    static std::unique_ptr<ServiceMessageQueue> instance;
    std::list<std::unique_ptr<ServiceMessage>> messageQueue;
    std::recursive_mutex mtx;
};

std::unique_ptr<ServiceMessageQueue> ServiceMessageQueue::instance;

bool ServiceMessageQueue::IsEmpty()
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    return messageQueue.empty();
}

void ServiceMessageQueue::Put(ServiceMessage* message)
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    messageQueue.push_back(std::unique_ptr<ServiceMessage>(message));
}

std::unique_ptr<ServiceMessage> ServiceMessageQueue::Get()
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    if (!IsEmpty())
    {
        std::unique_ptr<ServiceMessage> message = std::move(messageQueue.front());
        messageQueue.pop_front();
        return message;
    }
    return std::unique_ptr<ServiceMessage>();
}

void ServiceMessageQueue::Init()
{
    instance.reset(new ServiceMessageQueue());
}

void ServiceMessageQueue::Done()
{
    instance.reset();
}

ServiceMessage::ServiceMessage(ServiceMessageKind kind_) : kind(kind_)
{
}

ServiceMessage::~ServiceMessage()
{
}

ClearOutputServiceMessage::ClearOutputServiceMessage() : ServiceMessage(ServiceMessageKind::clearOutput)
{
}

OutputServiceMessage::OutputServiceMessage(const std::string& text_) : ServiceMessage(ServiceMessageKind::outputMessage), text(text_)
{
}

void SetServiceMessageHandlerView(wing::Window* view)
{
    serviceMessageHandlerView = view;
}

void PutServiceMessage(ServiceMessage* message)
{
    if (Exiting()) return;
    ServiceMessageQueue::Instance().Put(message);
    if (serviceMessageHandlerView)
    {
        SendMessage(serviceMessageHandlerView->Handle(), SM_SERVICE_MESSAGE_AVAILABLE, 0, 0);
    }
}

void PutClearOutputServiceMessage()
{
    PutServiceMessage(new ClearOutputServiceMessage());
}

void PutOutputServiceMessage(const std::string& messageText)
{
    PutServiceMessage(new OutputServiceMessage(messageText));
}

bool ServiceMessageQueueEmpty()
{
    return ServiceMessageQueue::Instance().IsEmpty();
}

std::unique_ptr<ServiceMessage> GetServiceMessage()
{
    std::unique_ptr<ServiceMessage> message = ServiceMessageQueue::Instance().Get();
    return message;
}

void InitServiceMessage()
{
    ServiceMessageQueue::Init();
}

void DoneServiceMessage()
{
    ServiceMessageQueue::Done();
}

} } // namespace cmajor::service
