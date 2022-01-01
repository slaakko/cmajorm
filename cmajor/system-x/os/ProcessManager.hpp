// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_OS_PROCESS_MANAGER_INCLUDED
#define CMSX_OS_PROCESS_MANAGER_INCLUDED
#include <system-x/os/Process.hpp>
#include <system-x/machine/Machine.hpp>

#undef CreateProcess

namespace cmsx::os {

class CMSX_OS_API ProcessManager
{
public:
    static void Init();
    static void Done();
    static ProcessManager& Instance() { return *instance; }
    ProcessManager(const ProcessManager&) = delete;
    ProcessManager& operator=(const ProcessManager&) = delete;
    ~ProcessManager();
    void SetMachine(cmsx::machine::Machine* machine_);
    Process* CreateProcess();
    Process* GetProcess(int32_t pid) const;
    void DeleteProcess(int32_t pid);
    Process* CurrentProcess() const { return currentProcess; }
    void SetCurrentProcess(Process* currentProcess_) { currentProcess = currentProcess_; }
private:
    ProcessManager();
    static std::unique_ptr<ProcessManager> instance;
    cmsx::machine::Machine* machine;
    int maxProcs;
    int32_t nextProcessId;
    std::vector<std::unique_ptr<Process>> processTable;
    Process* currentProcess;
};

CMSX_OS_API void InitProcessManager();
CMSX_OS_API void DoneProcessManager();

} // namespace cmsx::os

#endif // CMSX_OS_PROCESS_MANAGER_INCLUDED
