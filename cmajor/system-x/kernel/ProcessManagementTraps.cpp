// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/ProcessManagementTraps.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/kernel/Trap.hpp>
#include <system-x/kernel/OsApi.hpp>
#include <system-x/kernel/IO.hpp>
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

class TrapExecHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_exec"; }
};

uint64_t TrapExecHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int64_t filePathAddress = processor.Regs().Get(cmsx::machine::regAX);
        int64_t argvAddress = processor.Regs().Get(cmsx::machine::regBX);
        int64_t envpAddress = processor.Regs().Get(cmsx::machine::regCX);
        Exec(process, filePathAddress, argvAddress, envpAddress);
        return 0;
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

class TrapGetComputerNameHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_get_computer_name"; }
};

uint64_t TrapGetComputerNameHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int64_t bufferAddress = processor.Regs().Get(cmsx::machine::regAX);
        if (bufferAddress == 0)
        {
            throw SystemError(EPARAM, "buffer is null");
        }
        int64_t bufferSize = processor.Regs().Get(cmsx::machine::regBX);
        std::string computerName = OsGetComputerName();
        std::vector<uint8_t> buffer;
        for (char c : computerName)
        {
            buffer.push_back(static_cast<uint8_t>(c));
        }
        buffer.push_back(static_cast<uint8_t>(0));
        if (bufferSize >= buffer.size())
        {
            WriteProcessMemory(process, bufferAddress, buffer);
        }
        else
        {
            throw SystemError(EPARAM, "buffer too small");
        }
        return 0;
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapGetUserNameHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_get_user_name"; }
};

uint64_t TrapGetUserNameHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int64_t bufferAddress = processor.Regs().Get(cmsx::machine::regAX);
        if (bufferAddress == 0)
        {
            throw SystemError(EPARAM, "buffer is null");
        }
        int64_t bufferSize = processor.Regs().Get(cmsx::machine::regBX);
        std::string userName = OsGetUserName();
        std::vector<uint8_t> buffer;
        for (char c : userName)
        {
            buffer.push_back(static_cast<uint8_t>(c));
        }
        buffer.push_back(static_cast<uint8_t>(0));
        if (bufferSize >= buffer.size())
        {
            WriteProcessMemory(process, bufferAddress, buffer);
        }
        else
        {
            throw SystemError(EPARAM, "buffer too small");
        }
        return 0;
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapSetUidHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_setuid"; }
};

uint64_t TrapSetUidHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int32_t uid = static_cast<int32_t>(processor.Regs().Get(cmsx::machine::regAX));
        process->SetUID(uid);
        return 0;
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapSetGidHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_setgid"; }
};

uint64_t TrapSetGidHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int32_t gid = static_cast<int32_t>(processor.Regs().Get(cmsx::machine::regAX));
        process->SetGID(gid);
        return 0;
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapGetUidHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_getuid"; }
};

uint64_t TrapGetUidHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        return process->UID();
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapGetGidHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_getgid"; }
};

uint64_t TrapGetGidHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        return process->GID();
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
    SetTrapHandler(trap_exec, new TrapExecHandler());
    SetTrapHandler(trap_wait, new TrapWaitHandler());
    SetTrapHandler(trap_getpid, new TrapGetPidHandler());
    SetTrapHandler(trap_get_computer_name, new TrapGetComputerNameHandler());
    SetTrapHandler(trap_get_user_name, new TrapGetUserNameHandler());
    SetTrapHandler(trap_setuid, new TrapSetUidHandler());
    SetTrapHandler(trap_setgid, new TrapSetGidHandler());
    SetTrapHandler(trap_getuid, new TrapGetUidHandler());
    SetTrapHandler(trap_getgid, new TrapGetGidHandler());
}

void DoneProcessManagementTraps()
{
    SetTrapHandler(trap_getgid, nullptr);
    SetTrapHandler(trap_getuid, nullptr);
    SetTrapHandler(trap_setgid, nullptr);
    SetTrapHandler(trap_setuid, nullptr);
    SetTrapHandler(trap_get_user_name, nullptr);
    SetTrapHandler(trap_get_computer_name, nullptr);
    SetTrapHandler(trap_getpid, nullptr);
    SetTrapHandler(trap_wait, nullptr);
    SetTrapHandler(trap_exec, nullptr);
    SetTrapHandler(trap_fork, nullptr);
}

} // namespace cmsx::kernel
