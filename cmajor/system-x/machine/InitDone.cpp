// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/machine/InitDone.hpp>
#include <system-x/machine/OpCode.hpp>

namespace cmsx::machine {

void Init()
{
    OpCodeInit();
}

void Done()
{
    OpCodeDone();
}

} // cmsx::machine
