// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#define _CRT_RAND_S
#include <system-x/kernel/TrapRandom.hpp>
#include <system-x/kernel/Trap.hpp>
#include <system-x/kernel/Error.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/machine/Processor.hpp>
#include <stdlib.h>

namespace cmsx::kernel {

class TrapRandomSeedHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_random_seed"; }
};

uint64_t TrapRandomSeedHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    try
    {
        unsigned int seed = 0;
        errno_t error = rand_s(&seed);
        if (error == 0)
        {
            return static_cast<uint64_t>(seed);
        }
        else
        {
            throw SystemError(EFAIL, "rand_s failed", __FUNCTION__);
        }
    }
    catch (const SystemError& error)
    {
        Process* process = static_cast<Process*>(processor.CurrentProcess());
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
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
