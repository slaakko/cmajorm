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

} } // namespace cmajor::service

#endif // CMAJOR_SERVICE_CODE_COMPLETION_SERVICE_REQUEST_INCLUDED
