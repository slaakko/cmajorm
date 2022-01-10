// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_TRAP_RANDOM_INCLUDED
#define CMSX_KERNEL_TRAP_RANDOM_INCLUDED
#include <system-x/kernel/Api.hpp>

namespace cmsx::kernel {

CMSX_KERNEL_API void InitTrapRandom();
CMSX_KERNEL_API void DoneTrapRandom();

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_TRAP_RANDOM_INCLUDED
