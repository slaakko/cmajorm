// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/os/Trap.hpp>
#include <system-x/machine/Interrupt.hpp>
#include <system-x/machine/Machine.hpp>
#include <system-x/machine/Registers.hpp>
#include <soulng/util/TextUtils.hpp>
#include <memory>
#include <vector>

namespace cmsx::os {

using namespace soulng::util;

TrapHandler::~TrapHandler()
{
}

class SoftwareInterruptHandler : public cmsx::machine::InterruptHandler
{
public:
    SoftwareInterruptHandler();
    void HandleInterrupt(cmsx::machine::Machine& machine) override;
    void SetTrapHandler(uint8_t trap, TrapHandler* handler);
private:
    std::vector<std::unique_ptr<TrapHandler>> trapHandlers;
};

SoftwareInterruptHandler::SoftwareInterruptHandler()
{
    trapHandlers.resize(256);
}

void SoftwareInterruptHandler::SetTrapHandler(uint8_t trap, TrapHandler* handler)
{
    trapHandlers[trap].reset(handler);
}

void SoftwareInterruptHandler::HandleInterrupt(cmsx::machine::Machine& machine)
{
    uint64_t trap = machine.Regs().GetSpecial(cmsx::machine::rX);
    uint8_t trapZ = static_cast<uint8_t>(trap);
    trap = trap >> 8;
    uint8_t trapY = static_cast<uint8_t>(trap);
    trap = trap >> 8;
    uint8_t trapX = static_cast<uint8_t>(trap);
    if (trapX == 0 && trapZ == 0)
    {
        TrapHandler* trapHandler = trapHandlers[trapY].get();
        if (trapHandler)
        {
            uint64_t ax = machine.Regs().Get(cmsx::machine::regAX);
            uint64_t bx = machine.Regs().Get(cmsx::machine::regBX);
            uint64_t cx = machine.Regs().Get(cmsx::machine::regCX);
            uint64_t dx = machine.Regs().Get(cmsx::machine::regDX);
            ax = trapHandler->HandleTrap(machine, ax, bx, cx, dx);
            machine.Regs().Set(cmsx::machine::regAX, ax);
        }
        else
        {
            throw std::runtime_error("no handler for TRAP #" + ToHexString(trapY));
        }
    }
    else
    {
        throw std::runtime_error("invalid TRAP #" + ToHexString(trap));
    }
}

void SetTrapHandler(uint8_t trap, TrapHandler* handler)
{
    SoftwareInterruptHandler* softwareInterruptHandler = static_cast<SoftwareInterruptHandler*>(cmsx::machine::GetInterruptHandler(cmsx::machine::irq_software));
    softwareInterruptHandler->SetTrapHandler(trap, handler);
}

void InitTrap()
{
    cmsx::machine::SetInterruptHandler(cmsx::machine::irq_software, new SoftwareInterruptHandler());
}

void DoneTrap()
{
    cmsx::machine::SetInterruptHandler(cmsx::machine::irq_software, nullptr);
}

} // namespace cmsx::os
