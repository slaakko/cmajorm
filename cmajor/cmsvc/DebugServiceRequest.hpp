// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SERVICE_DEBUG_SERVICE_REQUEST_INCLUDED
#define CMAJOR_SERVICE_DEBUG_SERVICE_REQUEST_INCLUDED
#include <cmajor/cmsvc/Request.hpp>
#include <cmajor/cmsvc/Breakpoint.hpp>
#include <cmajor/cmsvc/DebugService.hpp>

namespace cmajor { namespace service {

class CMSVC_API StartDebugServiceRequest : public Request
{
public:
    StartDebugServiceRequest(const DebugServiceStartParams& startParams_, const std::vector<Breakpoint*>& breakpoints_);
    void Execute() override;
    std::string Name() const override { return "startDebugServiceRequest"; }
private:
    DebugServiceStartParams startParams;
    std::vector<Breakpoint*> breakpoints;
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

class CMSVC_API BreakDebugServiceRequest : public Request
{
public:
    BreakDebugServiceRequest(Breakpoint* breakpoint_);
    void Execute() override;
    std::string Name() const override { return "breakDebugServiceRequest"; }
private:
    Breakpoint* breakpoint;
};

class CMSVC_API DeleteDebugServiceRequest : public Request
{
public:
    DeleteDebugServiceRequest(const std::string& breakpointId_);
    void Execute() override;
    std::string Name() const override { return "deleteDebugServiceRequest"; }
private:
    std::string breakpointId;
};

class CMSVC_API DepthDebugServiceRequest : public Request
{
public:
    DepthDebugServiceRequest();
    void Execute() override;
    std::string Name() const override { return "depthDebugServiceRequest"; }
};

class CMSVC_API FramesDebugServiceRequest : public Request
{
public:
    FramesDebugServiceRequest(int lowFrame_, int highFrame_);
    void Execute() override;
    std::string Name() const override { return "framesDebugServiceRequest"; }
private:
    int lowFrame;
    int highFrame;
};

class CMSVC_API EvaluateDebugServiceRequest : public Request
{
public:
    EvaluateDebugServiceRequest(const std::string& expression_, int requestId_);
    void Execute() override;
    std::string Name() const override { return "evaluateDebugServiceRequest"; }
private:
    std::string expression;
    int requestId;
};

class CMSVC_API CountDebugServiceRequest : public Request
{
public:
    CountDebugServiceRequest(const std::string& expression_);
    void Execute() override;
    std::string Name() const override { return "countDebugServiceRequest"; }
private:
    std::string expression;
};

class CMSVC_API EvaluateChildDebugServiceRequest : public Request
{
public:
    EvaluateChildDebugServiceRequest(const std::string& expression_, int start_, int count_);
    void Execute() override;
    std::string Name() const override { return "evaluateChildDebugServiceRequest"; }
private:
    std::string expression;
    int start;
    int count;
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

#endif // CMAJOR_SERVICE_DEBUG_SERVICE_REQUEST_INCLUDED
