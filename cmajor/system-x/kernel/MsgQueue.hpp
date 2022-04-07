// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_MSG_QUEUE_INCLUDED
#define CMSX_KERNEL_MSG_QUEUE_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <system-x/machine/Process.hpp>
#include <stdint.h>
#include <vector>

class Process;

namespace cmsx::kernel {

class CMSX_KERNEL_API Msg
{
public:
    Msg(const std::vector<uint8_t>& data_);
    const std::vector<uint8_t>& Data() const { return data; }
private:
    std::vector<uint8_t> data;
};

CMSX_KERNEL_API int32_t MsgQ(Process* process, int64_t nameAddr);
CMSX_KERNEL_API void CloseMsgQ(Process* process, int32_t md);
CMSX_KERNEL_API void WaitMsg(Process* process, int32_t md);
CMSX_KERNEL_API bool IsMsgQOpen(int32_t md);
CMSX_KERNEL_API void PutMsg(int32_t md, const std::vector<std::uint8_t>& msgData);
CMSX_KERNEL_API void PutMsg(Process* process, int32_t md, int64_t msgDataAddr, int32_t msgSize);
CMSX_KERNEL_API int32_t GetMsgQueueLength(Process* process, int32_t md);
CMSX_KERNEL_API int32_t GetMsgSize(Process* process, int32_t md);
CMSX_KERNEL_API void GetMsg(Process* process, int32_t md, int64_t bufferAttr);
CMSX_KERNEL_API void InitMsgQueue();
CMSX_KERNEL_API void DoneMsgQueue();

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_MSG_QUEUE_INCLUDED
