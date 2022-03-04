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
    int32_t Id() const { return 0; }
    std::string FilePath() const override { return "KERNEL"; }
    void Sleep() override;
    void Wakeup(cmsx::machine::Scheduler* scheduler) override;
    cmsx::machine::Processor* GetProcessor() const override;
    void ReleaseProcessor(cmsx::machine::Processor* processor) override;
    uint64_t GetINodeKeyOfWorkingDir() const override;
    void SetINodeKeyOfWorkingDir(uint64_t inodeKeyAsULong) override;
    int32_t UID() const override { return 0; }
    int32_t GID() const override { return 0; }
    void SetUID(int32_t uid) override;
    void SetGID(int32_t gid) override;
    int32_t UMask() const override { return 0; }
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
    void ClearProcessData(cmsx::machine::Process* process);
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
