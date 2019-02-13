// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSX_VALUE_INCLUDED
#define CMAJOR_CMSX_VALUE_INCLUDED
#include <cmajor/cmsystemx/CmSystemxApi.hpp>
#include <string>
#include <stdint.h>
#include <iosfwd>

namespace cmsystemx {

class Type;
class PtrType;
class Context;
class Register;
class LocalRegister;
class SymbolicRegister;
class FrameLocation;
class LocalVariable;
class Argument;
class SByteValue;
class ByteValue;
class ShortValue;
class UShortValue;
class IntValue;
class UIntValue;
class LongValue;
class ULongValue;
class BoolValue;
class CharValue;
class WCharValue;
class UCharValue;
class FloatValue;
class DoubleValue;
class NullValue;
class Instruction;

enum class Operator
{
    unaryPlus, unaryMinus, complement, registerize,
    mul, div, mod, shiftLeft, shiftRight, bitAnd,
    add, sub, bitOr, bitXor
};

enum class Kind
{
    unaryExpr, binaryExpr, parenExpr, sbyteValue, byteValue, shortValue, ushortValue, intValue, uintValue, longValue, ulongValue, boolValue, charValue, wcharValue, ucharValue,
    floatValue, doubleValue, nullValue, register_, frameLocation, symbol, argument, localVariable, function
};

class CMSYSTEMX_API Value
{
public:
    Value(Kind kind_);
    virtual ~Value();
    virtual void Write(std::ostream& s) = 0;
    virtual Type* GetType(Context& context) = 0;
    virtual Value* Load(Context& context) { return this; }
    virtual Value* GetAddress(Context& context) { return this; }
    virtual Value* GetMemberAddress(int32_t memberIndex, Context& context) { return this; }
    virtual void Store(Value* value, Context& context) { }
    virtual void StoreTo(Value* target, Context& context) { }
    virtual void StoreArg(Argument* arg, Value* target, Context& context) { }
    virtual void StoreLocalReg(LocalRegister* reg, Context& context) { } 
    virtual void StoreRegArg(Register* reg, Argument* arg, Context& context) { }
    virtual void StoreFrameLocArg(FrameLocation* loc, Argument* arg, Context& context) { }
    virtual void StoreValue(SByteValue* value, Context& context) { }
    virtual void StoreValue(ByteValue* value, Context& context) { }
    virtual void StoreValue(ShortValue* value, Context& context) { }
    virtual void StoreValue(UShortValue* value, Context& context) { }
    virtual void StoreValue(IntValue* value, Context& context) { }
    virtual void StoreValue(UIntValue* value, Context& context) { }
    virtual void StoreValue(LongValue* value, Context& context) { }
    virtual void StoreValue(ULongValue* value, Context& context) { }
    virtual void StoreValue(BoolValue* value, Context& context) { }
    virtual void StoreValue(CharValue* value, Context& context) { }
    virtual void StoreValue(WCharValue* value, Context& context) { }
    virtual void StoreValue(UCharValue* value, Context& context) { }
    virtual void StoreValue(FloatValue* value, Context& context) { }
    virtual void StoreValue(DoubleValue* value, Context& context) { }
    virtual void StoreValue(NullValue* value, Context& context) { }
    virtual Value* CreateCallInst(int numLocalRegs, Context& context);
    virtual bool IsNonNegative(Context& context) const { return true; }
    virtual Value* SignExtend(Type* type, Context& context);
    Kind GetKind() const { return kind; }
    void SetHex() { hex = true; }
    bool Hex() const { return hex; }
private:
    Kind kind;
    bool hex;
};

class CMSYSTEMX_API UnaryExpr : public Value
{
public:
    UnaryExpr(Operator op_, Value* operand_);
    void Write(std::ostream& s) override;
    Type* GetType(Context& context) override;
private:
    Operator op;
    Value* operand;
};

class CMSYSTEMX_API BinaryExpr : public Value
{
public:
    BinaryExpr(Operator op_, Value* left_, Value* right_);
    void Write(std::ostream& s) override;
    Type* GetType(Context& context) override;
private:
    Operator op;
    Value* left;
    Value* right;
};

class CMSYSTEMX_API ParenExpr : public Value
{
public:
    ParenExpr(Value* operand_);
    void Write(std::ostream& s) override;
    Type* GetType(Context& context) override;
private:
    Value* operand;
};

class CMSYSTEMX_API SByteValue : public Value
{
public:
    SByteValue();
    SByteValue(int8_t value_);
    void Write(std::ostream& s) override;
    Type* GetType(Context& context) override;
    void StoreTo(Value* target, Context& context) override;
    int8_t GetValue() const { return value; }
    bool IsNonNegative(Context& context) const override { return value >= 0; }
    Value* SignExtend(Type* type, Context& context) override;
private:
    int8_t value;
};

class CMSYSTEMX_API ByteValue : public Value
{
public:
    ByteValue();
    ByteValue(uint8_t value_);
    void Write(std::ostream& s) override;
    Type* GetType(Context& context) override;
    void StoreTo(Value* target, Context& context) override;
    uint8_t GetValue() const { return value; }
private:
    uint8_t value;
};

class CMSYSTEMX_API ShortValue : public Value
{
public:
    ShortValue();
    ShortValue(int16_t value_);
    void Write(std::ostream& s) override;
    Type* GetType(Context& context) override;
    void StoreTo(Value* target, Context& context) override;
    int16_t GetValue() const { return value; }
    bool IsNonNegative(Context& context) const override { return value >= 0; }
    Value* SignExtend(Type* type, Context& context) override;
private:
    int16_t value;
};

class CMSYSTEMX_API UShortValue : public Value
{
public:
    UShortValue();
    UShortValue(uint16_t value_);
    void Write(std::ostream& s) override;
    Type* GetType(Context& context) override;
    void StoreTo(Value* target, Context& context) override;
    uint16_t GetValue() const { return value; }
private:
    uint16_t value;
};

class CMSYSTEMX_API IntValue : public Value
{
public:
    IntValue();
    IntValue(int32_t value_);
    void Write(std::ostream& s) override;
    Type* GetType(Context& context) override;
    void StoreTo(Value* target, Context& context) override;
    int32_t GetValue() const { return value; }
    bool IsNonNegative(Context& context) const override { return value >= 0; }
    Value* SignExtend(Type* type, Context& context) override;
private:
    int32_t value;
};

class CMSYSTEMX_API UIntValue : public Value
{
public:
    UIntValue();
    UIntValue(uint32_t value_);
    void Write(std::ostream& s) override;
    Type* GetType(Context& context) override;
    void StoreTo(Value* target, Context& context) override;
    uint32_t GetValue() const { return value; }
private:
    uint32_t value;
};

class CMSYSTEMX_API LongValue : public Value
{
public:
    LongValue();
    LongValue(int64_t value_);
    void Write(std::ostream& s) override;
    Type* GetType(Context& context) override;
    void StoreTo(Value* target, Context& context) override;
    int64_t GetValue() const { return value; }
    bool IsNonNegative(Context& context) const override { return value >= 0; }
    Value* SignExtend(Type* type, Context& context) override;
private:
    int64_t value;
};

class CMSYSTEMX_API ULongValue : public Value
{
public:
    ULongValue();
    ULongValue(uint64_t value_);
    void Write(std::ostream& s) override;
    Type* GetType(Context& context) override;
    void StoreTo(Value* target, Context& context) override;
    uint64_t GetValue() const { return value; }
private:
    uint64_t value;
};

class CMSYSTEMX_API BoolValue : public Value
{
public:
    BoolValue();
    BoolValue(bool value_);
    void Write(std::ostream& s) override;
    Type* GetType(Context& context) override;
    void StoreTo(Value* target, Context& context) override;
    bool GetValue() const { return value; }
private:
    bool value;
};

class CMSYSTEMX_API CharValue : public Value
{
public:
    CharValue();
    CharValue(unsigned char value_);
    void Write(std::ostream& s) override;
    Type* GetType(Context& context) override;
    void StoreTo(Value* target, Context& context) override;
    char GetValue() const { return value; }
private:
    unsigned char value;
};

class CMSYSTEMX_API WCharValue : public Value
{
public:
    WCharValue();
    WCharValue(char16_t value_);
    void Write(std::ostream& s) override;
    Type* GetType(Context& context) override;
    void StoreTo(Value* target, Context& context) override;
    char16_t GetValue() const { return value; }
private:
    char16_t value;
};

class CMSYSTEMX_API UCharValue : public Value
{
public:
    UCharValue();
    UCharValue(char32_t value_);
    void Write(std::ostream& s) override;
    Type* GetType(Context& context) override;
    void StoreTo(Value* target, Context& context) override;
    char32_t GetValue() const { return value; }
private:
    char32_t value;
};

class CMSYSTEMX_API FloatValue : public Value
{
public:
    FloatValue();
    FloatValue(float value_);
    void Write(std::ostream& s) override;
    Type* GetType(Context& context) override;
    void StoreTo(Value* target, Context& context) override;
    float GetValue() const { return value; }
    bool IsNonNegative(Context& context) const override { return value >= 0.0; }
private:
    float value;
};

class CMSYSTEMX_API DoubleValue : public Value
{
public:
    DoubleValue();
    DoubleValue(double value_);
    void Write(std::ostream& s) override;
    Type* GetType(Context& context) override;
    void StoreTo(Value* target, Context& context) override;
    double GetValue() const { return value; }
    bool IsNonNegative(Context& context) const override { return value >= 0.0; }
private:
    double value;
};

class CMSYSTEMX_API NullValue : public Value
{
public:
    NullValue(PtrType* ptrType_);
    void Write(std::ostream& s) override;
    Type* GetType(Context& context) override;
    void StoreTo(Value* target, Context& context) override;
private:
    PtrType* ptrType;
};

class CMSYSTEMX_API Register : public Value
{
public:
    Register(Type* type_);
    void SetType(Type* type_) { type = type_; }
    Type* GetType(Context& context) override;
    void StoreArg(Argument* arg, Value* target, Context& context) override;
    void StoreLocalReg(LocalRegister* reg, Context& context) override;
    void StoreValue(SByteValue* value, Context& context) override;
    void StoreValue(ByteValue* value, Context& context) override;
    void StoreValue(ShortValue* value, Context& context) override;
    void StoreValue(UShortValue* value, Context& context) override;
    void StoreValue(IntValue* value, Context& context) override;
    void StoreValue(UIntValue* value, Context& context) override;
    void StoreValue(LongValue* value, Context& context) override;
    void StoreValue(ULongValue* value, Context& context) override;
    void StoreValue(BoolValue* value, Context& context) override;
    void StoreValue(CharValue* value, Context& context) override;
    void StoreValue(WCharValue* value, Context& context) override;
    void StoreValue(UCharValue* value, Context& context) override;
    void StoreValue(FloatValue* value, Context& context) override;
    void StoreValue(DoubleValue* value, Context& context) override;
    void StoreValue(NullValue* value, Context& context) override;
private:
    void StoreConstant(uint64_t constant, Context& context);
    Type* type;
};

class CMSYSTEMX_API LocalRegister : public Register
{
public:
    LocalRegister(int number_, Type* type_);
    int Number() const { return number; }
    void Write(std::ostream& s) override;
    void StoreTo(Value* target, Context& context) override;
    void StoreRegArg(Register* reg, Argument* arg, Context& context) override;
    void StoreFrameLocArg(FrameLocation* loc, Argument* arg, Context& context) override;
    void StoreFrameLoc(FrameLocation* loc, Context& context);
    Value* SignExtend(Type* type, Context& context) override;
    Value* GetMemberAddress(int32_t memberIndex, Context& context) override;
private:
    int number;
};

class CMSYSTEMX_API SymbolicRegister : public Register
{
public:
    SymbolicRegister(const std::string& name_, Type* type_);
    void Write(std::ostream& s) override;
    const std::string& Name() const { return name; }
private:
    std::string name;
};

const int maxDirectFrameLoc = 256 / 8 - 1;
const int maxIxFrameLoc = 65536 / 8 - 1;

class CMSYSTEMX_API FrameLocation : public Value
{
public:
    FrameLocation(uint64_t offset_, SymbolicRegister* baseReg_, Type* type_);
    Value* GetAddress(Context& context) override;
    Value* GetMemberAddress(int32_t memberIndex, Context& context) override;
    void Write(std::ostream& s) override;
    Type* GetType(Context& context) override;
    uint64_t Offset() const { return offset; }
    SymbolicRegister* BaseReg() { return baseReg; }
    void Store(Value* value, Context& context) override;
    void StoreArg(Argument* arg, Value* target, Context& context) override;
    void StoreFrameLocArg(FrameLocation* loc, Argument* arg, Context& context) override;
    void StoreLocalReg(LocalRegister* reg, Context& context) override;
    void StoreValue(SByteValue* value, Context& context) override;
    void StoreValue(ByteValue* value, Context& context) override;
    void StoreValue(ShortValue* value, Context& context) override;
    void StoreValue(UShortValue* value, Context& context) override;
    void StoreValue(IntValue* value, Context& context) override;
    void StoreValue(UIntValue* value, Context& context) override;
    void StoreValue(LongValue* value, Context& context) override;
    void StoreValue(ULongValue* value, Context& context) override;
    void StoreValue(BoolValue* value, Context& context) override;
    void StoreValue(CharValue* value, Context& context) override;
    void StoreValue(WCharValue* value, Context& context) override;
    void StoreValue(UCharValue* value, Context& context) override;
    void StoreValue(FloatValue* value, Context& context) override;
    void StoreValue(DoubleValue* value, Context& context) override;
    void StoreValue(NullValue* value, Context& context) override;
    Value* SignExtend(Type* type, Context& context) override;
private:
    uint64_t offset;
    SymbolicRegister* baseReg;
    Type* type;
    void StoreCo(Value* constant, Context& context);
    void SetConstantAx(uint64_t constant, Context& context);
    void StoreAx(Instruction* storeInst, Context& context);
};

class CMSYSTEMX_API Symbol : public Value
{
public:
    Symbol(const std::string& name_);
    const std::string& Name() const { return name; }
    void Write(std::ostream& s) override;
    Type* GetType(Context& context) override;
private:
    std::string name;
};

class CMSYSTEMX_API Argument : public Value
{
public:
    Argument(Type* type_, Value* store_);
    void Write(std::ostream& s) override;
    Type* GetType(Context& context) override;
    void StoreTo(Value* target, Context& context) override;
private:
    Type* type;
    Value* store;
};

class CMSYSTEMX_API LocalVariable : public Value
{
public:
    LocalVariable(Type* type_, Value* store_);
    void Write(std::ostream& s) override;
    Type* GetType(Context& context) override;
    Value* Load(Context& context) override;
    void Store(Value* value, Context& context) override;
    void StoreRegArg(Register* reg, Argument* arg, Context& context) override;
    void StoreFrameLocArg(FrameLocation* loc, Argument* arg, Context& context) override;
    void StoreLocalReg(LocalRegister* reg, Context& context) override;
    void StoreValue(SByteValue* value, Context& context) override;
    void StoreValue(ByteValue* value, Context& context) override;
    void StoreValue(ShortValue* value, Context& context) override;
    void StoreValue(UShortValue* value, Context& context) override;
    void StoreValue(IntValue* value, Context& context) override;
    void StoreValue(UIntValue* value, Context& context) override;
    void StoreValue(LongValue* value, Context& context) override;
    void StoreValue(ULongValue* value, Context& context) override;
    void StoreValue(BoolValue* value, Context& context) override;
    void StoreValue(CharValue* value, Context& context) override;
    void StoreValue(WCharValue* value, Context& context) override;
    void StoreValue(UCharValue* value, Context& context) override;
    void StoreValue(FloatValue* value, Context& context) override;
    void StoreValue(DoubleValue* value, Context& context) override;
    void StoreValue(NullValue* value, Context& context) override;
    Value* SignExtend(Type* type, Context& context) override;
private:
    Type* type;
    Value* store;
};

} // namespace cmsystemx

#endif // CMAJOR_CMSX_VALUE_INCLUDED
