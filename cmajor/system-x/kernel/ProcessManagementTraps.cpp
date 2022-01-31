// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/ProcessManagementTraps.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/kernel/Trap.hpp>
#include <system-x/machine/Processor.hpp>

namespace cmsx::kernel {

class TrapForkHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_fork"; }
};

uint64_t TrapForkHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        return Fork(process);
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapWaitHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_wait"; }
};

uint64_t TrapWaitHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int64_t childExitCodeAddress = processor.Regs().Get(cmsx::machine::regAX);
        if (childExitCodeAddress == 0)
        { 
            throw SystemError(EPARAM, "child exit code pointer is null");
        }
        return Wait(process, childExitCodeAddress);
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapGetPidHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_getpid"; }
};

uint64_t TrapGetPidHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        return process->Id();
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

void InitProcessManagementTraps()
{
    SetTrapHandler(trap_fork, new TrapForkHandler());
    SetTrapHandler(trap_wait, new TrapWaitHandler());
    SetTrapHandler(trap_getpid, new TrapGetPidHandler());
}

void DoneProcessManagementTraps()
{
    SetTrapHandler(trap_getpid, nullptr);
    SetTrapHandler(trap_wait, nullptr);
    SetTrapHandler(trap_fork, nullptr);
}

} // namespace cmsx::kernel