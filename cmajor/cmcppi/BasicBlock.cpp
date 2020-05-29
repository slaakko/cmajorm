// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcppi/BasicBlock.hpp>
#include <cmajor/cmcppi/Context.hpp>
#include <soulng/util/TextUtils.hpp>

namespace cmcppi {

BasicBlock::BasicBlock(int id_, const std::string& name_) : id(id_), name(name_), included(false), isCleanupBlock(false), parent(nullptr), handlerBlock(nullptr), referenced(false), cleanupBlock(nullptr)
{
}

void BasicBlock::AddInstruction(Instruction* instruction)
{
    instructions.push_back(std::unique_ptr<Instruction>(instruction));
}

void BasicBlock::AddChild(BasicBlock* child)
{
    children.push_back(child);
}

void BasicBlock::SetParent(BasicBlock* parent_)
{
    if (parent_ != nullptr)
    {
        parent = parent_;
        parent->AddChild(this);
    }
}

void BasicBlock::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    if (!Referenced()) return;
    BasicBlock* prevBB = context.GetCurrentBasicBlock();
    context.SetCurrentBasicBlock(this);
    int indent = formatter.IndentSize();
    bool indentDecremented = false;
    if (formatter.CurrentIndent() > 0)
    {
        formatter.DecIndent();
        indentDecremented = true;
    }
    if (!IsCleanupBlock())
    {
        formatter.Write(Format("__bb" + std::to_string(id) + ":", indent, FormatWidth::min));
    }
    bool first = true;
    std::string n = "["+ name + "] ";
    int ni = instructions.size();
    if (ni == 0)
    {
        formatter.WriteLine("// " + n);
        if (indentDecremented)
        {
            formatter.IncIndent();
        }
        formatter.WriteLine(";");
    }
    for (int i = 0; i < ni; ++i)
    {
        Instruction* inst = instructions[i].get();
        formatter.WriteLine("// " + n + inst->IrName() + " : source line=" + std::to_string(inst->SourceLineNumber()) + ":");
        if (first)
        {
            if (indentDecremented)
            {
                formatter.IncIndent();
            }
            first = false;
        }
        inst->Write(formatter, function, context);
        if (!inst->NoSemicolon())
        {
            formatter.WriteLine(";");
        }
        n.clear();
    }
    if (cleanupBlock != nullptr)
    {
        if (!cleanupBlock->Included())
        {
            cleanupBlock->SetIncluded();
            cleanupBlock->Write(formatter, function, context);
        }
    }
    for (BasicBlock* child : children)
    {
        if (child == handlerBlock)
        {
            continue;
        }
        if (child == cleanupBlock)
        {
            continue;
        }
        if (!child->Included())
        {
            child->SetIncluded();
            formatter.WriteLine();
            child->Write(formatter, function, context);
        }
    }
    if (handlerBlock != nullptr)
    {
        if (!handlerBlock->Included())
        {
            handlerBlock->SetIncluded();
            formatter.WriteLine();
            handlerBlock->Write(formatter, function, context);
        }
    }
    context.SetCurrentBasicBlock(prevBB);
}

void BasicBlock::CollectReferencedBasicBlocks(std::set<BasicBlock*>& basicBlocks)
{
    for (const auto& inst : instructions)
    {
        inst->CollectReferencedBasicBlocks(this, basicBlocks);
    }
}

} // namespace cmcppi
