// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMPM_PORT_MAP_SERVER_INCLUDED
#define CMAJOR_CMPM_PORT_MAP_SERVER_INCLUDED
#include <cmajor/cmpm/Api.hpp>

namespace cmajor { namespace cmpm {

const int leaseExpirationTimeSecs = 120;
const int leaseRenewalTimeSecs = 60;

CMPM_API void InitPortMapServer();
CMPM_API void DonePortMapServer();
CMPM_API void RunPortMapServer();

} } // namespace cmajor::cmpm

#endif // CMAJOR_CMPM_PORT_MAP_SERVER_INCLUDED
