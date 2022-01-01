// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_MACHINE_CONFIG_INCLUDED
#define CMSX_MACHINE_CONFIG_INCLUDED
#include <system-x/machine/Api.hpp>
#include <string>

namespace cmsx::machine {

CMSX_MACHINE_API int MaxProcs();
CMSX_MACHINE_API std::string ConfigFilePath();
CMSX_MACHINE_API void InitConfig();
CMSX_MACHINE_API void DoneConfig();

} // cmsx::machine

#endif // CMSX_MACHINE_CONFIG_INCLUDED
