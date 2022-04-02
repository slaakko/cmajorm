// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_SOCKET_INCLUDED
#define CMSX_KERNEL_SOCKET_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <stdint.h>

namespace cmsx::kernel {

class Process;

CMSX_KERNEL_API int32_t Connect(Process* process, int64_t nodeAddr, int64_t serviceAddr);

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_SOCKET_INCLUDED
