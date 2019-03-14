// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsxi/Instruction.hpp>
#include <cmajor/cmsxi/Type.hpp>
#include <cmajor/cmsxi/Context.hpp>
#include <cmajor/cmsxi/Function.hpp>
#include <cmajor/util/Error.hpp>
#include <cmajor/util/TextUtils.hpp>

namespace cmsxi {

Instruction::Instruction() : resultId(-1)
{
}

Type* Instruction::GetType(Context& context)
{
    return context.GetVoidType();
}

std::string Instruction::Name(Context& context)
{
    return "$" + std::to_string(resultId);
}

void Instruction::WriteResult(CodeFormatter& formatter, Function& function, Context& context)
{
    formatter.Write(Format(GetType(context)->Name(), 7, FormatWidth::min));
    resultId = function.GetNextResultNumber();
    formatter.Write(" " + Name(context));
}

UnaryInstruction::UnaryInstruction(Value* arg_) : Instruction(), arg(arg_)
{
}

void UnaryInstruction::WriteArg(CodeFormatter& formatter, Context& context)
{
    formatter.Write(arg->GetType(context)->Name());
    formatter.Write(" ");
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
    Assert(left->GetType(context) == right->GetType(context), "types differ");
    return left->GetType(context);
}

void BinaryInstruction::WriteArgs(CodeFormatter& formatter, Context& context)
{
    formatter.Write(left->GetType(context)->Name());
    formatter.Write(" ");
    formatter.Write(left->Name(context));
    formatter.Write(", ");
    formatter.Write(right->GetType(context)->Name());
    formatter.Write(" ");
    formatter.Write(right->Name(context));
}

NotInstruction::NotInstruction(Value* arg_) : UnaryInstruction(arg_)
{
}

void NotInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = not ");
    WriteArg(formatter, context);
}

NegInstruction::NegInstruction(Value* arg_) : UnaryInstruction(arg_)
{
}

void NegInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = neg ");
    WriteArg(formatter, context);
}

AddInstruction::AddInstruction(Value* left_, Value* right_) : BinaryInstruction(left_, right_)
{
}

void AddInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = add ");
    WriteArgs(formatter, context);
}

SubInstruction::SubInstruction(Value* left_, Value* right_) : BinaryInstruction(left_, right_)
{
}

void SubInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = sub ");
    WriteArgs(formatter, context);
}

MulInstruction::MulInstruction(Value* left_, Value* right_) : BinaryInstruction(left_, right_)
{
}

void MulInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = mul ");
    WriteArgs(formatter, context);
}

DivInstruction::DivInstruction(Value* left_, Value* right_) : BinaryInstruction(left_, right_)
{
}

void DivInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = div ");
    WriteArgs(formatter, context);
}

ModInstruction::ModInstruction(Value* left_, Value* right_) : BinaryInstruction(left_, right_)
{
}

void ModInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = mod ");
    WriteArgs(formatter, context);
}

AndInstruction::AndInstruction(Value* left_, Value* right_) : BinaryInstruction(left_, right_)
{
}

void AndInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = and ");
    WriteArgs(formatter, context);
}

OrInstruction::OrInstruction(Value* left_, Value* right_) : BinaryInstruction(left_, right_)
{
}

void OrInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = or ");
    WriteArgs(formatter, context);
}

XorInstruction::XorInstruction(Value* left_, Value* right_) : BinaryInstruction(left_, right_)
{
}

void XorInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = xor ");
    WriteArgs(formatter, context);
}

ShlInstruction::ShlInstruction(Value* left_, Value* right_) : BinaryInstruction(left_, right_)
{
}

void ShlInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = shl ");
    WriteArgs(formatter, context);
}

ShrInstruction::ShrInstruction(Value* left_, Value* right_) : BinaryInstruction(left_, right_)
{
}

void ShrInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = shr ");
    WriteArgs(formatter, context);
}

EqualInstruction::EqualInstruction(Value* left_, Value* right_) : BinaryInstruction(left_, right_)
{
}

Type* EqualInstruction::GetType(Context& context)
{
    return context.GetBoolType();
}

void EqualInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = equal ");
    WriteArgs(formatter, context);
}

LessInstruction::LessInstruction(Value* left_, Value* right_) : BinaryInstruction(left_, right_)
{
}

Type* LessInstruction::GetType(Context& context)
{
    return context.GetBoolType();
}

void LessInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = less ");
    WriteArgs(formatter, context);
}

SignExtendInstruction::SignExtendInstruction(Value* arg_, Type* destType_) : UnaryTypeInstruction(arg_, destType_)
{
}

void SignExtendInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = signextend ");
    WriteArg(formatter, context);
}

ZeroExtendInstruction::ZeroExtendInstruction(Value* arg_, Type* destType_) : UnaryTypeInstruction(arg_, destType_)
{
}

void ZeroExtendInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = zeroextend ");
    WriteArg(formatter, context);
}

TruncateInstruction::TruncateInstruction(Value* arg_, Type* destType_) : UnaryTypeInstruction(arg_, destType_)
{
}

void TruncateInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = truncate ");
    WriteArg(formatter, context);
}

BitCastInstruction::BitCastInstruction(Value* arg_, Type* destType_) : UnaryTypeInstruction(arg_, destType_)
{
}

void BitCastInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = bitcast ");
    WriteArg(formatter, context);
}

IntToFloatInstruction::IntToFloatInstruction(Value* arg_, Type* destType_) : UnaryTypeInstruction(arg_, destType_)
{
}

void IntToFloatInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = inttofloat ");
    WriteArg(formatter, context);
}

FloatToIntInstruction::FloatToIntInstruction(Value* arg_, Type* destType_) : UnaryTypeInstruction(arg_, destType_)
{
}

void FloatToIntInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = floattoint ");
    WriteArg(formatter, context);
}

IntToPtrInstruction::IntToPtrInstruction(Value* arg_, Type* destType_) : UnaryTypeInstruction(arg_, destType_)
{
}

void IntToPtrInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = inttoptr ");
    WriteArg(formatter, context);
}

PtrToIntInstruction::PtrToIntInstruction(Value* arg_, Type* destType_) : UnaryTypeInstruction(arg_, destType_)
{
}

void PtrToIntInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = ptrtoint ");
    WriteArg(formatter, context);
}

ParamInstruction::ParamInstruction(Type* type_) : Instruction(), type(type_)
{
}

void ParamInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = param ");
}

LocalInstruction::LocalInstruction(Type* type_) : Instruction(), type(type_)
{
}

Type* LocalInstruction::GetType(Context& context)
{
    return context.GetPtrType(type);
}

void LocalInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = local ");
    formatter.Write(type->Name());
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

void LoadInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = load ");
    formatter.Write(ptr->GetType(context)->Name());
    formatter.Write(" ");
    formatter.Write(ptr->Name(context));
}

StoreInstruction::StoreInstruction(Value* value_, Value* ptr_) : Instruction(), value(value_), ptr(ptr_)
{
}

void StoreInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    formatter.Write(Format("store ", 8));
    formatter.Write(value->GetType(context)->Name());
    formatter.Write(" ");
    formatter.Write(value->Name(context));
    formatter.Write(", ");
    formatter.Write(ptr->GetType(context)->Name());
    formatter.Write(" ");
    formatter.Write(ptr->Name(context));
}

ArgInstruction::ArgInstruction(Value* arg_) : Instruction(), arg(arg_)
{
    if (arg == nullptr)
    {
        int x = 0;
    }
}

void ArgInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    formatter.Write(Format("arg ", 8));
    formatter.Write(arg->GetType(context)->Name());
    formatter.Write(" ");
    formatter.Write(arg->Name(context));
}

ElemAddrInstruction::ElemAddrInstruction(Value* ptr_, Value* index_) : Instruction(), ptr(ptr_), index(index_)
{
}

Type* ElemAddrInstruction::GetType(Context& context)
{
    Type* type = ptr->GetType(context);
    Assert(type->IsPtrType(), "pointer type expected");
    PtrType* ptrType = static_cast<PtrType*>(ptr->GetType(context));
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

void ElemAddrInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = elemaddr ");
    formatter.Write(ptr->GetType(context)->Name());
    formatter.Write(" ");
    formatter.Write(ptr->Name(context));
    formatter.Write(", ");
    formatter.Write(index->GetType(context)->Name());
    formatter.Write(" ");
    formatter.Write(index->Name(context));
}

PtrOffsetInstruction::PtrOffsetInstruction(Value* ptr_, Value* offset_) : Instruction(), ptr(ptr_), offset(offset_)
{
}

void PtrOffsetInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = ptroffset ");
    formatter.Write(ptr->GetType(context)->Name());
    formatter.Write(" ");
    formatter.Write(ptr->Name(context));
    formatter.Write(", ");
    formatter.Write(offset->GetType(context)->Name());
    formatter.Write(" ");
    formatter.Write(offset->Name(context));
}

PtrDiffInstruction::PtrDiffInstruction(Value* leftPtr_, Value* rightPtr_) : Instruction(), leftPtr(leftPtr_), rightPtr(rightPtr_)
{
}

Type* PtrDiffInstruction::GetType(Context& context)
{
    return context.GetLongType();
}

void PtrDiffInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = ptrdiff ");
    formatter.Write(leftPtr->GetType(context)->Name());
    formatter.Write(" ");
    formatter.Write(leftPtr->Name(context));
    formatter.Write(", ");
    formatter.Write(rightPtr->GetType(context)->Name());
    formatter.Write(" ");
    formatter.Write(rightPtr->Name(context));
}

CallInstruction::CallInstruction(Value* function_) : Instruction(), function(function_)
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

void CallInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    if (GetType(context)->IsVoidType())
    {
        formatter.Write(Format("call ", 8));
    }
    else
    {
        WriteResult(formatter, function, context);
        formatter.Write(" = call ");
    }
    formatter.Write(this->function->GetType(context)->Name());
    formatter.Write(" ");
    formatter.Write(this->function->Name(context));
}

RetInstruction::RetInstruction(Value* value_) : Instruction(), value(value_)
{
}

void RetInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    formatter.Write(Format("ret ", 8));
    if (value)
    {
        formatter.Write(value->GetType(context)->Name());
        formatter.Write(" ");
        formatter.Write(value->Name(context));
    }
    else
    {
        formatter.Write("void");
    }
}

JumpInstruction::JumpInstruction(BasicBlock* dest_) : Instruction(), dest(dest_)
{
}

void JumpInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    formatter.Write(Format("jmp ", 8));
    formatter.Write("@" + std::to_string(dest->Id()));
}

BranchInstruction::BranchInstruction(Value* cond_, BasicBlock* trueDest_, BasicBlock* falseDest_) : Instruction(), cond(cond_), trueDest(trueDest_), falseDest(falseDest_)
{
}

void BranchInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    formatter.Write(Format("branch ", 8));
    formatter.Write(cond->GetType(context)->Name());
    formatter.Write(" ");
    formatter.Write(cond->Name(context));
    formatter.Write(", ");
    formatter.Write("@" + std::to_string(trueDest->Id()));
    formatter.Write(", ");
    formatter.Write("@" + std::to_string(falseDest->Id()));
}

SwitchInstruction::SwitchInstruction(Value* cond_, BasicBlock* defaultDest_) : Instruction(), cond(cond_), defaultDest(defaultDest_), destinations()
{
}

void SwitchInstruction::AddCase(Value* caseValue, BasicBlock* dest)
{
    destinations.push_back(std::make_pair(caseValue, dest));
}

void SwitchInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    formatter.Write(Format("switch ", 8));
    formatter.Write(cond->GetType(context)->Name());
    formatter.Write(" ");
    formatter.Write(cond->Name(context));
    formatter.Write(" ");
    formatter.Write("@" + std::to_string(defaultDest->Id()));
    formatter.Write(", [");
    bool first = true;
    for (const auto& p : destinations)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            formatter.Write(" : ");
        }
        Value* value = p.first;
        BasicBlock* dest = p.second;
        formatter.Write(value->GetType(context)->Name());
        formatter.Write(" ");
        formatter.Write(value->Name(context));
        formatter.Write(", ");
        formatter.Write("@" + std::to_string(dest->Id()));
    }
    formatter.Write("]");
}

TrapInstruction::TrapInstruction(Value* b0_, Value* b1_, Value* b2_) : Instruction(), b0(b0_), b1(b1_), b2(b2_)
{
}

Type* TrapInstruction::GetType(Context& context)
{
    return context.GetLongType();
}

void TrapInstruction::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    WriteResult(formatter, function, context);
    formatter.Write(" = trap ");
    formatter.Write(b0->GetType(context)->Name());
    formatter.Write(" ");
    formatter.Write(b0->Name(context));
    formatter.Write(", ");
    formatter.Write(b1->GetType(context)->Name());
    formatter.Write(" ");
    formatter.Write(b1->Name(context));
    formatter.Write(", ");
    formatter.Write(b2->GetType(context)->Name());
    formatter.Write(" ");
    formatter.Write(b2->Name(context));
}

} // namespace cmsxi
