// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/os/InitDone.hpp>
#include <system-x/os/Trap.hpp>
#include <system-x/os/TrapExit.hpp>
#include <system-x/os/ProcessManager.hpp>

namespace cmsx::os {

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

} // namespace cmsx::os
