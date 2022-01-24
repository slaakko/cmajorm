// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Kernel.hpp>
#include <system-x/kernel/Clock.hpp>
#include <system-x/kernel/EventManager.hpp>
#include <system-x/kernel/ProcessManager.hpp>
#include <system-x/kernel/Scheduler.hpp>
#include <system-x/kernel/BlockManager.hpp>

namespace cmsx::kernel {

std::unique_ptr<Kernel> Kernel::instance;

void Kernel::Init()
{
    instance.reset(new Kernel());
}

void Kernel::Done()
{
    instance.reset();
}

void Kernel::SetMachine(cmsx::machine::Machine* machine_)
{
    machine = machine_;
    Clock::Instance().SetMachine(machine);
    EventManager::Instance().SetMachine(machine);
    ProcessManager::Instance().SetMachine(machine);
    Scheduler::Instance().SetMachine(machine);
}

void Kernel::Start()
{
    EventManager::Instance().Start();
    ProcessManager::Instance().Start();
    Scheduler::Instance().Start();
    Clock::Instance().Start();
    StartBlockManager();
}

void Kernel::Stop()
{
    StopBlockManager();
    Clock::Instance().Stop();
    Scheduler::Instance().Stop();
    ProcessManager::Instance().Stop();
    EventManager::Instance().Stop();
}

void InitKernel()
{
    Kernel::Init();
}

void DoneKernel()
{
    Kernel::Done();
}

} // namespace cmsx::kernel
