// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsvc/DebugServiceRequest.hpp>

namespace cmajor { namespace service {

StartDebugServiceRequest::StartDebugServiceRequest(const DebugServiceStartParams& startParams_, const std::vector<Breakpoint*>& breakpoints_) : startParams(startParams_), breakpoints(breakpoints_)
{
}

void StartDebugServiceRequest::Execute()
{
    StartDebugService(startParams, breakpoints);
}

StopDebugServiceRequest::StopDebugServiceRequest()
{
}

void StopDebugServiceRequest::Execute()
{
    StopDebugService();
}

ContinueDebugServiceRequest::ContinueDebugServiceRequest() 
{
}

void ContinueDebugServiceRequest::Execute()
{
    Continue();
}

NextDebugServiceRequest::NextDebugServiceRequest()
{
}

void NextDebugServiceRequest::Execute()
{
    Next();
}

StepDebugServiceRequest::StepDebugServiceRequest()
{
}

void StepDebugServiceRequest::Execute()
{
    Step();
}

FinishDebugServiceRequest::FinishDebugServiceRequest()
{
}

void FinishDebugServiceRequest::Execute()
{
    Finish();
}

UntilDebugServiceRequest::UntilDebugServiceRequest(const SourceLoc& sourceLocation_) : sourceLocation(sourceLocation_)
{
}

void UntilDebugServiceRequest::Execute() 
{
    Until(sourceLocation);
}

BreakDebugServiceRequest::BreakDebugServiceRequest(Breakpoint* breakpoint_) : breakpoint(breakpoint_)
{
}

void BreakDebugServiceRequest::Execute()
{
    Break(breakpoint);
}

DeleteDebugServiceRequest::DeleteDebugServiceRequest(const std::string& breakpointId_) : breakpointId(breakpointId_)
{
}

void DeleteDebugServiceRequest::Execute()
{
    Delete(breakpointId);
}

DepthDebugServiceRequest::DepthDebugServiceRequest()
{
}

void DepthDebugServiceRequest::Execute()
{
    Depth();
}

FramesDebugServiceRequest::FramesDebugServiceRequest(int lowFrame_, int highFrame_) : lowFrame(lowFrame_), highFrame(highFrame_)
{
}

void FramesDebugServiceRequest::Execute()
{
    Frames(lowFrame, highFrame);
}

EvaluateDebugServiceRequest::EvaluateDebugServiceRequest(const std::string& expression_, int requestId_) : expression(expression_), requestId(requestId_)
{
}

void EvaluateDebugServiceRequest::Execute()
{
    Evaluate(expression, requestId);
}

CountDebugServiceRequest::CountDebugServiceRequest(const std::string& expression_) : expression(expression_)
{
}

void CountDebugServiceRequest::Execute()
{
    Count(expression);
}

EvaluateChildDebugServiceRequest::EvaluateChildDebugServiceRequest(const std::string& expression_, int start_, int count_) : expression(expression_), start(start_), count(count_)
{
}

void EvaluateChildDebugServiceRequest::Execute() 
{
    EvaluateChild(expression, start, count);
}

SetTargetInputEofRequest::SetTargetInputEofRequest()
{
}

void SetTargetInputEofRequest::Execute()
{
    SetTargetInputEof();
}

PutTargetInputLineRequest::PutTargetInputLineRequest(const std::string& targetInputLine_) : targetInputLine(targetInputLine_)
{
}

void PutTargetInputLineRequest::Execute()
{
    PutTargetInputLine(targetInputLine);
}

} } // namespace cmajor::service
