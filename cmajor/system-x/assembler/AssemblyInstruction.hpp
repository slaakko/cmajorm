// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_ASSEMBLER_ASSEMBLY_INSTRUCTION_INCLUDED
#define CMSX_ASSEMBLER_ASSEMBLY_INSTRUCTION_INCLUDED
#include <system-x/assembler/Instruction.hpp>

namespace cmsx::assembler {

class Assembler;

class CMSX_ASSEMBLER_API AssemblyInstruction
{
public:
    AssemblyInstruction();
    virtual ~AssemblyInstruction();
    virtual void Assemble(Assembler& assembler) = 0;
};

CMSX_ASSEMBLER_API AssemblyInstruction* GetAssemblyInstruction(int opc);
CMSX_ASSEMBLER_API void InitAssemblyInstruction();
CMSX_ASSEMBLER_API void DoneAssemblyInstruction();

} // namespace cmsx::assembler

#endif // CMSX_ASSEMBLER_ASSEMBLY_INSTRUCTION_INCLUDED
