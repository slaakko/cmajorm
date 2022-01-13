// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Scheduler.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/machine/Interrupt.hpp>
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

cmsx::machine::Process* Scheduler::GetRunnableProcess()
{
    std::unique_lock<std::recursive_mutex> lock(machine->Lock());
    queueNotEmptyOrExiting.wait(lock, [this] { return machine->Exiting() || !runnableProcesses.empty(); });
    if (!machine->Exiting() && !runnableProcesses.empty())
    {
        cmsx::machine::Process* runnableProcess = runnableProcesses.front();
        runnableProcesses.pop_front();
        return runnableProcess;
    }
    return nullptr;
}

void Scheduler::AddRunnableProcess(cmsx::machine::Process* runnableProcess)
{
    if (runnableProcess->State() != cmsx::machine::ProcessState::zombie)
    {
        runnableProcess->SetState(cmsx::machine::ProcessState::runnable);
        runnableProcesses.push_back(runnableProcess);
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
    if (processor.CurrentProcess() && processor.CurrentProcess()->State() != cmsx::machine::ProcessState::zombie)
    {
        Scheduler::Instance().AddRunnableProcess(processor.CurrentProcess());
        processor.ResetCurrentProcess();
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
