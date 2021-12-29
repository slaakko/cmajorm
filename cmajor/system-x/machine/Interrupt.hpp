// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_MACHINE_INTERRUPT_INCLUDED
#define CMSX_MACHINE_INTERRUPT_INCLUDED
#include <system-x/machine/Api.hpp>
#include <stdint.h>

namespace cmsx::machine {

const uint8_t irq_software = 0;
const uint64_t SOFTWARE_INTERRUPT_BIT = static_cast<uint64_t>(1) << irq_software;

} // cmsx::machine

#endif // CMSX_MACHINE_INTERRUPT_INCLUDED
