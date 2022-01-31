// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/machine/Process.hpp>
#include <system-x/machine/Processor.hpp>
#include <system-x/machine/Machine.hpp>
#include <soulng/util/Fiber.hpp>

namespace cmsx::machine {

Process::~Process()
{
}

void UserProcess::Sleep()
{
    SetState(cmsx::machine::ProcessState::asleep);
    SetStartSleepTime();
}

void UserProcess::Wakeup(Scheduler* scheduler)
{
    AddSleepTime();
    scheduler->AddRunnableProcess(this, cmsx::machine::ProcessState::runnableInKernel);
}

void UserProcess::ReleaseProcessor(Processor* processor)
{
    soulng::util::SwitchToFiber(processor->MainFiber());
    SetStartSystemTime();
}

} // cmsx::machine
