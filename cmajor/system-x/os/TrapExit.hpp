// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_OS_TRAP_EXIT_INCLUDED
#define CMSX_OS_TRAP_EXIT_INCLUDED
#include <system-x/os/Api.hpp>
#include <system-x/machine/Machine.hpp>

namespace cmsx::os {

CMSX_OS_API void InitTrapExit();
CMSX_OS_API void DoneTrapExit();

} // namespace cmsx::os

#endif // CMSX_OS_TRAP_EXIT_INCLUDED
