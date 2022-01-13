// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_IO_INCLUDED
#define CMSX_KERNEL_IO_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <stdint.h>

namespace cmsx::kernel {

class Process;

CMSX_KERNEL_API int64_t Write(Process* process, int32_t fd, int64_t bufferAddr, int64_t count);
CMSX_KERNEL_API int64_t Read(Process* process, int32_t fd, int64_t bufferAddr, int64_t count);

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_IO_INCLUDED
