// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SERVICE_CONFIG_INCLUDED
#define CMAJOR_SERVICE_CONFIG_INCLUDED
#include <cmajor/cmsvc/ServiceApi.hpp>
#include <string>

namespace cmajor { namespace service {

CMSVC_API std::string CmajorRootDir();
CMSVC_API std::string CmajorConfigDir();
CMSVC_API std::string CmajorLogDir();
CMSVC_API std::string CmajorProjectsDir();
CMSVC_API std::string PortMapConfigFilePath();
CMSVC_API std::string PortMapLogFilePath();

} } // namespace cmajor::service

#endif // CMAJOR_SERVICE_CONFIG_INCLUDED

