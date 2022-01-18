// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Process.hpp>
#include <system-x/kernel/EventManager.hpp>
#include <system-x/machine/Config.hpp>
#include <system-x/machine/Machine.hpp>
#include <system-x/machine/Memory.hpp>

namespace cmsx::kernel {

Process::Process(int32_t id_) : 
    id(id_), rv(static_cast<uint64_t>(-1)), kernelSP(cmsx::machine::kernelBaseAddress), axAddress(0), bxAddress(0), cxAddress(0),
    state(cmsx::machine::ProcessState::created), entryPoint(-1), argumentsStartAddress(-1), argumentsLength(0), environmentStartAddress(-1), environmentLength(0), 
    heapStartAddress(-1), heapLength(0), stackStartAddress(-1), userTime(0), systemTime(0), exitCode(0), debugger(nullptr), processor(nullptr), observer(nullptr)
{
}

void Process::SetState(cmsx::machine::ProcessState state_)
{
    if (state != state_)
    {
        state = state_;
        if (state == cmsx::machine::ProcessState::asleep)
        {
            sleepStartTime = std::chrono::steady_clock::now();
        }
        if (observer)
        {
            observer->ProcessStateChanged(this);
        }
    }
}

void Process::SetFilePath(const std::string& filePath_)
{
    filePath = filePath_;
}

void Process::SetHeapLength(int64_t heapLength_) 
{ 
    heapLength = heapLength_; 
    Region poolRegion = regionTable.GetRegion(RegionId::pool);
    if (poolRegion.Valid())
    {
        int64_t heapEnd = heapStartAddress + heapLength;
        int64_t totalPoolLength = heapEnd - poolRegion.Start();
        Region newPoolRegion(RegionId::pool, poolRegion.Start(), totalPoolLength);
        regionTable.SetRegion(newPoolRegion);
    }
}

void Process::SetSymbolTable(cmsx::object::SymbolTable* symbolTable_)
{
    symbolTable.reset(symbolTable_);
}

void Process::RemoveFromParent()
{
    if (Parent())
    {
        Parent()->RemoveChild(this);
    }
}

void Process::AddUserTime(std::chrono::steady_clock::duration duration)
{
    userTime = userTime + duration;
}

void Process::AddSleepTime()
{
    std::chrono::steady_clock::time_point sleepEndTime = std::chrono::steady_clock::now();
    sleepTime = sleepTime + (sleepEndTime - sleepStartTime);
}

void Process::AddSystemTime(std::chrono::steady_clock::duration duration)
{
    systemTime = systemTime + duration;
}

void Process::SetError(const SystemError& error_)
{
    error = error_;
}

void Process::Exit(uint8_t exitCode_)
{
    SetExitCode(exitCode_);
    SetState(cmsx::machine::ProcessState::zombie);
    if (debugger)
    {
        debugger->ProcessExit();
    }
    Wakeup(Event(EventKind::processExitEvent, id));
}

void Process::SaveContext(cmsx::machine::Machine& machine, cmsx::machine::Registers& regs)
{
    cmsx::machine::Memory& mem = machine.Mem();
    kernelSP = cmsx::machine::kernelBaseAddress;
    mem.WriteOcta(rv, kernelSP, regs.Get(cmsx::machine::regSP), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.Get(cmsx::machine::regFP), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetPC(), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    axAddress = kernelSP;
    mem.WriteOcta(rv, kernelSP, regs.Get(cmsx::machine::regAX), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    bxAddress = kernelSP;
    mem.WriteOcta(rv, kernelSP, regs.Get(cmsx::machine::regBX), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    cxAddress = kernelSP;
    mem.WriteOcta(rv, kernelSP, regs.Get(cmsx::machine::regCX), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.Get(cmsx::machine::regDX), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.Get(cmsx::machine::regEX), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.Get(cmsx::machine::regIX), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    int numLocalRegs = cmsx::machine::NumLocalRegs();
    for (int i = 0; i < numLocalRegs; ++i)
    {
        uint8_t regNum = static_cast<uint8_t>(i);
        mem.WriteOcta(rv, kernelSP, regs.Get(regNum), cmsx::machine::Protection::write);
        kernelSP = kernelSP + 8;
    }
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rA), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rB), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rD), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rE), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rH), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rP), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rR), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rW), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rX), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rY), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rZ), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rL), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rG), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
}

void Process::RestoreContext(cmsx::machine::Machine& machine, cmsx::machine::Registers& regs)
{
    cmsx::machine::Memory& mem = machine.Mem();
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rG, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rL, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rZ, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rY, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rX, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rW, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rR, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rP, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rH, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rE, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rD, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rB, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rA, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    int numLocalRegs = cmsx::machine::NumLocalRegs();
    for (int i = numLocalRegs - 1; i >= 0; --i)
    {
        kernelSP = kernelSP - 8;
        uint8_t regNum = static_cast<uint8_t>(i);
        regs.Set(regNum, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    }
    kernelSP = kernelSP - 8;
    regs.Set(cmsx::machine::regIX, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.Set(cmsx::machine::regEX, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.Set(cmsx::machine::regDX, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.Set(cmsx::machine::regCX, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.Set(cmsx::machine::regBX, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.Set(cmsx::machine::regAX, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetPC(mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.Set(cmsx::machine::regFP, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.Set(cmsx::machine::regSP, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
}

void Process::SetRunning(cmsx::machine::Processor* processor_)
{
    processor = processor_;
    SetState(cmsx::machine::ProcessState::running);
}

void Process::ResetProcessor()
{
    processor = nullptr;
}

void Process::SetObserver(cmsx::machine::ProcessObserver* observer_)
{
    observer = observer_;
}

cmsx::machine::Debugger* Process::GetDebugger() const
{
    return debugger;
}

void Process::SetDebugger(cmsx::machine::Debugger* debugger_)
{
    debugger = debugger_;
}

} // namespace cmsx::kernel
