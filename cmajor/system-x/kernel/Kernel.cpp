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
#include <system-x/kernel/DevFs.hpp>
#include <system-x/kernel/Terminal.hpp>

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

void KernelProcess::SetUID(int32_t uid)
{
    throw SystemError(EPERMISSION, "cannot set UID of kernel process");
}

void KernelProcess::SetGID(int32_t gid)
{
    throw SystemError(EPERMISSION, "cannot set GID of kernel process");
}

void KernelProcess::SetEUID(int32_t euid)
{
    throw SystemError(EPERMISSION, "cannot set EUID of kernel process");
}

void KernelProcess::SetEGID(int32_t egid)
{
    throw SystemError(EPERMISSION, "cannot set EGID of kernel process");
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

Kernel::Kernel() : machine(nullptr), kernelProcess(new KernelProcess()), wakeup(false), inodeKeyOfRootDir()
{
}

cmsx::machine::Process* Kernel::GetKernelProcess() const
{
    return kernelProcess.get();
}

void Kernel::ClearProcessData(cmsx::machine::Process* process)
{
    MountTable& mountTable = GetMountTable();
    mountTable.ClearProcessData(process);
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
    SetTerminalMachine(machine);
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
    DevFilesystem* devFs = new DevFilesystem();
    mountTable->AddFilesystem(devFs);
    StartBlockManager();
    StartINodeManager();
    StartHostFileManager();
    StartIOManager();
    hostFs->Initialize();
    rootFs->Initialize();
    StartTerminal();
}

void Kernel::Stop()
{
    StopTerminal();
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
