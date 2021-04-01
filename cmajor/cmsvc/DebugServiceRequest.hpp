// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsvc/Request.hpp>
#include <cmajor/cmsvc/DebugService.hpp>

namespace cmajor { namespace service {

class CMSVC_API StartDebugServiceRequest : public Request
{
public:
    StartDebugServiceRequest(const DebugServiceStartParams& startParams_, const std::vector<SourceLoc>& breakpoints_);
    void Execute() override;
    std::string Name() const override { return "startDebugServiceRequest"; }
private:
    DebugServiceStartParams startParams;
    std::vector<SourceLoc> breakpoints;
};

class CMSVC_API StopDebugServiceRequest : public Request
{
public:
    StopDebugServiceRequest();
    void Execute() override;
    std::string Name() const override { return "stopDebugServiceRequest"; }
};

class CMSVC_API ContinueDebugServiceRequest : public Request
{
public:
    ContinueDebugServiceRequest();
    void Execute() override;
    std::string Name() const override { return "continueDebugServiceRequest"; }
};

class CMSVC_API NextDebugServiceRequest : public Request
{
public:
    NextDebugServiceRequest();
    void Execute() override;
    std::string Name() const override { return "nextDebugServiceRequest"; }
};

class CMSVC_API StepDebugServiceRequest : public Request
{
public:
    StepDebugServiceRequest();
    void Execute() override;
    std::string Name() const override { return "stepDebugServiceRequest"; }
};

class CMSVC_API FinishDebugServiceRequest : public Request
{
public:
    FinishDebugServiceRequest();
    void Execute() override;
    std::string Name() const override { return "finishDebugServiceRequest"; }
};

class CMSVC_API UntilDebugServiceRequest : public Request
{
public:
    UntilDebugServiceRequest(const SourceLoc& sourceLocation_);
    void Execute() override;
    std::string Name() const override { return "untilDebugServiceRequest"; }
private:
    SourceLoc sourceLocation;
};

class CMSVC_API SetTargetInputEofRequest : public Request
{
public:
    SetTargetInputEofRequest();
    void Execute() override;
    std::string Name() const override { return "setTargetInputEofRequest"; }
};

class CMSVC_API PutTargetInputLineRequest : public Request
{
public:
    PutTargetInputLineRequest(const std::string& targetInputLine_);
    void Execute() override;
    std::string Name() const override { return "putTargetInputLineRequest"; }
private:
    std::string targetInputLine;
};

} } // namespace cmajor::service
