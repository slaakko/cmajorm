// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_TRAP_EXIT_INCLUDED
#define CMSX_KERNEL_TRAP_EXIT_INCLUDED
#include <system-x/kernel/Api.hpp>

namespace cmsx::kernel {

CMSX_KERNEL_API void InitTrapExit();
CMSX_KERNEL_API void DoneTrapExit();

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_TRAP_EXIT_INCLUDED
