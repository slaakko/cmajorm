// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_EVENT_MANAGER_INCLUDED
#define CMSX_KERNEL_EVENT_MANAGER_INCLUDED
#include <system-x/kernel/Event.hpp>
#include <system-x/machine/Processor.hpp>
#include <map>
#include <memory>

namespace cmsx::kernel {

class CMSX_KERNEL_API EventManager
{
public:
    static void Init();
    static void Done();
    static EventManager& Instance() { return *instance; }
    void SetMachine(cmsx::machine::Machine* machine_);
    void Start();
    void Stop();
    void SleepOn(Event evnt, cmsx::machine::Process* process);
    void Wakeup(Event evnt);
private:
    static std::unique_ptr<EventManager> instance;
    EventManager();
    cmsx::machine::Machine* machine;
    std::map<Event, cmsx::machine::ProcessList> sleepingProcesses;
};

CMSX_KERNEL_API void Sleep(Event evnt, cmsx::machine::Process* process);
CMSX_KERNEL_API void Wakeup(Event evnt);
CMSX_KERNEL_API void InitEventManager();
CMSX_KERNEL_API void DoneEventManager();

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_EVENT_MANAGER_INCLUDED
