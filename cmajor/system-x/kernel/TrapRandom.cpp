// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#define _CRT_RAND_S
#include <system-x/kernel/TrapRandom.hpp>
#include <system-x/kernel/Trap.hpp>
#include <stdlib.h>

namespace cmsx::kernel {

class TrapRandomSeedHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Machine& machine, uint64_t ax, uint64_t bx, uint64_t cx, uint64_t dx) override;
    std::string TrapName() const { return "trap_random_seed"; }
};

uint64_t TrapRandomSeedHandler::HandleTrap(cmsx::machine::Machine& machine, uint64_t ax, uint64_t bx, uint64_t cx, uint64_t dx)
{
    unsigned int seed = 0;
    errno_t error = rand_s(&seed);
    if (error != 0)
    {
        return static_cast<uint64_t>(-1);
    }
    return static_cast<uint64_t>(seed);
}

void InitTrapRandom()
{
    SetTrapHandler(trap_random_seed, new TrapRandomSeedHandler());
}

void DoneTrapRandom()
{
    SetTrapHandler(trap_random_seed, nullptr);
}

} // namespace cmsx::kernel
