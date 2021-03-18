// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SERVICE_PORT_MAP_SERVICE_INCLUDED
#define CMAJOR_SERVICE_PORT_MAP_SERVICE_INCLUDED
#include <cmajor/cmsvc/ServiceApi.hpp>
#include <string>

namespace cmajor { namespace service {

CMSVC_API int GetPortMapServicePortNumberFromConfig();
CMSVC_API bool StartPortMapServer();
CMSVC_API int GetPortMapServicePortNumber(bool& portMapServerStarted);
CMSVC_API int GetFreePortNumber(const std::string& processName);

} } // namespace cmajor::service

#endif // CMAJOR_SERVICE_PORT_MAP_SERVICE_INCLUDED
