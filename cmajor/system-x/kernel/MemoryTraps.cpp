// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/MemoryTraps.hpp>
#include <system-x/kernel/Trap.hpp>
#include <system-x/kernel/ProcessManager.hpp>
#include <system-x/kernel/Heap.hpp>

namespace cmsx::kernel {

class TrapMemoryPageSizeHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Machine& machine, uint64_t ax, uint64_t bx, uint64_t cx, uint64_t dx) override;
    std::string TrapName() const { return "trap_memory_page_size"; }
};

uint64_t TrapMemoryPageSizeHandler::HandleTrap(cmsx::machine::Machine& machine, uint64_t ax, uint64_t bx, uint64_t cx, uint64_t dx)
{
    uint64_t memoryPageSize = cmsx::machine::pageSize;
    return memoryPageSize;
}

class TrapHeapStartHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Machine& machine, uint64_t ax, uint64_t bx, uint64_t cx, uint64_t dx) override;
    std::string TrapName() const { return "trap_heap_start"; }
};

uint64_t TrapHeapStartHandler::HandleTrap(cmsx::machine::Machine& machine, uint64_t ax, uint64_t bx, uint64_t cx, uint64_t dx)
{
    Process* currentProcess = ProcessManager::Instance().CurrentProcess();
    if (currentProcess)
    {
        return static_cast<uint64_t>(currentProcess->HeapStartAddress());
    }
    else
    {
        return static_cast<uint64_t>(-1);
    }
}

class TrapHeapLengthHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Machine& machine, uint64_t ax, uint64_t bx, uint64_t cx, uint64_t dx) override;
    std::string TrapName() const { return "trap_heap_length"; }
};

uint64_t TrapHeapLengthHandler::HandleTrap(cmsx::machine::Machine& machine, uint64_t ax, uint64_t bx, uint64_t cx, uint64_t dx)
{
    Process* currentProcess = ProcessManager::Instance().CurrentProcess();
    if (currentProcess)
    {
        return static_cast<uint64_t>(currentProcess->HeapLength());
    }
    else
    {
        return static_cast<uint64_t>(-1);
    }
}

class TrapAllocateMemoryPagesHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Machine& machine, uint64_t ax, uint64_t bx, uint64_t cx, uint64_t dx) override;
    std::string TrapName() const { return "trap_allocate_memory_pages"; }
};

uint64_t TrapAllocateMemoryPagesHandler::HandleTrap(cmsx::machine::Machine& machine, uint64_t ax, uint64_t bx, uint64_t cx, uint64_t dx)
{
    Process* currentProcess = ProcessManager::Instance().CurrentProcess();
    if (currentProcess)
    {
        int32_t numPages = static_cast<int32_t>(ax);
        if (numPages >= 0)
        {
            int64_t amountAllocated = cmsx::machine::pageSize * numPages;
            int64_t start = currentProcess->HeapStartAddress();
            int64_t length = currentProcess->HeapLength() + amountAllocated;
            machine.Mem().AllocateRange(start, length);
            currentProcess->SetHeapLength(length);
            return static_cast<uint64_t>(amountAllocated);
        }
    }
    return static_cast<uint64_t>(-1);
}

class TrapDumpHeapHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Machine& machine, uint64_t ax, uint64_t bx, uint64_t cx, uint64_t dx) override;
    std::string TrapName() const { return "trap_dump_heap"; }
};

uint64_t TrapDumpHeapHandler::HandleTrap(cmsx::machine::Machine& machine, uint64_t ax, uint64_t bx, uint64_t cx, uint64_t dx)
{
    uint64_t freeAddr = ax;
    DumpHeap(machine.Mem(), freeAddr);
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
