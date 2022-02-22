// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Pipe.hpp>
#include <system-x/kernel/File.hpp>
#include <system-x/kernel/Error.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/kernel/EventManager.hpp>
#include <system-x/kernel/DebugHelp.hpp>
#include <system-x/kernel/IO.hpp>
#include <system-x/machine/Processor.hpp>
#include <soulng/util/MemoryWriter.hpp>

namespace cmsx::kernel {

using namespace soulng::util;

const int32_t maxPipeSize = 64 * 1024;

class PipeDevice
{
public:
    static void Init();
    static void Done();
    static PipeDevice& Instance() { return *instance; }
    int32_t GetNextPipeId() { return nextPipeId++; }
private:
    PipeDevice();
    static std::unique_ptr<PipeDevice> instance;
    int32_t nextPipeId;
};

std::unique_ptr<PipeDevice> PipeDevice::instance;

void PipeDevice::Init()
{
    instance.reset(new PipeDevice());
}

void PipeDevice::Done()
{
    instance.reset();
}

PipeDevice::PipeDevice() : nextPipeId(0)
{
}

enum class PipeFlags : int32_t
{
    none = 0, empty = 1 << 0, full = 1 << 1
};

inline PipeFlags operator|(PipeFlags left, PipeFlags right)
{
    return PipeFlags(int32_t(left) | int32_t(right));
}

inline PipeFlags operator&(PipeFlags left, PipeFlags right)
{
    return PipeFlags(int32_t(left) & int32_t(right));
}

inline PipeFlags operator~(PipeFlags flags)
{
    return PipeFlags(~int32_t(flags));
}

class Pipe
{
public:
    Pipe(int32_t id_);
    int32_t Id() const { return id; }
    bool GetFlag(PipeFlags flag) const { return (flags & flag) != PipeFlags::none; }
    void SetFlag(PipeFlags flag) { flags = flags | flag; }
    void ResetFlag(PipeFlags flag) { flags = flags & ~flag; }
    int ReaderCount() const { return readerCount; }
    int WriterCount() const { return writerCount; }
    void IncrementReaderCount() { ++readerCount; }
    void DecrementReaderCount() { --readerCount; }
    void IncrementWriterCount() { ++writerCount; }
    void DecrementWriterCount() { --writerCount; }
    bool IsEmpty() const { return GetFlag(PipeFlags::empty);  }
    bool IsFull() const { return GetFlag(PipeFlags::full); }
    const cmsx::machine::Event& NotEmptyEvent() const { return notEmptyEvent; }
    const cmsx::machine::Event& NotFullEvent() const { return notFullEvent; }
    std::vector<uint8_t> Read(int64_t count);
    int64_t Write(const std::vector<uint8_t>& buffer);
private:
    int32_t id;
    PipeFlags flags;
    int readerCount;
    int writerCount;
    int64_t readPos;
    int64_t writePos;
    std::vector<uint8_t> data;
    cmsx::machine::Event notEmptyEvent;
    cmsx::machine::Event notFullEvent;
};

Pipe::Pipe(int32_t id_) :
    id(id_), flags(PipeFlags::empty), readerCount(1), writerCount(1), readPos(0), writePos(0),
    notEmptyEvent(cmsx::machine::EventKind::pipeNotEmptyEvent, id),
    notFullEvent(cmsx::machine::EventKind::pipeNotFullEvent, id)
{
}

std::vector<uint8_t> Pipe::Read(int64_t count)
{
    std::vector<uint8_t> bytes;
    if (readPos >= writePos)
    {
        int64_t n = std::min(count, int64_t(data.size()) - readPos);
        for (int64_t i = 0; i < n; ++i)
        {
            bytes.push_back(data[readPos + i]);
        }
        readPos += n;
        count -= n;
        if (readPos == data.size())
        {
            readPos = 0;
        }
    }
    if (readPos < writePos)
    {
        int64_t n = std::min(count, writePos - readPos);
        for (int64_t i = 0; i < n; ++i)
        {
            bytes.push_back(data[readPos + i]);
        }
        readPos += n;
        count -= n;
    }
    if (readPos == writePos)
    {
        SetFlag(PipeFlags::empty);
    }
    if (!bytes.empty())
    {
        ResetFlag(PipeFlags::full);
        Wakeup(notFullEvent);
    }
    return bytes;
}

int64_t Pipe::Write(const std::vector<uint8_t>& buffer)
{
    int64_t bytesWritten = 0;
    int64_t p = 0;
    if (writePos >= readPos)
    {
        int64_t n = std::min(int64_t(buffer.size()), int64_t(data.size()) - writePos);
        for (int64_t i = 0; i < n; ++i)
        {
            data[writePos + i] = buffer[p + i];
        }
        p += n;
        writePos += n;
        bytesWritten += n;
        if (data.size() < maxPipeSize)
        {
            int64_t n = std::min(int64_t(buffer.size()) - p, maxPipeSize - writePos);
            for (int64_t i = 0; i < n; ++i)
            {
                data.push_back(buffer[p + i]);
            }
            p += n;
            writePos += n;
            bytesWritten += n;
        }
        if (writePos == data.size())
        {
            writePos = 0;
        }
    }
    if (writePos < readPos)
    {
        int64_t n = std::min(int64_t(buffer.size()) - p, readPos - writePos);
        for (int64_t i = 0; i < n; ++i)
        {
            data[writePos + i] = buffer[p + i];
        }
        writePos += n;
        bytesWritten += n;
    }
    if (writePos == readPos)
    {
        SetFlag(PipeFlags::full);
    }
    if (bytesWritten > 0)
    {
        ResetFlag(PipeFlags::empty);
        Wakeup(notEmptyEvent);
    }
    return bytesWritten;
}

class PipeInputFile : public File
{
public:
    PipeInputFile(Pipe* pipe_);
    File* Share() override;
    void Release(cmsx::kernel::Process* process) override;
    void Close(cmsx::kernel::Process* process) override;
    bool IsReadable() const override { return true; }
    bool IsWritable() const override { return false; }
    bool IsConsole() const override { return false; }
    bool IsHostTextFile() const override { return false; }
    bool IsResource() const override { return false; }
    bool IsDecompressionFile() const override { return false; }
    bool HasColors() const override { return false; }
    int Columns() const override { return 0; }
    int Rows() const override { return 0; }
    bool IsDirFile() const override { return false; }
    std::vector<uint8_t> Read(int64_t count, cmsx::machine::Process* process) override;
private:
    Pipe* pipe;
    int32_t referenceCount;
};

PipeInputFile::PipeInputFile(Pipe* pipe_) : File("PIPE_INPUT"), pipe(pipe_), referenceCount(1)
{
}

File* PipeInputFile::Share()
{
    ++referenceCount;
    pipe->IncrementReaderCount();
    return this;
}

void PipeInputFile::Release(cmsx::kernel::Process* process)
{
    --referenceCount;
    if (referenceCount == 0)
    {
        delete this;
    }
}

void PipeInputFile::Close(cmsx::kernel::Process* process)
{
    cmsx::machine::Machine* machine = process->GetProcessor()->GetMachine();
#if (LOCK_DEBUG)
    DebugLock startDebugLock(&machine->Lock(), PIPE, process->Id(), NO_LOCK | CLOSE);
#endif 
    std::unique_lock<std::recursive_mutex> lock(machine->Lock());
#if (LOCK_DEBUG)
    DebugLock hasDebugLock(&machine->Lock(), PIPE, process->Id(), HAS_LOCK | CLOSE);
#endif
    pipe->DecrementReaderCount();
    if (pipe->ReaderCount() == 0 && pipe->WriterCount() == 0)
    {
        delete pipe;
    }
}

std::vector<uint8_t> PipeInputFile::Read(int64_t count, cmsx::machine::Process* process)
{
    while (true)
    {
        cmsx::machine::Machine* machine = process->GetProcessor()->GetMachine();
#if (LOCK_DEBUG)
        DebugLock startDebugLock(&machine->Lock(), PIPE, process->Id(), NO_LOCK | READ);
#endif 
        std::unique_lock<std::recursive_mutex> lock(machine->Lock());
#if (LOCK_DEBUG)
        DebugLock hasDebugLock(&machine->Lock(), PIPE, process->Id(), HAS_LOCK | READ);
#endif
        if (pipe->IsEmpty())
        {
            if (pipe->WriterCount() == 0)
            {
                return std::vector<uint8_t>();
            }
            else
            {
                Sleep(pipe->NotEmptyEvent(), process, lock);
                lock.lock();
                continue;
            }
        }
        else
        {
            return pipe->Read(count);
        }
    }
}

class PipeOutputFile : public File
{
public:
    PipeOutputFile(Pipe* pipe_);
    File* Share() override;
    void Release(cmsx::kernel::Process* process) override;
    void Close(cmsx::kernel::Process* process) override;
    bool IsReadable() const override { return false; }
    bool IsWritable() const override { return true; }
    bool IsConsole() const override { return false; }
    bool IsHostTextFile() const override { return false; }
    bool IsResource() const override { return false; }
    bool IsDecompressionFile() const override { return false; }
    bool HasColors() const override { return false; }
    int Columns() const override { return 0; }
    int Rows() const override { return 0; }
    bool IsDirFile() const override { return false; }
    int64_t Write(const std::vector<uint8_t>& buffer, cmsx::machine::Process* process) override;
private:
    Pipe* pipe;
    int32_t referenceCount;
};

PipeOutputFile::PipeOutputFile(Pipe* pipe_) : File("PIPE_OUTPUT"), pipe(pipe_), referenceCount(1)
{
}

File* PipeOutputFile::Share()
{
    ++referenceCount;
    pipe->IncrementWriterCount();
    return this;
}

void PipeOutputFile::Release(cmsx::kernel::Process* process)
{
    --referenceCount;
    if (referenceCount == 0)
    {
        delete this;
    }
}

void PipeOutputFile::Close(cmsx::kernel::Process* process)
{
    cmsx::machine::Machine* machine = process->GetProcessor()->GetMachine();
#if (LOCK_DEBUG)
    DebugLock startDebugLock(&machine->Lock(), PIPE, process->Id(), NO_LOCK | CLOSE);
#endif 
    std::unique_lock<std::recursive_mutex> lock(machine->Lock());
#if (LOCK_DEBUG)
    DebugLock hasDebugLock(&machine->Lock(), PIPE, process->Id(), HAS_LOCK | CLOSE);
#endif
    pipe->DecrementWriterCount();
    if (pipe->ReaderCount() == 0 && pipe->WriterCount() == 0)
    {
        delete pipe;
    }
}

int64_t PipeOutputFile::Write(const std::vector<uint8_t>& buffer, cmsx::machine::Process* process)
{
    while (true)
    {
        cmsx::machine::Machine* machine = process->GetProcessor()->GetMachine();
#if (LOCK_DEBUG)
        DebugLock startDebugLock(&machine->Lock(), PIPE, process->Id(), NO_LOCK | WRITE);
#endif 
        std::unique_lock<std::recursive_mutex> lock(machine->Lock());
#if (LOCK_DEBUG)
        DebugLock hasDebugLock(&machine->Lock(), PIPE, process->Id(), HAS_LOCK | WRITE);
#endif
        if (pipe->IsFull())
        {
            if (pipe->ReaderCount() == 0)
            {
                throw SystemError(EFAIL, "pipe " + std::to_string(pipe->Id()) + " has no readers");
            }
            else
            {
                Sleep(pipe->NotFullEvent(), process, lock);
                lock.lock();
                continue;
            }
        }
        else
        {
            return pipe->Write(buffer);
        }
    }
}

void MakePipe(cmsx::kernel::Process* process, int64_t readerFdAddr, int64_t writerFdAddr)
{
    cmsx::machine::Machine* machine = process->GetProcessor()->GetMachine();
#if (LOCK_DEBUG)
    DebugLock startDebugLock(&machine->Lock(), PIPE, process->Id(), NO_LOCK | OPEN);
#endif 
    std::unique_lock<std::recursive_mutex> lock(machine->Lock());
#if (LOCK_DEBUG)
    DebugLock hasDebugLock(&machine->Lock(), PIPE, process->Id(), HAS_LOCK | OPEN);
#endif
    if (readerFdAddr == 0)
    {
        throw SystemError(EPARAM, "pipe reader fd is null");
    }
    if (writerFdAddr == 0)
    {
        throw SystemError(EPARAM, "pipe writer fd is null");
    }
    ProcessFileTable& processFileTable = process->GetFileTable();
    Pipe* pipe = new Pipe(PipeDevice::Instance().GetNextPipeId());
    int32_t readerFd = processFileTable.AddFile(new PipeInputFile(pipe));
    int32_t writerFd = processFileTable.AddFile(new PipeOutputFile(pipe));
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    mem.WriteTetra(process->RV(), readerFdAddr, readerFd, cmsx::machine::Protection::write);
    mem.WriteTetra(process->RV(), writerFdAddr, writerFd, cmsx::machine::Protection::write);
}

void InitPipe()
{
    PipeDevice::Init();
}

void DonePipe()
{
    PipeDevice::Done();
}

} // namespace cmsx::kernel
