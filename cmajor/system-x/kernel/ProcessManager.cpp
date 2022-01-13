// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/ProcessManager.hpp>
#include <system-x/machine/Config.hpp>
#include <system-x/machine/Processor.hpp>

namespace cmsx::kernel {

std::unique_ptr<ProcessManager> ProcessManager::instance;

void ProcessManager::Init()
{
    instance.reset(new ProcessManager());
}

void ProcessManager::Done()
{
    instance.reset();
}

ProcessManager::ProcessManager() : machine(nullptr), maxProcs(cmsx::machine::MaxProcs()), nextProcessId(0)
{
    if (maxProcs <= 0 || maxProcs > 64 * 1024)
    {
        throw SystemError(EPARAM, "invalid 'maxProcs' (" + std::to_string(maxProcs) + ") in '" + cmsx::machine::ConfigFilePath() + 
            "': value should be in range 1..." + std::to_string(64 * 1024));
    }
    processTable.resize(maxProcs);
}

ProcessManager::~ProcessManager()
{
}

Process* ProcessManager::GetProcess(int32_t pid) const
{
    if (pid < 0 || pid > maxProcs)
    {
        throw SystemError(EPARAM, "invalid pid " + std::to_string(pid));
    }
    return processTable[pid].get();
}

void ProcessManager::Start()
{
    processTable.clear();
    processTable.resize(maxProcs);
    nextProcessId = 0;
}

void ProcessManager::Stop()
{
    processTable.clear();
}

Process* ProcessManager::CreateProcess()
{
    if (!machine)
    {
        throw SystemError(EFAIL, "machine not set in process manager");
    }
    std::lock_guard<std::recursive_mutex> lock(machine->Lock());
    if (nextProcessId >= maxProcs)
    {
        nextProcessId = 0;
    }
    while (nextProcessId < maxProcs && processTable[nextProcessId])
    {
        if (processTable[nextProcessId]->State() == cmsx::machine::ProcessState::zombie)
        {
            DeleteProcess(nextProcessId);
            break;
        }
        ++nextProcessId;
    }
    if (nextProcessId >= maxProcs)
    {
        throw SystemError(ELIMITEXCEEDED, "all process table entries in use");
    }
    int32_t processId = nextProcessId++;
    Process* process = new Process(processId);
    processTable[processId].reset(process);
    return process;
}

void ProcessManager::SetMachine(cmsx::machine::Machine* machine_)
{
    machine = machine_;
}

void ProcessManager::DeleteProcess(int32_t pid)
{
    if (!machine)
    {
        throw SystemError(EFAIL, "machine not set in process manager");
    }
    std::lock_guard<std::recursive_mutex> lock(machine->Lock());
    Process* process = GetProcess(pid);
    if (process)
    {
        if (process->RV() != static_cast<uint64_t>(-1))
        {
            machine->Mem().FreeMemory(process->RV());
        }
        process->RemoveFromParent();
        processTable[pid].reset();
    }
    else
    {
        throw SystemError(ENOTFOUND, "process with id " + std::to_string(pid) + " not found");
    }
}

void InitProcessManager()
{
    ProcessManager::Init();
}

void DoneProcessManager()
{
    ProcessManager::Done();
}

} // namespace cmsx::kernel
