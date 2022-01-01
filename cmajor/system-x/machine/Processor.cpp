// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/machine/Processor.hpp>
#include <system-x/machine/Machine.hpp>
#include <system-x/machine/Instruction.hpp>
#include <system-x/machine/Interrupt.hpp>
#include <system-x/machine/Debugger.hpp>

namespace cmsx::machine {

AddDurationFunc addUserTimeFunction;
AddDurationFunc addSystemTimeFunction;

void SetAddUserTimeFunc(AddDurationFunc addUserTimeFunc)
{
    addUserTimeFunction = addUserTimeFunc;
}

void SetAddSystemTimeFunc(AddDurationFunc addSystemTimeFunc)
{
    addSystemTimeFunction = addSystemTimeFunc;
}

void AddUserTime(std::chrono::steady_clock::duration userTime)
{
    if (addUserTimeFunction)
    {
        addUserTimeFunction(userTime);
    }
}

void AddSystemTime(std::chrono::steady_clock::duration systemTime)
{
    if (addSystemTimeFunction)
    {
        addSystemTimeFunction(systemTime);
    }
}

Processor::Processor(Machine& machine_) : machine(machine_), debugger(nullptr), exitCode(0), exiting(false), start(), end()
{
}

void Processor::Run()
{
    start = std::chrono::steady_clock::now();
    uint64_t pc = machine.Regs().GetPC();
    while (!exiting)
    {
        if (debugger)
        {
            debugger->Intercept();
            if (exiting) break;
        }
        uint64_t prevPC = pc;
        uint8_t x = 0;
        uint8_t y = 0;
        uint8_t z = 0;
        Instruction* inst = FetchInstruction(pc, x, y, z);
        inst->Execute(machine, x, y, z);
        SetPC(inst, pc, prevPC);
        CheckInterrupts();
        pc = machine.Regs().GetPC();
    }
    end = std::chrono::steady_clock::now();
    AddUserTime(end - start);
    if (debugger)
    {
        debugger->ProcessExit();
    }
}

void Processor::Exit(uint8_t exitCode_)
{
    exitCode = exitCode_;
    exiting = true;
}

Instruction* Processor::FetchInstruction(uint64_t& pc, uint8_t& x, uint8_t& y, uint8_t& z)
{
    Memory& mem = machine.Mem();
    uint8_t opCode = mem.ReadByte(pc, Protection::execute);
    ++pc;
    x = mem.ReadByte(pc, Protection::execute);
    ++pc;
    y = mem.ReadByte(pc, Protection::execute);
    ++pc;
    z = mem.ReadByte(pc, Protection::execute);
    ++pc;
    Instruction* inst = machine.GetInstruction(opCode);
    return inst;
}

void Processor::SetPC(Instruction* inst, uint64_t pc, uint64_t prevPC)
{
    uint64_t regsPC = machine.Regs().GetPC();
    if (!inst->IsJumpInstruction() && regsPC == prevPC)
    {
        machine.Regs().SetPC(pc);
    }
    machine.Regs().SetSpecial(rW, prevPC);
}

void Processor::CheckInterrupts()
{
    uint64_t interruptBits = machine.Regs().GetInterruptBits();
    if (interruptBits)
    {
        for (int irq = 0; irq < 64; ++irq)
        {
            uint64_t irqBit = static_cast<uint64_t>(1) << irq;
            if (interruptBits & irqBit)
            {
                InterruptHandler* handler = GetInterruptHandler(irq);
                if (handler)
                {
                    std::chrono::steady_clock::time_point stop = std::chrono::steady_clock::now();
                    AddUserTime(stop - start);
                    handler->HandleInterrupt(machine);
                    start = std::chrono::steady_clock::now();
                    AddSystemTime(start - stop);
                }
                else
                {
                    throw std::runtime_error("no interrupt handler for IRQ " + std::to_string(irq));
                }
            }
        }
    }
}

} // cmsx::machine
