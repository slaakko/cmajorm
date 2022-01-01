// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/os/TrapExit.hpp>
#include <system-x/os/Trap.hpp>

namespace cmsx::os {

class TrapExitHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Machine& machine, uint64_t ax, uint64_t bx, uint64_t cx, uint64_t dx) override;
};

uint64_t TrapExitHandler::HandleTrap(cmsx::machine::Machine& machine, uint64_t ax, uint64_t bx, uint64_t cx, uint64_t dx)
{
    uint8_t exitCode = static_cast<uint8_t>(ax);
    machine.GetProcessor().Exit(exitCode);
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

} // namespace cmsx::os
