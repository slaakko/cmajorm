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

CMSX_KERNEL_API void DumpHeap(cmsx::machine::Processor& processor, uint64_t freeAddr, int32_t tag, uint64_t ptr, uint64_t size);

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_HEAP_INCLUDED
