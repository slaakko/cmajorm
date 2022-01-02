// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/db/Debugger.hpp>
#include <system-x/machine/Machine.hpp>

namespace cmsx::db {

DebuggerObserver::~DebuggerObserver()
{
}

Debugger::Debugger(cmsx::machine::Machine& machine_) : machine(machine_), state(DebuggerState::idle), exiting(false), commandAvailable(false), observer(nullptr)
{
}

void Debugger::SetObserver(DebuggerObserver* observer_)
{
    observer = observer_;
}

void Debugger::Run()
{
    try
    {
        if (state != DebuggerState::idle)
        {
            throw std::runtime_error("debugger not ready");
        }
        machine.GetProcessor().SetDebugger(this);
        state = DebuggerState::started;
        machine.GetProcessor().Run();
    }
    catch (const std::exception& ex)
    {
        state = DebuggerState::idle;
        if (observer)
        {
            observer->DebuggerError(ex.what());
        }
    }
}

void Debugger::Stop()
{
    std::unique_lock<std::mutex> lock(mtx);
    exiting = true;
    machine.GetProcessor().Exit(exit_code_stop);
    Release();
}

void Debugger::SingleStep()
{
    std::unique_lock<std::mutex> lock(mtx);
    if (state != DebuggerState::waiting_for_command)
    {
        throw std::runtime_error("debugger not ready");
    }
    state = DebuggerState::singleStep;
    commandAvailable = true;
    Release();
}

void Debugger::StepOver()
{
    std::unique_lock<std::mutex> lock(mtx);
    if (state != DebuggerState::waiting_for_command)
    {
        throw std::runtime_error("debugger not ready");
    }
    uint64_t pc = machine.Regs().GetPC();
    uint8_t opc = machine.Mem().ReadByte(pc, cmsx::machine::Protection::read);
    switch (opc)
    {
        case cmsx::machine::CALL:
        case cmsx::machine::CALLI:
        {
            uint64_t address = pc + 4;
            breakpoints.insert(address);
            state = DebuggerState::running;
            commandAvailable = true;
            Release();
            break;
        }
        default:
        {
            lock.unlock();
            SingleStep();
            break;
        }
    }
}

void Debugger::Continue()
{
    std::unique_lock<std::mutex> lock(mtx);
    if (state != DebuggerState::waiting_for_command)
    {
        throw std::runtime_error("debugger not ready");
    }
    state = DebuggerState::running;
    commandAvailable = true;
    Release();
}

void Debugger::Intercept()
{
    switch (state)
    {
        case DebuggerState::started:
        case DebuggerState::singleStep:
        {
            WaitForCommand();
            break;
        }
        case DebuggerState::running:
        {
            if (!breakpoints.empty())
            {
                uint64_t pc = machine.Regs().GetPC();
                if (breakpoints.find(pc) != breakpoints.cend())
                {
                    WaitForCommand();
                }
            }
            break;
        }
    }
}

void Debugger::Release()
{
    commandAvailableOrExitingVar.notify_one();
}

void Debugger::WaitForCommand()
{
    std::unique_lock<std::mutex> lock(mtx);
    commandAvailable = false;
    exiting = false;
    state = DebuggerState::waiting_for_command;
    NotifyReady();
    commandAvailableOrExitingVar.wait(lock, [this]{ return commandAvailable || exiting; });
}

void Debugger::ProcessExit()
{
    state = DebuggerState::idle;
    if (observer)
    {
        observer->DebuggerProcessExit();
    }
}

void Debugger::NotifyReady()
{
    if (observer)
    {
        observer->DebuggerReady();
    }
}

void Debugger::AddBreakpoint(uint64_t address)
{
    std::unique_lock<std::mutex> lock(mtx);
    breakpoints.insert(address);
}

void Debugger::RemoveBreakpoint(uint64_t address)
{
    std::unique_lock<std::mutex> lock(mtx);
    breakpoints.erase(address);
}

} // namespace cmsx::db
