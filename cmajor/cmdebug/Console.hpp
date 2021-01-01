// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_CONSOLE_INCLUDED
#define CMAJOR_DEBUG_CONSOLE_INCLUDED
#include <cmajor/cmdebug/DebugApi.hpp>
#include <list>
#include <mutex>
#include <condition_variable>

namespace cmajor { namespace debug {

class GdbDriver;
class DebuggerCommand;

class DEBUG_API Console
{
public:
    Console();
    void SetDriver(GdbDriver* driver_);
    void Run();
    void SetActive();
    std::unique_ptr<DebuggerCommand> GetCommand();
    void SetTargetRunning();
    bool TargetInputEof() const { return targetInputEof; }
    std::string GetTargetInputLine();
    void Proceed();
    void Reset();
    bool Terminated() const { return terminated; }
private:
    GdbDriver* driver;
    std::mutex mtx;
    std::condition_variable commandAvailableVar;
    std::condition_variable commandReceivedVar;
    std::condition_variable targetRunningVar;
    std::condition_variable targetInputLineAvailableVar;
    std::condition_variable targetInputLineReceivedVar;
    std::condition_variable proceed;
    std::list<std::unique_ptr<DebuggerCommand>> commands;
    bool terminated;
    bool commandAvailable;
    bool commandReceived;
    bool targetRunning;
    bool canProceeed;
    std::string targetInputLine;
    bool needTargetInput;
    bool targetInputLineAvailable;
    bool targetInputEof;
    bool targetInputLineReceived;
    std::chrono::steady_clock::time_point activeTimeStamp;
};

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_CONSOLE_INCLUDED
