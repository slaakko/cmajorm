// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsvc/BuildServiceRequest.hpp>

namespace cmajor { namespace service {

StartBuildRequest::StartBuildRequest(const BuildServiceStartParams& serviceStartParams_, const BuildRequest& buildRequest_) : serviceStartParams(serviceStartParams_), buildRequest(buildRequest_)
{
}

void StartBuildRequest::Execute()
{
    if (!BuildServiceRunning())
    {
        StartBuildService(serviceStartParams);
    }
    EnqueueBuildServiceRequest(new RunBuildRequest(buildRequest));
}

StopBuildRequest::StopBuildRequest()
{
}

void StopBuildRequest::Execute()
{
    StopBuildService(true);
    PutServiceMessage(new StopBuildServiceMessage());
}

GotoDefinitionRequest::GotoDefinitionRequest(const BuildServiceStartParams& serviceStartParams_, const GetDefinitionRequest& getDefinitionRequest_) :
    serviceStartParams(serviceStartParams_), getDefinitionRequest(getDefinitionRequest_)
{
}

void GotoDefinitionRequest::Execute()
{
    if (!BuildServiceRunning())
    {
        StartBuildService(serviceStartParams);
    }
    EnqueueBuildServiceRequest(new RunGetDefinitionRequest(getDefinitionRequest));
}

} } // namespace cmajor::service
