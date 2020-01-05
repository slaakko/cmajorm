// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSXI_BASIC_BLOCK_INCLUDED
#define CMAJOR_CMSXI_BASIC_BLOCK_INCLUDED
#include <cmajor/cmsxi/Instruction.hpp>
#include <vector>

namespace cmsxi {

class CMSXI_API BasicBlock
{
public:
    BasicBlock(int id_);
    BasicBlock(const BasicBlock&) = delete;
    BasicBlock& operator=(const BasicBlock&) = delete;
    void AddInstruction(Instruction* instruction);
    bool IsEmpty() const { return instructions.empty(); }
    int Id() const { return id; }
    void SetId(int id_) { id = id_; }
    void Write(CodeFormatter& formatter, Function& function, Context& context);
private:
    int id;
    std::vector<std::unique_ptr<Instruction>> instructions;
};

} // namespace cmsxi

#endif // CMAJOR_CMSXI_BASIC_BLOCK_INCLUDED
