// =================================
// Copyright (c) 2021 Seppo Laakko
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

} } // namespace cmajor::service
