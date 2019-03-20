// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsxi/Function.hpp>
#include <cmajor/cmsxi/Context.hpp>
#include <cmajor/util/Error.hpp>

namespace cmsxi {

Function::Function(const std::string& name_, FunctionType* type_, Context& context) : Value(), name(name_), type(type_), nextResultNumber(0), linkOnce(false), mdId(-1)
{
    entryBlock.reset(new BasicBlock(basicBlocks.size()));
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
