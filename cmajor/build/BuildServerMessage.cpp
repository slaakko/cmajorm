// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/BuildServerMessage.hpp>
#include <cmajor/build/MessageHandler.hpp>

namespace cmajor { namespace build {

const char* pushProjectRequestId = "push-project-request";
const char* pushProjectResponseId = "push-project-response";
const char* pushProjectFileRequestId = "push-project-file-request";
const char* pushProjectFileResponseId = "push-project-file-response";
const char* pushProjectFileContentRequestId = "push-project-file-content-request";
const char* pushProjectFileContentResponseId = "push-project-file-content-response";
const char* buildProjectRequestId = "build-project-request";
const char* buildProjectResponseId = "build-project-response";
const char* showBuildMessageRequestId = "show-build-message-request";
const char* showBuildMessageResponseId = "show-build-message-response";
const char* errorResponseId = "error-response";
const char* closeConnectionRequestId = "close-connection-request";

PushProjectRequest::PushProjectRequest() : Message<ProjectInfo>()
{
}

PushProjectRequest::PushProjectRequest(JsonObject* messageObject) : Message<ProjectInfo>(messageObject)
{
}

void PushProjectRequest::DispatchTo(MessageHandler& handler)
{
    handler.Handle(*this);
}

PushProjectResponse::PushProjectResponse() : Message<ProjectUpToDate>()
{
}

PushProjectResponse::PushProjectResponse(JsonObject* messageObject) : Message<ProjectUpToDate>(messageObject)
{
}

void PushProjectResponse::DispatchTo(MessageHandler& handler)
{
    handler.Handle(*this);
}

PushProjectFileRequest::PushProjectFileRequest() : Message<ProjectInfo>()
{
}

PushProjectFileRequest::PushProjectFileRequest(JsonObject* messageObject) : Message<ProjectInfo>(messageObject)
{
}

void PushProjectFileRequest::DispatchTo(MessageHandler& handler)
{
    handler.Handle(*this);
}

PushProjectFileResponse::PushProjectFileResponse() : Message<ProjectInfo>()
{
}

PushProjectFileResponse::PushProjectFileResponse(JsonObject* messageObject) : Message<ProjectInfo>(messageObject)
{
}

void PushProjectFileResponse::DispatchTo(MessageHandler& handler)
{
    handler.Handle(*this);
}

PushProjectFileContentRequest::PushProjectFileContentRequest() : Message<ProjectInfo>()
{
}

PushProjectFileContentRequest::PushProjectFileContentRequest(JsonObject* messageObject) : Message<ProjectInfo>(messageObject)
{
}

void PushProjectFileContentRequest::DispatchTo(MessageHandler& handler)
{
    handler.Handle(*this);
}

PushProjectFileContentResponse::PushProjectFileContentResponse() : Message<ProjectUpdated>()
{
}

PushProjectFileContentResponse::PushProjectFileContentResponse(JsonObject* messageObject) : Message<ProjectUpdated>(messageObject)
{
}

void PushProjectFileContentResponse::DispatchTo(MessageHandler& handler)
{
    handler.Handle(*this);
}

BuildProjectRequest::BuildProjectRequest() : Message<ProjectToBuild>()
{
}

BuildProjectRequest::BuildProjectRequest(JsonObject* messageObject) : Message<ProjectToBuild>(messageObject)
{
}

void BuildProjectRequest::DispatchTo(MessageHandler& handler)
{
    handler.Handle(*this);
}

BuildProjectResponse::BuildProjectResponse() : Message<ProjectBuilt>()
{
}

BuildProjectResponse::BuildProjectResponse(JsonObject* messageObject) : Message<ProjectBuilt>(messageObject)
{
}

void BuildProjectResponse::DispatchTo(MessageHandler& handler)
{
    handler.Handle(*this);
}

ShowBuildMessageRequest::ShowBuildMessageRequest() : Message<MessageLine>()
{
}

ShowBuildMessageRequest::ShowBuildMessageRequest(JsonObject* messageObject) : Message<MessageLine>(messageObject)
{
}

void ShowBuildMessageRequest::DispatchTo(MessageHandler& handler)
{
    handler.Handle(*this);
}

ShowBuildMessageResponse::ShowBuildMessageResponse() : Message<MessageShown>()
{
}

ShowBuildMessageResponse::ShowBuildMessageResponse(JsonObject* messageObject) : Message<MessageShown>(messageObject)
{
}

void ShowBuildMessageResponse::DispatchTo(MessageHandler& handler)
{
    handler.Handle(*this);
}

ErrorResponse::ErrorResponse() : Message<Error>()
{
}

ErrorResponse::ErrorResponse(JsonObject* messageObject) : Message<Error>(messageObject)
{
}

void ErrorResponse::DispatchTo(MessageHandler& handler)
{
    handler.Handle(*this);
}

CloseConnectionRequest::CloseConnectionRequest() : Message<CloseConnection>()
{
}

CloseConnectionRequest::CloseConnectionRequest(JsonObject* messageObject) : Message<CloseConnection>(messageObject)
{
}

void CloseConnectionRequest::DispatchTo(MessageHandler& handler)
{
    handler.Handle(*this);
}

void BuildServerMessageInit()
{
    MessageFactory::Instance().Register(pushProjectRequestId, new ConcreteMessageCreator<PushProjectRequest>());
    MessageFactory::Instance().Register(pushProjectResponseId, new ConcreteMessageCreator<PushProjectResponse>());
    MessageFactory::Instance().Register(pushProjectFileRequestId, new ConcreteMessageCreator<PushProjectFileRequest>());
    MessageFactory::Instance().Register(pushProjectFileResponseId, new ConcreteMessageCreator<PushProjectFileResponse>());
    MessageFactory::Instance().Register(pushProjectFileContentRequestId, new ConcreteMessageCreator<PushProjectFileContentRequest>());
    MessageFactory::Instance().Register(pushProjectFileContentResponseId, new ConcreteMessageCreator<PushProjectFileContentResponse>());
    MessageFactory::Instance().Register(buildProjectRequestId, new ConcreteMessageCreator<BuildProjectRequest>());
    MessageFactory::Instance().Register(buildProjectResponseId, new ConcreteMessageCreator<BuildProjectResponse>());
    MessageFactory::Instance().Register(showBuildMessageRequestId, new ConcreteMessageCreator<ShowBuildMessageRequest>());
    MessageFactory::Instance().Register(showBuildMessageResponseId, new ConcreteMessageCreator<ShowBuildMessageResponse>());
    MessageFactory::Instance().Register(errorResponseId, new ConcreteMessageCreator<ErrorResponse>());
    MessageFactory::Instance().Register(closeConnectionRequestId, new ConcreteMessageCreator<CloseConnectionRequest>());
}

void BuildServerMessageDone()
{
}

} } // namespace cmajor::build
