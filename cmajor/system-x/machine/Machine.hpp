// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_MACHINE_MACHINE_INCLUDED
#define CMSX_MACHINE_MACHINE_INCLUDED
#include <system-x/machine/Processor.hpp>
#include <system-x/machine/Clock.hpp>
#include <system-x/machine/Memory.hpp>
#include <system-x/machine/Instruction.hpp>
#include <vector>
#include <memory>
#include <mutex>

namespace cmsx::machine {

class CMSX_MACHINE_API Scheduler
{
public:
    virtual ~Scheduler();
    virtual void SetMachine(Machine* machine_) = 0;
    virtual Process* GetRunnableProcess() = 0;
    virtual void AddRunnableProcess(Process* process) = 0;
    virtual void Stop() = 0;
};

class CMSX_MACHINE_API Machine
{
public:
    Machine();
    Machine(const Machine&) = delete;
    Machine& operator=(const Machine&) = delete;
    ~Machine();
    Clock& GetClock() { return clock; }
    Memory& Mem() { return memory; }
    void SetScheduler(Scheduler* scheduler_) { scheduler = scheduler_; }
    Scheduler* GetScheduler() const { return scheduler; }
    Instruction* GetInstruction(uint8_t opCode) const { return insts[opCode]; }
    std::recursive_mutex& Lock() { return lock; }
    void Start();
    void Exit();
    void SetExiting() { exiting = true; }
    bool Exiting() const { return exiting; }
    std::vector<Processor>& Processors() { return processors; }
    void CheckExceptions();
private:
    void SetInstruction(Instruction* inst);
    std::vector<Processor> processors;
    Clock clock;
    Scheduler* scheduler;
    Memory memory;
    bool exiting;
    std::vector<std::unique_ptr<Instruction>> instructions;
    Instruction* insts[256];
    std::recursive_mutex lock;
};

} // cmsx::machine

#endif // CMSX_MACHINE_MACHINE_INCLUDED
