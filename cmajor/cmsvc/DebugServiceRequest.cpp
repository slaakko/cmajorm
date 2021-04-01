// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsvc/DebugServiceRequest.hpp>

namespace cmajor { namespace service {

StartDebugServiceRequest::StartDebugServiceRequest(const DebugServiceStartParams& startParams_, const std::vector<SourceLoc>& breakpoints_) : startParams(startParams_), breakpoints(breakpoints_)
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
