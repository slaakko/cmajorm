// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/MathTraps.hpp>
#include <system-x/kernel/Trap.hpp>
#include <cmath>

namespace cmsx::kernel {

class TrapPowHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_pow"; }
};

uint64_t TrapPowHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    int64_t lx = processor.Regs().Get(cmsx::machine::regAX);
    int64_t ly = processor.Regs().Get(cmsx::machine::regBX);
    double x = *static_cast<double*>(static_cast<void*>(&lx));
    double y = *static_cast<double*>(static_cast<void*>(&ly));
    double result = std::pow(x, y);
    int64_t r = *static_cast<int64_t*>(static_cast<void*>(&result));
    return static_cast<uint64_t>(r);
}

void InitMathTraps()
{
    SetTrapHandler(trap_pow, new TrapPowHandler());
}

void DoneMathTraps()
{
    SetTrapHandler(trap_pow, nullptr);
}

} // namespace cmsx::kernel
