// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_ASSEMBLY_CONFIG_INCLUDED
#define CMSX_INTERMEDIATE_ASSEMBLY_CONFIG_INCLUDED
#include <system-x/intermediate/Api.hpp>

namespace cmsx::intermediate {

CMSX_INTERMEDIATE_API int GetNumLocalRegs();
CMSX_INTERMEDIATE_API void InitAssemblyConfig();
CMSX_INTERMEDIATE_API void DoneAssemblyConfig();

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_ASSEMBLY_CONFIG_INCLUDED
