// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/TrapExit.hpp>
#include <system-x/kernel/Trap.hpp>
#include <system-x/kernel/ProcessManager.hpp>

namespace cmsx::kernel {

class TrapExitHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Machine& machine, uint64_t ax, uint64_t bx, uint64_t cx, uint64_t dx) override;
    std::string TrapName() const { return "trap_exit"; }
};

uint64_t TrapExitHandler::HandleTrap(cmsx::machine::Machine& machine, uint64_t ax, uint64_t bx, uint64_t cx, uint64_t dx)
{
    uint8_t exitCode = static_cast<uint8_t>(ax);
    Process* currentProces = ProcessManager::Instance().CurrentProcess();
    if (currentProces)
    {
        currentProces->SetExitCode(exitCode);
    }
    machine.GetProcessor().Exit();
    return 0;
}

void InitTrapExit()
{
    SetTrapHandler(trap_exit, new TrapExitHandler());
}

void DoneTrapExit()
{
    SetTrapHandler(trap_exit, nullptr);
}

} // namespace cmsx::kernel
