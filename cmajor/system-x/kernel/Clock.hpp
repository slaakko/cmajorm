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

struct AlarmMsg
{
    AlarmMsg(int32_t md_, const std::vector<uint8_t>& msgData_);
    int32_t md;
    std::vector<uint8_t> msgData;
};

struct CMSX_KERNEL_API Alarm
{
    Alarm(Process* process_, std::chrono::steady_clock::time_point dueTime_, bool sleep_, AlarmMsg* msg_) : id(-1), process(process_), dueTime(dueTime_), sleep(sleep_), msg(msg_) {}
    int id;
    Process* process;
    std::chrono::steady_clock::time_point dueTime;
    bool sleep;
    AlarmMsg* msg;
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

