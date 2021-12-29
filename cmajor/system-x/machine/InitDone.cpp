// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/machine/InitDone.hpp>
#include <system-x/machine/OpCode.hpp>
#include <system-x/machine/Registers.hpp>

namespace cmsx::machine {

void Init()
{
    InitOpCode();
    InitReg();
}

void Done()
{
    DoneReg();
    DoneOpCode();
}

} // cmsx::machine
