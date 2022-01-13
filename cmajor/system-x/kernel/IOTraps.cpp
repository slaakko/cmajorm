// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/IOTraps.hpp>
#include <system-x/kernel/IO.hpp>
#include <system-x/kernel/Trap.hpp>
#include <system-x/kernel/Process.hpp>

namespace cmsx::kernel {

class TrapWriteHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_write"; }
};

uint64_t TrapWriteHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int32_t fd = static_cast<int32_t>(processor.Regs().Get(cmsx::machine::regAX));
        int64_t bufferAddr = static_cast<int64_t>(processor.Regs().Get(cmsx::machine::regBX));
        int64_t count = static_cast<int64_t>(processor.Regs().Get(cmsx::machine::regCX));
        return Write(process, fd, bufferAddr, count);
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

void InitIOTraps()
{
    SetTrapHandler(trap_write, new TrapWriteHandler());
}

void DoneIOTraps()
{
    SetTrapHandler(trap_write, nullptr);
}

} // namespace cmsx::kernel
