// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/machine/Processor.hpp>
#include <system-x/machine/Machine.hpp>
#include <system-x/machine/Instruction.hpp>

namespace cmsx::machine {

Processor::Processor(Machine& machine_) : machine(machine_), exiting(false)
{
}

void Processor::Run()
{
    uint64_t pc = machine.Regs().GetPC();
    while (!exiting)
    {
        uint64_t prevPC = pc;
        uint8_t x = 0;
        uint8_t y = 0;
        uint8_t z = 0;
        Instruction* inst = FetchInstruction(pc, x, y, z);
        inst->Execute(machine, x, y, z);
        SetPC(inst, pc, prevPC);
        pc = machine.Regs().GetPC();
    }
}

void Processor::Exit()
{
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

} // cmsx::machine
