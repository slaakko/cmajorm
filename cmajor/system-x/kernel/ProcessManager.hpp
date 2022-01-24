// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_PROCESS_MANAGER_INCLUDED
#define CMSX_KERNEL_PROCESS_MANAGER_INCLUDED
#include <system-x/kernel/Process.hpp>
#include <system-x/machine/Machine.hpp>

#undef CreateProcess

namespace cmsx::kernel {

class CMSX_KERNEL_API ProcessManager
{
public:
    static void Init();
    static void Done();
    static ProcessManager& Instance() { return *instance; }
    ProcessManager(const ProcessManager&) = delete;
    ProcessManager& operator=(const ProcessManager&) = delete;
    ~ProcessManager();
    void SetMachine(cmsx::machine::Machine* machine_);
    cmsx::machine::Machine* GetMachine() const { return machine; }
    void Start();
    void Stop();
    Process* CreateProcess();
    Process* GetProcess(int32_t pid) const;
    void DeleteProcess(int32_t pid);
    void IncrementRunnableProcesses();
    void DecrementRunnableProcesses();
    void WaitForProcessesExit();
private:
    ProcessManager();
    static std::unique_ptr<ProcessManager> instance;
    cmsx::machine::Machine* machine;
    int maxProcs;
    int32_t nextProcessId;
    std::vector<std::unique_ptr<Process>> processTable;
    int32_t numProcessTableSlots;
    int numRunnableProcesses;
    std::condition_variable_any processesExit;
};

CMSX_KERNEL_API void InitProcessManager();
CMSX_KERNEL_API void DoneProcessManager();

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_PROCESS_MANAGER_INCLUDED
