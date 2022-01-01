// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/machine/InitDone.hpp>
#include <system-x/machine/OpCode.hpp>
#include <system-x/machine/Registers.hpp>
#include <system-x/machine/Interrupt.hpp>
#include <system-x/machine/Config.hpp>

namespace cmsx::machine {

void Init()
{
    InitOpCode();
    InitReg();
    InitInterrupt();
    InitConfig();
}

void Done()
{
    DoneConfig();
    DoneInterrupt();
    DoneReg();
    DoneOpCode();
}

} // cmsx::machine
