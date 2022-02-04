// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_HEAP_INCLUDED
#define CMSX_KERNEL_HEAP_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <system-x/machine/Processor.hpp>
#include <stdint.h>

namespace cmsx::kernel {

class Process;

CMSX_KERNEL_API void DumpHeap(cmsx::machine::Processor& processor, uint64_t freeAddr, int32_t tag, uint64_t ptr, uint64_t size);
CMSX_KERNEL_API void MCpy(cmsx::kernel::Process* process, uint64_t sourceBufferAddr, uint64_t targetBufferAddr, uint64_t count);

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_HEAP_INCLUDED
