// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/MemoryTraps.hpp>
#include <system-x/kernel/Trap.hpp>
#include <system-x/kernel/Heap.hpp>
#include <system-x/kernel/Process.hpp>

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
                throw SystemError(ELIMITEXCEEDED, "out of memory");
            }
        }
        else
        {
            throw SystemError(EPARAM, "invalid number of pages");
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


void InitMemoryTraps()
{
    SetTrapHandler(trap_memory_page_size, new TrapMemoryPageSizeHandler());
    SetTrapHandler(trap_heap_start, new TrapHeapStartHandler());
    SetTrapHandler(trap_heap_length, new TrapHeapLengthHandler());
    SetTrapHandler(trap_allocate_memory_pages, new TrapAllocateMemoryPagesHandler());
    SetTrapHandler(trap_dump_heap, new TrapDumpHeapHandler());
}

void DoneMemoryTraps()
{
    SetTrapHandler(trap_dump_heap, nullptr);
    SetTrapHandler(trap_allocate_memory_pages, nullptr);
    SetTrapHandler(trap_heap_length, nullptr);
    SetTrapHandler(trap_heap_start, nullptr);
    SetTrapHandler(trap_memory_page_size, nullptr);
}

} // namespace cmsx::kernel
