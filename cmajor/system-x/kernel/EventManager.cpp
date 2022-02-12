// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/EventManager.hpp>
#include <system-x/kernel/Scheduler.hpp>
#include <system-x/machine/Processor.hpp>
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

void EventManager::SleepOn(const cmsx::machine::Event& evnt, cmsx::machine::Process* process, std::unique_lock<std::recursive_mutex>& lock)
{
    sleepingProcesses[evnt].push_back(process);
    lock.unlock();
    process->Sleep();
}

void EventManager::Wakeup(const cmsx::machine::Event& evnt)
{
    std::lock_guard<std::recursive_mutex> lock(machine->Lock());
    cmsx::machine::ProcessList processes;
    sleepingProcesses[evnt].swap(processes);
    sleepingProcesses.erase(evnt);
    for (auto& process : processes)
    {
        process->Wakeup(machine->GetScheduler());
    }
}

void Sleep(const cmsx::machine::Event& evnt, cmsx::machine::Process* process, std::unique_lock<std::recursive_mutex>& lock)
{
    cmsx::machine::Processor* processor = process->GetProcessor();
    if (processor)
    {
        processor->ResetCurrentProcess(true, true);
    }
    Scheduler::Instance().CheckRunnable();
    EventManager::Instance().SleepOn(evnt, process, lock);
    if (processor)
    {
        process->ReleaseProcessor(processor);
    }
}

void Wakeup(const cmsx::machine::Event& evnt)
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
