// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SERVICE_CODE_COMPLETION_SERVICE_REQUEST_INCLUDED
#define CMAJOR_SERVICE_CODE_COMPLETION_SERVICE_REQUEST_INCLUDED
#include <cmajor/cmsvc/Request.hpp>
#include <cmajor/cmsvc/CodeCompletionService.hpp>

namespace cmajor { namespace service {

class CMSVC_API StartCodeCompletionServiceRequest : public Request
{
public:
    StartCodeCompletionServiceRequest(const CodeCompletionServiceStartParams& startParams_);
    void Execute() override;
    std::string Name() const override { return "startCodeCompletionServiceRequest"; }
private:
    CodeCompletionServiceStartParams startParams;
};

class CMSVC_API LoadEditModuleCodeCompletionServiceRequest : public Request
{
public:
    LoadEditModuleCodeCompletionServiceRequest(const std::string& projectFilePath_, const std::string& backend_, const std::string& config_);
    void Execute() override;
    std::string Name() const override { return "loadEditModuleCodeCompletionServiceRequest"; }
private:
    std::string projectFilePath;
    std::string backend;
    std::string config;
};

class CMSVC_API ResetEditModuleCacheCodeCompletionServiceRequest : public Request
{
public:
    ResetEditModuleCacheCodeCompletionServiceRequest();
    void Execute() override;
    std::string Name() const override { return "resetEditModuleCacheCodeCompletionServiceRequest"; }
};

class CMSVC_API ParseSourceCodeCompletionServiceRequest : public Request
{
public:
    ParseSourceCodeCompletionServiceRequest(const std::string& projectFilePath_, const std::string& backend_, const std::string& config_, const std::string& sourceFilePath_, std::u32string&& sourceCode_);
    void Execute() override;
    std::string Name() const override { return "parseSourceCodeCompletionServiceRequest"; }
private:
    std::string projectFilePath;
    std::string backend;
    std::string config;
    std::string sourceFilePath;
    std::u32string sourceCode;
};

class CMSVC_API GetCCListCodeCompletionServiceRequest : public Request
{
public:
    GetCCListCodeCompletionServiceRequest(const std::string& projectFilePath_, const std::string& backend_, const std::string& config_, const std::string& sourceFilePath_, const std::u32string& ccText_, 
        const std::vector<int>& ruleContext_, const std::u32string& cursorLine_);
    void Execute() override;
    std::string Name() const override { return "getCCListCodeCompletionServiceRequest"; }
private:
    std::string projectFilePath;
    std::string backend;
    std::string config;
    std::string sourceFilePath;
    std::u32string ccText;
    std::vector<int> ruleContext;
    std::u32string cursorLine;
};

class CMSVC_API GetParamHelpListCodeCompletionServiceRequest : public Request
{
public:
    GetParamHelpListCodeCompletionServiceRequest(const std::string& projectFilePath_, const std::string& backend_, const std::string& config_, const std::string& sourceFilePath_, int symbolIndex_);
    void Execute() override;
    std::string Name() const override { return "getParamHelpListCodeCompletionServiceRequest"; }
private:
    std::string projectFilePath;
    std::string backend;
    std::string config;
    std::string sourceFilePath;
    int symbolIndex;
};

} } // namespace cmajor::service

#endif // CMAJOR_SERVICE_CODE_COMPLETION_SERVICE_REQUEST_INCLUDED
