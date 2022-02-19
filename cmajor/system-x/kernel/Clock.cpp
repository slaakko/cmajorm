// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Clock.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/kernel/Scheduler.hpp>
#include <system-x/kernel/EventManager.hpp>
#include <system-x/machine/Machine.hpp>
#include <algorithm>

namespace cmsx::kernel {

std::unique_ptr<Clock> Clock::instance;

void Clock::Init()
{
    instance.reset(new Clock());
}

void Clock::Done()
{
    instance.reset();
}

Clock::Clock() : machine(nullptr), nextAlarmId(0)
{
}

void Clock::SetMachine(cmsx::machine::Machine* machine_)
{
    machine = machine_;
}

void Clock::Start()
{
    machine->GetClock().AddObserver(this);
    alarms.clear();
}

void Clock::Stop()
{
    if (machine)
    {
        machine->GetClock().RemoveObserver(this);
    }
    alarms.clear();
}

void Clock::Tick()
{
    if (alarms.empty()) return;
    std::lock_guard<std::recursive_mutex> lock(machine->Lock());
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    while (!alarms.empty() && now >= alarms.front().dueTime)
    {
        Alarm alarm = alarms.front();
        alarms.erase(alarms.begin());
        Wakeup(cmsx::machine::Event(cmsx::machine::EventKind::alarmEvent, alarm.id));
    }
}

struct AlarmEarlier
{
    bool operator()(const Alarm& left, const Alarm& right) const
    {
        if (left.dueTime < right.dueTime) return true;
        if (left.dueTime > right.dueTime) return false;
        return left.process->Id() < right.process->Id();
    }
};

void Clock::Schedule(Alarm& alarm)
{
    std::unique_lock<std::recursive_mutex> lock(machine->Lock());
    alarm.id = nextAlarmId++;
    alarms.push_back(alarm);
    std::sort(alarms.begin(), alarms.end(), AlarmEarlier());
    Sleep(cmsx::machine::Event(cmsx::machine::EventKind::alarmEvent, alarm.id), alarm.process, lock);
}

void InitClock()
{
    Clock::Init();
}

void DoneClock()
{
    Clock::Done();
}

} // namespace cmsx::kernel