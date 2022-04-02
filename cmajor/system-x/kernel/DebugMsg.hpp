// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_DEBUG_MSG_INCLUDED
#define CMSX_KERNEL_DEBUG_MSG_INCLUDED
#include <system-x/kernel/MsgQueue.hpp>
#include <string>

namespace cmsx::kernel {

CMSX_KERNEL_API std::string GetMsgStr(const Msg& msg);

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_DEBUG_MSG_INCLUDED
