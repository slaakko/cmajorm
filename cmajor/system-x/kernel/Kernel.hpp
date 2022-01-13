// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_KERNEL_INCLUDED
#define CMSX_KERNEL_KERNEL_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <system-x/machine/Machine.hpp>

namespace cmsx::kernel {

class CMSX_KERNEL_API Kernel
{
public:
    static void Init();
    static void Done();
    static Kernel& Instance() { return *instance; }
    void SetMachine(cmsx::machine::Machine* machine);
    void Start();
    void Stop();
private:
    static std::unique_ptr<Kernel> instance;
    cmsx::machine::Machine* machine;
};

CMSX_KERNEL_API void InitKernel();
CMSX_KERNEL_API void DoneKernel();

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_KERNEL_INCLUDED
