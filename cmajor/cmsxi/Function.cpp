// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsxi/Function.hpp>
#include <cmajor/util/Error.hpp>

namespace cmsxi {

Function::Function(const std::string& name_, FunctionType* type_) : Value(), name(name_), type(type_), nextResultNumber(0)
{
    entryBlock.reset(new BasicBlock(basicBlocks.size()));
    for (Type* paramType : type->ParamTypes())
    {
        Instruction* paramInst = new ParamInstruction(paramType);
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
    BasicBlock* bb = new BasicBlock(basicBlocks.size());
    basicBlocks.push_back(std::unique_ptr<BasicBlock>(bb));
    return bb;
}

Value* Function::GetParam(int index) const
{
    Assert(index >= 0 && index < params.size(), "invalid param index");
    return params[index];
}

void Function::Write(CodeFormatter& formatter, Context& context)
{
    if (basicBlocks.empty()) return;
    formatter.WriteLine("function " + type->Name() + " " + name);
    formatter.WriteLine("{");
    formatter.IncIndent();
    bool first = true;
    for (const auto& bb : basicBlocks)
    {
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
