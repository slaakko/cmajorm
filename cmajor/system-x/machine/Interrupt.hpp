// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_MACHINE_INTERRUPT_INCLUDED
#define CMSX_MACHINE_INTERRUPT_INCLUDED
#include <system-x/machine/Api.hpp>
#include <stdint.h>

namespace cmsx::machine {

class Machine;

const uint8_t irq_software = 0;
const uint64_t SOFTWARE_INTERRUPT_BIT = static_cast<uint64_t>(1) << irq_software;

class CMSX_MACHINE_API InterruptHandler
{
public:
    virtual ~InterruptHandler();
    virtual void HandleInterrupt(Machine& machine) = 0;
};

CMSX_MACHINE_API void SetInterruptHandler(uint8_t irq, InterruptHandler* handler);
CMSX_MACHINE_API InterruptHandler* GetInterruptHandler(uint8_t irq);
CMSX_MACHINE_API void InitInterrupt();
CMSX_MACHINE_API void DoneInterrupt();

} // cmsx::machine

#endif // CMSX_MACHINE_INTERRUPT_INCLUDED
