// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/TimeTraps.hpp>
#include <system-x/kernel/Trap.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/kernel/Clock.hpp>
#include <system-x/kernel/Time.hpp>
#include <ctime>

namespace cmsx::kernel {

class TrapCurrentTimePointHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_current_time_point"; }
};

uint64_t TrapCurrentTimePointHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        std::chrono::steady_clock::time_point tp = std::chrono::steady_clock::now();
        return tp.time_since_epoch().count();
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapSleepHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_sleep"; }
};

uint64_t TrapSleepHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        std::chrono::steady_clock::duration duration(static_cast<int64_t>(processor.Regs().Get(cmsx::machine::regAX)));
        std::chrono::steady_clock::time_point dueTime = now + duration;
        processor.ResetCurrentProcess();
        Alarm alarm(process, dueTime);
        Clock::Instance().Schedule(alarm);
        return 0;
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapCurrentDateHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_current_date"; }
};

uint64_t TrapCurrentDateHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        Date currentDate = CurrentDate();
        uint64_t ax = processor.Regs().Get(cmsx::machine::regAX);
        uint64_t bx = processor.Regs().Get(cmsx::machine::regBX);
        uint64_t cx = processor.Regs().Get(cmsx::machine::regCX);
        if (ax == 0)
        {
            throw SystemError(EPARAM, "year pointer is null");
        }
        if (bx == 0)
        {
            throw SystemError(EPARAM, "month pointer is null");
        }
        if (cx == 0)
        {
            throw SystemError(EPARAM, "day pointer is null");
        }
        cmsx::machine::Memory& mem = processor.GetMachine()->Mem();
        WriteDate(currentDate, ax, bx, cx, process->RV(),mem);
        return 0;
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapCurrentDateTimeHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_current_date_time"; }
};

uint64_t TrapCurrentDateTimeHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        DateTime currentDateTime = CurrentDateTime();
        uint64_t ax = processor.Regs().Get(cmsx::machine::regAX);
        uint64_t bx = processor.Regs().Get(cmsx::machine::regBX);
        uint64_t cx = processor.Regs().Get(cmsx::machine::regCX);
        uint64_t dx = processor.Regs().Get(cmsx::machine::regDX);
        if (ax == 0)
        {
            throw SystemError(EPARAM, "year pointer is null");
        }
        if (bx == 0)
        {
            throw SystemError(EPARAM, "month pointer is null");
        }
        if (cx == 0)
        {
            throw SystemError(EPARAM, "day pointer is null");
        }
        if (dx == 0)
        {
            throw SystemError(EPARAM, "seconds pointer is null");
        }
        cmsx::machine::Memory& mem = processor.GetMachine()->Mem();
        WriteDateTime(currentDateTime, ax, bx, cx, dx, process->RV(), mem);
        return 0;
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

void InitTimeTraps()
{
    SetTrapHandler(trap_current_time_point, new TrapCurrentTimePointHandler());
    SetTrapHandler(trap_sleep, new TrapSleepHandler());
    SetTrapHandler(trap_current_date, new TrapCurrentDateHandler());
    SetTrapHandler(trap_current_date_time, new TrapCurrentDateTimeHandler());
}

void DoneTimeTraps()
{
    SetTrapHandler(trap_current_date_time, nullptr);
    SetTrapHandler(trap_current_date, nullptr);
    SetTrapHandler(trap_sleep, nullptr);
    SetTrapHandler(trap_current_time_point, nullptr);
}

} // namespace cmsx::kernel
