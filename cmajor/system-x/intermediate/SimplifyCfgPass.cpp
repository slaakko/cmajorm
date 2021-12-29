// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/SimplifyCfgPass.hpp>
#include <system-x/intermediate/Code.hpp>
#include <system-x/intermediate/Util.hpp>
#include <set>

namespace cmsx::intermediate {

void Visit(BasicBlock* block, std::set<BasicBlock*>& visited)
{
    visited.insert(block);
    for (BasicBlock* succ : block->Successors())
    {
        if (visited.find(succ) == visited.cend())
        {
            Visit(succ, visited);
        }
    }
}

void RemoveUnreachableBlocks(Function& function)
{
    std::vector<BasicBlock*> toRemove;
    std::set<BasicBlock*> visited;
    Visit(function.FirstBasicBlock(), visited);
    BasicBlock* block = function.FirstBasicBlock();
    while (block)
    {
        if (visited.find(block) == visited.cend())
        {
            AddPtrToSet(block, toRemove);
        }
        block = block->Next();
    }
    for (BasicBlock* block : toRemove)
    {
        function.RemoveBasicBlock(block);
    }
}

void Simplify(Function& function)
{
    RemoveUnreachableBlocks(function);
}

SimplifyCfgPass::SimplifyCfgPass() : FunctionPass("simplify-cfg")
{
}

void SimplifyCfgPass::Run(Function& function)
{
    Simplify(function);
}

} // cmsx::intermediate
