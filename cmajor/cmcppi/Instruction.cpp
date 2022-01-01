// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcppi/Instruction.hpp>
#include <cmajor/cmcppi/Type.hpp>
#include <cmajor/cmcppi/Context.hpp>
#include <cmajor/cmcppi/Function.hpp>
#include <cmajor/cmdebug/DebugInfoIo.hpp>
#include <soulng/util/Error.hpp>
#include <soulng/util/TextUtils.hpp>

namespace cmcppi {

Instruction::Instruction() : resultId(-1), span(), cppLineNumber(0), cppLineIndex(0), scopeId(-1), flags(0), noSemicolon(false)
{
}

Type* Instruction::GetType(Context& context)
{
    return context.GetVoidType();
}

std::string Instruction::Name(Context& context)
{
    return "__v" + std::to_string(resultId);
}

void Instruction::ObtainResultId(Function& function)
{
    resultId = function.GetNextResultNumber();
}

void Instruction::WriteResultDeclaration(CodeFormatter& formatter, Function& function, Context& context)
{
    formatter.Write(Format(GetType(context)->Name(), 7, FormatWidth::min));
    formatter.WriteLine(" " + Name(context) + ";");
}

void Instruction::WriteResult(CodeFormatter& formatter, Function& function, Context& context)
{
    formatter.Write(Name(context));
}

void Instruction::CollectReferencedBasicBlocks(BasicBlock* parent, std::set<BasicBlock*>& basicBlocks)
{
}

void Instruction::SetLineNumbers(CodeFormatter& formatter, Context& context)
{
    cppLineNumber = formatter.Line() + 1; // formatter.Line() is a comment, formatter.Line() + 1 is the line number of the instruction itself
    if (span.line != 0 && context.GetSourceSpan() == span)
    {
        context.SetCppLineIndex(context.CppLineIndex() + 1);
    }
    else
    {
        context.SetCppLineIndex(0);
    }
    cppLineIndex = context.CppLineIndex();
    context.SetSourceSpan(span);
}

void Instruction::WriteDebugInfoRecord(BinaryWriter& writer, int32_t& numInsts)
{
    if (span.line != 0)
    {
        cmajor::debug::WriteInstructionRecord(writer, cppLineNumber, span, cppLineIndex, scopeId, flags);
        ++numInsts;
    }
}

UnaryInstruction::UnaryInstruction(Value* arg_) : Instruction(), arg(arg_)
{
}

void UnaryInstruction::WriteArg(CodeFormatter& formatter, Context& context)
{
    formatter.Write(arg->Name(context));
}

UnaryTypeInstruction::UnaryTypeInstruction(Value* arg_, Type* type_) : UnaryInstruction(arg_), type(type_)
{
}

BinaryInstruction::BinaryInstruction(Value* left_, Value* right_) : Instruction(), left(left_), right(right_)
{
}

Type* BinaryInstruction::GetType(Context& context)
{
    return left->GetType(context);
}

void BinaryInstruction::WriteArgs(CodeFormatter& formatter, Context& context, const std::string& op)
{
    formatter.Write(left->Name(context));
    formatter.Write(" " + op + " ");
    formatter.Write(right->Name(context));
}

NotInstruction::NotInstruction(Value* arg_) : UnaryInstruction(arg_)
{
}

void NotInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    if (Arg()->GetType(context)->Id() == boolTypeId)
    {
        formatter.Write(" = !");
    }
    else
    {
        formatter.Write(" = ~");
    }
    WriteArg(formatter, context);
}

NegInstruction::NegInstruction(Value* arg_) : UnaryInstruction(arg_)
{
}

void NegInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = -");
    WriteArg(formatter, context);
}

AddInstruction::AddInstruction(Value* left_, Value* right_) : BinaryInstruction(left_, right_)
{
}

void AddInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = ");
    WriteArgs(formatter, context, "+");
}

SubInstruction::SubInstruction(Value* left_, Value* right_) : BinaryInstruction(left_, right_)
{
}

void SubInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = ");
    WriteArgs(formatter, context, "-");
}

MulInstruction::MulInstruction(Value* left_, Value* right_) : BinaryInstruction(left_, right_)
{
}

void MulInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = ");
    WriteArgs(formatter, context, "*");
}

DivInstruction::DivInstruction(Value* left_, Value* right_) : BinaryInstruction(left_, right_)
{
}

void DivInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = ");
    WriteArgs(formatter, context, "/");
}

ModInstruction::ModInstruction(Value* left_, Value* right_) : BinaryInstruction(left_, right_)
{
}

void ModInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = ");
    WriteArgs(formatter, context, "%");
}

AndInstruction::AndInstruction(Value* left_, Value* right_) : BinaryInstruction(left_, right_)
{
}

void AndInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = ");
    WriteArgs(formatter, context, "&");
}

OrInstruction::OrInstruction(Value* left_, Value* right_) : BinaryInstruction(left_, right_)
{
}

void OrInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = ");
    WriteArgs(formatter, context, "|");
}

XorInstruction::XorInstruction(Value* left_, Value* right_) : BinaryInstruction(left_, right_)
{
}

void XorInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = ");
    WriteArgs(formatter, context, "^");
}

ShlInstruction::ShlInstruction(Value* left_, Value* right_) : BinaryInstruction(left_, right_)
{
}

void ShlInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = ");
    WriteArgs(formatter, context, "<<");
}

ShrInstruction::ShrInstruction(Value* left_, Value* right_) : BinaryInstruction(left_, right_)
{
}

void ShrInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" =  ");
    WriteArgs(formatter, context, ">>");
}

EqualInstruction::EqualInstruction(Value* left_, Value* right_) : BinaryInstruction(left_, right_)
{
}

Type* EqualInstruction::GetType(Context& context)
{
    return context.GetBoolType();
}

void EqualInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = ");
    WriteArgs(formatter, context, "==");
}

LessInstruction::LessInstruction(Value* left_, Value* right_) : BinaryInstruction(left_, right_)
{
}

Type* LessInstruction::GetType(Context& context)
{
    return context.GetBoolType();
}

void LessInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = ");
    WriteArgs(formatter, context, "<");
}

SignExtendInstruction::SignExtendInstruction(Value* arg_, Type* destType_) : UnaryTypeInstruction(arg_, destType_)
{
}

void SignExtendInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = ");
    WriteArg(formatter, context);
}

ZeroExtendInstruction::ZeroExtendInstruction(Value* arg_, Type* destType_) : UnaryTypeInstruction(arg_, destType_)
{
}

void ZeroExtendInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = ");
    WriteArg(formatter, context);
}

TruncateInstruction::TruncateInstruction(Value* arg_, Type* destType_) : UnaryTypeInstruction(arg_, destType_)
{
}

void TruncateInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = static_cast<" + GetType(context)->Name() + ">(");
    WriteArg(formatter, context);
    formatter.Write(")");
}

BitCastInstruction::BitCastInstruction(Value* arg_, Type* destType_) : UnaryTypeInstruction(arg_, destType_)
{
}

void BitCastInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    if (Arg()->IsNullValue())
    {
        formatter.Write(" = ");
        WriteArg(formatter, context);
    }
    else
    {
        if (GetType(context)->IsPrimitiveType())
        {
            formatter.Write(" = static_cast<" + GetType(context)->Name() + ">(");
        }
        else
        {
            formatter.Write(" = reinterpret_cast<" + GetType(context)->Name() + ">(");
        }
        WriteArg(formatter, context);
        formatter.Write(")");
    }
}

IntToFloatInstruction::IntToFloatInstruction(Value* arg_, Type* destType_) : UnaryTypeInstruction(arg_, destType_)
{
}

void IntToFloatInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = static_cast<" + GetType(context)->Name() + ">(");
    WriteArg(formatter, context);
    formatter.Write(")");
}

FloatToIntInstruction::FloatToIntInstruction(Value* arg_, Type* destType_) : UnaryTypeInstruction(arg_, destType_)
{
}

void FloatToIntInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = static_cast<" + GetType(context)->Name() + ">(");
    WriteArg(formatter, context);
    formatter.Write(")");
}

IntToPtrInstruction::IntToPtrInstruction(Value* arg_, Type* destType_) : UnaryTypeInstruction(arg_, destType_)
{
}

void IntToPtrInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = reinterpret_cast<" + GetType(context)->Name() + ">(");
    WriteArg(formatter, context);
    formatter.Write(")");
}

PtrToIntInstruction::PtrToIntInstruction(Value* arg_, Type* destType_) : UnaryTypeInstruction(arg_, destType_)
{
}

void PtrToIntInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = *reinterpret_cast<" + GetType(context)->Name() + "*>(&");
    WriteArg(formatter, context);
    formatter.Write(")");
}

ParamInstruction::ParamInstruction(Type* type_, const std::string& paramName_) : Instruction(), type(type_), paramName(paramName_)
{
}

void ParamInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
}

LocalInstruction::LocalInstruction(Type* type_) : Instruction(), type(type_)
{
}

Type* LocalInstruction::GetType(Context& context)
{
    return context.GetPtrType(type);
}

void LocalInstruction::ObtainResultId(Function& function)
{
    Instruction::ObtainResultId(function);
    ObtainLocalName(function);
}

void LocalInstruction::ObtainLocalName(Function& function)
{
    if (localName.empty())
    {
        int localNumber = function.GetNextLocalNumber();
        localName = "__local" + std::to_string(localNumber);
    }
}

void LocalInstruction::WriteResultDeclaration(CodeFormatter& formatter, Function& function, Context& context)
{
    Instruction::WriteResultDeclaration(formatter, function, context);
    formatter.WriteLine(type->Name() + " " + localName + ";");
}

void LocalInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = &" + localName);
}

LoadInstruction::LoadInstruction(Value* ptr_) : Instruction(), ptr(ptr_)
{
}

Type* LoadInstruction::GetType(Context& context)
{
    Assert(ptr->GetType(context)->IsPtrType(), "pointer type expected");
    PtrType* ptrType = static_cast<PtrType*>(ptr->GetType(context));
    return ptrType->BaseType();
}

void LoadInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = *" + ptr->Name(context));
}

StoreInstruction::StoreInstruction(Value* value_, Value* ptr_) : Instruction(), value(value_), ptr(ptr_)
{
}

void StoreInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    formatter.Write("*" + ptr->Name(context) + " = " + value->Name(context));
}

ArgInstruction::ArgInstruction(Value* arg_) : Instruction(), arg(arg_)
{
}

void ArgInstruction::ObtainResultId(Function& function)
{
    argName = "__arg" + std::to_string(function.GetNextArgumentNumber());
}

void ArgInstruction::WriteResultDeclaration(CodeFormatter& formatter, Function& function, Context& context)
{
    formatter.WriteLine(arg->GetType(context)->Name() + " " + argName + ";");
}

void ArgInstruction::WriteResult(CodeFormatter& formatter, Function& function, Context& context)
{
    formatter.Write(argName);
}

void ArgInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = " + arg->Name(context));
}

ElemAddrInstruction::ElemAddrInstruction(Value* ptr_, Value* index_) : Instruction(), ptr(ptr_), index(index_)
{
}

Type* ElemAddrInstruction::GetType(Context& context)
{
    Type* type = ptr->GetType(context);
    Assert(type->IsPtrType(), "pointer type expected");
    PtrType* ptrType = static_cast<PtrType*>(type);
    Type* aggregateType = ptrType->BaseType();
    if (aggregateType->IsStructureType())
    {
        if (index->IsLongValue())
        {
            int64_t idx = static_cast<LongValue*>(index)->GetValue();
            StructureType* structureType = static_cast<StructureType*>(aggregateType);
            return context.GetPtrType(structureType->GetMemberType(idx));
        }
        else
        {
            Assert(false, "long valued index expected");
            return nullptr;
        }
    }
    else if (aggregateType->IsArrayType())
    {
        ArrayType* arrayType = static_cast<ArrayType*>(aggregateType);
        return context.GetPtrType(arrayType->ElementType());
    }
    else
    {
        Assert(false, "structure or array type expected");
        return nullptr;
    }
}

void ElemAddrInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    Type* type = ptr->GetType(context);
    Assert(type->IsPtrType(), "pointer type expected");
    PtrType* ptrType = static_cast<PtrType*>(type);
    Type* aggregateType = ptrType->BaseType();
    if (aggregateType->IsStructureType())
    {
        WriteResult(formatter, function, context);
        formatter.Write(" = &");
        formatter.Write(ptr->Name(context));
        formatter.Write("->m");
        formatter.Write(index->Name(context));
    }
    else if (aggregateType->IsArrayType())
    {
        WriteResult(formatter, function, context);
        formatter.Write(" = &");
        formatter.Write("(*");
        formatter.Write(ptr->Name(context));
        formatter.Write(")[");
        formatter.Write(index->Name(context));
        formatter.Write("]");
    }
    else 
    {
        Assert(false, "structure or array type expected");
    }
}

PtrOffsetInstruction::PtrOffsetInstruction(Value* ptr_, Value* offset_) : Instruction(), ptr(ptr_), offset(offset_)
{
}

void PtrOffsetInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = (");
    formatter.Write(ptr->GetType(context)->Name());
    formatter.Write(")");
    formatter.Write(ptr->Name(context));
    formatter.Write(" + ");
    formatter.Write(offset->Name(context));
}

PtrDiffInstruction::PtrDiffInstruction(Value* leftPtr_, Value* rightPtr_) : Instruction(), leftPtr(leftPtr_), rightPtr(rightPtr_)
{
}

Type* PtrDiffInstruction::GetType(Context& context)
{
    return context.GetLongType();
}

void PtrDiffInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = (");
    formatter.Write(leftPtr->GetType(context)->Name());
    formatter.Write(")");
    formatter.Write(leftPtr->Name(context));
    formatter.Write(" - (");
    formatter.Write(rightPtr->GetType(context)->Name());
    formatter.Write(")");
    formatter.Write(rightPtr->Name(context));
}

CallInstruction::CallInstruction(Value* function_, const std::vector<Value*>& args_) : Instruction(), function(function_), args(args_)
{
}

Type* CallInstruction::GetType(Context& context)
{
    Type* type = function->GetType(context);
    if (type->IsPtrType())
    {
        PtrType* ptrType = static_cast<PtrType*>(type);
        type = ptrType->BaseType();
    }
    if (type->IsFunctionType())
    {
        FunctionType* functionType = static_cast<FunctionType*>(type);
        return functionType->ReturnType();
    }
    else
    {
        Assert(false, "function or function pointer type expected");
        return nullptr;
    }
}

bool CallInstruction::IsResultInstruction(Context& context)
{
    Type* type = GetType(context);
    if (!type->IsVoidType())
    {
        return true;
    }
    return false;
}

void CallInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    if (!GetType(context)->IsVoidType())
    {
        WriteResult(formatter, function, context);
        formatter.Write(" = ");
    }
    Type* type = this->function->GetType(context);
    if (type->IsPtrType())
    {
        formatter.Write("(*" + this->function->Name(context) + ")");
    }
    else
    {
        formatter.Write(this->function->Name(context));
    }
    formatter.Write("(");
    bool first = true;
    for (Value* arg : args)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            formatter.Write(", ");
        }
        formatter.Write(arg->Name(context));
    }
    formatter.Write(")");
}

InvokeInstruction::InvokeInstruction(Value* function_, const std::vector<Value*> args_, BasicBlock* normalBlockNext_, BasicBlock* unwindBlockNext_) :
    function(function_), args(args_), normalBlockNext(normalBlockNext_), unwindBlockNext(unwindBlockNext_)
{
}

Type* InvokeInstruction::GetType(Context& context)
{
    Type* type = function->GetType(context);
    if (type->IsPtrType())
    {
        PtrType* ptrType = static_cast<PtrType*>(type);
        type = ptrType->BaseType();
    }
    if (type->IsFunctionType())
    {
        FunctionType* functionType = static_cast<FunctionType*>(type);
        return functionType->ReturnType();
    }
    else
    {
        Assert(false, "function or function pointer type expected");
        return nullptr;
    }
}

bool InvokeInstruction::IsResultInstruction(Context& context)
{
    Type* type = GetType(context);
    if (!type->IsVoidType())
    {
        return true;
    }
    return false;
}

void InvokeInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    if (!GetType(context)->IsVoidType())
    {
        WriteResult(formatter, function, context); 
        formatter.WriteLine(";");
    }
    if (!GetType(context)->IsVoidType())
    {
        formatter.Write(Name(context));
        formatter.Write(" = ");
    }
    Type* type = this->function->GetType(context);
    if (type->IsPtrType())
    {
        formatter.Write("(*" + this->function->Name(context) + ")");
    }
    else
    {
        formatter.Write(this->function->Name(context));
    }
    formatter.Write("(");
    bool first = true;
    for (Value* arg : args)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            formatter.Write(", ");
        }
        formatter.Write(arg->Name(context));
    }
    formatter.WriteLine(");");
    formatter.WriteLine("goto __bb" + std::to_string(normalBlockNext->Id()) + ";");
    SetNoSemicolon();
}

void InvokeInstruction::CollectReferencedBasicBlocks(BasicBlock* parent, std::set<BasicBlock*>& basicBlocks)
{
    basicBlocks.insert(normalBlockNext);
    basicBlocks.insert(unwindBlockNext);
}

RetInstruction::RetInstruction(Value* value_) : Instruction(), value(value_)
{
}

void RetInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    formatter.Write(Format("return", 8));
    if (value)
    {
        formatter.Write(value->Name(context));
    }
}

JumpInstruction::JumpInstruction(BasicBlock* dest_) : Instruction(), dest(dest_)
{
}

void JumpInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    formatter.Write(Format("goto ", 8));
    formatter.Write("__bb" + std::to_string(dest->Id()));
}

void JumpInstruction::CollectReferencedBasicBlocks(BasicBlock* parent, std::set<BasicBlock*>& basicBlocks)
{
    basicBlocks.insert(dest);
}

BranchInstruction::BranchInstruction(Value* cond_, BasicBlock* trueDest_, BasicBlock* falseDest_) : Instruction(), cond(cond_), trueDest(trueDest_), falseDest(falseDest_)
{
}

void BranchInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    formatter.Write("if (");
    formatter.Write(cond->Name(context));
    formatter.Write(") ");
    formatter.Write("goto __bb" + std::to_string(trueDest->Id()) + ";");
    formatter.Write(" else ");
    formatter.Write("goto __bb" + std::to_string(falseDest->Id()));
}

void BranchInstruction::CollectReferencedBasicBlocks(BasicBlock* parent, std::set<BasicBlock*>& basicBlocks)
{
    basicBlocks.insert(trueDest);
    basicBlocks.insert(falseDest);
}

SwitchInstruction::SwitchInstruction(Value* cond_, BasicBlock* defaultDest_) : Instruction(), cond(cond_), defaultDest(defaultDest_), destinations()
{
}

void SwitchInstruction::AddCase(Value* caseValue, BasicBlock* dest)
{
    destinations.push_back(std::make_pair(caseValue, dest));
}

void SwitchInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    formatter.Write("switch (");
    formatter.Write(cond->Name(context));
    formatter.WriteLine(")");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("default: goto __bb" + std::to_string(defaultDest->Id()) + ";");
    for (const auto& p : destinations)
    {
        Value* value = p.first;
        BasicBlock* dest = p.second;
        formatter.Write("case " + value->Name(context));
        formatter.Write(": goto __bb");
        formatter.WriteLine(std::to_string(dest->Id()) + ";");
    }
    formatter.DecIndent();
    formatter.WriteLine("}");
    SetNoSemicolon();
}

void SwitchInstruction::CollectReferencedBasicBlocks(BasicBlock* parent, std::set<BasicBlock*>& basicBlocks)
{
    if (defaultDest)
    {
        basicBlocks.insert(defaultDest);
    }
    for (const auto& destination : destinations)
    {
        basicBlocks.insert(destination.second);
    }
}

NoOperationInstruction::NoOperationInstruction() : Instruction()
{
}

void NoOperationInstruction::WriteResultDeclaration(CodeFormatter& formatter, Function& function, Context& context)
{
    if (function.NopResultDeclarationWritten()) return;
    function.SetNopResultDeclarationWritten();
    formatter.WriteLine("int __nop;");
}

void NoOperationInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    formatter.Write("__nop = 0");
}

BeginTryInstruction::BeginTryInstruction() : Instruction()
{
}

void BeginTryInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    formatter.WriteLine("try");
    formatter.WriteLine("{");
    formatter.IncIndent();
    SetNoSemicolon();
}

EndTryInstruction::EndTryInstruction(BasicBlock* nextDest_) : Instruction(), nextDest(nextDest_)
{
}

void EndTryInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    if (nextDest)
    {
        formatter.WriteLine("goto __bb" + std::to_string(nextDest->Id()) + ";");
    }
    for (BasicBlock* child : context.GetCurrentBasicBlock()->Children())
    {
        if (child == context.GetCurrentBasicBlock()->HandlerBlock())
        {
            continue;
        }
        if (child->IsCleanupBlock())
        {
            continue;
        }
        if (!child->Included())
        {
            child->SetIncluded();
            formatter.WriteLine();
            child->Write(formatter, function, context, writer, numInsts, writeDebugInfoRecords);
            SetNoSemicolon();
        }
    }
    if (nextDest)
    {
        formatter.WriteLine("goto __bb" + std::to_string(nextDest->Id()) + ";");
    }
    formatter.DecIndent();
    formatter.WriteLine("}");
    SetNoSemicolon();
}

void EndTryInstruction::CollectReferencedBasicBlocks(BasicBlock* parent, std::set<BasicBlock*>& basicBlocks)
{
    basicBlocks.insert(parent);
    if (nextDest)
    {
        basicBlocks.insert(nextDest);
    }
}

BeginCatchInstruction::BeginCatchInstruction() : Instruction()
{
}

void BeginCatchInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    formatter.WriteLine("catch (...)");
    formatter.WriteLine("{");
    formatter.IncIndent();
    SetNoSemicolon();
}

void BeginCatchInstruction::CollectReferencedBasicBlocks(BasicBlock* parent, std::set<BasicBlock*>& basicBlocks)
{
    basicBlocks.insert(parent);
}

EndCatchInstruction::EndCatchInstruction(BasicBlock* nextDest_) : Instruction(), nextDest(nextDest_)
{
}

void EndCatchInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    for (BasicBlock* child : context.GetCurrentBasicBlock()->Children())
    {
        if (child == context.GetCurrentBasicBlock()->HandlerBlock())
        {
            continue;
        }
        if (!child->Included())
        {
            child->SetIncluded();
            child->Write(formatter, function, context, writer, numInsts, writeDebugInfoRecords);
            formatter.WriteLine();
            SetNoSemicolon();
        }
    }
    formatter.DecIndent();
    formatter.WriteLine("}");
    if (nextDest)
    {
        formatter.Write("goto __bb" + std::to_string(nextDest->Id()));
    }
    else
    {
        SetNoSemicolon();
    }
}

void EndCatchInstruction::CollectReferencedBasicBlocks(BasicBlock* parent, std::set<BasicBlock*>& basicBlocks)
{
    basicBlocks.insert(parent);
    if (nextDest)
    {
        basicBlocks.insert(nextDest);
    }
}

ResumeInstruction::ResumeInstruction() : Instruction()
{
}

void ResumeInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    formatter.Write("throw");
}

IncludeBasicBlockInstruction::IncludeBasicBlockInstruction(BasicBlock* block_) : block(block_)
{
}

void IncludeBasicBlockInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
    if (!block->Included())
    {
        block->SetIncluded();
        block->Write(formatter, function, context, writer, numInsts, writeDebugInfoRecords);
        formatter.WriteLine();
        SetNoSemicolon();
    }
}

void IncludeBasicBlockInstruction::CollectReferencedBasicBlocks(BasicBlock* parent, std::set<BasicBlock*>& basicBlocks)
{
    basicBlocks.insert(parent);
    basicBlocks.insert(block);
}

StartFunctionInstruction::StartFunctionInstruction()
{
}

void StartFunctionInstruction::Write(CodeFormatter& formatter, Function& function, Context& context, BinaryWriter& writer, int32_t& numInsts, bool writeDebugInfoRecords)
{
}

} // namespace cmcppi
