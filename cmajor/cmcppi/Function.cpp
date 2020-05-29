// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcppi/Function.hpp>
#include <cmajor/cmcppi/Context.hpp>
#include <soulng/util/Error.hpp>

namespace cmcppi {

Function::Function(const std::string& name_, FunctionType* type_, Context& context) : Value(), name(name_), type(type_), nextResultNumber(0), nextLocalNumber(0), nextArgumentNumber(0), 
    linkOnce(false), nextBBNumber(0)
{
    context.SetCurrentFunction(this);
    entryBlock.reset(new BasicBlock(nextBBNumber++, "entry"));
    int paramIndex = 0;
    for (Type* paramType : type->ParamTypes())
    {
        ParamInstruction* paramInst = new ParamInstruction(paramType, "__param" + std::to_string(paramIndex));
        context.AddLineInfo(paramInst);
        params.push_back(std::unique_ptr<ParamInstruction>(paramInst));
        ++paramIndex;
    }
}

BasicBlock* Function::CreateBasicBlock(const std::string& name, Context& context)
{
    if (basicBlocks.empty())
    {
        BasicBlock* bb = entryBlock.get();
        bb->SetReferenced();
        basicBlocks.push_back(std::move(entryBlock));
        return bb;
    }
    BasicBlock* bb = new BasicBlock(nextBBNumber++, name);
    basicBlocks.push_back(std::unique_ptr<BasicBlock>(bb));
    bb->SetParent(context.CurrentParent());
    return bb;
}

BasicBlock* Function::CreateCleanupBasicBlock(Context& context)
{
    BasicBlock* cubb = new BasicBlock(-1, "cleanup");
    cleanupBasicBlocks.push_back(std::unique_ptr<BasicBlock>(cubb));
    cubb->SetParent(context.CurrentParent());
    return cubb;
}

void Function::Finalize()
{
    nextBBNumber = 0;
    for (std::unique_ptr<BasicBlock>& cubb : cleanupBasicBlocks)
    {
        if (cubb->Referenced())
        {
            basicBlocks.push_back(std::move(cubb));
        }
    }
    RemoveUnreferencedBasicBlocks();
    for (auto& bb : basicBlocks)
    {
        if (!bb->Referenced())
        {
            continue;
        }
        bb->SetId(nextBBNumber++);
    }
}

Value* Function::GetParam(int index) const
{
    Assert(index >= 0 && index < params.size(), "invalid param index");
    return params[index].get();
}

void Function::SetFullName(const std::string& functionName)
{
    fullName = functionName;
}

void Function::WriteDeclaration(CodeFormatter& formatter, Context& context)
{
    std::string once;
    if (linkOnce)
    {
        once = "inline ";
    }
    formatter.Write(once + type->ReturnType()->Name() + " " + name + "(");
    for (int i = 0; i < type->ParamTypes().size(); ++i)
    {
        if (i > 0)
        {
            formatter.Write(", ");
        }
        formatter.Write(params[i]->GetType(context)->Name() + " " + params[i]->Name(context));
    }
    formatter.WriteLine(");");
}

void Function::RemoveUnreferencedBasicBlocks()
{
    std::set<BasicBlock*> referencedBasicBlocks;
    for (const auto& bb : basicBlocks)
    {
        bb->CollectReferencedBasicBlocks(referencedBasicBlocks);
    }
    for (BasicBlock* bb : referencedBasicBlocks)
    {
        bb->SetReferenced();
    }
}

void Function::Write(CodeFormatter& formatter, Context& context)
{
    if (basicBlocks.empty()) return;
    context.SetCurrentBasicBlock(nullptr);
    context.SetCurrentFunction(this);
    std::string once;
    if (linkOnce)
    {
        once = "inline ";
    }
    formatter.Write(once + type->ReturnType()->Name() + " " + name + "(");
    for (int i = 0; i < type->ParamTypes().size(); ++i)
    {
        if (i > 0)
        {
            formatter.Write(", ");
        }
        formatter.Write(params[i]->GetType(context)->Name() + " " + params[i]->Name(context));
    }
    formatter.WriteLine(")");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("// " + fullName);
    formatter.WriteLine();
    bool first = true;
    for (const auto& bb : basicBlocks)
    {
        if (bb->Included())
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

} // namespace cmcppi
