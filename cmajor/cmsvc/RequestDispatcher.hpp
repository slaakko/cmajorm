// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SERVICE_REQUEST_DISPATCHER_INCLUDED
#define CMAJOR_SERVICE_REQUEST_DISPATCHER_INCLUDED
#include <cmajor/cmsvc/ServiceApi.hpp>

namespace cmajor { namespace service {

CMSVC_API void StartRequestDispatcher();
CMSVC_API void StopRequestDispatcher();
CMSVC_API void InitRequestDispatcher();
CMSVC_API void DoneRequestDispatcher();

} } // namespace cmajor::service

#endif // CMAJOR_SERVICE_REQUEST_DISPATCHER_INCLUDED
