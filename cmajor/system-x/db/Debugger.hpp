// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_DB_DEBUGGER_INCLUDED
#define CMSX_DB_DEBUGGER_INCLUDED
#include <system-x/machine/Debugger.hpp>
#include <condition_variable>
#include <set>

namespace cmsx::db {

const uint8_t exit_code_stop = 255;

enum class DebuggerState
{
    idle = 0, waiting_for_command = 1, started = 2, singleStep = 3, running = 4
};

class DebuggerObserver
{
public:
    virtual ~DebuggerObserver();
    virtual void DebuggerReady() = 0;
    virtual void DebuggerError(const std::string& message) = 0;
    virtual void DebuggerProcessExit() = 0;
};

class Debugger : public cmsx::machine::Debugger
{
public:
    Debugger(cmsx::machine::Machine& machine_);
    void SetObserver(DebuggerObserver* observer_);
    void Run();
    void Stop();
    void SingleStep();
    void StepOver();
    void Continue();
    void Release();
    void WaitForCommand();
    void AddBreakpoint(uint64_t address);
    void RemoveBreakpoint(uint64_t address);
    void Intercept() override;
    void ProcessExit() override;
private:
    void NotifyReady();
    cmsx::machine::Machine& machine;
    DebuggerObserver* observer;
    DebuggerState state;
    std::mutex mtx;
    std::condition_variable commandAvailableOrExitingVar;
    bool exiting;
    bool commandAvailable;
    std::set<uint64_t> breakpoints;
};

} // namespace cmsx::db

#endif // CMSX_DB_DEBUGGER_INCLUDED