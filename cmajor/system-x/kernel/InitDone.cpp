// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/InitDone.hpp>
#include <system-x/kernel/Trap.hpp>
#include <system-x/kernel/TrapExit.hpp>
#include <system-x/kernel/MemoryTraps.hpp>
#include <system-x/kernel/IOTraps.hpp>
#include <system-x/kernel/TimeTraps.hpp>
#include <system-x/kernel/MathTraps.hpp>
#include <system-x/kernel/Kernel.hpp>
#include <system-x/kernel/ProcessManager.hpp>
#include <system-x/kernel/Scheduler.hpp>
#include <system-x/kernel/EventManager.hpp>
#include <system-x/kernel/Clock.hpp>

namespace cmsx::kernel {

void Init()
{
    InitTrap();
    InitTrapExit();
    InitMemoryTraps();
    InitIOTraps();
    InitTimeTraps();
    InitMathTraps();
    InitProcessManager();
    InitScheduler();
    InitEventManager();
    InitClock();
    InitKernel();
}

void Done()
{
    DoneKernel();
    DoneClock();
    DoneEventManager();
    DoneScheduler();
    DoneProcessManager();
    DoneMathTraps();
    DoneTimeTraps();
    DoneIOTraps();
    DoneMemoryTraps();
    DoneTrapExit();
    DoneTrap();
}

} // namespace cmsx::kernel
