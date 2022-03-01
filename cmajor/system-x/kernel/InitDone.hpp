// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_INIT_DONE_INCLUDED
#define CMSX_KERNEL_INIT_DONE_INCLUDED
#include <system-x/kernel/Api.hpp>

namespace cmsx::kernel {

CMSX_KERNEL_API void Init(bool initTerminal);
CMSX_KERNEL_API void Done();

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_INIT_DONE_INCLUDED
