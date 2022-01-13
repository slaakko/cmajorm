// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_MACHINE_CLOCK_INCLUDED
#define CMSX_MACHINE_CLOCK_INCLUDED
#include <system-x/machine/Api.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <chrono>
#include <condition_variable>
#include <thread>

namespace cmsx::machine {

class Machine;

class CMSX_MACHINE_API ClockObserver
{
public:
    virtual ~ClockObserver();
    virtual void Tick() = 0;
};

class CMSX_MACHINE_API Clock
{
public:
    Clock();
    void SetMachine(Machine* machine_);
    void Start();
    void Stop();
    void Run();
    void AddObserver(ClockObserver* observer);
    void RemoveObserver(ClockObserver* observer);
private:
    Machine* machine;
    int tickMilliseconds;
    std::chrono::steady_clock::duration tickPeriod;
    std::condition_variable_any clockTickOrExiting;
    std::thread thread;
    std::list<ClockObserver*, boost::fast_pool_allocator<ClockObserver*>> observers;
};

} // cmsx::machine

#endif // CMSX_MACHINE_CLOCK_INCLUDED
