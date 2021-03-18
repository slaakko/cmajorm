// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

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

} } // namespace cmajor::service
