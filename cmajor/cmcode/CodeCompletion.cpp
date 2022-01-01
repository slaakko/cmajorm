// =================================
// Copyright (c) 2022 Seppo Laakko
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

void LoadEditModule(const std::string& projectFilePath, const std::string& backend, const std::string& config)
{
    PutRequest(new LoadEditModuleCodeCompletionServiceRequest(projectFilePath, backend, config));
}

void ResetEditModuleCache()
{
    PutRequest(new ResetEditModuleCacheCodeCompletionServiceRequest());
}

void ParseSource(const std::string& projectFilePath, const std::string& backend, const std::string& config, const std::string& sourceFilePath, std::u32string&& sourceCode)
{
    PutRequest(new ParseSourceCodeCompletionServiceRequest(projectFilePath, backend, config, sourceFilePath, std::move(sourceCode)));
}

void GetCCList(const std::string& projectFilePath, const std::string& backend, const std::string& config, const std::string& sourceFilePath, const std::u32string& ccText, const std::vector<int>& ruleContext, 
    const std::u32string& cursorLine)
{
    PutRequest(new GetCCListCodeCompletionServiceRequest(projectFilePath, backend, config, sourceFilePath, ccText, ruleContext, cursorLine));
}

void GetParamHelpList(const std::string& projectFilePath, const std::string& backend, const std::string& config, const std::string& sourceFilePath, int symbolIndex)
{
    PutRequest(new GetParamHelpListCodeCompletionServiceRequest(projectFilePath, backend, config, sourceFilePath, symbolIndex));
}

} // namespace cmcode
