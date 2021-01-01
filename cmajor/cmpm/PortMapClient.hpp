// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMPM_PORT_MAP_CLIENT_INCLUDED
#define CMAJOR_CMPM_PORT_MAP_CLIENT_INCLUDED
#include <cmajor/cmpm/Api.hpp>
#include <vector>
#include <string>

namespace cmajor { namespace cmpm {

CMPM_API void InitPortMapClient();
CMPM_API void DonePortMapClient();
CMPM_API void StartPortMapClient(int portMapServicePort, const std::vector<int>& portNumbers, const std::string& programName, int pid);
CMPM_API void StopPortMapClient();

} } // namespace cmajor::cmpm

#endif // CMAJOR_CMPM_PORT_MAP_CLIENT_INCLUDED
