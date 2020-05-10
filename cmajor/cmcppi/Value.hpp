// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMCPPI_VALUE_INCLUDED
#define CMAJOR_CMCPPI_VALUE_INCLUDED
#include <cmajor/cmcppi/CmcppiApi.hpp>
#include <vector>
#include <string>
#include <stdint.h>

namespace cmcppi {

class Type;
class Context;
class PtrType;

class CMCPPI_API Value
{
public:
    Value();
    virtual ~Value();
    virtual Type* GetType(Context& context) = 0;
    virtual std::string Name(Context& context) = 0;
    virtual bool IsLongValue() const { return false; }
    virtual bool IsAggregateValue() const { return false; }
    virtual bool IsStringValue() const { return false; }
};

class CMCPPI_API ConstantValue : public Value
{
public:
    std::string Name(Context& context) override { return "constant"; }
};

class CMCPPI_API  BoolValue : public ConstantValue
{
public:
    BoolValue();
    BoolValue(bool value_);
    Type* GetType(Context& context) override;
    std::string Name(Context& context) override;
private:
    bool value;
};

class CMCPPI_API SByteValue : public ConstantValue
{
public:
    SByteValue();
    SByteValue(int8_t value_);
    Type* GetType(Context& context) override;
    std::string Name(Context& context) override;
private:
    int8_t value;
};

class CMCPPI_API ByteValue : public ConstantValue
{
public:
    ByteValue();
    ByteValue(uint8_t value_);
    Type* GetType(Context& context) override;
    std::string Name(Context& context) override;
private:
    uint8_t value;
};

class CMCPPI_API ShortValue : public ConstantValue
{
public:
    ShortValue();
    ShortValue(int16_t value_);
    Type* GetType(Context& context) override;
    std::string Name(Context& context) override;
private:
    int16_t value;
};

class CMCPPI_API UShortValue : public ConstantValue
{
public:
    UShortValue();
    UShortValue(uint16_t value_);
    Type* GetType(Context& context) override;
    std::string Name(Context& context) override;
private:
    uint16_t value;
};

class CMCPPI_API IntValue : public ConstantValue
{
public:
    IntValue();
    IntValue(int32_t value_);
    Type* GetType(Context& context) override;
    std::string Name(Context& context) override;
private:
    int32_t value;
};

class CMCPPI_API UIntValue : public ConstantValue
{
public:
    UIntValue();
    UIntValue(uint32_t value_);
    Type* GetType(Context& context) override;
    std::string Name(Context& context) override;
private:
    uint32_t value;
};

class CMCPPI_API LongValue : public ConstantValue
{
public:
    LongValue();
    LongValue(int64_t value_);
    Type* GetType(Context& context) override;
    std::string Name(Context& context) override;
    bool IsLongValue() const override { return true; }
    int64_t GetValue() const { return value; }
private:
    int64_t value;
};

class CMCPPI_API ULongValue : public ConstantValue
{
public:
    ULongValue();
    ULongValue(uint64_t value_);
    Type* GetType(Context& context) override;
    std::string Name(Context& context) override;
private:
    uint64_t value;
};

class CMCPPI_API FloatValue : public ConstantValue
{
public:
    FloatValue();
    FloatValue(float value_);
    Type* GetType(Context& context) override;
    std::string Name(Context& context) override;
private:
    float value;
};

class CMCPPI_API DoubleValue : public ConstantValue
{
public:
    DoubleValue();
    DoubleValue(double value_);
    Type* GetType(Context& context) override;
    std::string Name(Context& context) override;
private:
    double value;
};

class CMCPPI_API NullValue : public ConstantValue
{
public:
    NullValue(PtrType* ptrType_);
    Type* GetType(Context& context) override;
    std::string Name(Context& context) override;
private:
    PtrType* ptrType;
};

class CMCPPI_API ArrayValue : public ConstantValue
{
public:
    ArrayValue(Type* type_, const std::vector<ConstantValue*>& elements_, const std::string& prefix_);
    std::string Name(Context& context) override;
    Type* GetType(Context& context) override { return type; }
    void AddElement(ConstantValue* element);
    bool IsAggregateValue() const override { return true; }
private:
    Type* type;
    std::vector<ConstantValue*> elements;
    std::string prefix;
};

class CMCPPI_API StructureValue : public ConstantValue
{
public:
    StructureValue(Type* type_, const std::vector<ConstantValue*>& members_);
    std::string Name(Context& context) override;
    Type* GetType(Context& context) override;
    void AddMember(ConstantValue* member);
    bool IsAggregateValue() const override { return true; }
private:
    Type* type;
    std::vector<ConstantValue*> members;
};

class CMCPPI_API StringValue : public ConstantValue
{
public:
    StringValue(Type* type_, const std::string& value_);
    std::string Name(Context& context) override;
    Type* GetType(Context& context) override;
    bool IsStringValue() const override { return true; }
private:
    Type* type;
    std::string value;
};

class CMCPPI_API ConversionValue : public ConstantValue
{
public:
    ConversionValue(Type* type_, ConstantValue* from_);
    std::string Name(Context& context) override;
    Type* GetType(Context& context) override;
private:
    Type* type;
    ConstantValue* from;
};

} // namespace cmcppi

#endif // CMAJOR_CMCPPI_VALUE_INCLUDED
