// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/TimeTraps.hpp>
#include <system-x/kernel/Trap.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/kernel/Clock.hpp>
#include <system-x/kernel/Time.hpp>
#include <system-x/kernel/IO.hpp>
#include <system-x/machine/Processor.hpp>
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
        Alarm alarm(process, dueTime, true, nullptr);
        Clock::Instance().Schedule(alarm);
        return 0;
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapTimerMsgHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_timer_msg"; }
};

uint64_t TrapTimerMsgHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        std::chrono::steady_clock::duration duration(static_cast<int64_t>(processor.Regs().Get(cmsx::machine::regAX)));
        std::chrono::steady_clock::time_point dueTime = now + duration;
        int32_t md = static_cast<int32_t>(processor.Regs().Get(cmsx::machine::regBX));
        int64_t msgDataAddr = static_cast<int64_t>(processor.Regs().Get(cmsx::machine::regCX));
        int32_t msgSize = static_cast<int32_t>(processor.Regs().Get(cmsx::machine::regDX));
        cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
        std::vector<uint8_t> data = ReadProcessMemory(process, msgDataAddr, msgSize);
        AlarmMsg* msg = new AlarmMsg(md, data);
        Alarm alarm(process, dueTime, false, msg);
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

class TrapTimesHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_times"; }
};

uint64_t TrapTimesHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        uint64_t ax = processor.Regs().Get(cmsx::machine::regAX);
        uint64_t bx = processor.Regs().Get(cmsx::machine::regBX);
        uint64_t cx = processor.Regs().Get(cmsx::machine::regCX);
        if (ax == 0)
        {
            throw SystemError(EPARAM, "user time pointer is null");
        }
        if (bx == 0)
        {
            throw SystemError(EPARAM, "sleep time pointer is null");
        }
        if (cx == 0)
        {
            throw SystemError(EPARAM, "system time pointer is null");
        }
        cmsx::machine::Memory& mem = processor.GetMachine()->Mem();
        mem.WriteOcta(process->RV(), ax, process->UserTime().count(), cmsx::machine::Protection::write);
        mem.WriteOcta(process->RV(), bx, process->SleepTime().count(), cmsx::machine::Protection::write);
        mem.WriteOcta(process->RV(), cx, process->SystemTime().count(), cmsx::machine::Protection::write);
        return 0;
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapChildTimesHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_child_times"; }
};

uint64_t TrapChildTimesHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        uint64_t ax = processor.Regs().Get(cmsx::machine::regAX);
        uint64_t bx = processor.Regs().Get(cmsx::machine::regBX);
        uint64_t cx = processor.Regs().Get(cmsx::machine::regCX);
        if (ax == 0)
        {
            throw SystemError(EPARAM, "child user time pointer is null");
        }
        if (bx == 0)
        {
            throw SystemError(EPARAM, "child sleep time pointer is null");
        }
        if (cx == 0)
        {
            throw SystemError(EPARAM, "child system time pointer is null");
        }
        cmsx::machine::Memory& mem = processor.GetMachine()->Mem();
        mem.WriteOcta(process->RV(), ax, process->ChildUserTime().count(), cmsx::machine::Protection::write);
        mem.WriteOcta(process->RV(), bx, process->ChildSleepTime().count(), cmsx::machine::Protection::write);
        mem.WriteOcta(process->RV(), cx, process->ChildSystemTime().count(), cmsx::machine::Protection::write);
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
    SetTrapHandler(trap_times, new TrapTimesHandler());
    SetTrapHandler(trap_child_times, new TrapChildTimesHandler());
    SetTrapHandler(trap_timer_msg, new TrapTimerMsgHandler());
}

void DoneTimeTraps()
{
    SetTrapHandler(trap_timer_msg, nullptr);
    SetTrapHandler(trap_child_times, nullptr);
    SetTrapHandler(trap_times, nullptr);
    SetTrapHandler(trap_current_date_time, nullptr);
    SetTrapHandler(trap_current_date, nullptr);
    SetTrapHandler(trap_sleep, nullptr);
    SetTrapHandler(trap_current_time_point, nullptr);
}

} // namespace cmsx::kernel
