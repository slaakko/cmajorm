// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/Mem2RegPass.hpp>
#include <system-x/intermediate/Visit.hpp>
#include <system-x/intermediate/Code.hpp>
#include <system-x/intermediate/Util.hpp>
#include <system-x/intermediate/PassManager.hpp>
#include <system-x/intermediate/DominatorTreePass.hpp>
#include <system-x/intermediate/DominatorTreeCooper.hpp>

namespace cmsx::intermediate {

bool CanPromote(LocalInstruction* local)
{
    for (Instruction* user : local->Users())
    {
        if (user->IsLoadInstruction())
        {
            // load is OK
        }
        else if (user->IsStoreInstruction())
        {
            StoreInstruction* store = static_cast<StoreInstruction*>(user);
            if (store->GetValue() == local)
            {
                return false;
            }
            else
            {
                // store is OK
            }
        }
        else
        {
            return false;
        }
    }
    return true;
}

std::vector<StoreInstruction*> Stores(LocalInstruction* local)
{
    std::vector<StoreInstruction*> stores;
    for (Instruction* inst : local->Users())
    {
        if (inst->IsStoreInstruction())
        {
            StoreInstruction* store = static_cast<StoreInstruction*>(inst);
            stores.push_back(store);
        }
    }
    return stores;
}

std::vector<LoadInstruction*> Loads(LocalInstruction* local)
{
    std::vector<LoadInstruction*> loads;
    for (Instruction* inst : local->Users())
    {
        if (inst->IsLoadInstruction())
        {
            LoadInstruction* load = static_cast<LoadInstruction*>(inst);
            loads.push_back(load);
        }
    }
    return loads;
}

Mem2Reg::Mem2Reg(DominatorTreeCooper* dominatorTree_) : dominatorTree(dominatorTree_), iterCount(0)
{
}

void Mem2Reg::GetPromotableLocals(const std::vector<BasicBlock*>& blocks)
{
    for (BasicBlock* block : blocks)
    {
        Instruction* inst = block->FirstInstruction();
        while (inst)
        {
            if (inst->IsLocalInstruction())
            {
                LocalInstruction* local = static_cast<LocalInstruction*>(inst);
                if (CanPromote(local))
                {
                    promotableLocals.push_back(local);
                }
            }
            inst = inst->Next();
        }
    }
}

bool Mem2Reg::Dominates(Instruction* left, Instruction* right) const
{
    BasicBlock* leftBlock = left->Parent();
    BasicBlock* rightBlock = right->Parent();
    if (leftBlock == rightBlock)
    {
        int leftIndex = leftBlock->IndexOf(left);
        int rightIndex = rightBlock->IndexOf(right);
        return leftIndex < rightIndex;
    }
    else
    {
        if (dominatorTree->Dominates(leftBlock, rightBlock))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

void Mem2Reg::Run(Function& function)
{
    std::vector<BasicBlock*> blocks = Preorder(function);
    GetPromotableLocals(blocks);
    iterCount = 0;
    for (BasicBlock* block : blocks)
    {
        hasAlready[block] = 0;
        work[block] = 0;
    }
    std::vector<BasicBlock*> w;
    for (LocalInstruction* local : promotableLocals)
    {
        if (local->Users().empty())
        {
            local->Remove();
            continue;
        }
        else
        {
            std::vector<StoreInstruction*> stores = Stores(local);
            if (stores.size() == 1)
            {
                bool allLoadsReplaced = false;
                StoreInstruction* store = stores.front();
                Value* v = store->GetValue();
                std::vector<LoadInstruction*> loads = Loads(local);
                for (LoadInstruction* load : loads)
                {
                    if (Dominates(store, load))
                    {
                        load->ReplaceUsesWith(v);
                        load->Remove();
                    }
                }
                if (allLoadsReplaced)
                {
                    store->Remove();
                    continue;
                }
            }
        }
        locals.push_back(local);
        ++iterCount;
        std::vector<BasicBlock*> a;
        for (Instruction* inst : local->Users())
        {
            AddPtrToSet(inst->Parent(), a);
        }
        for (BasicBlock* x : a)
        {
            work[x] = iterCount;
            AddPtrToSet(x, w);
        }
        while (!w.empty())
        {
            BasicBlock* x = w.front();
            RemovePtrFromSet(x, w);
            for (BasicBlock* y : dominatorTree->DominanceFrontier(x))
            {
                int i = hasAlready[y];
                if (i < iterCount)
                {
                    PhiInstruction* phi = new PhiInstruction(local->GetSourcePos(), local->Result());
                    for (BasicBlock* block : y->Predecessors())
                    {
                        BlockValue blockValue(local->Result(), block);
                        phi->AddBlockValue(blockValue);
                    }
                    y->InsertFront(phi);
                    hasAlready[y] = iterCount;
                    int j = work[y];
                    if (j < iterCount)
                    {
                        work[y] = iterCount;
                        AddPtrToSet(y, w);
                    }
                }
            }
        }
    }
}

RenameVariables::RenameVariables(const std::vector<LocalInstruction*>& locals_, DominatorTreeCooper* dominatorTree_) : locals(locals_), dominatorTree(dominatorTree_)
{
}

void RenameVariables::Run(Function& function)
{
    for (LocalInstruction* local : locals)
    {
        counters[local] = 0;
        stacks[local] = std::stack<int>();
    }
    Search(dominatorTree->Root());
}

void RenameVariables::Search(DominatorTreeNode* node)
{
    BasicBlock* x = node->GetBasicBlock();
    Instruction* inst = x->FirstInstruction();
    while (inst)
    {
        // todo
        inst = inst->Next();
    }
}

Mem2RegPass::Mem2RegPass() : FunctionPass("mem2reg"), mem2reg(nullptr)
{
}

void Mem2RegPass::Run(Function& function)
{
    Pass* dtPass = PassManager::Instance().GetPass("dominator-tree");
    if (!dtPass)
    {
        throw std::runtime_error("dominator-tree pass not found");
    }
    DominatorTreePass* dominatorTreePass = static_cast<DominatorTreePass*>(dtPass);
    DominatorTreeCooper* dominatorTree = dominatorTreePass->GetDominatorTree();
    mem2reg.reset(new Mem2Reg(dominatorTree));
    mem2reg->Run(function);
}

void Mem2RegPass::Clear(Function& function)
{
    mem2reg.reset();
}

} // cmsx::intermediate
