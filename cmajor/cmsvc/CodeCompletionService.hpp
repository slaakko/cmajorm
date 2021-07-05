// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SERVICE_CODE_COMPLETION_SERVICE_INCLUDED
#define CMAJOR_SERVICE_CODE_COMPLETION_SERVICE_INCLUDED
#include <cmajor/cmsvc/Message.hpp>
#include <cmajor/cmmsg/CodeCompletionServerMessage.hpp>

namespace cmajor { namespace service {

struct CMSVC_API CodeCompletionServiceStartParams
{
    CodeCompletionServiceStartParams();
    CodeCompletionServiceStartParams& ProcessName(const std::string& processName_);
    CodeCompletionServiceStartParams& Pid(int pid_);
    CodeCompletionServiceStartParams& DebugServer(bool debugServer_);
    CodeCompletionServiceStartParams& Wait(bool wait_);
    std::string processName;
    int pid;
    bool debugServer;
    bool wait;
};

class CMSVC_API CodeCompletionServiceRequest
{
public:
    virtual ~CodeCompletionServiceRequest();
    virtual void Execute() = 0;
    virtual std::string Name() const = 0;
    virtual void Failed(const std::string& error) = 0;
};

CMSVC_API void StartCodeCompletionService(CodeCompletionServiceStartParams& startParams_);
CMSVC_API void StopCodeCompletionService(bool log);
CMSVC_API bool CodeCompletionServiceRunning();
CMSVC_API void InitCodeCompletionService();
CMSVC_API void DoneCodeCompletionService();

} } // namespace cmajor::service

#endif // CMAJOR_SERVICE_CODE_COMPLETION_SERVICE_INCLUDED
