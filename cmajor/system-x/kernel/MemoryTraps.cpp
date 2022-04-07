// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/MemoryTraps.hpp>
#include <system-x/kernel/Trap.hpp>
#include <system-x/kernel/Heap.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/kernel/Resource.hpp>
#include <system-x/kernel/Compression.hpp>

namespace cmsx::kernel {

class TrapMemoryPageSizeHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_memory_page_size"; }
};

uint64_t TrapMemoryPageSizeHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    uint64_t memoryPageSize = cmsx::machine::pageSize;
    return memoryPageSize;
}

class TrapHeapStartHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_heap_start"; }
};

uint64_t TrapHeapStartHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    cmsx::machine::UserProcess* currentProcess = processor.CurrentProcess();
    return static_cast<uint64_t>(currentProcess->HeapStartAddress());
}

class TrapHeapLengthHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_heap_length"; }
};

uint64_t TrapHeapLengthHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    cmsx::machine::UserProcess* currentProcess = processor.CurrentProcess();
    return static_cast<uint64_t>(currentProcess->HeapLength());
}

class TrapAllocateMemoryPagesHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_allocate_memory_pages"; }
};

uint64_t TrapAllocateMemoryPagesHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* currentProcess = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        uint64_t rv = currentProcess->RV();
        int32_t numPages = static_cast<int32_t>(processor.Regs().Get(cmsx::machine::regAX));
        if (numPages >= 0)
        {
            int64_t amountToAllocate = cmsx::machine::pageSize * numPages;
            int64_t currentLength = currentProcess->HeapLength();
            int64_t currentEnd = currentProcess->HeapStartAddress() + currentLength;
            uint64_t newEnd = static_cast<uint64_t>(currentEnd) + static_cast<uint64_t>(amountToAllocate);
            if (newEnd >= cmsx::machine::poolSegmentBaseAddress && newEnd < cmsx::machine::stackSegmentBaseAddress)
            {
                processor.GetMachine()->Mem().AllocateRange(rv, currentEnd, amountToAllocate);
                currentProcess->SetHeapLength(currentLength + amountToAllocate);
                return static_cast<uint64_t>(amountToAllocate);
            }
            else
            {
                throw SystemError(ELIMITEXCEEDED, "out of memory", __FUNCTION__);
            }
        }
        else
        {
            throw SystemError(EPARAM, "invalid number of pages", __FUNCTION__);
        }
    }
    catch (const SystemError& error)
    {
        currentProcess->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapDumpHeapHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_dump_heap"; }
};

uint64_t TrapDumpHeapHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    uint64_t freeAddr = processor.Regs().Get(cmsx::machine::regAX);
    int32_t tag = static_cast<int32_t>(processor.Regs().Get(cmsx::machine::regBX));
    uint64_t ptr = processor.Regs().Get(cmsx::machine::regCX);
    uint64_t size = processor.Regs().Get(cmsx::machine::regDX);
    DumpHeap(processor, freeAddr, tag, ptr, size);
    return 0;
}

class TrapMCpyHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_mcpy"; }
};

uint64_t TrapMCpyHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* currentProcess = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        uint64_t sourceBufferAddr = processor.Regs().Get(cmsx::machine::regAX);
        uint64_t targetBufferAddr = processor.Regs().Get(cmsx::machine::regBX);
        uint64_t count = processor.Regs().Get(cmsx::machine::regCX);
        MCpy(currentProcess, sourceBufferAddr, targetBufferAddr, count);
        return 0;
    }
    catch (const SystemError& error)
    {
        currentProcess->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapOpenResourceHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_open_resource"; }
};

uint64_t TrapOpenResourceHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* currentProcess = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        uint64_t resource_name_addr = processor.Regs().Get(cmsx::machine::regAX);
        return OpenResource(currentProcess, resource_name_addr);
    }
    catch (const SystemError& error)
    {
        currentProcess->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapCloseResourceHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_close_resource"; }
};

uint64_t TrapCloseResourceHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* currentProcess = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int32_t rd = processor.Regs().Get(cmsx::machine::regAX);
        CloseResource(currentProcess, rd);
        return 0;
    }
    catch (const SystemError& error)
    {
        currentProcess->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapGetResourceSizeHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_get_resource_size"; }
};

uint64_t TrapGetResourceSizeHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* currentProcess = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int32_t rd = processor.Regs().Get(cmsx::machine::regAX);
        return GetResourceSize(currentProcess, rd);
    }
    catch (const SystemError& error)
    {
        currentProcess->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapReadResourceHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_read_resource"; }
};

uint64_t TrapReadResourceHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* currentProcess = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int32_t rd = processor.Regs().Get(cmsx::machine::regAX);
        int64_t offset = processor.Regs().Get(cmsx::machine::regBX);
        int64_t length = processor.Regs().Get(cmsx::machine::regCX);
        int64_t buffer_addr = processor.Regs().Get(cmsx::machine::regDX);
        ReadResource(currentProcess, rd, offset, length, buffer_addr);
        return 0;
    }
    catch (const SystemError& error)
    {
        currentProcess->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapDecompressHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_decompress"; }
};

uint64_t TrapDecompressHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* currentProcess = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int64_t buffer_addr = processor.Regs().Get(cmsx::machine::regAX);
        int64_t count = processor.Regs().Get(cmsx::machine::regBX);
        return Decompress(currentProcess, buffer_addr, count);
    }
    catch (const SystemError& error)
    {
        currentProcess->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapGetDecompressedDataSizeHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_get_decompressed_data_size"; }
};

uint64_t TrapGetDecompressedDataSizeHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* currentProcess = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int32_t dd = static_cast<int32_t>(processor.Regs().Get(cmsx::machine::regAX));
        return GetDecompressedDataSize(currentProcess, dd);
    }
    catch (const SystemError& error)
    {
        currentProcess->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapGetDecompressedDataHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_get_decompressed_data"; }
};

uint64_t TrapGetDecompressedDataHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* currentProcess = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int32_t dd = static_cast<int32_t>(processor.Regs().Get(cmsx::machine::regAX));
        int64_t buffer_addr = processor.Regs().Get(cmsx::machine::regBX);
        int64_t count = processor.Regs().Get(cmsx::machine::regCX);
        GetDecompressedData(currentProcess, dd, buffer_addr, count);
        return 0;
    }
    catch (const SystemError& error)
    {
        currentProcess->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapCloseDecompressionHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_close_decompression"; }
};

uint64_t TrapCloseDecompressionHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* currentProcess = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int32_t dd = static_cast<int32_t>(processor.Regs().Get(cmsx::machine::regAX));
        CloseDecompression(currentProcess, dd);
        return 0;
    }
    catch (const SystemError& error)
    {
        currentProcess->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

void InitMemoryTraps()
{
    SetTrapHandler(trap_memory_page_size, new TrapMemoryPageSizeHandler());
    SetTrapHandler(trap_heap_start, new TrapHeapStartHandler());
    SetTrapHandler(trap_heap_length, new TrapHeapLengthHandler());
    SetTrapHandler(trap_allocate_memory_pages, new TrapAllocateMemoryPagesHandler());
    SetTrapHandler(trap_dump_heap, new TrapDumpHeapHandler());
    SetTrapHandler(trap_mcpy, new TrapMCpyHandler());
    SetTrapHandler(trap_open_resource, new TrapOpenResourceHandler());
    SetTrapHandler(trap_close_resource, new TrapCloseResourceHandler());
    SetTrapHandler(trap_get_resource_size, new TrapGetResourceSizeHandler());
    SetTrapHandler(trap_read_resource, new TrapReadResourceHandler());
    SetTrapHandler(trap_decompress, new TrapDecompressHandler());
    SetTrapHandler(trap_get_decompressed_data_size, new TrapGetDecompressedDataSizeHandler());
    SetTrapHandler(trap_get_decompressed_data, new TrapGetDecompressedDataHandler());
    SetTrapHandler(trap_close_decompression, new TrapCloseDecompressionHandler());
}

void DoneMemoryTraps()
{
    SetTrapHandler(trap_decompress, nullptr);
    SetTrapHandler(trap_get_decompressed_data_size, nullptr);
    SetTrapHandler(trap_get_decompressed_data, nullptr);
    SetTrapHandler(trap_close_decompression, nullptr);
    SetTrapHandler(trap_read_resource, nullptr);
    SetTrapHandler(trap_get_resource_size, nullptr);
    SetTrapHandler(trap_close_resource, nullptr);
    SetTrapHandler(trap_open_resource, nullptr);
    SetTrapHandler(trap_mcpy, nullptr);
    SetTrapHandler(trap_dump_heap, nullptr);
    SetTrapHandler(trap_allocate_memory_pages, nullptr);
    SetTrapHandler(trap_heap_length, nullptr);
    SetTrapHandler(trap_heap_start, nullptr);
    SetTrapHandler(trap_memory_page_size, nullptr);
}

} // namespace cmsx::kernel
