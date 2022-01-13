// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_TIME_TRAPS_INCLUDED
#define CMSX_KERNEL_TIME_TRAPS_INCLUDED
#include <system-x/kernel/Api.hpp>

namespace cmsx::kernel {

CMSX_KERNEL_API void InitTimeTraps();
CMSX_KERNEL_API void DoneTimeTraps();

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_TIME_TRAPS_INCLUDED
