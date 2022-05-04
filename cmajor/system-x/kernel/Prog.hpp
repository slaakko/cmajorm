// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_PROG_INCLUDED
#define CMSX_KERNEL_PROG_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <stdint.h>

namespace cmsx::kernel {

class Process;

const int sxbsPortNumber = 55001;

CMSX_KERNEL_API int32_t Start(Process* process, int64_t progAddr);
CMSX_KERNEL_API void Stop(int32_t prog);
CMSX_KERNEL_API void InitProg();
CMSX_KERNEL_API void DoneProg();

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_PROG_INCLUDED
