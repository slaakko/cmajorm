// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/InitDone.hpp>
#include <system-x/kernel/Trap.hpp>
#include <system-x/kernel/TrapExit.hpp>
#include <system-x/kernel/MemoryTraps.hpp>
#include <system-x/kernel/ProcessManager.hpp>

namespace cmsx::kernel {

void Init()
{
    InitTrap();
    InitTrapExit();
    InitMemoryTraps();
    InitProcessManager();
}

void Done()
{
    DoneProcessManager();
    DoneMemoryTraps();
    DoneTrapExit();
    DoneTrap();
}

} // namespace cmsx::kernel
