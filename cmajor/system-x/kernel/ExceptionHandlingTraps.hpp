// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_EXCEPTION_HANDLING_TRAPS_INCLUDED
#define CMSX_KERNEL_EXCEPTION_HANDLING_TRAPS_INCLUDED
#include <system-x/kernel/Api.hpp>

namespace cmsx::kernel {

CMSX_KERNEL_API void InitExceptionHandlingTraps();
CMSX_KERNEL_API void DoneExceptionHandlingTraps();

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_EXCEPTION_HANDLING_TRAPS_INCLUDED
