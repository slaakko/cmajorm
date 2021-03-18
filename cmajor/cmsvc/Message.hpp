// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SERVICE_MESSAGE_INCLUDED
#define CMAJOR_SERVICE_MESSAGE_INCLUDED
#include <cmajor/cmsvc/ServiceApi.hpp>
#include <cmajor/wing/Window.hpp>
#include <string>

namespace cmajor { namespace service {

const int SM_SERVICE_MESSAGE_AVAILABLE = WM_USER + 1;

enum class ServiceMessageKind : int
{
    clearOutput = 0, outputMessage = 1, buildReply = 2, buildError = 3, stopBuild = 4
};

class CMSVC_API ServiceMessage
{
public:
    ServiceMessage(ServiceMessageKind kind_);
    virtual ~ServiceMessage();
    ServiceMessageKind Kind() const { return kind; }
private:
    ServiceMessageKind kind;
};

class CMSVC_API ClearOutputServiceMessage : public ServiceMessage
{
public:
    ClearOutputServiceMessage();
};

class CMSVC_API OutputServiceMessage : public ServiceMessage
{
public:
    OutputServiceMessage(const std::string& text_);
    const std::string& Text() const { return text; }
private:
    std::string text;
};

CMSVC_API void SetServiceMessageHandlerView(wing::Window* view);
CMSVC_API void PutServiceMessage(ServiceMessage* message);
CMSVC_API void PutClearOutputServiceMessage();
CMSVC_API void PutOutputServiceMessage(const std::string& messageText);
CMSVC_API bool ServiceMessageQueueEmpty();
CMSVC_API std::unique_ptr<ServiceMessage> GetServiceMessage();
CMSVC_API void InitServiceMessage();
CMSVC_API void DoneServiceMessage();

} } // namespace cmajor::service

#endif // CMAJOR_SVC_MESSAGE_INCLUDED
