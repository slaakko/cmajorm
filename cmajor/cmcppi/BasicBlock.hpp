// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMCPPI_BASIC_BLOCK_INCLUDED
#define CMAJOR_CMCPPI_BASIC_BLOCK_INCLUDED
#include <cmajor/cmcppi/Instruction.hpp>
#include <vector>

namespace cmcppi {

class CMCPPI_API BasicBlock
{
public:
    BasicBlock(int id_, const std::string& name_);
    BasicBlock(const BasicBlock&) = delete;
    BasicBlock& operator=(const BasicBlock&) = delete;
    void AddInstruction(Instruction* instruction);
    bool IsEmpty() const { return instructions.empty(); }
    int Id() const { return id; }
    void SetId(int id_) { id = id_; }
    void Write(CodeFormatter& formatter, Function& function, Context& context);
private:
    int id;
    std::string name;
    std::vector<std::unique_ptr<Instruction>> instructions;
};

} // namespace cmcppi

#endif // CMAJOR_CMCPPI_BASIC_BLOCK_INCLUDED
