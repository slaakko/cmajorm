// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsxi/Function.hpp>
#include <cmajor/cmsxi/Context.hpp>
#include <soulng/util/Error.hpp>

namespace cmsxi {

Function::Function(const std::string& name_, FunctionType* type_, Context& context) : Value(), name(name_), type(type_), nextResultNumber(0), linkOnce(false), mdId(-1), nextBBNumber(0)
{
    entryBlock.reset(new BasicBlock(nextBBNumber++));
    for (Type* paramType : type->ParamTypes())
    {
        Instruction* paramInst = new ParamInstruction(paramType);
        context.AddLineInfo(paramInst);
        entryBlock->AddInstruction(paramInst);
        params.push_back(paramInst);
    }
}

BasicBlock* Function::CreateBasicBlock()
{
    if (basicBlocks.empty())
    {
        BasicBlock* bb = entryBlock.get();
        basicBlocks.push_back(std::move(entryBlock));
        return bb;
    }
    BasicBlock* bb = new BasicBlock(nextBBNumber++);
    basicBlocks.push_back(std::unique_ptr<BasicBlock>(bb));
    return bb;
}

BasicBlock* Function::CreateCleanupBasicBlock()
{
    BasicBlock* cubb = new BasicBlock(-1);
    cleanupBasicBlocks.push_back(std::unique_ptr<BasicBlock>(cubb));
    return cubb;
}

void Function::Finalize()
{
    nextBBNumber = 0;
    for (std::unique_ptr<BasicBlock>& cubb : cleanupBasicBlocks)
    {
        basicBlocks.push_back(std::move(cubb));
    }
    for (auto& bb : basicBlocks)
    {
        if (bb->IsEmpty())
        {
            continue;
        }
        bb->SetId(nextBBNumber++);
    }
}

Value* Function::GetParam(int index) const
{
    Assert(index >= 0 && index < params.size(), "invalid param index");
    return params[index];
}

void Function::Write(CodeFormatter& formatter, Context& context)
{
    if (basicBlocks.empty()) return;
    std::string once;
    if (linkOnce)
    {
        once = " once";
    }
    std::string mdIdStr;
    if (mdId != -1)
    {
        mdIdStr = " !" + std::to_string(mdId);
    }
    formatter.WriteLine("function " + type->Name() + once + " " + name + mdIdStr);
    formatter.WriteLine("{");
    formatter.IncIndent();
    bool first = true;
    for (const auto& bb : basicBlocks)
    {
        if (bb->IsEmpty())
        {
            continue;
        }
        if (first)
        {
            first = false;
        }
        else
        {
            formatter.WriteLine();
        }
        bb->Write(formatter, *this, context);
    }
    formatter.DecIndent();
    formatter.WriteLine("}");
}

} // namespace cmsxi
