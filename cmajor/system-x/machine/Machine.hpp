// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_MACHINE_MACHINE_INCLUDED
#define CMSX_MACHINE_MACHINE_INCLUDED
#include <system-x/machine/Processor.hpp>
#include <system-x/machine/Memory.hpp>
#include <system-x/machine/Registers.hpp>
#include <system-x/machine/Instruction.hpp>
#include <vector>
#include <memory>

namespace cmsx::machine {

class CMSX_MACHINE_API Machine
{
public:
    Machine();
    Machine(const Machine&) = delete;
    Machine& operator=(const Machine&) = delete;
    Processor& GetProcessor() { return processor; }
    Memory& Mem() { return memory; }
    Registers& Regs() { return registers; }
    Instruction* GetInstruction(uint8_t opCode) const { return insts[opCode]; }
private:
    void SetInstruction(Instruction* inst);
    Processor processor;
    Memory memory;
    Registers registers;
    std::vector<std::unique_ptr<Instruction>> instructions;
    Instruction* insts[256];
};

} // cmsx::machine

#endif // CMSX_MACHINE_MACHINE_INCLUDED
