// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/assembler/InitDone.hpp>
#include <system-x/assembler/OpCode.hpp>
#include <system-x/assembler/AssemblyInstruction.hpp>

namespace cmsx::assembler {

void Init()
{
    InitOpCode();
    InitAssemblyInstruction();
}

void Done()
{
    DoneAssemblyInstruction();
    DoneOpCode();
}

} // namespace cmsx::assembler
