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

namespace cmsx::kernel {

const int firstScreenMessageId = 0;
const int quitMessageId = 0;
const int keyPressedMessageId = 1;
const int writeScreenMessageId = 2;
const int timerMessageId = 3;
const int lastScreenMessageId = 3;

const int firstCompletionMessageId = 10;
const int completionInitMessageId = 10;
const int completionExitMessageId = 11;
const int completionRequestMessageId = 12;
const int completionReplyMessageId = 13;
const int completionErrorReplyMessageId = 14;
const int lastCompletionMessageId = 14;

const int firstUserMessageId = 100;

class Process;

class CMSX_KERNEL_API ProcessMessageQueues
{
public:
    void Add(int32_t md);
    void Remove(int32_t md);
    void CloseAll(cmsx::machine::Process* process);
private:
    std::vector<int32_t> mds;
};

class CMSX_KERNEL_API Msg
{
public:
    Msg(const std::vector<uint8_t>& data_);
    const std::vector<uint8_t>& Data() const { return data; }
private:
    std::vector<uint8_t> data;
};

CMSX_KERNEL_API int32_t MsgQ(cmsx::kernel::Process* process, int64_t nameAddr);
CMSX_KERNEL_API void CloseMsgQ(cmsx::machine::Process* process, int32_t md);
CMSX_KERNEL_API void CloseMsgQ(cmsx::machine::Process* process, int32_t md, bool remove);
CMSX_KERNEL_API void WaitMsgLocked(std::unique_lock<std::recursive_mutex>& lock, cmsx::machine::Process* process, int32_t md);
CMSX_KERNEL_API void WaitMsg(cmsx::machine::Process* process, int32_t md);
CMSX_KERNEL_API bool IsMsgQOpen(int32_t md);
CMSX_KERNEL_API void PutMsg(int32_t md, const std::vector<std::uint8_t>& msgData);
CMSX_KERNEL_API void PutMsg(cmsx::kernel::Process* process, int32_t md, int64_t msgDataAddr, int32_t msgSize);
CMSX_KERNEL_API int32_t GetMsgQueueLength(cmsx::machine::Process* process, int32_t md);
CMSX_KERNEL_API int32_t GetMsgSize(cmsx::machine::Process* process, int32_t md);
CMSX_KERNEL_API std::vector<uint8_t> GetMsg(int32_t md);
CMSX_KERNEL_API void GetMsg(cmsx::kernel::Process* process, int32_t md, int64_t bufferAttr);
CMSX_KERNEL_API void InitMsgQueue();
CMSX_KERNEL_API void DoneMsgQueue();

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_MSG_QUEUE_INCLUDED
