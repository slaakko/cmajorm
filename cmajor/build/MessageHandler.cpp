// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/MessageHandler.hpp>
#include <cmajor/build/BuildServerMessage.hpp>
#include <stdexcept>

namespace cmajor { namespace build {

MessageHandler::~MessageHandler()
{
}

void Requester::Handle(PushProjectRequest& request)
{
    throw std::runtime_error(Name() + ": unexpected push-project-request received, requester state=" + GetStateStr());
}

void Requester::Handle(PushProjectResponse& response)
{
    throw std::runtime_error(Name() + ": unexpected push-project-response received, requester state=" + GetStateStr());
}

void Requester::Handle(PushProjectFileRequest& request)
{
    throw std::runtime_error(Name() + ": unexpected push-project-file-request received, requester state=" + GetStateStr());
}

void Requester::Handle(PushProjectFileResponse& response)
{
    throw std::runtime_error(Name() + ": unexpected push-project-file-response received, requester state=" + GetStateStr());
}

void Requester::Handle(PushProjectFileContentRequest& request)
{
    throw std::runtime_error(Name() + ": unexpected push-project-file-content-request received, requester state=" + GetStateStr());
}

void Requester::Handle(PushProjectFileContentResponse& response)
{
    throw std::runtime_error(Name() + ": unexpected push-project-file-content-response received, requester state=" + GetStateStr());
}

void Requester::Handle(BuildProjectRequest& request)
{
    throw std::runtime_error(Name() + ": unexpected build-project-request receivced, requester state=" + GetStateStr());
}

void Requester::Handle(BuildProjectResponse& response)
{
    throw std::runtime_error(Name() + ": unexpected build-project-response received, requester state=" + GetStateStr());
}

void Requester::Handle(ShowBuildMessageRequest& request)
{

}

void Requester::Handle(ShowBuildMessageResponse& response)
{

}

void Requester::Handle(ErrorResponse& response)
{
    throw std::runtime_error(Name() + ": unexpected error-response[\"" + response.body.error + "\"] received, requester state=" + GetStateStr());
}

void Requester::Handle(CloseConnectionRequest& request)
{
    throw std::runtime_error(Name() + ": unexpected close-connection-request received, requester state=" + GetStateStr());
}

void RequestHandler::Handle(PushProjectRequest& request)
{
    throw std::runtime_error(Name() + ": unexpected push-project-request received");
}

void RequestHandler::Handle(PushProjectResponse& response)
{
    throw std::runtime_error(Name() + ": unexpected push-project-response received");
}

void RequestHandler::Handle(PushProjectFileRequest& request)
{
    throw std::runtime_error(Name() + ": unexpected push-project-file-request received");
}

void RequestHandler::Handle(PushProjectFileResponse& response)
{
    throw std::runtime_error(Name() + ": unexpected push-project-file-response received");
}

void RequestHandler::Handle(PushProjectFileContentRequest& request)
{
    throw std::runtime_error(Name() + ": unexpected push-project-file-content-request received");
}

void RequestHandler::Handle(PushProjectFileContentResponse& response)
{
    throw std::runtime_error(Name() + ": unexpected push-project-file-content-response received");
}

void RequestHandler::Handle(BuildProjectRequest& request)
{
    throw std::runtime_error(Name() + ": unexpected build-project-request received");
}

void RequestHandler::Handle(ShowBuildMessageResponse& response)
{
    throw std::runtime_error(Name() + ": unexpected show-build-message-response received");
}

void RequestHandler::Handle(ErrorResponse& response)
{
    throw std::runtime_error(Name() + ": unexpected error-response[" + response.body.error + "] received");
}

void RequestHandler::Handle(CloseConnectionRequest& request)
{
    throw std::runtime_error(Name() + ": unexpected close-connection-request received");
}

} } // namespace cmajor::build
