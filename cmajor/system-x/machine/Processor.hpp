// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_MACHINE_PROCESSOR_INCLUDED
#define CMSX_MACHINE_PROCESSOR_INCLUDED
#include <system-x/machine/Registers.hpp>
#include <chrono>
#include <thread>

namespace cmsx::machine {

class Machine;
class UserProcess;
class Instruction;
class InterruptHandler;

class CMSX_MACHINE_API Processor
{
public:
    Processor();
    void SetId(int id_) { id = id_; }
    Machine* GetMachine() const { return machine; }
    void SetMachine(Machine* machine_) { machine = machine_; }
    Registers& Regs() { return registers; }
    void Start();
    void Stop();
    void Run();
    void EnableInterrupts();
    UserProcess* CurrentProcess() const { return currentProcess; }
    void ResetCurrentProcess(bool addSystemTime, bool saveContext);
    void CheckException();
    void RunKernel();
    void* MainFiber() const { return mainFiber; }
private:
    Instruction* FetchInstruction(uint64_t& pc, uint8_t& x, uint8_t& y, uint8_t& z);
    void SetPC(Instruction* inst, uint64_t pc, uint64_t prevPC);
    void CheckInterrupts();
    int id;
    void* mainFiber;
    Machine* machine;
    Registers registers;
    UserProcess* currentProcess;
    InterruptHandler* currentHandler;
    std::thread thread;
    std::exception_ptr exception;
    int kernelStackSize;
};

} // cmsx::machine

#endif // CMSX_MACHINE_PROCESSOR_INCLUDED
