// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/EventManager.hpp>
#include <system-x/kernel/Scheduler.hpp>
#include <system-x/machine/Machine.hpp>
#include <soulng/util/Fiber.hpp>

namespace cmsx::kernel {

void EventManager::Init()
{
    instance.reset(new EventManager());
}

void EventManager::Done()
{
    instance.reset();
}

std::unique_ptr<EventManager> EventManager::instance;

EventManager::EventManager() : machine(nullptr)
{
}

void EventManager::SetMachine(cmsx::machine::Machine* machine_)
{
    machine = machine_;
}

void EventManager::Start()
{
    sleepingProcesses.clear();
}

void EventManager::Stop()
{
    sleepingProcesses.clear();
}

void EventManager::SleepOn(Event evnt, cmsx::machine::Process* process)
{
    std::lock_guard<std::recursive_mutex> lock(machine->Lock());
    process->SetState(cmsx::machine::ProcessState::asleep);
    sleepingProcesses[evnt].push_back(process);
}

void EventManager::Wakeup(Event evnt)
{
    cmsx::machine::ProcessList processes;
    {
        std::lock_guard<std::recursive_mutex> lock(machine->Lock());
        sleepingProcesses[evnt].swap(processes);
    }
    for (auto& process : processes)
    {
        Scheduler::Instance().AddRunnableProcess(process, cmsx::machine::ProcessState::runnableInKernel);
    }
}

void Sleep(Event evnt, cmsx::machine::Process* process)
{
    cmsx::machine::Processor* processor = process->GetProcessor();
    processor->ResetCurrentProcess();
    EventManager::Instance().SleepOn(evnt, process);
    soulng::util::SwitchToFiber(processor->MainFiber());
}

void Wakeup(Event evnt)
{
    EventManager::Instance().Wakeup(evnt);
}

void InitEventManager()
{
    EventManager::Init();
}

void DoneEventManager()
{
    EventManager::Done();
}

} // namespace cmsx::kernel
