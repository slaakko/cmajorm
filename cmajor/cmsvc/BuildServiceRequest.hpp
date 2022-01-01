// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SERVICE_BUILD_SERVICE_REQUEST_INCLUDED
#define CMAJOR_SERVICE_BUILD_SERVICE_REQUEST_INCLUDED
#include <cmajor/cmsvc/Request.hpp>
#include <cmajor/cmsvc/BuildService.hpp>

namespace cmajor { namespace service {

class CMSVC_API StartBuildRequest : public Request
{
public:
    StartBuildRequest(const BuildServiceStartParams& serviceStartParams_, const BuildRequest& buildRequest_);
    std::string Name() const override { return "startBuildRequest"; }
    void Execute() override;
private:
    BuildServiceStartParams serviceStartParams;
    BuildRequest buildRequest;
};

class CMSVC_API StopBuildRequest : public Request
{
public:
    StopBuildRequest();
    std::string Name() const override { return "stopBuildRequest"; }
    void Execute() override;
};

class CMSVC_API GotoDefinitionRequest : public Request
{
public:
    GotoDefinitionRequest(const BuildServiceStartParams& serviceStartParams_, const GetDefinitionRequest& getDefinitionRequest_);
    std::string Name() const override { return "gotoDefinitionRequest"; }
    void Execute() override;
private:
    BuildServiceStartParams serviceStartParams;
    GetDefinitionRequest getDefinitionRequest;
};

} } // namespace cmajor::service

#endif // CMAJOR_SERVICE_BUILD_SERVICE_REQUEST_INCLUDED
