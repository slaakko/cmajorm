// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Scheduler.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/kernel/DebugHelp.hpp>
#include <system-x/machine/Interrupt.hpp>
#include <system-x/machine/Processor.hpp>
#include <condition_variable>

namespace cmsx::kernel {

std::unique_ptr<Scheduler> Scheduler::instance;

void Scheduler::Init()
{
    instance.reset(new Scheduler());
}

void Scheduler::Done()
{
    instance.reset();
}

Scheduler::Scheduler() : machine(nullptr)
{
}

void Scheduler::SetMachine(cmsx::machine::Machine* machine_)
{
    machine = machine_;
}

cmsx::machine::UserProcess* Scheduler::GetRunnableProcess()
{
#if (LOCK_DEBUG)
    DebugLock startDebugLock(&machine->Lock(), SCHEDULER, 0, NO_LOCK | GET_RUNNABLE_PROCESS);
#endif 
    std::unique_lock<std::recursive_mutex> lock(machine->Lock());
    queueNotEmptyOrExiting.wait(lock, [this]{ return machine->Exiting() || !runnableProcesses.empty(); });
#if (LOCK_DEBUG)
    DebugLock hasDebugLock(&machine->Lock(), SCHEDULER, 0, HAS_LOCK | GET_RUNNABLE_PROCESS);
#endif 
    if (!machine->Exiting() && !runnableProcesses.empty())
    {
        cmsx::machine::UserProcess* runnableProcess = runnableProcesses.front();
        runnableProcesses.pop_front();
        return runnableProcess;
    }
    return nullptr;
}

void Scheduler::AddRunnableProcess(cmsx::machine::UserProcess* runnableProcess, cmsx::machine::ProcessState processState)
{
#if (LOCK_DEBUG)
    DebugLock startDebugLock(&machine->Lock(), SCHEDULER, runnableProcess->Id(), NO_LOCK | ADD_RUNNABLE_PROCESS);
#endif 
    std::unique_lock<std::recursive_mutex> lock(machine->Lock());
#if (LOCK_DEBUG)
    DebugLock hasDebugLock(&machine->Lock(), SCHEDULER, runnableProcess->Id(), HAS_LOCK | ADD_RUNNABLE_PROCESS);
#endif 
    if (runnableProcess->State() != cmsx::machine::ProcessState::zombie)
    {
        runnableProcess->SetState(processState);
        runnableProcesses.push_back(runnableProcess);
        queueNotEmptyOrExiting.notify_all();
    }
}

void Scheduler::CheckRunnable()
{
#if (LOCK_DEBUG)
    DebugLock startDebugLock(&machine->Lock(), SCHEDULER, 0, NO_LOCK | CHECK_RUNNABLE);
#endif 
    std::unique_lock<std::recursive_mutex> lock(machine->Lock());
#if (LOCK_DEBUG)
    DebugLock hasDebugLock(&machine->Lock(), SCHEDULER, 0, HAS_LOCK | CHECK_RUNNABLE);
#endif 
    if (!runnableProcesses.empty())
    {
        queueNotEmptyOrExiting.notify_all();
    }
}

void Scheduler::Start()
{
    machine->SetScheduler(this);
    runnableProcesses.clear();
}

void Scheduler::Stop()
{
    queueNotEmptyOrExiting.notify_all();
}

class ClockInterruptHandler : public cmsx::machine::InterruptHandler
{
public:
    ClockInterruptHandler();
    void HandleInterrupt(cmsx::machine::Processor& processor) override;
};

ClockInterruptHandler::ClockInterruptHandler()
{
}

void ClockInterruptHandler::HandleInterrupt(cmsx::machine::Processor& processor)
{
    cmsx::machine::UserProcess* currentProcess = processor.CurrentProcess();
    if (currentProcess && currentProcess->State() != cmsx::machine::ProcessState::zombie && currentProcess->State() != cmsx::machine::ProcessState::asleep)
    {
        Scheduler::Instance().AddRunnableProcess(processor.CurrentProcess(), cmsx::machine::ProcessState::runnableInUser);
        processor.ResetCurrentProcess(true, true);
    }
}

void InitScheduler()
{
    Scheduler::Init();
    cmsx::machine::SetInterruptHandler(cmsx::machine::irq_clock, new ClockInterruptHandler());
}

void DoneScheduler()
{
    Scheduler::Done();
    cmsx::machine::SetInterruptHandler(cmsx::machine::irq_clock, nullptr);
}

} // namespace cmsx::kernel
