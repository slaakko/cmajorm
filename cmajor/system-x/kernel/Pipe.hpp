// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_PIPE_INCLUDED
#define CMSX_KERNEL_PIPE_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <stdint.h>

namespace cmsx::kernel {

class Process;

CMSX_KERNEL_API void MakePipe(cmsx::kernel::Process* process, int64_t readerFdAddr, int64_t writerFdAddr);
CMSX_KERNEL_API void InitPipe();
CMSX_KERNEL_API void DonePipe();

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_PIPE_INCLUDED
