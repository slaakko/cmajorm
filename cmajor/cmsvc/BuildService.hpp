// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SERVICE_BUILD_SERVICE_INCLUDED
#define CMAJOR_SERVICE_BUILD_SERVICE_INCLUDED
#include <cmajor/cmsvc/Message.hpp>
#include <cmajor/cmmsg/BuildServerMessage.hpp>

namespace cmajor { namespace service {

struct CMSVC_API BuildServiceStartParams
{
    BuildServiceStartParams();
    BuildServiceStartParams& ProcessName(const std::string& processName_);
    BuildServiceStartParams& DebugServer(bool debugServer_);
    BuildServiceStartParams& Log(bool log_);
    BuildServiceStartParams& Wait(bool wait_);
    std::string processName;
    bool debugServer;
    bool log;
    bool wait;
};

class CMSVC_API BuildServiceRequest
{
public:
    virtual ~BuildServiceRequest();
    virtual void Execute() = 0;
    virtual std::string Name() const = 0;
    virtual void Failed(const std::string& error) = 0;
};

class CMSVC_API RunBuildRequest : public BuildServiceRequest
{
public:
    RunBuildRequest(const BuildRequest& buildRequest_);
    std::string Name() const override { return "runBuildRequest"; }
    void Execute() override;
    void Failed(const std::string& error) override;
private:
    BuildRequest buildRequest;
};

class CMSVC_API BuildReplyServiceMessage : public ServiceMessage
{
public:
    BuildReplyServiceMessage(const BuildReply& buildReply_);
    BuildReply& GetBuildReply() { return buildReply; }
private:
    BuildReply buildReply;
};

class CMSVC_API BuildErrorServiceMessage : public ServiceMessage
{
public:
    BuildErrorServiceMessage(const std::string& error_);
    const std::string& Error() const { return error; }
private:
    std::string error;
};

class CMSVC_API StopBuildServiceMessage : public ServiceMessage
{
public:
    StopBuildServiceMessage();
};

CMSVC_API void StartBuildService(BuildServiceStartParams& startParams);
CMSVC_API void EnqueueBuildServiceRequest(BuildServiceRequest* request);
CMSVC_API void StopBuildService(bool log);
CMSVC_API bool BuildServiceRunning();
CMSVC_API void InitBuildService();
CMSVC_API void DoneBuildService();

} } // namespace cmajor::service

#endif // CMAJOR_SERVICE_BUILD_SERVICE_INCLUDED
