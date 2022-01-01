// =================================
// Copyright (c) 2022 Seppo Laakko
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

class CMSVC_API RunLoadEditModuleServiceRequest : public CodeCompletionServiceRequest
{
public:
    RunLoadEditModuleServiceRequest(const std::string& projectFilePath_, const std::string& backend_, const std::string& config_);
    void Execute() override;
    std::string Name() const override { return "runLoadEditModuleServiceRequest"; }
    void Failed(const std::string& error) override;
private:
    std::string projectFilePath;
    std::string backend;
    std::string config;
};

class CMSVC_API LoadEditModuleReplyServiceMessage : public ServiceMessage
{
public:
    LoadEditModuleReplyServiceMessage(const LoadEditModuleReply& reply_);
    const LoadEditModuleReply& Reply() const { return reply; }
private:
    LoadEditModuleReply reply;
};

class CMSVC_API LoadEditModuleErrorServiceMessage : public ServiceMessage
{
public:
    LoadEditModuleErrorServiceMessage(const std::string& error_);
    const std::string& Error() const { return error; }
private:
    std::string error;
};

class CMSVC_API RunResetEditModuleCacheServiceRequest : public CodeCompletionServiceRequest
{
public:
    void Execute() override;
    std::string Name() const override { return "runResetEditModuleCacheServiceRequest"; }
    void Failed(const std::string& error) override;
};

class CMSVC_API ResetEditModuleCacheReplyServiceMessage : public ServiceMessage
{
public:
    ResetEditModuleCacheReplyServiceMessage(const ResetEditModuleCacheReply& reply_);
    const ResetEditModuleCacheReply& Reply() const { return reply; }
private:
    ResetEditModuleCacheReply reply;
};

class CMSVC_API ResetEditModuleCacheErrorServiceMessage : public ServiceMessage
{
public:
    ResetEditModuleCacheErrorServiceMessage(const std::string& error_);
    const std::string& Error() const { return error; }
private:
    std::string error;
};

class CMSVC_API RunParseSourceServiceRequest : public CodeCompletionServiceRequest
{
public:
    RunParseSourceServiceRequest(const std::string& projectFilePath_, const std::string& backend_, const std::string& config_, const std::string& sourceFilePath_, std::u32string&& sourceCode_);
    void Execute() override;
    std::string Name() const override { return "runParseSourceServiceRequest"; }
    void Failed(const std::string& error) override;
private:
    std::string projectFilePath;
    std::string backend;
    std::string config;
    std::string sourceFilePath;
    std::u32string sourceCode;
};

class CMSVC_API ParseSourceReplyServiceMessage : public ServiceMessage
{
public:
    ParseSourceReplyServiceMessage(const ParseSourceReply& reply_);
    const ParseSourceReply& Reply() const { return reply; }
private:
    ParseSourceReply reply;
};

class CMSVC_API ParseSourceErrorServiceMessage : public ServiceMessage
{
public:
    ParseSourceErrorServiceMessage(const std::string& error_);
    const std::string& Error() const { return error; }
private:
    std::string error;
};

class CMSVC_API RunGetCCListServiceRequest : public CodeCompletionServiceRequest
{
public:
    RunGetCCListServiceRequest(const std::string& projectFilePath_, const std::string& backend_, const std::string& config_, const std::string& sourceFilePath_, const std::u32string& ccText_, 
        const std::vector<int>& ruleContext, const std::u32string& cursorLine_);
    void Execute() override;
    std::string Name() const override { return "runGetCCListServiceRequest"; }
    void Failed(const std::string& error) override;
private:
    std::string projectFilePath;
    std::string backend;
    std::string config;
    std::string sourceFilePath;
    std::u32string ccText;
    std::vector<int> ruleContext;
    std::u32string cursorLine;
};

class CMSVC_API GetCCListReplyServiceMessage : public ServiceMessage
{
public:
    GetCCListReplyServiceMessage(const GetCCListReply& reply_);
    const GetCCListReply& Reply() const { return reply; }
private:
    GetCCListReply reply;
};

class CMSVC_API GetCCListErrorServiceMessage : public ServiceMessage
{
public:
    GetCCListErrorServiceMessage(const std::string& error_);
    const std::string& Error() const { return error; }
private:
    std::string error;
};

class CMSVC_API RunGetParamHelpListServiceRequest : public CodeCompletionServiceRequest
{
public:
    RunGetParamHelpListServiceRequest(const std::string& projectFilePath_, const std::string& backend_, const std::string& config_, const std::string& sourceFilePath_, int symbolIndex_);
    void Execute() override;
    std::string Name() const override { return "runGetParamHelpListServiceRequest"; }
    void Failed(const std::string& error) override;
private:
    std::string projectFilePath;
    std::string backend;
    std::string config;
    std::string sourceFilePath;
    int symbolIndex;
};

class CMSVC_API GetParamHelpListReplyServiceMessage : public ServiceMessage
{
public:
    GetParamHelpListReplyServiceMessage(const GetParamHelpListReply& reply_);
    const GetParamHelpListReply& Reply() const { return reply; }
private:
    GetParamHelpListReply reply;
};

class CMSVC_API GetParamHelpListErrorServiceMessage : public ServiceMessage
{
public:
    GetParamHelpListErrorServiceMessage(const std::string& error_);
    const std::string& Error() const { return error; }
private:
    std::string error;
};

CMSVC_API void StartCodeCompletionService(CodeCompletionServiceStartParams& startParams_);
CMSVC_API void StopCodeCompletionService(bool log);
CMSVC_API bool CodeCompletionServiceRunning();
CMSVC_API void LoadEditModule(const std::string& projectFilePath, const std::string& backend, const std::string& config);
CMSVC_API void ResetEditModuleCache();
CMSVC_API void ParseSource(const std::string& projectFilePath, const std::string& backend, const std::string& config, const std::string& sourceFilePath, std::u32string&& sourceCode);
CMSVC_API void GetCCList(const std::string& projectFilePath, const std::string& backend, const std::string& config, const std::string& sourceFilePath, const std::u32string& ccText, const std::vector<int>& ruleContext,
    const std::u32string& cursorLine);
CMSVC_API void GetParamHelpList(const std::string& projectFilePath, const std::string& backend, const std::string& config, const std::string& sourceFilePath, int symbolIndex);
CMSVC_API void InitCodeCompletionService();
CMSVC_API void DoneCodeCompletionService();

} } // namespace cmajor::service

#endif // CMAJOR_SERVICE_CODE_COMPLETION_SERVICE_INCLUDED
