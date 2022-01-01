// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_MESSAGE_HANDLER_INCLUDED
#define CMAJOR_BUILD_MESSAGE_HANDLER_INCLUDED
#include <cmajor/build/BuildApi.hpp>
#include <string>

namespace cmajor { namespace build {

struct PushProjectRequest;
struct PushProjectResponse;
struct PushProjectFileRequest;
struct PushProjectFileResponse;
struct PushProjectFileContentRequest;
struct PushProjectFileContentResponse;
struct BuildProjectRequest;
struct BuildProjectResponse;
struct ShowBuildMessageRequest;
struct ShowBuildMessageResponse;
struct ErrorResponse;
struct CloseConnectionRequest;

class BUILD_API MessageHandler
{
public:
    virtual ~MessageHandler();
    virtual void Handle(PushProjectRequest& request) {}
    virtual void Handle(PushProjectResponse& response) {}
    virtual void Handle(PushProjectFileRequest& request) {}
    virtual void Handle(PushProjectFileResponse& response) {}
    virtual void Handle(PushProjectFileContentRequest& request) {}
    virtual void Handle(PushProjectFileContentResponse& response) {}
    virtual void Handle(BuildProjectRequest& request) {}
    virtual void Handle(BuildProjectResponse& response) {}
    virtual void Handle(ShowBuildMessageRequest& request) {}
    virtual void Handle(ShowBuildMessageResponse& response) {}
    virtual void Handle(ErrorResponse& response) {}
    virtual void Handle(CloseConnectionRequest& request) {}
};

class BUILD_API Requester : public MessageHandler
{
public:
    virtual std::string Name() const = 0;
    virtual std::string GetStateStr() const = 0;
    void Handle(PushProjectRequest& request) override;
    void Handle(PushProjectResponse& response) override;
    void Handle(PushProjectFileRequest& request) override;
    void Handle(PushProjectFileResponse& response) override;
    void Handle(PushProjectFileContentRequest& request) override;
    void Handle(PushProjectFileContentResponse& response) override;
    void Handle(BuildProjectRequest& request) override;
    void Handle(BuildProjectResponse& response) override;
    void Handle(ShowBuildMessageRequest& request) override;
    void Handle(ShowBuildMessageResponse& response) override;
    void Handle(ErrorResponse& response) override;
    void Handle(CloseConnectionRequest& request) override;
};

class BUILD_API RequestHandler : public MessageHandler
{
public:
    virtual std::string Name() const = 0;
    void Handle(PushProjectRequest& request) override;
    void Handle(PushProjectResponse& response) override;
    void Handle(PushProjectFileRequest& request) override;
    void Handle(PushProjectFileResponse& response) override;
    void Handle(PushProjectFileContentRequest& request) override;
    void Handle(PushProjectFileContentResponse& response) override;
    void Handle(BuildProjectRequest& request) override;
    void Handle(ShowBuildMessageResponse& response) override;
    void Handle(ErrorResponse& response) override;
    void Handle(CloseConnectionRequest& request) override;
};

} } // namespace cmajor::build

#endif // CMAJOR_BUILD_MESSAGE_HANDLER_INCLUDED
