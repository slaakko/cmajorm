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
#include <system-x/kernel/TrapRandom.hpp>
#include <system-x/kernel/ExceptionHandlingTraps.hpp>
#include <system-x/kernel/ProcessManagementTraps.hpp>
#include <system-x/kernel/Kernel.hpp>
#include <system-x/kernel/Mount.hpp>
#include <system-x/kernel/Fs.hpp>
#include <system-x/kernel/ProcessManager.hpp>
#include <system-x/kernel/Scheduler.hpp>
#include <system-x/kernel/EventManager.hpp>
#include <system-x/kernel/Clock.hpp>
#include <system-x/kernel/BlockManager.hpp>
#include <system-x/kernel/HostFileManager.hpp>
#include <system-x/kernel/IOManager.hpp>
#include <system-x/kernel/INodeManager.hpp>
#include <system-x/kernel/Pipe.hpp>
#include <system-x/kernel/Terminal.hpp>

namespace cmsx::kernel {

void Init(bool initTerminal)
{
    InitTrap();
    InitTrapExit();
    InitMemoryTraps();
    InitIOTraps();
    InitTimeTraps();
    InitMathTraps();
    InitTrapRandom();
    InitExceptionHandlingTraps();
    InitProcessManager();
    InitProcessManagementTraps();
    InitScheduler();
    InitEventManager();
    InitBlockManager();
    InitINodeManager();
    InitIOManager();
    InitHostFileManager();
    InitClock();
    InitPipe();
    if (initTerminal)
    {
        InitTerminal();
    }
    InitKernel();
}

void Done()
{
    DoneKernel();
    DoneTerminal();
    DonePipe();
    DoneClock();
    DoneIOManager();
    DoneHostFileManager();
    DoneINodeManager();
    DoneBlockManager();
    DoneEventManager();
    DoneScheduler();
    DoneProcessManagementTraps();
    DoneProcessManager();
    DoneExceptionHandlingTraps();
    DoneTrapRandom();
    DoneMathTraps();
    DoneTimeTraps();
    DoneIOTraps();
    DoneMemoryTraps();
    DoneTrapExit();
    DoneTrap();
}

} // namespace cmsx::kernel
