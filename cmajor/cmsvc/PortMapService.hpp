// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SERVICE_PORT_MAP_SERVICE_INCLUDED
#define CMAJOR_SERVICE_PORT_MAP_SERVICE_INCLUDED
#include <cmajor/cmsvc/ServiceApi.hpp>
#include <cmajor/cmpm/PortMapMessage.hpp>

namespace cmajor { namespace service {

CMSVC_API int GetPortMapServicePortNumberFromConfig();
CMSVC_API bool StartPortMapServer();
CMSVC_API bool StopPortMapServer();
CMSVC_API int GetPortMapServicePortNumber(bool& portMapServerStarted);
CMSVC_API int GetFreePortNumber(const std::string& processName);
CMSVC_API bool IsPortMapServerRunning();
CMSVC_API std::vector<PortLease> GetPortLeases();

} } // namespace cmajor::service

#endif // CMAJOR_SERVICE_PORT_MAP_SERVICE_INCLUDED
