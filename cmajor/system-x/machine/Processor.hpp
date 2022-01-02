// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_MACHINE_PROCESSOR_INCLUDED
#define CMSX_MACHINE_PROCESSOR_INCLUDED
#include <system-x/machine/Api.hpp>
#include <chrono>
#include <stdint.h>

namespace cmsx::machine {

class Machine;
class Debugger;
class Instruction;

using AddDurationFunc = void(*)(std::chrono::steady_clock::duration duration);

CMSX_MACHINE_API void SetAddUserTimeFunc(AddDurationFunc addUserTimeFunc);
CMSX_MACHINE_API void SetAddSystemTimeFunc(AddDurationFunc addSystemTimeFunc);

class CMSX_MACHINE_API Processor
{
public:
    Processor(Machine& machine_);
    void SetDebugger(Debugger* debugger_) { debugger = debugger_; }
    void Run();
    void Exit(uint8_t exitCode_);
    uint8_t GetExitCode() const { return exitCode; }
    void EnableInterrupts();
private:
    Instruction* FetchInstruction(uint64_t& pc, uint8_t& x, uint8_t& y, uint8_t& z);
    void SetPC(Instruction* inst, uint64_t pc, uint64_t prevPC);
    void CheckInterrupts();
    Machine& machine;
    Debugger* debugger;
    bool exiting;
    uint8_t exitCode;
    std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::time_point end;
};

} // cmsx::machine

#endif // CMSX_MACHINE_PROCESSOR_INCLUDED
