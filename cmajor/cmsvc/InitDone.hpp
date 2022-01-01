// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SERVICE_INIT_DONE_INCLUDED
#define CMAJOR_SERVICE_INIT_DONE_INCLUDED
#include <cmajor/cmsvc/ServiceApi.hpp>

namespace cmajor { namespace service {

CMSVC_API void Init();
CMSVC_API void Done();

} } // namespace cmajor::service

#endif // CMAJOR_SERVICE_INIT_DONE_INCLUDED
