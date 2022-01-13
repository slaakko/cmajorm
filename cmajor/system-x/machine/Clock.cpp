// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/machine/Clock.hpp>
#include <system-x/machine/Machine.hpp>
#include <system-x/machine/Interrupt.hpp>
#include <system-x/machine/Config.hpp>

namespace cmsx::machine {

ClockObserver::~ClockObserver()
{
}

Clock::Clock() : 
    machine(nullptr), 
    tickMilliseconds(ClockTickMilliseconds()), 
    tickPeriod(std::chrono::milliseconds(tickMilliseconds))
{
}

void Clock::SetMachine(Machine* machine_)
{
    machine = machine_;
}

void RunClock(Clock* clock)
{
    clock->Run();
}

void Clock::Start()
{
    thread = std::thread(RunClock, this);
}

void Clock::Stop()
{
    clockTickOrExiting.notify_one();
    thread.join();
}

void Clock::Run()
{
    while (!machine->Exiting())
    {
        std::unique_lock<std::recursive_mutex> lock(machine->Lock());
        if (clockTickOrExiting.wait_for(lock, tickPeriod, [this] { return machine->Exiting(); }))
        {
            return;
        }
        for (auto& processor : machine->Processors())
        {
            processor.Regs().SetInterrupt(CLOCK_INTERRUPT_BIT);
        }
        for (const auto& observer : observers)
        {
            observer->Tick();
        }
    }
}

void Clock::AddObserver(ClockObserver* observer)
{
    std::lock_guard<std::recursive_mutex> lock(machine->Lock());
    if (std::find(observers.begin(), observers.end(), observer) == observers.end())
    {
        observers.push_back(observer);
    }
}

void Clock::RemoveObserver(ClockObserver* observer)
{
    std::lock_guard<std::recursive_mutex> lock(machine->Lock());
    observers.remove(observer);
}

} // cmsx::machine
