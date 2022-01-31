// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_KERNEL_INCLUDED
#define CMSX_KERNEL_KERNEL_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <system-x/machine/Machine.hpp>
#include <system-x/machine/Process.hpp>

namespace cmsx::kernel {

class MountTable;

class KernelProcess : public cmsx::machine::Process
{
public:
    void Sleep() override;
    void Wakeup(cmsx::machine::Scheduler* scheduler) override;
    cmsx::machine::Processor* GetProcessor() const override;
    void ReleaseProcessor(cmsx::machine::Processor* processor) override;
    uint64_t GetINodeKeyOfWorkingDir() const override;
    void SetINodeKeyOfWorkingDir(uint64_t inodeKeyAsULong) override;
};

class CMSX_KERNEL_API Kernel
{
public:
    static void Init();
    static void Done();
    static Kernel& Instance() { return *instance; }
    void SetMachine(cmsx::machine::Machine* machine);
    cmsx::machine::Machine* GetMachine() const { return machine; }
    void Start();
    void Stop();
    void Sleep();
    void Wakeup();
    cmsx::machine::Process* GetKernelProcess() const;
    MountTable& GetMountTable() { return *mountTable; }
    uint64_t GetINodeKeyOfRootDir() const { return inodeKeyOfRootDir; }
    void SetINodeKeyOfRootDir(uint64_t inodeKeyOfRootDir_) { inodeKeyOfRootDir = inodeKeyOfRootDir_; }
private:
    Kernel();
    static std::unique_ptr<Kernel> instance;
    cmsx::machine::Machine* machine;
    std::unique_ptr<KernelProcess> kernelProcess;
    std::unique_ptr<MountTable> mountTable;
    std::condition_variable_any sleepVar;
    bool wakeup;
    uint64_t inodeKeyOfRootDir;
};

CMSX_KERNEL_API void InitKernel();
CMSX_KERNEL_API void DoneKernel();

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_KERNEL_INCLUDED
