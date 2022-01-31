// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_EVENT_MANAGER_INCLUDED
#define CMSX_KERNEL_EVENT_MANAGER_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <system-x/machine/Event.hpp>
#include <system-x/machine/Machine.hpp>
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
    void SleepOn(const cmsx::machine::Event& evnt, cmsx::machine::Process* process, std::unique_lock<std::recursive_mutex>& lock);
    void Wakeup(const cmsx::machine::Event& evnt);
private:
    static std::unique_ptr<EventManager> instance;
    EventManager();
    cmsx::machine::Machine* machine;
    std::map<cmsx::machine::Event, cmsx::machine::ProcessList> sleepingProcesses;
};

CMSX_KERNEL_API void Sleep(const cmsx::machine::Event& evnt, cmsx::machine::Process* process, std::unique_lock<std::recursive_mutex>& lock);
CMSX_KERNEL_API void Wakeup(const cmsx::machine::Event& evnt);
CMSX_KERNEL_API void InitEventManager();
CMSX_KERNEL_API void DoneEventManager();

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_EVENT_MANAGER_INCLUDED
