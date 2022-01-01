// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmsvc/CodeCompletionServiceRequest.hpp>

namespace cmajor { namespace service {

StartCodeCompletionServiceRequest::StartCodeCompletionServiceRequest(const CodeCompletionServiceStartParams& startParams_) : startParams(startParams_)
{
}

void StartCodeCompletionServiceRequest::Execute()
{
    StartCodeCompletionService(startParams);
}

LoadEditModuleCodeCompletionServiceRequest::LoadEditModuleCodeCompletionServiceRequest(const std::string& projectFilePath_, const std::string& backend_, const std::string& config_) :
    projectFilePath(projectFilePath_), backend(backend_), config(config_)
{
}

void LoadEditModuleCodeCompletionServiceRequest::Execute()
{
    LoadEditModule(projectFilePath, backend, config);
}

ResetEditModuleCacheCodeCompletionServiceRequest::ResetEditModuleCacheCodeCompletionServiceRequest()
{
}

void ResetEditModuleCacheCodeCompletionServiceRequest::Execute()
{
    ResetEditModuleCache();
}

ParseSourceCodeCompletionServiceRequest::ParseSourceCodeCompletionServiceRequest(const std::string& projectFilePath_, const std::string& backend_, const std::string& config_, const std::string& sourceFilePath_, std::u32string&& sourceCode_) :
    projectFilePath(projectFilePath_), backend(backend_), config(config_), sourceFilePath(sourceFilePath_), sourceCode(std::move(sourceCode_))
{
}

void ParseSourceCodeCompletionServiceRequest::Execute()
{
    ParseSource(projectFilePath, backend, config, sourceFilePath, std::move(sourceCode));
}

GetCCListCodeCompletionServiceRequest::GetCCListCodeCompletionServiceRequest(const std::string& projectFilePath_, const std::string& backend_, const std::string& config_, const std::string& sourceFilePath_, 
    const std::u32string& ccText_, const std::vector<int>& ruleContext_, const std::u32string& cursorLine_) : 
    projectFilePath(projectFilePath_), backend(backend_), config(config_), sourceFilePath(sourceFilePath_), ccText(ccText_), ruleContext(ruleContext_), cursorLine(cursorLine_)
{
}

void GetCCListCodeCompletionServiceRequest::Execute()
{
    GetCCList(projectFilePath, backend, config, sourceFilePath, ccText, ruleContext, cursorLine);
}

GetParamHelpListCodeCompletionServiceRequest::GetParamHelpListCodeCompletionServiceRequest(const std::string& projectFilePath_, const std::string& backend_, const std::string& config_, 
    const std::string& sourceFilePath_, int symbolIndex_) :
    projectFilePath(projectFilePath_), backend(backend_), config(config_), sourceFilePath(sourceFilePath_), symbolIndex(symbolIndex_)
{
}

void GetParamHelpListCodeCompletionServiceRequest::Execute()
{
    GetParamHelpList(projectFilePath, backend, config, sourceFilePath, symbolIndex);
}

} } // namespace cmajor::service
