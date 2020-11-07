// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMPM_PORT_MAP_SERVER_INCLUDED
#define CMAJOR_CMPM_PORT_MAP_SERVER_INCLUDED
#include <cmajor/cmpm/Api.hpp>

namespace cmajor { namespace cmpm {

CMPM_API void InitPortMapServer();
CMPM_API void DonePortMapServer();
CMPM_API void RunPortMapServer();

} } // namespace cmajor::cmpm

#endif // CMAJOR_CMPM_PORT_MAP_SERVER_INCLUDED
