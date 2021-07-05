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

CMSVC_API void StartCodeCompletionService(CodeCompletionServiceStartParams& startParams_);
CMSVC_API void StopCodeCompletionService(bool log);
CMSVC_API bool CodeCompletionServiceRunning();
CMSVC_API void LoadEditModule(const std::string& projectFilePath, const std::string& backend, const std::string& config);
CMSVC_API void InitCodeCompletionService();
CMSVC_API void DoneCodeCompletionService();

} } // namespace cmajor::service

#endif // CMAJOR_SERVICE_CODE_COMPLETION_SERVICE_INCLUDED
