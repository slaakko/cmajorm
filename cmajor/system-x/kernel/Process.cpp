// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Process.hpp>
#include <system-x/kernel/ProcessManager.hpp>
#include <system-x/kernel/EventManager.hpp>
#include <system-x/kernel/Scheduler.hpp>
#include <system-x/kernel/Kernel.hpp>
#include <system-x/kernel/Mount.hpp>
#include <system-x/kernel/Fs.hpp>
#include <system-x/kernel/IO.hpp>
#include <system-x/kernel/Load.hpp>
#include <system-x/kernel/DebugHelp.hpp>
#include <system-x/machine/Config.hpp>
#include <system-x/machine/Machine.hpp>
#include <system-x/machine/Memory.hpp>
#include <system-x/machine/Registers.hpp>
#include <system-x/machine/Processor.hpp>
#include <soulng/util/Fiber.hpp>
#include <soulng/util/MemoryStream.hpp>

namespace cmsx::kernel {

Process::Process(int32_t id_) : 
    soulng::util::IntrusiveListNode<Process>(this), id(id_), rv(static_cast<uint64_t>(-1)), kernelSP(cmsx::machine::kernelBaseAddress), axAddress(0), bxAddress(0), cxAddress(0),
    state(cmsx::machine::ProcessState::created), entryPoint(-1), argumentsStartAddress(-1), argumentsLength(0), environmentStartAddress(-1), environmentLength(0), 
    heapStartAddress(-1), heapLength(0), stackStartAddress(-1), startUserTime(), startSleepTime(), startSystemTime(), userTime(0), sleepTime(0), systemTime(0),
    exitCode(0), debugger(nullptr), processor(nullptr), currentExceptionAddress(0), currentExceptionClassId(0), currentTryRecord(nullptr), kernelFiber(nullptr),
    inodeKeyOfWorkingDirAsULong(-1), uid(0), gid(0), euid(0), egid(0), umask(0), directoriesChanged(false)
{
    SetINodeKeyOfWorkingDir(Kernel::Instance().GetINodeKeyOfRootDir());
}

void Process::SetState(cmsx::machine::ProcessState state_)
{
    state = state_;
}

void Process::SetFilePath(const std::string& filePath_)
{
    filePath = filePath_;
#if (LOCK_DEBUG)
    DebugLock create(nullptr, PROCESS_MANAGER, Id(), SET_FILE_PATH_PROCESS, filePath);
#endif
}

void Process::SetHeapLength(int64_t heapLength_) 
{ 
    heapLength = heapLength_; 
    Region poolRegion = regionTable.GetRegion(RegionId::pool);
    if (poolRegion.Valid())
    {
        int64_t heapEnd = heapStartAddress + heapLength;
        int64_t totalPoolLength = heapEnd - poolRegion.Start();
        Region newPoolRegion(RegionId::pool, poolRegion.Start(), totalPoolLength);
        regionTable.SetRegion(newPoolRegion);
    }
}

void Process::SetAddressesFrom(Process* parent)
{
    entryPoint = parent->entryPoint;
    argumentsStartAddress = parent->argumentsStartAddress;
    argumentsLength = parent->argumentsLength;
    environmentStartAddress = parent->environmentStartAddress;
    environmentLength = parent->environmentLength;
    heapStartAddress = parent->heapStartAddress;
    heapLength = parent->heapLength;
    stackStartAddress = parent->stackStartAddress;
}

void Process::SetSymbolTable(const std::shared_ptr<cmsx::object::SymbolTable>& symbolTable_)
{
    symbolTable = symbolTable_;
}

cmsx::object::FunctionTable* Process::GetFunctionTable()
{
    if (!functionTable)
    {
        functionTable.reset(new cmsx::object::FunctionTable());
    }
    return functionTable.get();
}

void Process::RemoveFromParent()
{
    if (Parent())
    {
        Parent()->RemoveChild(this);
    }
}

void Process::SetStartUserTime()
{
    startUserTime = std::chrono::steady_clock::now();
}

void Process::SetStartSleepTime()
{
    startSleepTime = std::chrono::steady_clock::now();
}

void Process::SetStartSystemTime()
{
    startSystemTime = std::chrono::steady_clock::now();
}

void Process::AddUserTime()
{
    std::chrono::steady_clock::time_point endUserTime = std::chrono::steady_clock::now();
    userTime = userTime + (endUserTime - startUserTime);
}

void Process::AddSleepTime()
{
    std::chrono::steady_clock::time_point endSleepTime = std::chrono::steady_clock::now();
    sleepTime = sleepTime + (endSleepTime - startSleepTime);
}

void Process::AddSystemTime()
{
    std::chrono::steady_clock::time_point endSystemTime = std::chrono::steady_clock::now();
    systemTime = systemTime + (endSystemTime - startSystemTime);
}

void Process::AddChildUserTime(const std::chrono::steady_clock::duration& childUserTime_)
{
    childUserTime = childUserTime + childUserTime_;
}

void Process::AddChildSleepTime(const std::chrono::steady_clock::duration & childSleepTime_)
{
    childSleepTime = childSleepTime + childSleepTime_;
}

void Process::AddChildSystemTime(const std::chrono::steady_clock::duration& childSystemTime_)
{
    childSystemTime = childSystemTime + childSystemTime_;
}

void Process::SetError(const SystemError& error_)
{
    error = error_;
}

void Process::SetUID(int32_t uid_)
{
    if (uid == 0 || euid == 0)
    {
        uid = uid_;
        euid = uid_;
    }
    else
    {
        throw SystemError(EPERMISSION, "unauthorized", __FUNCTION__);
    }
}

void Process::SetEUID(int32_t euid_)
{
    if (uid == 0 || euid == 0)
    {
        euid = euid_;
    }
}

void Process::SetEUIDTrusted(int32_t euid_)
{
    euid = euid_;
}

void Process::SetGID(int32_t gid_)
{
    if (uid == 0 || euid == 0 || gid == 0 || egid == 0)
    {
        gid = gid_;
        egid = gid_;
    }
    else
    {
        throw SystemError(EPERMISSION, "unauthorized", __FUNCTION__);
    }
}

void Process::SetEGID(int32_t egid_)
{
    if (uid == 0 || euid == 0)
    {
        egid = egid_;
    }
}

void Process::SetEGIDTrusted(int32_t egid_)
{
    egid = egid_;
}

void Process::Exit(uint8_t exitCode_)
{
    cmsx::machine::Machine* machine = ProcessManager::Instance().GetMachine();
#if (LOCK_DEBUG)
    DebugLock startDebugLock(&machine->Lock(), PROCESS_MANAGER, Id(), NO_LOCK | EXIT);
#endif 
    std::unique_lock<std::recursive_mutex> lock(machine->Lock());
#if (LOCK_DEBUG)
    DebugLock hasDebugLock(&machine->Lock(), PROCESS_MANAGER, Id(), HAS_LOCK | EXIT);
#endif 
    SetExitCode(exitCode_);
    SetState(cmsx::machine::ProcessState::zombie);
    if (debugger)
    {
        debugger->ProcessExit();
    }
    else
    {
        if (rv != static_cast<uint64_t>(-1))
        {
            uint64_t rv_ = rv;
            rv = static_cast<uint64_t>(-1);
            cmsx::machine::Machine* machine = ProcessManager::Instance().GetMachine();
            machine->Mem().FreeMemory(rv_);
        }
    }
    symbolTable.reset();
    functionTable.reset();
    fileTable.CloseFiles(this);
    RemoveDirChangeNotifications(id);
    messageQueues.CloseAll(this);
    Process* parent = Parent();
    if (parent)
    {
        cmsx::kernel::Wakeup(cmsx::machine::Event(cmsx::machine::EventKind::childExitEvent, parent->Id()));
    }
    ProcessManager::Instance().DecrementRunnableProcesses();
}

void Process::DeleteKernelFiber()
{
    if (kernelFiber)
    {
        soulng::util::DeleteFiber(kernelFiber);
        kernelFiber = nullptr;
    }
}

void Process::SaveContext(cmsx::machine::Machine& machine, cmsx::machine::Registers& regs)
{
    cmsx::machine::Memory& mem = machine.Mem();
    kernelSP = cmsx::machine::kernelBaseAddress;
    mem.WriteOcta(rv, kernelSP, regs.Get(cmsx::machine::regSP), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.Get(cmsx::machine::regFP), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetPC(), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    axAddress = kernelSP;
    mem.WriteOcta(rv, kernelSP, regs.Get(cmsx::machine::regAX), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    bxAddress = kernelSP;
    mem.WriteOcta(rv, kernelSP, regs.Get(cmsx::machine::regBX), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    cxAddress = kernelSP;
    mem.WriteOcta(rv, kernelSP, regs.Get(cmsx::machine::regCX), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.Get(cmsx::machine::regDX), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.Get(cmsx::machine::regEX), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.Get(cmsx::machine::regIX), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    int numLocalRegs = cmsx::machine::NumLocalRegs();
    for (int i = 0; i < numLocalRegs; ++i)
    {
        uint8_t regNum = static_cast<uint8_t>(i);
        mem.WriteOcta(rv, kernelSP, regs.Get(regNum), cmsx::machine::Protection::write);
        kernelSP = kernelSP + 8;
    }
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rA), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rB), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rD), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rE), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rH), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rP), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rR), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rW), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rX), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rY), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rZ), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rL), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
    mem.WriteOcta(rv, kernelSP, regs.GetSpecial(cmsx::machine::rG), cmsx::machine::Protection::write);
    kernelSP = kernelSP + 8;
}

void Process::RestoreContext(cmsx::machine::Machine& machine, cmsx::machine::Registers& regs)
{
    cmsx::machine::Memory& mem = machine.Mem();
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rG, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rL, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rZ, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rY, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rX, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rW, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rR, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rP, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rH, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rE, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rD, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rB, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetSpecial(cmsx::machine::rA, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    int numLocalRegs = cmsx::machine::NumLocalRegs();
    for (int i = numLocalRegs - 1; i >= 0; --i)
    {
        kernelSP = kernelSP - 8;
        uint8_t regNum = static_cast<uint8_t>(i);
        regs.Set(regNum, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    }
    kernelSP = kernelSP - 8;
    regs.Set(cmsx::machine::regIX, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.Set(cmsx::machine::regEX, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.Set(cmsx::machine::regDX, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.Set(cmsx::machine::regCX, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.Set(cmsx::machine::regBX, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.Set(cmsx::machine::regAX, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.SetPC(mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.Set(cmsx::machine::regFP, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    kernelSP = kernelSP - 8;
    regs.Set(cmsx::machine::regSP, mem.ReadOcta(rv, kernelSP, cmsx::machine::Protection::read));
    regs.SetSpecial(cmsx::machine::rV, rv);
}

void Process::SetRunning(cmsx::machine::Processor* processor_)
{
    processor = processor_;
    SetState(cmsx::machine::ProcessState::running);
}

void Process::ResetProcessor()
{
    processor = nullptr;
}

cmsx::machine::Debugger* Process::GetDebugger() const
{
    return debugger;
}

void Process::SetDebugger(cmsx::machine::Debugger* debugger_)
{
    debugger = debugger_;
}

void Process::SetUMask(int32_t mask)
{
    umask = mask & 0777;
}

void Process::RemoveMessageQueue(int32_t md)
{
    messageQueues.Remove(md);
}

cmsx::machine::Machine* Process::GetMachine()
{
    return processor->GetMachine();
}

void SetupRegions(Process* parent, Process* child)
{
    Region textRegion = parent->GetRegionTable().GetRegion(RegionId::text);
    ShareRegion(textRegion, parent, child);
    Region dataRegion = parent->GetRegionTable().GetRegion(RegionId::data);
    CopyRegion(dataRegion, parent, child);
    Region stackRegion = parent->GetRegionTable().GetRegion(RegionId::stack);
    uint64_t sp = parent->GetProcessor()->Regs().Get(cmsx::machine::regSP);
    int64_t stackSize = sp - stackRegion.Start();
    stackRegion.SetLength(stackSize);
    CopyRegion(stackRegion, parent, child);
    Region poolRegion = parent->GetRegionTable().GetRegion(RegionId::pool);
    CopyRegion(poolRegion, parent, child);
}

int32_t Fork(Process* parent)
{
    cmsx::machine::Machine* machine = ProcessManager::Instance().GetMachine();
    cmsx::machine::Processor* processor = parent->GetProcessor();
#if (LOCK_DEBUG)
    DebugLock startDebugLock(&machine->Lock(), PROCESS_MANAGER, parent->Id(), NO_LOCK | FORK);
#endif 
    std::lock_guard<std::recursive_mutex> lock(machine->Lock());
#if (LOCK_DEBUG)
    DebugLock hasDebugLock(&machine->Lock(), PROCESS_MANAGER, parent->Id(), HAS_LOCK | FORK);
#endif 
    Process* child = ProcessManager::Instance().CreateProcess();
    child->SetProcessor(processor);
    uint64_t rv = machine->Mem().AllocateTranslationMap();
    child->SetRV(rv);
    child->SetUID(parent->UID());
    child->SetEUID(parent->EUID());
    child->SetGID(parent->GID());
    child->SetEGID(parent->EGID());
    child->SetUMask(parent->UMask());
    TextSegmentWriteProtectionGuard guard(rv, machine->Mem());
    SetupRegions(parent, child);
    child->GetFileTable().CopyFrom(parent->GetFileTable());
    child->SetAddressesFrom(parent);
    child->SetSymbolTable(parent->GetSymbolTablePtr());
    child->SetINodeKeyOfWorkingDir(parent->GetINodeKeyOfWorkingDir());
    parent->AddChild(child);
    uint64_t regAX = processor->Regs().Get(cmsx::machine::regAX);
    processor->Regs().Set(cmsx::machine::regAX, 0);
    uint64_t regRV = processor->Regs().GetSpecial(cmsx::machine::rV);
    processor->Regs().SetSpecial(cmsx::machine::rV, rv);
    child->SaveContext(*machine, processor->Regs());
    processor->Regs().SetSpecial(cmsx::machine::rV, regRV);
    processor->Regs().Set(cmsx::machine::regAX, regAX);
    ProcessManager::Instance().IncrementRunnableProcesses();
    Scheduler::Instance().AddRunnableProcess(child, cmsx::machine::ProcessState::runnableInUser);
    return child->Id();
}

int32_t Wait(Process* parent, int32_t pid, int64_t childExitCodeAddress)
{
    while (true)
    {
        cmsx::machine::Machine* machine = ProcessManager::Instance().GetMachine();
#if (LOCK_DEBUG)
        DebugLock startDebugLock(&machine->Lock(), PROCESS_MANAGER, parent->Id(), NO_LOCK | WAIT);
#endif 
        std::unique_lock<std::recursive_mutex> lock(machine->Lock());
#if (LOCK_DEBUG)
        DebugLock hasDebugLock(&machine->Lock(), PROCESS_MANAGER, parent->Id(), HAS_LOCK | WAIT);
#endif
        Process* child = parent->FirstChild();
        while (child)
        {
            if (child->State() == cmsx::machine::ProcessState::zombie)
            {
                if (pid == -1 || child->Id() == pid)
                {
                    child->RemoveFromParent();
                    uint8_t exitCode = child->ExitCode();
                    parent->GetProcessor()->GetMachine()->Mem().WriteByte(parent->RV(), childExitCodeAddress, exitCode, cmsx::machine::Protection::write);
                    parent->AddChildUserTime(child->UserTime());
                    parent->AddChildSleepTime(child->SleepTime());
                    parent->AddChildSystemTime(child->SystemTime());
                    return child->Id();
                }
            }
            child = child->NextSibling();
        }
        child = parent->FirstChild();
        if (child)
        {
            Sleep(cmsx::machine::Event(cmsx::machine::EventKind::childExitEvent, parent->Id()), parent, lock);
            lock.lock();
        }
        bool hasChildInZombieState = false;
        child = parent->FirstChild();
        while (child)
        {
            if (child->State() == cmsx::machine::ProcessState::zombie)
            {
                hasChildInZombieState = true;
                if (pid == -1 || child->Id() == pid)
                {
                    child->RemoveFromParent();
                    uint8_t exitCode = child->ExitCode();
                    machine->Mem().WriteByte(parent->RV(), childExitCodeAddress, exitCode, cmsx::machine::Protection::write);
                    parent->AddChildUserTime(child->UserTime());
                    parent->AddChildSleepTime(child->SleepTime());
                    parent->AddChildSystemTime(child->SystemTime());
                    return child->Id();
                }
            }
            child = child->NextSibling();
        }
        if (pid == -1 || !hasChildInZombieState)
        {
            break;
        }
    }
    throw SystemError(ENOCHILD, "no child in zombie state", __FUNCTION__);
}

void Exec(Process* process, int64_t filePathAddress, int64_t argvAddress, int64_t envpAddress)
{
    cmsx::machine::Machine* machine = process->GetProcessor()->GetMachine();
    cmsx::machine::Memory& mem = machine->Mem();
    std::string filePath = ReadString(process, filePathAddress, mem);
    Filesystem* fs = GetFs(rootFSNumber);
    INodePtr inodePtr = PathToINode(filePath, fs, process);
    INode* inode = inodePtr.Get();
    if (!inode)
    {
        throw SystemError(ENOTFOUND, "could not execute: path '" + filePath + "' not found", __FUNCTION__);
    }
    if (!filePath.starts_with("/mnt/sx/bin"))
    {
        CheckAccess(Access::execute, process->EUID(), process->EGID(), inode, "could not execute '" + filePath + "'");
    }
    if (inode->SetUIDBit())
    {
        process->SetEUIDTrusted(inode->UID());
    }
    if (inode->SetGIDBit())
    {
        process->SetEGIDTrusted(inode->GID());
    }
    process->SetFilePath(filePath);
    std::vector<uint8_t> content = ReadFile(process, filePathAddress);
    std::vector<std::string> args = ReadStringPointerArray(process, argvAddress, mem);
    std::vector<std::string> env = ReadStringPointerArray(process, envpAddress, mem);
    mem.FreeMemory(process->RV());
    mem.AllocateTranslationMap(process->RV());
    process->GetRegionTable().FreeRegions();
    process->SetState(cmsx::machine::ProcessState::exec);
    soulng::util::MemoryStream memoryStream(content.data(), content.size());
    soulng::util::BinaryStreamReader reader(memoryStream);
    std::unique_ptr<cmsx::object::BinaryFile> binaryFile(cmsx::object::ReadBinaryFile(reader, filePath));
    Load(process, binaryFile.get(), args, env, *machine, process->RV(), false);
}

void Kill(Process* parent, Process* process)
{
    bool allow = false;
    if (parent->EUID() == 0 || parent->UID() == 0)
    {
        allow = true;
    }
    else
    {
        Process* par = process->Parent();
        while (par && par != parent)
        {
            par = par->Parent();
        }
        if (par == parent)
        {
            allow = true;
        }
    }
    if (allow)
    {
        process->Exit(255);
    }
    else
    {
        throw SystemError(EPERMISSION, "unauthorized", __FUNCTION__);
    }
}

} // namespace cmsx::kernel
