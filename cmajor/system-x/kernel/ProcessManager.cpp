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

void AddUserTime(std::chrono::steady_clock::duration duration)
{
    Process* currentProcess = ProcessManager::Instance().CurrentProcess();
    if (currentProcess)
    {
        currentProcess->AddUserTime(duration);
    }
}

void AddSystemTime(std::chrono::steady_clock::duration duration)
{
    Process* currentProcess = ProcessManager::Instance().CurrentProcess();
    if (currentProcess)
    {
        currentProcess->AddSystemTime(duration);
    }
}

ProcessManager::ProcessManager() : machine(nullptr), maxProcs(cmsx::machine::MaxProcs()), nextProcessId(0), currentProcess(nullptr)
{
    if (maxProcs <= 0 || maxProcs > 64 * 1024)
    {
        throw std::runtime_error("invalid 'maxProcs' (" + std::to_string(maxProcs) + ") in '" + cmsx::machine::ConfigFilePath() + 
            "': value should be in range 1..." + std::to_string(64 * 1024));
    }
    processTable.resize(maxProcs);
    cmsx::machine::SetAddUserTimeFunc(AddUserTime);
    cmsx::machine::SetAddSystemTimeFunc(AddSystemTime);
}

ProcessManager::~ProcessManager()
{
    cmsx::machine::SetAddUserTimeFunc(nullptr);
    cmsx::machine::SetAddSystemTimeFunc(nullptr);
}

Process* ProcessManager::GetProcess(int32_t pid) const
{
    if (pid < 0 || pid > maxProcs)
    {
        throw std::runtime_error("invalid pid " + std::to_string(pid));
    }
    return processTable[pid].get();
}

Process* ProcessManager::CreateProcess()
{
    if (!machine)
    {
        throw std::runtime_error("machine not set in process manager");
    }
    std::lock_guard<std::recursive_mutex> lock(machine->Lock());
    if (nextProcessId >= maxProcs)
    {
        nextProcessId = 0;
    }
    while (nextProcessId < maxProcs && processTable[nextProcessId])
    {
        ++nextProcessId;
    }
    if (nextProcessId >= maxProcs)
    {
        throw std::runtime_error("all process table entries in use");
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
        throw std::runtime_error("machine not set in process manager");
    }
    std::lock_guard<std::recursive_mutex> lock(machine->Lock());
    Process* process = GetProcess(pid);
    if (process)
    {
        machine->Mem().FreeMemory(process->RV());
        process->RemoveFromParent();
        processTable[pid].reset();
        if (process == currentProcess)
        {
            currentProcess = nullptr;
        }
    }
    else
    {
        throw std::runtime_error("process with id " + std::to_string(pid) + " not found");
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
