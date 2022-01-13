// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_SCHEDULER_INCLUDED
#define CMSX_KERNEL_SCHEDULER_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <system-x/machine/Machine.hpp>

namespace cmsx::kernel {

class Scheduler : public cmsx::machine::Scheduler
{
public:
    static void Init();
    static void Done();
    static Scheduler& Instance() { return *instance; }
    void SetMachine(cmsx::machine::Machine* machine_) override;
    cmsx::machine::Process* GetRunnableProcess() override;
    void AddRunnableProcess(cmsx::machine::Process* runnableProcess) override;
    void Start();
    void Stop() override;
private:
    Scheduler();
    static std::unique_ptr<Scheduler> instance;
    cmsx::machine::Machine* machine;
    cmsx::machine::ProcessList runnableProcesses;
    std::condition_variable_any queueNotEmptyOrExiting;
};

CMSX_KERNEL_API void InitScheduler();
CMSX_KERNEL_API void DoneScheduler();

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_SCHEDULER_INCLUDED
