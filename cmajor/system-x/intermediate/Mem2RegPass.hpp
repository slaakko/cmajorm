// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_MEM2REG_PASS_INCLUDED
#define CMSX_INTERMEDIATE_MEM2REG_PASS_INCLUDED
#include <system-x/intermediate/Pass.hpp>
#include <map>
#include <vector>
#include <memory>
#include <stack>

namespace cmsx::intermediate {

class DominatorTreeCooper;
class DominatorTreeNode;
class BasicBlock;
class Instruction;
class LocalInstruction;

struct CMSX_INTERMEDIATE_API Mem2Reg
{
    Mem2Reg(DominatorTreeCooper* dominatorTree_);
    void GetPromotableLocals(const std::vector<BasicBlock*>& blocks);
    bool Dominates(Instruction* left, Instruction* right) const;
    void Run(Function& function);
    DominatorTreeCooper* dominatorTree;
    std::vector<LocalInstruction*> promotableLocals;
    std::vector<LocalInstruction*> locals;
    std::map<BasicBlock*, int> hasAlready;
    std::map<BasicBlock*, int> work;
    int iterCount;
};

struct CMSX_INTERMEDIATE_API RenameVariables
{
    RenameVariables(const std::vector<LocalInstruction*>& locals_, DominatorTreeCooper* dominatorTree_);
    void Run(Function& function);
    void Search(DominatorTreeNode* node);
    std::vector<LocalInstruction*> locals;
    DominatorTreeCooper* dominatorTree;
    std::map<LocalInstruction*, std::stack<int>> stacks;
    std::map<LocalInstruction*, int> counters;
};

class CMSX_INTERMEDIATE_API Mem2RegPass : public FunctionPass
{
public:
    Mem2RegPass();
    void Run(Function& function) override;
    void Clear(Function& function) override;
private:
    std::unique_ptr<Mem2Reg> mem2reg;
};

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_MEM2REG_PASS_INCLUDED
