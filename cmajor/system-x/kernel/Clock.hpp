// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_CLOCK_INCLUDED
#define CMSX_KERNEL_CLOCK_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <system-x/machine/Clock.hpp>
#include <chrono>

namespace cmsx::kernel {

class Process;

struct CMSX_KERNEL_API Alarm
{
    Alarm(Process* process_, std::chrono::steady_clock::time_point dueTime_) : id(-1), process(process_), dueTime(dueTime_) {}
    int id;
    Process* process;
    std::chrono::steady_clock::time_point dueTime;
};

class CMSX_KERNEL_API Clock : public cmsx::machine::ClockObserver
{
public:
    static void Init();
    static void Done();
    static Clock& Instance() { return *instance; }
    void SetMachine(cmsx::machine::Machine* machine_);
    void Start();
    void Stop();
    void Tick() override;
    void Schedule(Alarm& alarm);
private:
    Clock();
    static std::unique_ptr<Clock> instance;
    cmsx::machine::Machine* machine;
    std::vector<Alarm> alarms;
    int nextAlarmId;
};

CMSX_KERNEL_API void InitClock();
CMSX_KERNEL_API void DoneClock();

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_CLOCK_INCLUDED

