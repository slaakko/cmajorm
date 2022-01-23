// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/ExceptionHandlingTraps.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/kernel/ExceptionHandling.hpp>
#include <system-x/kernel/IO.hpp>
#include <system-x/kernel/Trap.hpp>

namespace cmsx::kernel {

class TrapThrowHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_throw"; }
};

uint64_t TrapThrowHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        uint64_t exceptionAddress = processor.Regs().Get(cmsx::machine::regAX);
        if (exceptionAddress == 0)
        {
            throw SystemError(EPARAM, "exception is null");
        }
        uint64_t exceptionClassId = processor.Regs().Get(cmsx::machine::regBX);
        if (exceptionClassId == 0)
        {
            throw SystemError(EPARAM, "exception class id is 0");
        }
        uint64_t setPC = 0;
        uint64_t setFP = 0;
        uint64_t setSP = 0;
        DispatchException(exceptionAddress, exceptionClassId, process, setFP, setSP, setPC);
        processor.Regs().SetPC(setPC);
        processor.Regs().Set(cmsx::machine::regFP, setFP);
        processor.Regs().Set(cmsx::machine::regSP, setSP);
        return 0;
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapCatchHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_catch"; }
};

uint64_t TrapCatchHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        if (process->CurrentExceptionAddress() == 0)
        {
            throw SystemError(EFAIL, "current exception not set");
        }
        return process->CurrentExceptionAddress();
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(0);
    }
}

class TrapResumeHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_resume"; }
};

uint64_t TrapResumeHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        uint64_t setPC = 0;
        uint64_t setFP = 0;
        uint64_t setSP = 0;
        ContinueExceptionDispatch(process, setFP, setSP, setPC);
        processor.Regs().SetPC(setPC);
        processor.Regs().Set(cmsx::machine::regFP, setFP);
        processor.Regs().Set(cmsx::machine::regSP, setSP);
        return 0;
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapStackTraceHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_stack_trace"; }
};

uint64_t TrapStackTraceHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        uint64_t bufferAddress = processor.Regs().Get(cmsx::machine::regAX);
        if (bufferAddress == 0)
        {
            throw SystemError(EPARAM, "buffer is null");
        }
        int64_t count = processor.Regs().Get(cmsx::machine::regBX);
        if (count <= 0)
        {
            throw SystemError(EPARAM, "count is nonpositive");
        }
        std::string stackTrace = GetStackTrace(process);
        std::vector<std::uint8_t> buffer = MakeStringBuffer(stackTrace, count);
        WriteProcessMemory(process, bufferAddress, buffer);
        return 0;
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapGetSystemErrorHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_get_system_error"; }
};

uint64_t TrapGetSystemErrorHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        SystemError systemError = process->GetError();
        uint64_t errorCodeAddress = processor.Regs().Get(cmsx::machine::regAX);
        if (errorCodeAddress == 0)
        {
            throw SystemError(EPARAM, "error code pointer is null");
        }
        processor.GetMachine()->Mem().WriteTetra(process->RV(), errorCodeAddress, systemError.ErrorCode(), cmsx::machine::Protection::write);
        uint64_t bufferAddress = processor.Regs().Get(cmsx::machine::regBX);
        if (bufferAddress == 0)
        {
            throw SystemError(EPARAM, "buffer pointer is null");
        }
        uint64_t bufferSize = processor.Regs().Get(cmsx::machine::regCX);
        std::vector<uint8_t> messageBuffer = MakeStringBuffer(systemError.Message(), bufferSize);
        WriteProcessMemory(process, bufferAddress, messageBuffer);
        return 0;
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

void InitExceptionHandlingTraps()
{
    SetTrapHandler(trap_throw, new TrapThrowHandler());
    SetTrapHandler(trap_catch, new TrapCatchHandler());
    SetTrapHandler(trap_resume, new TrapResumeHandler());
    SetTrapHandler(trap_stack_trace, new TrapStackTraceHandler());
    SetTrapHandler(trap_get_system_error, new TrapGetSystemErrorHandler());
}

void DoneExceptionHandlingTraps()
{
    SetTrapHandler(trap_get_system_error, nullptr);
    SetTrapHandler(trap_stack_trace, nullptr);
    SetTrapHandler(trap_resume, nullptr);
    SetTrapHandler(trap_catch, nullptr);
    SetTrapHandler(trap_throw, nullptr);
}

} // namespace cmsx::kernel
