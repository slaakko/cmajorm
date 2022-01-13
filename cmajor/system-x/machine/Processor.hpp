// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_MACHINE_PROCESSOR_INCLUDED
#define CMSX_MACHINE_PROCESSOR_INCLUDED
#include <system-x/machine/Registers.hpp>
#include <chrono>
#include <thread>
#include <stdint.h>
#include <boost/pool/pool_alloc.hpp>

namespace cmsx::machine {

class Machine;
class Instruction;
class Processor;
class Process;
class Debugger;

class Process;

using ProcessList = std::list<Process*, boost::fast_pool_allocator<Process*>>;

enum class ProcessState
{
    created, asleep, runnable, running, zombie
};

class CMSX_MACHINE_API ProcessObserver
{
public:
    virtual ~ProcessObserver();
    virtual void ProcessStateChanged(Process* process) = 0;
};

class CMSX_MACHINE_API Process
{
public:
    virtual ~Process();
    virtual uint64_t RV() const = 0;
    virtual void SaveContext(Machine& machine, Registers& regs) = 0;
    virtual void RestoreContext(Machine& machine, Registers& regs) = 0;
    virtual ProcessState State() const = 0;
    virtual void SetState(ProcessState state) = 0;
    virtual void Exit(uint8_t exitCode) = 0;
    virtual void SetRunning(Processor* processor) = 0;
    virtual void ResetProcessor() = 0;
    virtual Debugger* GetDebugger() const = 0;
    virtual void SetDebugger(Debugger* debugger) = 0;
    virtual void SetObserver(ProcessObserver* observer) = 0;
    virtual void AddUserTime(std::chrono::steady_clock::duration duration) = 0;
    virtual void AddSystemTime(std::chrono::steady_clock::duration duration) = 0;
    virtual int64_t HeapStartAddress() const = 0;
    virtual int64_t HeapLength() const = 0;
    virtual void SetHeapLength(int64_t heapLength) = 0;
};

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
    Process* CurrentProcess() const { return currentProcess; }
    void ResetCurrentProcess();
    void CheckException();
private:
    Instruction* FetchInstruction(uint64_t& pc, uint8_t& x, uint8_t& y, uint8_t& z);
    void SetPC(Instruction* inst, uint64_t pc, uint64_t prevPC);
    void CheckInterrupts();
    int id;
    Machine* machine;
    Registers registers;
    Process* currentProcess;
    std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::time_point stop;
    std::thread thread;
    std::exception_ptr exception;
};

} // cmsx::machine

#endif // CMSX_MACHINE_PROCESSOR_INCLUDED
