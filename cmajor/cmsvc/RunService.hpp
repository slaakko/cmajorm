// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SERVICE_RUN_SERVICE_INCLUDED
#define CMAJOR_SERVICE_RUN_SERVICE_INCLUDED
#include <cmajor/cmsvc/Message.hpp>

namespace cmajor { namespace service {

class CMSVC_API ProcessTerminatedServiceMessage : public ServiceMessage
{
public:
    ProcessTerminatedServiceMessage();
};

class CMSVC_API RunServiceStoppedServiceMessage : public ServiceMessage
{
public:
    RunServiceStoppedServiceMessage();
};

CMSVC_API void InitRunService();
CMSVC_API void DoneRunService();
CMSVC_API void StartProgram(const std::string& executableName, const std::string& programArguments, const std::string& processName);
CMSVC_API void TerminateProcess();
CMSVC_API void StopRunService();
CMSVC_API void SetProgramTargetInputEof();
CMSVC_API void PutProgramTargetInputLine(const std::string& targetInputLine);


} } // namespace cmajor::service

#endif // CMAJOR_SERVICE_RUN_SERVICE_INCLUDED
