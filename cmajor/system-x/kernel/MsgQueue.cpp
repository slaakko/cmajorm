// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/MsgQueue.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/kernel/Error.hpp>
#include <system-x/kernel/IO.hpp>
#include <system-x/machine/Machine.hpp>
#include <system-x/machine/Processor.hpp>
#include <memory>
#include <list>

namespace cmsx::kernel {

class Msg
{
public:
    Msg(const std::vector<uint8_t>& data_);
    const std::vector<uint8_t>& Data() const { return data; }
private:
    std::vector<uint8_t> data;
};

Msg::Msg(const std::vector<uint8_t>& data_) : data(data_)
{
}

class MsgQueue
{
public:
    MsgQueue(const std::string& name_);
    void Put(const Msg& msg);
    Msg Get();
    bool IsEmpty() const { return queue.empty(); }
    int32_t Length() const { return queue.size(); }
    const Msg& Front() const { return queue.front(); }
    void IncrementReferenceCount() { ++referenceCount; }
    void DecrementReferenceCount() { --referenceCount; }
    int32_t ReferenceCount() const { return referenceCount; }
private:
    int32_t referenceCount;
    std::string name;
    std::list<Msg> queue;
};

MsgQueue::MsgQueue(const std::string& name_) : name(name_), queue(), referenceCount(1)
{
}

void MsgQueue::Put(const Msg& msg)
{
    queue.push_back(msg);
}

Msg MsgQueue::Get()
{
    Msg msg = queue.front();
    queue.pop_front();
    return msg;
}

class MsgQueues
{
public:
    static void Init();
    static void Done();
    static MsgQueues& Instance() { return *instance; }
    int32_t Open(const std::string& name);
    MsgQueue* Get(int32_t md);
    void Delete(int32_t md);
private:
    static std::unique_ptr<MsgQueues> instance;
    std::map<std::string, int32_t> queueMap;
    std::vector<std::unique_ptr<MsgQueue>> queues;
};

std::unique_ptr<MsgQueues> MsgQueues::instance;

void MsgQueues::Init()
{
    instance.reset(new MsgQueues());
}

void MsgQueues::Done()
{
    instance.reset();
}

int32_t MsgQueues::Open(const std::string& name)
{
    auto it = queueMap.find(name);
    if (it != queueMap.cend())
    {
        int32_t md = it->second;
        MsgQueue* queue = Get(md);
        queue->IncrementReferenceCount();
        return md;
    }
    int32_t md = queues.size();
    queueMap[name] = md;
    MsgQueue* queue = new MsgQueue(name);
    queues.push_back(std::unique_ptr<MsgQueue>(queue));
    return md;
}

MsgQueue* MsgQueues::Get(int32_t md)
{
    if (md >= 0 && md < queues.size())
    {
        MsgQueue* q = queues[md].get();
        if (q)
        {
            return q;
        }
        else
        {
            throw SystemError(EBADF, "message queue " + std::to_string(md) + " is closed");
        }
    }
    else
    {
        throw SystemError(EBADF, "invalid message queue descriptor " + std::to_string(md));
    }
}

void MsgQueues::Delete(int32_t md)
{
    if (md >= 0 && md < queues.size())
    {
        return queues[md].reset();
    }
    else
    {
        throw SystemError(EBADF, "invalid message queue descriptor " + std::to_string(md));
    }
}

int32_t MsgQ(Process* process, int64_t nameAddr)
{
    if (nameAddr == 0)
    {
        throw SystemError(EPARAM, "name is null");
    }
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    std::string name = ReadString(process, nameAddr, mem);
    return MsgQueues::Instance().Open(name);
}

void CloseMsgQ(Process* process, int32_t md)
{
    MsgQueue* queue = MsgQueues::Instance().Get(md);
    queue->DecrementReferenceCount();
    if (queue->ReferenceCount() == 0)
    {
        MsgQueues::Instance().Delete(md);
    }
}

void PutMsg(Process* process, int32_t md, int64_t msgDataAddr, int32_t msgSize)
{
    if (msgSize == 0)
    {
        throw SystemError(EPARAM, "message is empty");;
    }
    MsgQueue* queue = MsgQueues::Instance().Get(md);
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    std::vector<uint8_t> data = ReadProcessMemory(process, msgDataAddr, msgSize);
    queue->Put(Msg(data));
}

int32_t GetMsgQueueLength(Process* process, int32_t md)
{
    MsgQueue* queue = MsgQueues::Instance().Get(md);
    return queue->Length();
}

int32_t GetMsgSize(Process* process, int32_t md)
{
    MsgQueue* queue = MsgQueues::Instance().Get(md);
    if (queue->IsEmpty())
    {
        return 0;
    }
    else
    {
        return queue->Front().Data().size();
    }
}

void GetMsg(Process* process, int32_t md, int64_t bufferAddr)
{
    if (bufferAddr == 0)
    {
        throw SystemError(EPARAM, "buffer is null");
    }
    MsgQueue* queue = MsgQueues::Instance().Get(md);
    if (queue->IsEmpty())
    {
        throw SystemError(EFAIL, "message queue " + std::to_string(md) + " is empty");
    }
    Msg msg = queue->Get();
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    WriteProcessMemory(process, bufferAddr, msg.Data());
}

void InitMsgQueue()
{
    MsgQueues::Init();
}

void DoneMsgQueue()
{
    MsgQueues::Done();
}

} // namespace cmsx::kernel
