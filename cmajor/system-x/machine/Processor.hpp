// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_MACHINE_PROCESSOR_INCLUDED
#define CMSX_MACHINE_PROCESSOR_INCLUDED
#include <system-x/machine/Api.hpp>
#include <stdint.h>

namespace cmsx::machine {

class Machine;
class Instruction;

class CMSX_MACHINE_API Processor
{
public:
    Processor(Machine& machine_);
    void Run();
    void Exit();
private:
    Instruction* FetchInstruction(uint64_t& pc, uint8_t& x, uint8_t& y, uint8_t& z);
    void SetPC(Instruction* inst, uint64_t pc, uint64_t prevPC);
    Machine& machine;
    bool exiting;
};

} // cmsx::machine

#endif // CMSX_MACHINE_PROCESSOR_INCLUDED
