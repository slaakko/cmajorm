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
#include <system-x/kernel/INodeManager.hpp>
#include <system-x/kernel/HostFileManager.hpp>
#include <system-x/kernel/IOManager.hpp>
#include <system-x/kernel/Mount.hpp>
#include <system-x/kernel/RootFs.hpp>
#include <system-x/kernel/HostFs.hpp>

namespace cmsx::kernel {

void KernelProcess::Sleep()
{
    Kernel::Instance().Sleep();
}

void KernelProcess::Wakeup(cmsx::machine::Scheduler* scheduler)
{
    Kernel::Instance().Wakeup();
}

cmsx::machine::Processor* KernelProcess::GetProcessor() const
{
    return nullptr;
}

void KernelProcess::ReleaseProcessor(cmsx::machine::Processor* processor)
{
}

uint64_t KernelProcess::GetINodeKeyOfWorkingDir() const
{
    uint64_t rootDirINodeKey = Kernel::Instance().GetINodeKeyOfRootDir();
    return rootDirINodeKey;
}

void KernelProcess::SetINodeKeyOfWorkingDir(uint64_t inodeKeyAsULong)
{
    Kernel::Instance().SetINodeKeyOfRootDir(inodeKeyAsULong);
}

std::unique_ptr<Kernel> Kernel::instance;

void Kernel::Init()
{
    instance.reset(new Kernel());
}

void Kernel::Done()
{
    instance.reset();
}

Kernel::Kernel() : machine(nullptr), kernelProcess(new KernelProcess()), wakeup(false)
{
}

cmsx::machine::Process* Kernel::GetKernelProcess() const
{
    return kernelProcess.get();
}

void Kernel::SetMachine(cmsx::machine::Machine* machine_)
{
    machine = machine_;
    Clock::Instance().SetMachine(machine);
    EventManager::Instance().SetMachine(machine);
    ProcessManager::Instance().SetMachine(machine);
    Scheduler::Instance().SetMachine(machine);
    SetBlockManagerMachine(machine);
    SetINodeManagerMachine(machine);
    SetHostFileManagerMachine(machine);
    SetIOManagerMachine(machine);
}

void Kernel::Start()
{
    EventManager::Instance().Start();
    ProcessManager::Instance().Start();
    Scheduler::Instance().Start();
    Clock::Instance().Start();
    mountTable.reset(new MountTable(machine));
    RootFilesystem* rootFs = new RootFilesystem();
    mountTable->AddFilesystem(rootFs);
    HostFilesystem* hostFs = new HostFilesystem(hostFSNumber, std::string());
    mountTable->AddFilesystem(hostFs);
    StartBlockManager();
    StartINodeManager();
    StartHostFileManager();
    StartIOManager();
    hostFs->Initialize();
    rootFs->Initialize();
}

void Kernel::Stop()
{
    StopIOManager();
    StopHostFileManager();
    StopBlockManager();
    StopINodeManager();
    mountTable.reset();
    Clock::Instance().Stop();
    Scheduler::Instance().Stop();
    ProcessManager::Instance().Stop();
    EventManager::Instance().Stop();
}

void Kernel::Sleep()
{
    std::unique_lock<std::recursive_mutex> lock(machine->Lock());
    sleepVar.wait(lock, [this]{ return wakeup || machine->Exiting(); });
    wakeup = false;
}

void Kernel::Wakeup()
{
    wakeup = true;
    sleepVar.notify_one();
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
