// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsystemx/Context.hpp>
#include <cmajor/cmsystemx/Function.hpp>
#include <cmajor/cmsystemx/OpCode.hpp>

namespace cmsystemx {

SymbolicRegisters::SymbolicRegisters(Context& context) :
    ax("ax", context.GetULongType()), bx("bx", context.GetULongType()), cx("cx", context.GetULongType()), dx("dx", context.GetULongType()), ex("ex", context.GetULongType())
{
}

SymbolicRegister* SymbolicRegisters::GetSymbolicRegister(int index)
{
    switch (index)
    {
        case 0: return &ax;
        case 1: return &bx;
        case 2: return &cx;
        case 3: return &dx;
        case 4: return &ex;
    }
    throw std::runtime_error("invalid symbolic register index");
}

Context::Context() : currentBasicBlock(nullptr), symbolicRegisters(*this), fp("fp", GetULongType()), sp("sp", GetULongType()), ix("ix", GetULongType()), rR("rR", GetULongType()),
    zero(0), one(1), eight(8)
{
}

Instruction* Context::CreateRetVoid()
{
    return currentBasicBlock->CreateRetVoid(*this);
}

Instruction* Context::CreateRet(Value* value)
{
    return currentBasicBlock->CreateRet(value, *this);
}

Instruction* Context::CreateInstruction(int opCode)
{
    return currentBasicBlock->CreateInstruction(opCode);
}

Value* Context::GetBoolValue(bool value)
{
    Value* val = new BoolValue(value);
    AddValue(val);
    return val;
}

Value* Context::GetSByteValue(int8_t value)
{
    Value* val = new SByteValue(value);
    AddValue(val);
    return val;
}

Value* Context::GetByteValue(uint8_t value)
{
    Value* val = new ByteValue(value);
    AddValue(val);
    return val;
}

Value* Context::GetShortValue(int16_t value)
{
    Value* val = new ShortValue(value);
    AddValue(val);
    return val;
}

Value* Context::GetUShortValue(uint16_t value)
{
    Value* val = new UShortValue(value);
    AddValue(val);
    return val;
}

Value* Context::GetIntValue(int32_t value)
{
    Value* val = new IntValue(value);
    AddValue(val);
    return val;
}

Value* Context::GetUIntValue(uint32_t value)
{
    Value* val = new UIntValue(value);
    AddValue(val);
    return val;
}

Value* Context::GetLongValue(int64_t value)
{
    Value* val = new LongValue(value);
    AddValue(val);
    return val;
}

Value* Context::GetULongValue(uint64_t value)
{
    Value* val = new ULongValue(value);
    AddValue(val);
    return val;
}

Value* Context::GetFloatValue(float value)
{
    Value* val = new FloatValue(value);
    AddValue(val);
    return val;
}

Value* Context::GetDoubleValue(double value)
{
    Value* val = new DoubleValue(value);
    AddValue(val);
    return val;
}

Value* Context::GetCharValue(char value)
{
    Value* val = new CharValue(value);
    AddValue(val);
    return val;
}

Value* Context::GetWCharValue(char16_t value)
{
    Value* val = new WCharValue(value);
    AddValue(val);
    return val;
}

Value* Context::GetUCharValue(char32_t value)
{
    Value* val = new UCharValue(value);
    AddValue(val);
    return val;
}

Value* Context::GetNullValue(PtrType* ptrType)
{
    Value* val = new NullValue(ptrType);
    AddValue(val);
    return val;
}

Value* Context::CreateUnaryExpr(Operator op, Value* value)
{
    Value* val = new UnaryExpr(op, value);
    AddValue(val);
    return val;
}

Value* Context::CreateBinaryExpr(Operator op, Value* left, Value* right)
{
    Value* val = new BinaryExpr(op, left, right);
    AddValue(val);
    return val;
}

Value* Context::CreateParenExpr(Value* value)
{
    Value* val = new ParenExpr(value);
    AddValue(val);
    return val;
}

Value* Context::CreateAdd(Value* left, Value* right)
{
    int opCode = ADD;
    if (left->GetType(*this)->IsUnsigned())
    {
        opCode = ADDU;
    }
    return CreateBinaryOpInst(left, right, opCode);
}

Value* Context::CreateSub(Value* left, Value* right)
{
    int opCode = SUB;
    if (left->GetType(*this)->IsUnsigned())
    {
        opCode = SUBU;
    }
    return CreateBinaryOpInst(left, right, opCode);
}

Value* Context::CreateMul(Value* left, Value* right)
{
    int opCode = MUL;
    if (left->GetType(*this)->IsUnsigned())
    {
        opCode = MULU;
    }
    return CreateBinaryOpInst(left, right, opCode);
}

Value* Context::CreateDiv(Value* left, Value* right)
{
    int opCode = DIV;
    if (left->GetType(*this)->IsUnsigned())
    {
        opCode = DIVU;
    }
    return CreateBinaryOpInst(left, right, opCode);
}

Value* Context::CreateRem(Value* left, Value* right)
{
    Value* result = CreateDiv(left, right);
    Instruction* getInst = CreateInstruction(GET);
    getInst->AddOperand(result);
    getInst->AddOperand(RR());
    return result;
}

Value* Context::CreateBinaryOpInst(Value* left, Value* right, int opCode)
{
    Function* currentFunction = currentBasicBlock->GetFunction();
    Type* type = left->GetType(*this);
    Value* result = currentFunction->GetReg(type);
    Instruction* inst = CreateInstruction(opCode);
    inst->AddOperand(result);
    if (left->GetKind() == Kind::register_)
    {
        inst->AddOperand(left);
    }
    else
    {
        Value* left_ = currentFunction->GetReg(type);
        left->StoreTo(left_, *this);
        inst->AddOperand(left_);
    }
    if (right->GetKind() == Kind::register_)
    {
        inst->AddOperand(right);
    }
    else
    {
        Value* right_ = currentFunction->GetReg(type);
        right->StoreTo(right_, *this);
        inst->AddOperand(right_);
    }
    return result;
}

void Context::AddValue(Value* value)
{
    values.push_back(std::unique_ptr<Value>(value));
}

} // namespace cmsystemx
