// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMCPPI_BASIC_BLOCK_INCLUDED
#define CMAJOR_CMCPPI_BASIC_BLOCK_INCLUDED
#include <cmajor/cmcppi/Instruction.hpp>
#include <vector>
#include <memory>

namespace cmcppi {

class CMCPPI_API BasicBlock
{
public:
    BasicBlock(int id_, const std::string& name_);
    BasicBlock(const BasicBlock&) = delete;
    BasicBlock& operator=(const BasicBlock&) = delete;
    void AddInstruction(Instruction* instruction);
    void AddChild(BasicBlock* child);
    void SetParent(BasicBlock* parent_);
    BasicBlock* Parent() const { return parent; }
    const std::vector<BasicBlock*>& Children() const { return children; }
    bool IsEmpty() const { return instructions.empty(); }
    int Id() const { return id; }
    void SetId(int id_) { id = id_; }
    int Included() const { return included; }
    void SetIncluded() { included = true; }
    bool IsCleanupBlock() const { return isCleanupBlock; }
    void SetAsCleanupBlock() { isCleanupBlock = true; }
    void SetHandlerBlock(BasicBlock* handlerBlock_) { handlerBlock = handlerBlock_; }
    BasicBlock* HandlerBlock() const { return handlerBlock; }
    void SetCleanupBlock(BasicBlock* cleanupBlock_) { cleanupBlock = cleanupBlock_; }
    BasicBlock* CleanupBlock() const { return cleanupBlock; }
    void Write(CodeFormatter& formatter, Function& function, Context& context);
    void CollectReferencedBasicBlocks(std::set<BasicBlock*>& basicBlocks);
    void SetReferenced() { referenced = true; }
    bool Referenced() const { return referenced; }
private:
    int id;
    bool included;
    bool isCleanupBlock;
    std::string name;
    BasicBlock* parent;
    BasicBlock* handlerBlock;
    BasicBlock* cleanupBlock;
    std::vector<BasicBlock*> children;
    std::vector<std::unique_ptr<Instruction>> instructions;
    bool referenced;
};

} // namespace cmcppi

#endif // CMAJOR_CMCPPI_BASIC_BLOCK_INCLUDED
