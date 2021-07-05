// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmcode/CodeCompletion.hpp>
#include <cmajor/cmsvc/CodeCompletionServiceRequest.hpp>
#include <cmajor/cmcode/Config.hpp>

namespace cmcode {

using namespace cmajor::service;

CodeCompletionServiceStartParams MakeCodeCompletionServiceStartParams(int pid)
{
    CodeCompletionServiceStartParams startParams;
    return startParams.ProcessName("cmcode").Pid(pid).DebugServer(UseDebugServers()).Wait(DebugWait());
}

void StartCodeCompletionService(int pid)
{
    CodeCompletionServiceStartParams startParams = MakeCodeCompletionServiceStartParams(pid);
    PutRequest(new StartCodeCompletionServiceRequest(startParams));
}

void StopCodeCompletionService(bool log)
{
    if (cmajor::service::CodeCompletionServiceRunning())
    {
        cmajor::service::StopCodeCompletionService(log);
    }
}

} // namespace cmcode
