// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_MSG_QUEUE_INCLUDED
#define CMSX_KERNEL_MSG_QUEUE_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <stdint.h>

class Process;

namespace cmsx::kernel {

CMSX_KERNEL_API int32_t MsgQ(Process* process, int64_t nameAddr);
CMSX_KERNEL_API void CloseMsgQ(Process* process, int32_t md);
CMSX_KERNEL_API void PutMsg(Process* process, int32_t md, int64_t msgDataAddr, int32_t msgSize);
CMSX_KERNEL_API int32_t GetMsgQueueLength(Process* process, int32_t md);
CMSX_KERNEL_API int32_t GetMsgSize(Process* process, int32_t md);
CMSX_KERNEL_API void GetMsg(Process* process, int32_t md, int64_t bufferAttr);
CMSX_KERNEL_API void InitMsgQueue();
CMSX_KERNEL_API void DoneMsgQueue();

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_MSG_QUEUE_INCLUDED
