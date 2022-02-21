// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/EventManager.hpp>
#include <system-x/kernel/Scheduler.hpp>
#include <system-x/kernel/DebugHelp.hpp>
#include <system-x/machine/Processor.hpp>
#include <soulng/util/Fiber.hpp>

namespace cmsx::kernel {

const int eventManager = -1;

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
#if (LOCK_DEBUG)
    DebugLock hasDebugLock(lock.mutex(), EVENT_MANAGER, process->Id(), HAS_LOCK | SLEEP, evnt);
#endif 
    sleepingProcesses[evnt].push_back(process);
    lock.unlock();
    process->Sleep();
#if (LOCK_DEBUG)
    DebugLock startDebugLock(lock.mutex(), EVENT_MANAGER, process->Id(), HAS_LOCK | SLEEP, evnt);
#endif
}

void EventManager::Wakeup(const cmsx::machine::Event& evnt)
{
#if (LOCK_DEBUG)
    DebugLock startDebugLock(&machine->Lock(), EVENT_MANAGER, 0, NO_LOCK | WAKEUP, evnt);
#endif 
    std::lock_guard<std::recursive_mutex> lock(machine->Lock());
    cmsx::machine::ProcessList processes;
    sleepingProcesses[evnt].swap(processes);
    sleepingProcesses.erase(evnt);
    for (auto& process : processes)
    {
#if (LOCK_DEBUG)
        DebugLock hasDebugLock(&machine->Lock(), EVENT_MANAGER, process->Id(), HAS_LOCK | WAKEUP, evnt);
#endif 
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
