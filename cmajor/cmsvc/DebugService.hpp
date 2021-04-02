// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SERVICE_DEBUG_SERVICE_INCLUDED
#define CMAJOR_SERVICE_DEBUG_SERVICE_INCLUDED
#include <cmajor/cmsvc/Message.hpp>
#include <cmajor/cmsvc/Breakpoint.hpp>
#include <cmajor/cmmsg/CmdbMessage.hpp>

namespace cmajor { namespace service {

struct CMSVC_API DebugServiceStartParams
{
    DebugServiceStartParams();
    DebugServiceStartParams& ProcessName(const std::string& processName_);
    DebugServiceStartParams& Pid(int pid_);
    DebugServiceStartParams& Backend(const std::string& backend_);
    DebugServiceStartParams& ToolChain(const std::string& toolChain_);
    DebugServiceStartParams& Config(const std::string& config_);
    DebugServiceStartParams& ProjectFilePath(const std::string& projectFilePath_);
    DebugServiceStartParams& ExecutableName(const std::string& executableName_);
    DebugServiceStartParams& ProgramArguments(const std::string& programArguments_);
    DebugServiceStartParams& DebugServer(bool debugServer_);
    DebugServiceStartParams& Log(bool log_);
    DebugServiceStartParams& Wait(bool wait_);
    DebugServiceStartParams& Verbose(bool verbose_);
    std::string processName;
    int pid;
    std::string backend;
    std::string toolChain;
    std::string config;
    std::string projectFilePath;
    std::string executableName;
    std::string programArguments;
    bool debugServer;
    bool log;
    bool wait;
    bool verbose;
};

class CMSVC_API DebugServiceRequest
{
public:
    virtual ~DebugServiceRequest();
    virtual void Execute() = 0;
    virtual std::string Name() const = 0;
    virtual void Failed(const std::string& error) = 0;
};

class CMSVC_API RunStartDebugServiceRequest : public DebugServiceRequest
{
public:
    RunStartDebugServiceRequest();
    void Execute() override;
    std::string Name() const override;
    void Failed(const std::string& error) override;
};

class CMSVC_API StartReplyServiceMessage : public ServiceMessage
{
public:
    StartReplyServiceMessage(const StartDebugReply& startReply_);
    const StartDebugReply& GetStartDebugReply() const { return startReply; }
private:
    StartDebugReply startReply;
};

class CMSVC_API StartErrorServiceMessage : public ServiceMessage
{
public:
    StartErrorServiceMessage(const std::string& error_);
    const std::string& Error() const { return error; }
private:
    std::string error;
};

class CMSVC_API RunStopDebugServiceRequest : public DebugServiceRequest
{
public:
    RunStopDebugServiceRequest();
    void Execute() override;
    std::string Name() const override;
    void Failed(const std::string& error) override;
};

class CMSVC_API TargetRunningServiceMessage : public ServiceMessage
{
public:
    TargetRunningServiceMessage();
};

class CMSVC_API TargetInputServiceMessage : public ServiceMessage
{
public:
    TargetInputServiceMessage();
};

class CMSVC_API TargetOutputServiceMessage : public ServiceMessage
{
public:
    TargetOutputServiceMessage(const TargetOutputRequest& targetOutputRequest_);
    const TargetOutputRequest& GetTargetOutputRequest() const { return targetOutputRequest; }
private:
    TargetOutputRequest targetOutputRequest;
};

class CMSVC_API RunContinueDebugServiceRequest : public DebugServiceRequest
{
public:
    RunContinueDebugServiceRequest();
    void Execute() override;
    std::string Name() const override;
    void Failed(const std::string& error) override;
};

class CMSVC_API ContinueReplyServiceMessage : public ServiceMessage
{
public:
    ContinueReplyServiceMessage(const ContinueReply& continueReply_);
    const ContinueReply& GetContinueReply() const { return continueReply; }
private:
    ContinueReply continueReply;
};

class CMSVC_API RunNextDebugServiceRequest : public DebugServiceRequest
{
public:
    RunNextDebugServiceRequest();
    void Execute() override;
    std::string Name() const override;
    void Failed(const std::string& error) override;
};

class CMSVC_API NextReplyServiceMessage : public ServiceMessage
{
public:
    NextReplyServiceMessage(const NextReply& nextReply_);
    const NextReply& GetNextReply() const { return nextReply; }
private:
    NextReply nextReply;
};

class CMSVC_API RunStepDebugServiceRequest : public DebugServiceRequest
{
public:
    RunStepDebugServiceRequest();
    void Execute() override;
    std::string Name() const override;
    void Failed(const std::string& error) override;
};

class CMSVC_API StepReplyServiceMessage : public ServiceMessage
{
public:
    StepReplyServiceMessage(const StepReply& stepReply_);
    const StepReply& GetStepReply() const { return stepReply; }
private:
    StepReply stepReply;
};

class CMSVC_API RunFinishDebugServiceRequest : public DebugServiceRequest
{
public:
    RunFinishDebugServiceRequest();
    void Execute() override;
    std::string Name() const override;
    void Failed(const std::string& error) override;
};

class CMSVC_API FinishReplyServiceMessage : public ServiceMessage
{
public:
    FinishReplyServiceMessage(const FinishReply& finishReply_);
    const FinishReply& GetFinishReply() const { return finishReply; }
private:
    FinishReply finishReply;
};

class CMSVC_API RunUntilDebugServiceRequest : public DebugServiceRequest
{
public:
    RunUntilDebugServiceRequest(const SourceLoc& sourceLocation_);
    void Execute() override;
    std::string Name() const override;
    void Failed(const std::string& error) override;
private:
    SourceLoc sourceLocation;
};

class CMSVC_API UntilReplyServiceMessage : public ServiceMessage
{
public:
    UntilReplyServiceMessage(const UntilReply& untilReply_);
    const UntilReply& GetUntilReply() const { return untilReply; }
private:
    UntilReply untilReply;
};

class CMSVC_API RunBreakDebugServiceRequest : public DebugServiceRequest
{
public:
    RunBreakDebugServiceRequest(Breakpoint* breakpoint_);
    void Execute() override;
    std::string Name() const override;
    void Failed(const std::string& error) override;
private:
    Breakpoint* breakpoint;
};

class CMSVC_API BreakReplyServiceMessage : public ServiceMessage
{
public:
    BreakReplyServiceMessage(const BreakReply& breakReply_);
    const BreakReply& GetBreakReply() const { return breakReply; }
private:
    BreakReply breakReply;
};

class CMSVC_API RunDeleteDebugServiceRequest : public DebugServiceRequest
{
public:
    RunDeleteDebugServiceRequest(const std::string& breakpointId_);
    void Execute() override;
    std::string Name() const override;
    void Failed(const std::string& error) override;
private:
    std::string breakpointId;
};

class CMSVC_API DeleteReplyServiceMessage : public ServiceMessage
{
public:
    DeleteReplyServiceMessage(const DeleteReply& deleteReply_);
    const DeleteReply& GetDeleteReply() const { return deleteReply; }
private:
    DeleteReply deleteReply;
};

class CMSVC_API RunDepthDebugServiceRequest : public DebugServiceRequest
{
public:
    RunDepthDebugServiceRequest();
    void Execute() override;
    std::string Name() const override;
    void Failed(const std::string& error) override;
};

class CMSVC_API DepthReplyServiceMessage : public ServiceMessage
{
public:
    DepthReplyServiceMessage(const DepthReply& depthReply_);
    const DepthReply& GetDepthReply() const { return depthReply; }
private:
    DepthReply depthReply;
};

class CMSVC_API RunFramesDebugServiceRequest : public DebugServiceRequest
{
public:
    RunFramesDebugServiceRequest(int lowFrame_, int highFrame_);
    void Execute() override;
    std::string Name() const override;
    void Failed(const std::string& error) override;
private:
    int lowFrame;
    int highFrame;
};

class CMSVC_API FramesReplyServiceMessage : public ServiceMessage
{
public:
    FramesReplyServiceMessage(const FramesReply& framesReply_);
    const FramesReply& GetFramesReply() const { return framesReply; }
private:
    FramesReply framesReply;
};

class CMSVC_API DebugServiceStoppedServiceMessage : public ServiceMessage
{
public:
    DebugServiceStoppedServiceMessage();
};

CMSVC_API void InitDebugService();
CMSVC_API void DoneDebugService();
CMSVC_API void StartDebugService(DebugServiceStartParams& startParams, const std::vector<Breakpoint*>& breakpoints);
CMSVC_API void StopDebugService();
CMSVC_API void Continue();
CMSVC_API void Next();
CMSVC_API void Step();
CMSVC_API void Finish();
CMSVC_API void Until(const SourceLoc& sourceLocation);
CMSVC_API void SetTargetInputEof();
CMSVC_API void PutTargetInputLine(const std::string& targetInputLine);
CMSVC_API bool DebugRequestInProgress(std::string& requestName);
CMSVC_API void Break(Breakpoint* breakpoint);
CMSVC_API void Delete(const std::string& breakpointId);
CMSVC_API void Depth();
CMSVC_API void Frames(int lowFrame, int highFrame);

} } // namespace cmajor::service

#endif // CMAJOR_SERVICE_DEBUG_SERVICE_INCLUDED
