// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsystemx/Value.hpp>
#include <cmajor/cmsystemx/Context.hpp>
#include <cmajor/cmsystemx/OpCode.hpp>
#include <cmajor/cmsystemx/Function.hpp>
#include <cmajor/util/Error.hpp>
#include <ostream>

namespace cmsystemx {

Value::Value(Kind kind_) : kind(kind_), hex(false)
{
}

Value::~Value()
{
}

Value* Value::CreateCallInst(int numLocalRegs, Context& context)
{
    Assert(false, "invalid call creator value");
    return nullptr;
}

Value* Value::SignExtend(Type* type, Context& context)
{
    Assert(false, "sign extend not implemented for this value");
    return nullptr;
}

std::string OperatorStr(Operator op)
{
    switch (op)
    {
        case Operator::unaryPlus: return "+";
        case Operator::unaryMinus: return"-";
        case Operator::complement: return "~";
        case Operator::registerize: return "$";
        case Operator::mul: return "*";
        case Operator::div: return "/";
        case Operator::mod: return "%";
        case Operator::shiftLeft: return "<<";
        case Operator::shiftRight: return ">>";
        case Operator::bitAnd: return "&";
        case Operator::add: return "+";
        case Operator::sub: return "-";
        case Operator::bitOr: return "|";
        case Operator::bitXor: return "^";
    }
    return "";
}

UnaryExpr::UnaryExpr(Operator op_, Value* operand_) : Value(Kind::unaryExpr), op(op_), operand(operand_)
{
}

void UnaryExpr::Write(std::ostream& s)
{
    s << OperatorStr(op);
    operand->Write(s);
}

Type* UnaryExpr::GetType(Context& context)
{
    return operand->GetType(context);
}

BinaryExpr::BinaryExpr(Operator op_, Value* left_, Value* right_) : Value(Kind::binaryExpr), op(op_), left(left_), right(right_)
{
}

void BinaryExpr::Write(std::ostream& s)
{
    left->Write(s);
    s << OperatorStr(op);
    right->Write(s);
}

Type* BinaryExpr::GetType(Context& context)
{
    return left->GetType(context);
}

ParenExpr::ParenExpr(Value* operand_) : Value(Kind::parenExpr), operand(operand_)
{
}

void ParenExpr::Write(std::ostream& s)
{
    s << "(";
    operand->Write(s);
    s << ")";
}

Type* ParenExpr::GetType(Context& context)
{
    return operand->GetType(context);
}

SByteValue::SByteValue() : Value(Kind::sbyteValue), value(0)
{
}

SByteValue::SByteValue(int8_t value_) : Value(Kind::sbyteValue), value(value_)
{
}

void SByteValue::Write(std::ostream& s)
{
    s << int32_t(value);
}

Type* SByteValue::GetType(Context& context)
{
    return context.GetSByteType();
}

void SByteValue::StoreTo(Value* target, Context& context)
{
    target->StoreValue(this, context);
}

Value* SByteValue::SignExtend(Type* type, Context& context)
{
    return type->GetValue(value, context);
}

ByteValue::ByteValue() : Value(Kind::byteValue), value(0)
{
}

ByteValue::ByteValue(uint8_t value_) : Value(Kind::byteValue), value(value_)
{
}

void ByteValue::Write(std::ostream& s)
{
    if (Hex())
    {
        s << "#" << std::hex << uint32_t(value);
    }
    else
    {
        s << uint32_t(value);
    }
}

Type* ByteValue::GetType(Context& context)
{
    return context.GetByteType();
}

void ByteValue::StoreTo(Value* target, Context& context)
{
    target->StoreValue(this, context);
}

ShortValue::ShortValue() : Value(Kind::shortValue), value(0)
{
}

ShortValue::ShortValue(int16_t value_) : Value(Kind::shortValue), value(value_)
{
}

void ShortValue::Write(std::ostream& s)
{
    s << int32_t(value);
}

Type* ShortValue::GetType(Context& context)
{
    return context.GetShortType();
}

void ShortValue::StoreTo(Value* target, Context& context)
{
    target->StoreValue(this, context);
}

Value* ShortValue::SignExtend(Type* type, Context& context)
{
    return type->GetValue(value, context);
}

UShortValue::UShortValue() : Value(Kind::ushortValue), value(0)
{
}

UShortValue::UShortValue(uint16_t value_) : Value(Kind::ushortValue), value(value_)
{
}

void UShortValue::Write(std::ostream& s)
{
    if (Hex())
    {
        s << "#" << std::hex << uint32_t(value);
    }
    else
    {
        s << uint32_t(value);
    }
}

Type* UShortValue::GetType(Context& context)
{
    return context.GetUShortType();
}

void UShortValue::StoreTo(Value* target, Context& context)
{
    target->StoreValue(this, context);
}

IntValue::IntValue() : Value(Kind::intValue), value(0)
{
}

IntValue::IntValue(int32_t value_) : Value(Kind::intValue), value(value_)
{
}

void IntValue::Write(std::ostream& s)
{
    s << value;
}

Type* IntValue::GetType(Context& context)
{
    return context.GetIntType();
}

void IntValue::StoreTo(Value* target, Context& context)
{
    target->StoreValue(this, context);
}

Value* IntValue::SignExtend(Type* type, Context& context)
{
    return type->GetValue(value, context);
}

UIntValue::UIntValue() : Value(Kind::uintValue), value(0)
{
}

UIntValue::UIntValue(uint32_t value_) : Value(Kind::uintValue), value(value_)
{
}

void UIntValue::Write(std::ostream& s)
{
    if (Hex())
    {
        s << "#" << std::hex << value;
    }
    else
    {
        s << value;
    }
}

Type* UIntValue::GetType(Context& context)
{
    return context.GetUIntType();
}

void UIntValue::StoreTo(Value* target, Context& context)
{
    target->StoreValue(this, context);
}

LongValue::LongValue() : Value(Kind::longValue), value(0)
{
}

LongValue::LongValue(int64_t value_) : Value(Kind::longValue), value(value_)
{
}

void LongValue::Write(std::ostream& s)
{
    s << value;
}

Type* LongValue::GetType(Context& context)
{
    return context.GetLongType();
}

void LongValue::StoreTo(Value* target, Context& context)
{
    target->StoreValue(this, context);
}

Value* LongValue::SignExtend(Type* type, Context& context)
{
    return type->GetValue(value, context);
}

ULongValue::ULongValue() : Value(Kind::ulongValue), value(0)
{
}

ULongValue::ULongValue(uint64_t value_) : Value(Kind::ulongValue), value(value_)
{
}

void ULongValue::Write(std::ostream& s)
{
    if (Hex())
    {
        s << "#" << std::hex << value;
    }
    else
    {
        s << value;
    }
}

Type* ULongValue::GetType(Context& context)
{
    return context.GetULongType();
}

void ULongValue::StoreTo(Value* target, Context& context)
{
    target->StoreValue(this, context);
}

BoolValue::BoolValue() : Value(Kind::boolValue), value(false)
{
}

BoolValue::BoolValue(bool value_) : Value(Kind::boolValue), value(value_)
{
}

void BoolValue::Write(std::ostream& s)
{
    s << int32_t(value);
}

Type* BoolValue::GetType(Context& context)
{
    return context.GetBoolType();
}

void BoolValue::StoreTo(Value* target, Context& context)
{
    target->StoreValue(this, context);
}

CharValue::CharValue() : Value(Kind::charValue), value('\0')
{
}

CharValue::CharValue(unsigned char value_) : Value(Kind::charValue), value(value_)
{
}

void CharValue::Write(std::ostream& s)
{
    s << uint32_t(value);
}

Type* CharValue::GetType(Context& context)
{
    return context.GetCharType();
}

void CharValue::StoreTo(Value* target, Context& context)
{
    target->StoreValue(this, context);
}

WCharValue::WCharValue() : Value(Kind::wcharValue), value('\0')
{
}

WCharValue::WCharValue(char16_t value_) : Value(Kind::wcharValue), value(value_)
{
}

void WCharValue::Write(std::ostream& s)
{
    s << uint32_t(value);
}

Type* WCharValue::GetType(Context& context)
{
    return context.GetWCharType();
}

void WCharValue::StoreTo(Value* target, Context& context)
{
    target->StoreValue(this, context);
}

UCharValue::UCharValue() : Value(Kind::ucharValue), value('\0')
{
}

UCharValue::UCharValue(char32_t value_) : Value(Kind::ucharValue), value(value_)
{
}

void UCharValue::Write(std::ostream& s)
{
    s << uint32_t(value);
}

Type* UCharValue::GetType(Context& context)
{
    return context.GetUCharType();
}

void UCharValue::StoreTo(Value* target, Context& context)
{
    target->StoreValue(this, context);
}

FloatValue::FloatValue() : Value(Kind::floatValue), value(0.0)
{
}

FloatValue::FloatValue(float value_) : Value(Kind::floatValue), value(value_)
{
}

void FloatValue::Write(std::ostream& s)
{
    s << value;
}

Type* FloatValue::GetType(Context& context)
{
    return context.GetFloatType();
}

void FloatValue::StoreTo(Value* target, Context& context)
{
    target->StoreValue(this, context);
}

DoubleValue::DoubleValue() : Value(Kind::doubleValue), value(0.0)
{
}

DoubleValue::DoubleValue(double value_) : Value(Kind::doubleValue), value(value_)
{
}

void DoubleValue::Write(std::ostream& s)
{
    s << value;
}

Type* DoubleValue::GetType(Context& context)
{
    return context.GetDoubleType();
}

void DoubleValue::StoreTo(Value* target, Context& context)
{
    target->StoreValue(this, context);
}

NullValue::NullValue(PtrType* ptrType_) : Value(Kind::nullValue), ptrType(ptrType_)
{
}

void NullValue::Write(std::ostream& s)
{
    s << "0";
}

Type* NullValue::GetType(Context& context)
{
    return ptrType;
}

void NullValue::StoreTo(Value* target, Context& context)
{
    target->StoreValue(this, context);
}

Register::Register(Type* type_) : Value(Kind::register_), type(type_)
{
}

Type* Register::GetType(Context& context)
{
    return type;
}

void Register::StoreArg(Argument* arg, Value* target, Context& context)
{
    target->StoreRegArg(this, arg, context);
}

void Register::StoreLocalReg(LocalRegister* reg, Context& context)
{
    Instruction* setInst = context.CreateInstruction(SET);
    setInst->AddOperand(this);
    setInst->AddOperand(reg);
}

void Register::StoreValue(SByteValue* value, Context& context)
{
    Instruction* setInst = context.CreateInstruction(SET);
    setInst->AddOperand(this);
    setInst->AddOperand(value);
}

void Register::StoreValue(ByteValue* value, Context& context)
{
    Instruction* setInst = context.CreateInstruction(SET);
    setInst->AddOperand(this);
    setInst->AddOperand(value);
}

void Register::StoreValue(ShortValue* value, Context& context)
{
    Instruction* setInst = context.CreateInstruction(SET);
    setInst->AddOperand(this);
    setInst->AddOperand(value);
}

void Register::StoreValue(UShortValue* value, Context& context)
{
    Instruction* setInst = context.CreateInstruction(SET);
    setInst->AddOperand(this);
    setInst->AddOperand(value);
}

void Register::StoreValue(IntValue* value, Context& context)
{
    if (value->GetValue() >= 0 && value->GetValue() <= std::numeric_limits<uint16_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(this);
        setInst->AddOperand(value);
    }
    else
    {
        uint64_t val = static_cast<uint64_t>(value->GetValue());
        StoreConstant(val, context);
    }
}

void Register::StoreValue(UIntValue* value, Context& context)
{
    if (value->GetValue() <= std::numeric_limits<uint16_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(this);
        setInst->AddOperand(value);
    }
    else
    {
        uint64_t val = static_cast<uint64_t>(value->GetValue());
        StoreConstant(val, context);
    }
}

void Register::StoreValue(LongValue* value, Context& context)
{
    if (value->GetValue() >= 0 && value->GetValue() <= std::numeric_limits<uint16_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(this);
        setInst->AddOperand(value);
    }
    else
    {
        uint64_t val = static_cast<uint64_t>(value->GetValue());
        StoreConstant(val, context);
    }
}

void Register::StoreValue(ULongValue* value, Context& context)
{
    if (value->GetValue() <= std::numeric_limits<uint16_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(this);
        setInst->AddOperand(value);
    }
    else
    {
        uint64_t val = static_cast<uint64_t>(value->GetValue());
        StoreConstant(val, context);
    }
}

void Register::StoreValue(BoolValue* value, Context& context)
{
    Instruction* setInst = context.CreateInstruction(SET);
    setInst->AddOperand(this);
    setInst->AddOperand(value);
}

void Register::StoreValue(CharValue* value, Context& context)
{
    Instruction* setInst = context.CreateInstruction(SET);
    setInst->AddOperand(this);
    setInst->AddOperand(value);
}

void Register::StoreValue(WCharValue* value, Context& context)
{
    Instruction* setInst = context.CreateInstruction(SET);
    setInst->AddOperand(this);
    setInst->AddOperand(value);
}

void Register::StoreValue(UCharValue* value, Context& context)
{
    if (value->GetValue() < std::numeric_limits<uint16_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(this);
        setInst->AddOperand(value);
    }
    else
    {
        uint64_t val = static_cast<uint64_t>(value->GetValue());
        StoreConstant(val, context);
    }
}

void Register::StoreValue(FloatValue* value, Context& context)
{
    // todo
}

void Register::StoreValue(DoubleValue* value, Context& context)
{
    // todo
}

void Register::StoreValue(NullValue* value, Context& context)
{
    Instruction* setInst = context.CreateInstruction(SET);
    setInst->AddOperand(this);
    setInst->AddOperand(value);
}

void Register::StoreConstant(uint64_t constant, Context& context)
{
    uint16_t v0 = static_cast<uint16_t>(constant);
    uint16_t v1 = static_cast<uint16_t>(constant >> 16);
    uint16_t v2 = static_cast<uint16_t>(constant >> 32);
    uint16_t v3 = static_cast<uint16_t>(constant >> 48);
    Instruction* setInst = context.CreateInstruction(SET);
    setInst->AddOperand(this);
    setInst->AddOperand(context.GetUShortValue(v0));
    Instruction* orMLInst = context.CreateInstruction(ORML);
    orMLInst->AddOperand(this);
    orMLInst->AddOperand(context.GetUShortValue(v1));
    Instruction* orMHInst = context.CreateInstruction(ORMH);
    orMHInst->AddOperand(this);
    orMHInst->AddOperand(context.GetUShortValue(v2));
    Instruction* orHInst = context.CreateInstruction(ORH);
    orHInst->AddOperand(this);
    orHInst->AddOperand(context.GetUShortValue(v3));
}

LocalRegister::LocalRegister(int number_, Type* type_) : Register(type_), number(number_)
{
}

void LocalRegister::Write(std::ostream& s)
{
    s << '$' << number;
}

void LocalRegister::StoreTo(Value* target, Context& context)
{
    target->StoreLocalReg(this, context);
}

void LocalRegister::StoreFrameLoc(FrameLocation* loc, Context& context)
{
    uint64_t offset = loc->Offset();
    if (offset <= std::numeric_limits<uint8_t>::max())
    {
        Instruction* inst = loc->GetType(context)->CreateLoad(context);
        inst->AddOperand(this);
        inst->AddOperand(loc->BaseReg());
        inst->AddOperand(context.GetULongValue(offset));
    }
    else if (offset <= std::numeric_limits<uint16_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(context.Ix());
        setInst->AddOperand(context.GetULongValue(offset));
        Instruction* loadInst = loc->GetType(context)->CreateLoad(context);
        loadInst->AddOperand(loc->BaseReg());
        loadInst->AddOperand(context.Ix());
    }
    else if (offset <= std::numeric_limits<uint32_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(context.Ix());
        setInst->AddOperand(context.GetULongValue(offset & 0xFFFF));
        Instruction* orMLInst = context.CreateInstruction(ORML);
        orMLInst->AddOperand(context.Ix());
        orMLInst->AddOperand(context.GetULongValue((offset >> 16) & 0xFFFF));
        Instruction* loadInst = loc->GetType(context)->CreateLoad(context);
        loadInst->AddOperand(loc->BaseReg());
        loadInst->AddOperand(context.Ix());
    }
    else
    {
        throw std::runtime_error("frame too big");
    }
}

void LocalRegister::StoreRegArg(Register* reg, Argument* arg, Context& context)
{
    Instruction* inst = context.CreateInstruction(SET);
    inst->AddOperand(this);
    inst->AddOperand(reg);
}

void LocalRegister::StoreFrameLocArg(FrameLocation* loc, Argument* arg, Context& context)
{
    uint64_t offset = loc->Offset();
    if (offset <= std::numeric_limits<uint8_t>::max())
    {
        Instruction* inst = arg->GetType(context)->CreateLoad(context);
        inst->AddOperand(this);
        inst->AddOperand(loc->BaseReg());
        inst->AddOperand(context.GetULongValue(offset));
    }
    else if (offset <= std::numeric_limits<uint16_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(context.Ix());
        setInst->AddOperand(context.GetULongValue(offset));
        Instruction* loadInst = arg->GetType(context)->CreateLoad(context);
        loadInst->AddOperand(loc->BaseReg());
        loadInst->AddOperand(context.Ix());
    }
    else if (offset <= std::numeric_limits<uint32_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(context.Ix());
        setInst->AddOperand(context.GetULongValue(offset & 0xFFFF));
        Instruction* orMLInst = context.CreateInstruction(ORML);
        orMLInst->AddOperand(context.Ix());
        orMLInst->AddOperand(context.GetULongValue((offset >> 16) & 0xFFFF));
        Instruction* loadInst = arg->GetType(context)->CreateLoad(context);
        loadInst->AddOperand(loc->BaseReg());
        loadInst->AddOperand(context.Ix());
    }
    else
    {
        throw std::runtime_error("frame too big");
    }
}

Value* LocalRegister::SignExtend(Type* type, Context& context)
{
    LocalRegister* reg = context.GetCurrentBasicBlock()->GetFunction()->GetReg(context.GetULongType());
    Instruction* setInst = context.CreateInstruction(SET);
    setInst->AddOperand(reg);
    Value* minusOne = context.GetULongValue(-1);
    minusOne->SetHex();
    Value* shiftExpr = context.CreateBinaryExpr(Operator::shiftLeft, minusOne, context.GetULongValue(GetType(context)->SizeInBits()));
    setInst->AddOperand(shiftExpr);
    Instruction* zsn = context.CreateInstruction(ZSN);
    zsn->AddOperand(reg);
    zsn->AddOperand(this);
    zsn->AddOperand(reg);
    LocalRegister* result = context.GetCurrentBasicBlock()->GetFunction()->GetReg(type);
    Instruction* orInst = context.CreateInstruction(OR);
    orInst->AddOperand(result);
    orInst->AddOperand(reg);
    orInst->AddOperand(this);
    return result;
}

Value* LocalRegister::GetMemberAddress(int32_t memberIndex, Context& context)
{
    Type* type = GetType(context);
    Assert(type->GetKind() == TypeKind::ptrType, "pointer type expected");
    PtrType* ptrType = static_cast<PtrType*>(type);
    Assert(ptrType->BaseType()->GetKind() == TypeKind::classType, "class type expected");
    ClassType* classType = static_cast<ClassType*>(ptrType->BaseType());
    Assert(memberIndex >= 0 && memberIndex < classType->MemberTypes().size(), "invalid member index");
    const MemberType& memberType = classType->MemberTypes()[memberIndex];
    uint64_t offset = memberType.offset;
    Value* address = GetAddress(context);
    Value* memberAddress = context.GetCurrentBasicBlock()->GetFunction()->GetReg(context.GetPtrType(memberType.type));
    address->StoreTo(memberAddress, context);
    if (offset <= std::numeric_limits<uint16_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(context.Ix());
        setInst->AddOperand(context.GetULongValue(offset));
        Instruction* ldaInst = context.CreateInstruction(LDA);
        ldaInst->AddOperand(memberAddress);
        ldaInst->AddOperand(memberAddress);
        ldaInst->AddOperand(context.Ix());
    }
    else if (offset <= std::numeric_limits<uint32_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(context.Ix());
        setInst->AddOperand(context.GetULongValue(offset & 0xFFFF));
        Instruction* orMLInst = context.CreateInstruction(ORML);
        orMLInst->AddOperand(context.Ix());
        orMLInst->AddOperand(context.GetULongValue((offset >> 16) & 0xFFFF));
        Instruction* ldaInst = context.CreateInstruction(LDA);
        ldaInst->AddOperand(memberAddress);
        ldaInst->AddOperand(memberAddress);
        ldaInst->AddOperand(context.Ix());
    }
    else
    {
        throw std::runtime_error("frame too big");
    }
    return memberAddress;
}

SymbolicRegister::SymbolicRegister(const std::string& name_, Type* type_) : Register(type_), name(name_)
{
}

void SymbolicRegister::Write(std::ostream& s)
{
    s << name;
}

FrameLocation::FrameLocation(uint64_t offset_, SymbolicRegister* baseReg_, Type* type_) : Value(Kind::frameLocation), offset(offset_), baseReg(baseReg_), type(type_)
{
}

Value* FrameLocation::GetAddress(Context& context)
{
    Value* result = context.GetCurrentBasicBlock()->GetFunction()->GetReg(type);
    if (offset <= std::numeric_limits<uint8_t>::max())
    {
        Instruction* ldaInst = context.CreateInstruction(LDA);
        ldaInst->AddOperand(result);
        ldaInst->AddOperand(baseReg);
        ldaInst->AddOperand(context.GetULongValue(offset));
    }
    else if (offset <= std::numeric_limits<uint16_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(context.Ix());
        setInst->AddOperand(context.GetULongValue(offset));
        Instruction* ldaInst = context.CreateInstruction(LDA);
        ldaInst->AddOperand(result);
        ldaInst->AddOperand(baseReg);
        ldaInst->AddOperand(context.Ix());
    }
    else if (offset <= std::numeric_limits<uint32_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(context.Ix());
        setInst->AddOperand(context.GetULongValue(offset & 0xFFFF));
        Instruction* orMLInst = context.CreateInstruction(ORML);
        orMLInst->AddOperand(context.Ix());
        orMLInst->AddOperand(context.GetULongValue((offset >> 16) & 0xFFFF));
        Instruction* ldaInst = context.CreateInstruction(LDA);
        ldaInst->AddOperand(result);
        ldaInst->AddOperand(baseReg);
        ldaInst->AddOperand(context.Ix());
    }
    else
    {
        throw std::runtime_error("frame too big");
    }
    return result;
}

Value* FrameLocation::GetMemberAddress(int32_t memberIndex, Context& context)
{
    Value* reg = context.GetCurrentBasicBlock()->GetFunction()->GetReg(type);
    StoreTo(reg, context);
    return reg->GetMemberAddress(memberIndex, context);
}

void FrameLocation::Write(std::ostream& s)
{
    throw std::runtime_error("cannot write frame location");
}

Type* FrameLocation::GetType(Context& context)
{
    return type;
}

void FrameLocation::Store(Value* value, Context& context)
{
    value->StoreTo(this, context);
}

void FrameLocation::StoreArg(Argument* arg, Value* target, Context& context)
{
    target->StoreFrameLocArg(this, arg, context);
}

void FrameLocation::StoreFrameLocArg(FrameLocation* loc, Argument* arg, Context& context)
{
    uint64_t locOffset = loc->Offset();
    if (locOffset <= std::numeric_limits<uint8_t>::max())
    {
        Instruction* loadToAxInst = arg->GetType(context)->CreateLoad(context);
        loadToAxInst->AddOperand(context.Ax());
        loadToAxInst->AddOperand(loc->BaseReg());
        loadToAxInst->AddOperand(context.GetULongValue(locOffset));
        if (offset <= std::numeric_limits<uint16_t>::max())
        {
            Instruction* setInst = context.CreateInstruction(SET);
            setInst->AddOperand(context.Ix());
            setInst->AddOperand(context.GetULongValue(offset));
            Instruction* storeInst = arg->GetType(context)->CreateStore(context);
            storeInst->AddOperand(context.Ax());
            storeInst->AddOperand(baseReg);
            storeInst->AddOperand(context.Ix());
        }
        else if (offset <= std::numeric_limits<uint32_t>::max())
        {
            Instruction* setInst = context.CreateInstruction(SET);
            setInst->AddOperand(context.Ix());
            setInst->AddOperand(context.GetULongValue(offset & 0xFFFF));
            Instruction* orMLInst = context.CreateInstruction(ORML);
            orMLInst->AddOperand(context.Ix());
            orMLInst->AddOperand(context.GetULongValue((offset >> 16) & 0xFFFF));
            Instruction* storeInst = arg->GetType(context)->CreateStore(context);
            storeInst->AddOperand(context.Ax());
            storeInst->AddOperand(baseReg);
            storeInst->AddOperand(context.Ix());
        }
        else
        {
            throw std::runtime_error("frame too big");
        }
    }
    else if (locOffset <= std::numeric_limits<uint16_t>::max())
    {
        Instruction* setArgInst = context.CreateInstruction(SET);
        setArgInst->AddOperand(context.Ix());
        setArgInst->AddOperand(context.GetULongValue(locOffset));
        Instruction* loadToAxInst = arg->GetType(context)->CreateLoad(context);
        loadToAxInst->AddOperand(context.Ax());
        loadToAxInst->AddOperand(loc->BaseReg());
        loadToAxInst->AddOperand(context.Ix());
        if (offset <= std::numeric_limits<uint16_t>::max())
        {
            Instruction* setInst = context.CreateInstruction(SET);
            setInst->AddOperand(context.Ix());
            setInst->AddOperand(context.GetULongValue(offset));
            Instruction* storeInst = arg->GetType(context)->CreateStore(context);
            storeInst->AddOperand(context.Ax());
            storeInst->AddOperand(baseReg);
            storeInst->AddOperand(context.Ix());
        }
        else if (offset <= std::numeric_limits<uint32_t>::max())
        {
            Instruction* setInst = context.CreateInstruction(SET);
            setInst->AddOperand(context.Ix());
            setInst->AddOperand(context.GetULongValue(offset & 0xFFFF));
            Instruction* orMLInst = context.CreateInstruction(ORML);
            orMLInst->AddOperand(context.Ix());
            orMLInst->AddOperand(context.GetULongValue((offset >> 16) & 0xFFFF));
            Instruction* storeInst = arg->GetType(context)->CreateStore(context);
            storeInst->AddOperand(context.Ax());
            storeInst->AddOperand(baseReg);
            storeInst->AddOperand(context.Ix());
        }
        else
        {
            throw std::runtime_error("frame too big");
        }
    }
    else if (locOffset <= std::numeric_limits<uint32_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(context.Ix());
        setInst->AddOperand(context.GetULongValue(locOffset & 0xFFFF));
        Instruction* orMLInst = context.CreateInstruction(ORML);
        orMLInst->AddOperand(context.Ix());
        orMLInst->AddOperand(context.GetULongValue((locOffset >> 16) & 0xFFFF));
        Instruction* loadToAxInst = arg->GetType(context)->CreateLoad(context);
        loadToAxInst->AddOperand(context.Ax());
        loadToAxInst->AddOperand(loc->BaseReg());
        loadToAxInst->AddOperand(context.Ix());
        if (offset <= std::numeric_limits<uint16_t>::max())
        {
            Instruction* setInst = context.CreateInstruction(SET);
            setInst->AddOperand(context.Ix());
            setInst->AddOperand(context.GetULongValue(offset));
            Instruction* storeInst = arg->GetType(context)->CreateStore(context);
            storeInst->AddOperand(context.Ax());
            storeInst->AddOperand(baseReg);
            storeInst->AddOperand(context.Ix());
        }
        else if (offset <= std::numeric_limits<uint32_t>::max())
        {
            Instruction* setInst = context.CreateInstruction(SET);
            setInst->AddOperand(context.Ix());
            setInst->AddOperand(context.GetULongValue(offset & 0xFFFF));
            Instruction* orMLInst = context.CreateInstruction(ORML);
            orMLInst->AddOperand(context.Ix());
            orMLInst->AddOperand(context.GetULongValue((offset >> 16) & 0xFFFF));
            Instruction* storeInst = arg->GetType(context)->CreateStore(context);
            storeInst->AddOperand(context.Ax());
            storeInst->AddOperand(baseReg);
            storeInst->AddOperand(context.Ix());
        }
        else
        {
            throw std::runtime_error("frame too big");
        }
    }
    else
    {
        throw std::runtime_error("frame too big");
    }
}

void FrameLocation::StoreCo(Value* constant, Context& context)
{
    if (offset <= std::numeric_limits<uint8_t>::max())
    {
        Instruction* storeInst = context.CreateInstruction(STCO);
        storeInst->AddOperand(constant);
        storeInst->AddOperand(baseReg);
        storeInst->AddOperand(context.GetULongValue(offset));
    }
    else if (offset <= std::numeric_limits<uint16_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(context.Ix());
        setInst->AddOperand(context.GetULongValue(offset));
        Instruction* storeInst = context.CreateInstruction(STCO);
        storeInst->AddOperand(constant);
        storeInst->AddOperand(baseReg);
        storeInst->AddOperand(context.Ix());
    }
    else if (offset <= std::numeric_limits<uint32_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(context.Ix());
        setInst->AddOperand(context.GetULongValue(offset & 0xFFFF));
        Instruction* orMLInst = context.CreateInstruction(ORML);
        orMLInst->AddOperand(context.Ix());
        orMLInst->AddOperand(context.GetULongValue((offset >> 16) & 0xFFFF));
        Instruction* storeInst = context.CreateInstruction(STCO);
        storeInst->AddOperand(constant);
        storeInst->AddOperand(baseReg);
        storeInst->AddOperand(context.Ix());
    }
    else
    {
        throw std::runtime_error("frame too big");
    }
}

void FrameLocation::SetConstantAx(uint64_t constant, Context& context)
{
    uint16_t v0 = static_cast<uint16_t>(constant);
    uint16_t v1 = static_cast<uint16_t>(constant >> 16);
    uint16_t v2 = static_cast<uint16_t>(constant >> 32);
    uint16_t v3 = static_cast<uint16_t>(constant >> 48);
    Instruction* setInst = context.CreateInstruction(SET);
    setInst->AddOperand(context.Ax());
    setInst->AddOperand(context.GetUShortValue(v0));
    Instruction* orMLInst = context.CreateInstruction(ORML);
    orMLInst->AddOperand(context.Ax());
    orMLInst->AddOperand(context.GetUShortValue(v1));
    Instruction* orMHInst = context.CreateInstruction(ORMH);
    orMHInst->AddOperand(context.Ax());
    orMHInst->AddOperand(context.GetUShortValue(v2));
    Instruction* orHInst = context.CreateInstruction(ORH);
    orHInst->AddOperand(context.Ax());
    orHInst->AddOperand(context.GetUShortValue(v3));
}

void FrameLocation::StoreAx(Instruction* storeInst, Context& context)
{
    if (offset <= std::numeric_limits<uint8_t>::max())
    {
        storeInst->AddOperand(context.Ax());
        storeInst->AddOperand(baseReg);
        storeInst->AddOperand(context.GetULongValue(offset));
    }
    else if (offset <= std::numeric_limits<uint16_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(context.Ix());
        setInst->AddOperand(context.GetULongValue(offset));
        storeInst->AddOperand(context.Ax());
        storeInst->AddOperand(baseReg);
        storeInst->AddOperand(context.Ix());
    }
    else if (offset <= std::numeric_limits<uint32_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(context.Ix());
        setInst->AddOperand(context.GetULongValue(offset & 0xFFFF));
        Instruction* orMLInst = context.CreateInstruction(ORML);
        orMLInst->AddOperand(context.Ix());
        orMLInst->AddOperand(context.GetULongValue((offset >> 16) & 0xFFFF));
        storeInst->AddOperand(context.Ax());
        storeInst->AddOperand(baseReg);
        storeInst->AddOperand(context.Ix());
    }
    else
    {
        throw std::runtime_error("frame too big");
    }
}

void FrameLocation::StoreLocalReg(LocalRegister* reg, Context& context)
{
    if (offset <= std::numeric_limits<uint8_t>::max())
    {
        Instruction* storeInst = reg->GetType(context)->CreateStore(context);
        storeInst->AddOperand(reg);
        storeInst->AddOperand(baseReg);
        storeInst->AddOperand(context.GetULongValue(offset));
    }
    else if (offset <= std::numeric_limits<uint16_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(context.Ix());
        setInst->AddOperand(context.GetULongValue(offset));
        Instruction* storeInst = reg->GetType(context)->CreateStore(context);
        storeInst->AddOperand(reg);
        storeInst->AddOperand(baseReg);
        storeInst->AddOperand(context.Ix());
    }
    else if (offset <= std::numeric_limits<uint32_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(context.Ix());
        setInst->AddOperand(context.GetULongValue(offset & 0xFFFF));
        Instruction* orMLInst = context.CreateInstruction(ORML);
        orMLInst->AddOperand(context.Ix());
        orMLInst->AddOperand(context.GetULongValue((offset >> 16) & 0xFFFF));
        Instruction* storeInst = reg->GetType(context)->CreateStore(context);
        storeInst->AddOperand(reg);
        storeInst->AddOperand(baseReg);
        storeInst->AddOperand(context.Ix());
    }
    else
    {
        throw std::runtime_error("frame location index to big");
    }
}

void FrameLocation::StoreValue(SByteValue* value, Context& context)
{
    Instruction* setInst = context.CreateInstruction(SET);
    setInst->AddOperand(context.Ax());
    setInst->AddOperand(value);
    Instruction* storeInst = value->GetType(context)->CreateStore(context);
    StoreAx(storeInst, context);
}

void FrameLocation::StoreValue(ByteValue* value, Context& context)
{
    Instruction* setInst = context.CreateInstruction(SET);
    setInst->AddOperand(context.Ax());
    setInst->AddOperand(value);
    Instruction* storeInst = value->GetType(context)->CreateStore(context);
    StoreAx(storeInst, context);
}

void FrameLocation::StoreValue(ShortValue* value, Context& context)
{
    Instruction* setInst = context.CreateInstruction(SET);
    setInst->AddOperand(context.Ax());
    setInst->AddOperand(value);
    Instruction* storeInst = value->GetType(context)->CreateStore(context);
    StoreAx(storeInst, context);
}

void FrameLocation::StoreValue(UShortValue* value, Context& context)
{
    Instruction* setInst = context.CreateInstruction(SET);
    setInst->AddOperand(context.Ax());
    setInst->AddOperand(value);
    Instruction* storeInst = value->GetType(context)->CreateStore(context);
    StoreAx(storeInst, context);
}

void FrameLocation::StoreValue(IntValue* value, Context& context)
{
    Instruction* storeInst = value->GetType(context)->CreateStore(context);
    if (value->GetValue() >= 0 && value->GetValue() <= std::numeric_limits<uint16_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(context.Ax());
        setInst->AddOperand(value);
    }
    else
    {
        SetConstantAx(static_cast<uint64_t>(value->GetValue()), context);
    }
    StoreAx(storeInst, context);
}

void FrameLocation::StoreValue(UIntValue* value, Context& context)
{
    Instruction* storeInst = value->GetType(context)->CreateStore(context);
    if (value->GetValue() <= std::numeric_limits<uint16_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(context.Ax());
        setInst->AddOperand(value);
    }
    else
    {
        SetConstantAx(static_cast<uint64_t>(value->GetValue()), context);
    }
    StoreAx(storeInst, context);
}

void FrameLocation::StoreValue(LongValue* value, Context& context)
{
    Instruction* storeInst = value->GetType(context)->CreateStore(context);
    if (value->GetValue() >= 0 && value->GetValue() <= std::numeric_limits<uint16_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(context.Ax());
        setInst->AddOperand(value);
    }
    else
    {
        SetConstantAx(static_cast<uint64_t>(value->GetValue()), context);
    }
    StoreAx(storeInst, context);
}

void FrameLocation::StoreValue(ULongValue* value, Context& context)
{
    Instruction* storeInst = value->GetType(context)->CreateStore(context);
    if (value->GetValue() <= std::numeric_limits<uint16_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(context.Ax());
        setInst->AddOperand(value);
    }
    else
    {
        SetConstantAx(static_cast<uint64_t>(value->GetValue()), context);
    }
    StoreAx(storeInst, context);
}

void FrameLocation::StoreValue(BoolValue* value, Context& context)
{
    Instruction* storeInst = value->GetType(context)->CreateStore(context);
    Instruction* setInst = context.CreateInstruction(SET);
    setInst->AddOperand(context.Ax());
    setInst->AddOperand(value);
    StoreAx(storeInst, context);
}

void FrameLocation::StoreValue(CharValue* value, Context& context)
{
    Instruction* storeInst = value->GetType(context)->CreateStore(context);
    Instruction* setInst = context.CreateInstruction(SET);
    setInst->AddOperand(context.Ax());
    setInst->AddOperand(value);
    StoreAx(storeInst, context);
}

void FrameLocation::StoreValue(WCharValue* value, Context& context)
{
    Instruction* storeInst = value->GetType(context)->CreateStore(context);
    Instruction* setInst = context.CreateInstruction(SET);
    setInst->AddOperand(context.Ax());
    setInst->AddOperand(value);
    StoreAx(storeInst, context);
}

void FrameLocation::StoreValue(UCharValue* value, Context& context)
{
    Instruction* storeInst = value->GetType(context)->CreateStore(context);
    if (value->GetValue() <= std::numeric_limits<uint16_t>::max())
    {
        Instruction* setInst = context.CreateInstruction(SET);
        setInst->AddOperand(context.Ax());
        setInst->AddOperand(value);
    }
    else
    {
        SetConstantAx(static_cast<uint64_t>(value->GetValue()), context);
    }
    StoreAx(storeInst, context);
}

void FrameLocation::StoreValue(FloatValue* value, Context& context)
{
    // todo
}

void FrameLocation::StoreValue(DoubleValue* value, Context& context)
{
    // todo
}

void FrameLocation::StoreValue(NullValue* value, Context& context)
{
    StoreCo(context.GetByteValue(static_cast<uint8_t>(0)), context);
}

Value* FrameLocation::SignExtend(Type* type, Context& context)
{
    LocalRegister* reg = context.GetCurrentBasicBlock()->GetFunction()->GetReg(GetType(context));
    reg->StoreFrameLoc(this, context);
    return reg->SignExtend(type, context);
}

Symbol::Symbol(const std::string& name_) : Value(Kind::symbol), name(name_)
{
}

void Symbol::Write(std::ostream& s)
{
    s << name;
}

Type* Symbol::GetType(Context& context)
{
    return context.GetULongType();
}

Argument::Argument(Type* type_, Value* store_) : Value(Kind::argument), type(type_), store(store_)
{
}

void Argument::Write(std::ostream& s)
{
    throw std::runtime_error("cannot write argument");
}

Type* Argument::GetType(Context& context)
{
    return type;
}

void Argument::StoreTo(Value* target, Context& context)
{
    store->StoreArg(this, target, context);
}

LocalVariable::LocalVariable(Type* type_, Value* store_) : Value(Kind::localVariable), type(type_), store(store_)
{
}

void LocalVariable::Write(std::ostream& s)
{
    store->Write(s);
}

Type* LocalVariable::GetType(Context& context)
{
    return type;
}

Value* LocalVariable::Load(Context& context)
{
    if (type->GetKind() == TypeKind::classType)
    {
        return store->GetAddress(context);
    }
    else
    {
        return store->Load(context);
    }
}

void LocalVariable::Store(Value* value, Context& context)
{
    value->StoreTo(this, context);
}

void LocalVariable::StoreRegArg(Register* reg, Argument* arg, Context& context)
{
    store->StoreRegArg(reg, arg, context);
}

void LocalVariable::StoreFrameLocArg(FrameLocation* loc, Argument* arg, Context& context)
{
    store->StoreFrameLocArg(loc, arg, context);
}

void LocalVariable::StoreLocalReg(LocalRegister* reg, Context& context)
{
    store->StoreLocalReg(reg, context);
}

void LocalVariable::StoreValue(SByteValue* value, Context& context)
{
    store->StoreValue(value, context);
}

void LocalVariable::StoreValue(ByteValue* value, Context& context)
{
    store->StoreValue(value, context);
}

void LocalVariable::StoreValue(ShortValue* value, Context& context)
{
    store->StoreValue(value, context);
}

void LocalVariable::StoreValue(UShortValue* value, Context& context)
{
    store->StoreValue(value, context);
}

void LocalVariable::StoreValue(IntValue* value, Context& context)
{
    store->StoreValue(value, context);
}

void LocalVariable::StoreValue(UIntValue* value, Context& context)
{
    store->StoreValue(value, context);
}

void LocalVariable::StoreValue(LongValue* value, Context& context)
{
    store->StoreValue(value, context);
}

void LocalVariable::StoreValue(ULongValue* value, Context& context)
{
    store->StoreValue(value, context);
}

void LocalVariable::StoreValue(BoolValue* value, Context& context)
{
    store->StoreValue(value, context);
}

void LocalVariable::StoreValue(CharValue* value, Context& context)
{
    store->StoreValue(value, context);
}

void LocalVariable::StoreValue(WCharValue* value, Context& context)
{
    store->StoreValue(value, context);
}

void LocalVariable::StoreValue(UCharValue* value, Context& context)
{
    store->StoreValue(value, context);
}

void LocalVariable::StoreValue(FloatValue* value, Context& context)
{
    store->StoreValue(value, context);
}

void LocalVariable::StoreValue(DoubleValue* value, Context& context)
{
    store->StoreValue(value, context);
}

void LocalVariable::StoreValue(NullValue* value, Context& context)
{
    store->StoreValue(value, context);
}

Value* LocalVariable::SignExtend(Type* type, Context& context)
{
    return store->SignExtend(type, context);
}

} // namespace cmsystemx
