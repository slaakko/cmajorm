// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_OS_TRAP_INCLUDED
#define CMSX_OS_TRAP_INCLUDED
#include <system-x/os/Api.hpp>
#include <system-x/machine/Machine.hpp>

namespace cmsx::os {

const uint8_t trap_exit = 0;

class CMSX_OS_API TrapHandler
{
public:
    virtual ~TrapHandler();
    virtual uint64_t HandleTrap(cmsx::machine::Machine& machine, uint64_t ax, uint64_t bx, uint64_t cx, uint64_t dx) = 0;
};

CMSX_OS_API void SetTrapHandler(uint8_t trap, TrapHandler* handler);
CMSX_OS_API void InitTrap();
CMSX_OS_API void DoneTrap();

} // namespace cmsx::os

#endif // CMSX_OS_TRAP_INCLUDED
