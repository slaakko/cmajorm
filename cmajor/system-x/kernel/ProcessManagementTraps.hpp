// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_PROCESS_MANAGEMENT_TRAPS_INCLUDED
#define CMSX_KERNEL_PROCESS_MANAGEMENT_TRAPS_INCLUDED
#include <system-x/kernel/Api.hpp>

namespace cmsx::kernel {

CMSX_KERNEL_API void InitProcessManagementTraps();
CMSX_KERNEL_API void DoneProcessManagementTraps();

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_PROCESS_MANAGEMENT_TRAPS_INCLUDED
