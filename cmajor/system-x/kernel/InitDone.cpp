// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/InitDone.hpp>
#include <system-x/kernel/Trap.hpp>
#include <system-x/kernel/TrapExit.hpp>
#include <system-x/kernel/ProcessManager.hpp>

namespace cmsx::kernel {

void Init()
{
    InitTrap();
    InitTrapExit();
    InitProcessManager();
}

void Done()
{
    DoneProcessManager();
    DoneTrapExit();
    DoneTrap();
}

} // namespace cmsx::kernel
