// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcppi/Function.hpp>
#include <cmajor/cmcppi/Context.hpp>
#include <cmajor/cmdebug/DebugInfoIo.hpp>
#include <soulng/util/Error.hpp>

namespace cmcppi {

Function::Function(const std::string& name_, FunctionType* type_, Context& context) : Value(), name(name_), type(type_), nextResultNumber(0), nextLocalNumber(0), nextArgumentNumber(0), 
    linkOnce(false), nextBBNumber(0), nothrow(false), fileIndex(-1), functionId(), nopResultDeclarationWritten(false)
{
    entryBlock.reset(new BasicBlock(nextBBNumber++, "entry"));
    int paramIndex = 0;
    for (Type* paramType : type->ParamTypes())
    {
        ParamInstruction* paramInst = new ParamInstruction(paramType, "__param" + std::to_string(paramIndex));
        context.AddLineInfoScopeIdAndFlags(paramInst);
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

void Function::Finalize(bool hasCleanup)
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
    if (hasCleanup)
    {
        RemoveNothrow();
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
    formatter.Write(")");
    if (nothrow)
    {
        formatter.Write(" noexcept");
    }
    formatter.WriteLine(";");
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

void Function::Write(CodeFormatter& formatter, Context& context, BinaryWriter& writer, int32_t& numFunctions)
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
    formatter.Write(")");
    if (nothrow)
    {
        formatter.Write(" noexcept");
    }
    formatter.WriteLine();
    int32_t numInsts = 0;
    uint32_t numInstsPos = 0;
    if (fileIndex != -1 && !functionId.is_nil())
    {
        cmajor::debug::WriteCompileUnitFunctionRecord(writer, fileIndex, functionId);
        numInstsPos = writer.Pos();
        cmajor::debug::WriteNumberOfInstructionRecords(writer, numInsts);
        StartFunctionInstruction startInst;
        startInst.SetSourceLineNumber(-1);
        startInst.SetCppLineNumber(formatter.Line());
        startInst.SetCppLineIndex(0);
        startInst.SetScopeId(-1);
        startInst.SetFlags(16); // InstructionFlags::startFunction
        startInst.WriteDebugInfoRecord(writer, numInsts);
    }
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("// " + fullName);
    formatter.WriteLine();
    WriteValueDeclarations(formatter, context);
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
        bb->Write(formatter, *this, context, writer, numInsts);
    }
    formatter.DecIndent();
    formatter.WriteLine("}");
    if (fileIndex != -1 && !functionId.is_nil())
    {
        uint32_t currentPos = writer.Pos();
        writer.Seek(numInstsPos);
        cmajor::debug::WriteNumberOfInstructionRecords(writer, numInsts);
        writer.Seek(currentPos);
        int32_t numScopes = scopes.size();
        cmajor::debug::WriteNumberOfScopes(writer, numScopes);
        for (const std::unique_ptr<Scope>& scope : scopes)
        {
            int32_t numLocalVariables = scope->LocalVariables().size();
            cmajor::debug::WriteScopeRecord(writer, scope->Id(), scope->ParentScopeId(), numLocalVariables);
            for (int32_t i = 0; i < numLocalVariables; ++i)
            {
                cmajor::debug::DIVariable* localVariable = scope->GetLocalVariable(i);
                localVariable->Write(writer);
            }
        }
        ++numFunctions;
    }
}

void Function::WriteValueDeclarations(CodeFormatter& formatter, Context& context)
{
    for (Instruction* inst : resultInstructions)
    {
        inst->ObtainResultId(*this);
        inst->WriteResultDeclaration(formatter, *this, context);
        inst->SetLineNumbers(formatter, context);
    }
}

void Function::AddResultInstruction(Instruction* instruction)
{
    resultInstructions.push_back(instruction);
}

void Function::SetFileIndex(int32_t fileIndex_)
{
    fileIndex = fileIndex_;
}

void Function::SetFunctionId(const boost::uuids::uuid& functionId_)
{
    functionId = functionId_;
}

void Function::AddScope(Scope* scope)
{
    scopes.push_back(std::unique_ptr<Scope>(scope));
}

Scope* Function::GetScope(int16_t scopeId)
{
    return scopes[scopeId].get();
}

} // namespace cmcppi
