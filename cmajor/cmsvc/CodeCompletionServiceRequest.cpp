// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmsvc/CodeCompletionServiceRequest.hpp>

namespace cmajor { namespace service {

StartCodeCompletionServiceRequest::StartCodeCompletionServiceRequest(const CodeCompletionServiceStartParams& startParams_) : startParams(startParams_)
{
}

void StartCodeCompletionServiceRequest::Execute()
{
    StartCodeCompletionService(startParams);
}

LoadEditModuleCodeCompletionServiceRequest::LoadEditModuleCodeCompletionServiceRequest(const std::string& projectFilePath_, const std::string& backend_, const std::string& config_) :
    projectFilePath(projectFilePath_), backend(backend_), config(config_)
{
}

void LoadEditModuleCodeCompletionServiceRequest::Execute()
{
    LoadEditModule(projectFilePath, backend, config);
}

} } // namespace cmajor::service
