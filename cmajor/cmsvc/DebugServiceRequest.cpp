// =================================
// Copyright (c) 2021 Seppo Laakko
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
