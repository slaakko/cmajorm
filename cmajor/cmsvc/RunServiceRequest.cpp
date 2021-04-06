// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsvc/RunServiceRequest.hpp>
#include <cmajor/cmsvc/RunService.hpp>

namespace cmajor { namespace service {

StartProgramRequest::StartProgramRequest(const std::string& executableName_, const std::string& programArguments_, const std::string& processName_) : 
    executableName(executableName_), programArguments(programArguments_), processName(processName_)
{
}

void StartProgramRequest::Execute()
{
    StartProgram(executableName, programArguments, processName);
}

std::string StartProgramRequest::Name() const
{
    return "startProgramRequest";
}

PutProgramInputLineRequest::PutProgramInputLineRequest(const std::string& inputLine_) : inputLine(inputLine_)
{
}

void PutProgramInputLineRequest::Execute()
{
    PutProgramTargetInputLine(inputLine);
}

std::string PutProgramInputLineRequest::Name() const
{
    return "putProgramInputLineRequest";
}

SetProgramEofRequest::SetProgramEofRequest()
{
}

void SetProgramEofRequest::Execute()
{
    SetProgramTargetInputEof();
}

std::string SetProgramEofRequest::Name() const
{
    return "setProgramEofRequest";
}

TerminateProcessRequest::TerminateProcessRequest()
{
}

void TerminateProcessRequest::Execute()
{
    TerminateProcess();
}

std::string TerminateProcessRequest::Name() const
{
    return "terminateProcessRequest";
}

StopRunServiceRequest::StopRunServiceRequest()
{
}

void StopRunServiceRequest::Execute()
{
    StopRunService();
}

std::string StopRunServiceRequest::Name() const
{
    return "stopRunServiceRequest";
}

} } // namespace cmajor::service
