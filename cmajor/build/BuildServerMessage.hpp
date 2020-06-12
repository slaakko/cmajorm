// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_BUILD_SERVER_MESSAGE_INCLUDED
#define CMAJOR_BUILD_BUILD_SERVER_MESSAGE_INCLUDED
#include <cmajor/build/ProjectInfo.hpp>
#include <cmajor/build/Message.hpp>

namespace cmajor { namespace build {

extern const char* pushProjectRequestId;
extern const char* pushProjectResponseId;
extern const char* pushProjectFileRequestId;
extern const char* pushProjectFileResponseId;
extern const char* pushProjectFileContentRequestId;
extern const char* pushProjectFileContentResponseId;
extern const char* buildProjectRequestId;
extern const char* buildProjectResponseId;
extern const char* showBuildMessageRequestId;
extern const char* showBuildMessageResponseId;
extern const char* errorResponseId;
extern const char* closeConnectionRequestId;

struct BUILD_API PushProjectRequest : public Message<ProjectInfo>
{
    PushProjectRequest();
    PushProjectRequest(JsonObject* messageObject);
    const char* Id() const override { return pushProjectRequestId; }
    void DispatchTo(MessageHandler& handler) override;
};

struct BUILD_API PushProjectResponse : public Message<ProjectUpToDate>
{
    PushProjectResponse();
    PushProjectResponse(JsonObject* messageObject);
    const char* Id() const override { return pushProjectResponseId; }
    void DispatchTo(MessageHandler& handler) override;
};

struct BUILD_API PushProjectFileRequest : public Message<ProjectInfo>
{
    PushProjectFileRequest();
    PushProjectFileRequest(JsonObject* messageObject);
    const char* Id() const override { return pushProjectFileRequestId; }
    void DispatchTo(MessageHandler& handler) override;
};

struct BUILD_API PushProjectFileResponse : public Message<ProjectInfo>
{
    PushProjectFileResponse();
    PushProjectFileResponse(JsonObject* messageObject);
    const char* Id() const override { return pushProjectFileResponseId; }
    void DispatchTo(MessageHandler& handler) override;
};

struct BUILD_API PushProjectFileContentRequest : public Message<ProjectInfo>
{
    PushProjectFileContentRequest();
    PushProjectFileContentRequest(JsonObject* messageObject);
    const char* Id() const override { return pushProjectFileContentRequestId; }
    void DispatchTo(MessageHandler& handler) override;
};

struct BUILD_API PushProjectFileContentResponse : public Message<ProjectUpdated>
{
    PushProjectFileContentResponse();
    PushProjectFileContentResponse(JsonObject* messageObject);
    const char* Id() const override { return pushProjectFileContentResponseId; }
    void DispatchTo(MessageHandler& handler) override;
};

struct BUILD_API BuildProjectRequest : public Message<ProjectToBuild>
{
    BuildProjectRequest();
    BuildProjectRequest(JsonObject* messageObject);
    const char* Id() const override { return buildProjectRequestId; }
    void DispatchTo(MessageHandler& handler) override;
};

struct BUILD_API BuildProjectResponse : public Message<ProjectBuilt>
{
    BuildProjectResponse();
    BuildProjectResponse(JsonObject* messageObject);
    const char* Id() const override { return buildProjectResponseId; }
    void DispatchTo(MessageHandler& handler) override;
};

struct BUILD_API ShowBuildMessageRequest : public Message<MessageLine>
{
    ShowBuildMessageRequest();
    ShowBuildMessageRequest(JsonObject* messageObject);
    const char* Id() const override { return showBuildMessageRequestId; }
    void DispatchTo(MessageHandler& handler) override;
};

struct BUILD_API ShowBuildMessageResponse : public Message<MessageShown>
{
    ShowBuildMessageResponse();
    ShowBuildMessageResponse(JsonObject* messageObject);
    const char* Id() const override { return showBuildMessageResponseId; }
    void DispatchTo(MessageHandler& handler) override;
};

struct BUILD_API ErrorResponse : public Message<Error>
{
    ErrorResponse();
    ErrorResponse(JsonObject* messageObject);
    const char* Id() const override { return errorResponseId; }
    void DispatchTo(MessageHandler& handler) override;
};

struct BUILD_API CloseConnectionRequest : Message<CloseConnection>
{
    CloseConnectionRequest();
    CloseConnectionRequest(JsonObject* messageObject);
    const char* Id() const override { return closeConnectionRequestId; }
    void DispatchTo(MessageHandler& handler) override;
};

BUILD_API void BuildServerMessageInit();
BUILD_API void BuildServerMessageDone();

} } // namespace cmajor::build

#endif // CMAJOR_BUILD_BUILD_SERVER_MESSAGE_INCLUDED
