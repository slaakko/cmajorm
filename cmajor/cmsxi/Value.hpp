// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSXI_VALUE_INCLUDED
#define CMAJOR_CMSXI_VALUE_INCLUDED
#include <cmajor/cmsxi/CmsxiApi.hpp>
#include <vector>
#include <string>
#include <stdint.h>

namespace cmsxi {

class Type;
class PtrType;
class Context;

class CMSXI_API Value
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

class CMSXI_API ConstantValue : public Value
{
public:
    std::string Name(Context& context) override { return "constant"; }
};

class CMSXI_API BoolValue : public ConstantValue
{
public:
    BoolValue();
    BoolValue(bool value_);
    Type* GetType(Context& context) override;
    std::string Name(Context& context) override;
private:
    bool value;
};

class CMSXI_API SByteValue : public ConstantValue
{
public:
    SByteValue();
    SByteValue(int8_t value_);
    Type* GetType(Context& context) override;
    std::string Name(Context& context) override;
private:
    int8_t value;
};

class CMSXI_API ByteValue : public ConstantValue
{
public:
    ByteValue();
    ByteValue(uint8_t value_);
    Type* GetType(Context& context) override;
    std::string Name(Context& context) override;
private:
    uint8_t value;
};

class CMSXI_API ShortValue : public ConstantValue
{
public:
    ShortValue();
    ShortValue(int16_t value_);
    Type* GetType(Context& context) override;
    std::string Name(Context& context) override;
private:
    int16_t value;
};

class CMSXI_API UShortValue : public ConstantValue
{
public:
    UShortValue();
    UShortValue(uint16_t value_);
    Type* GetType(Context& context) override;
    std::string Name(Context& context) override;
private:
    uint16_t value;
};

class CMSXI_API IntValue : public ConstantValue
{
public:
    IntValue();
    IntValue(int32_t value_);
    Type* GetType(Context& context) override;
    std::string Name(Context& context) override;
private:
    int32_t value;
};

class CMSXI_API UIntValue : public ConstantValue
{
public:
    UIntValue();
    UIntValue(uint32_t value_);
    Type* GetType(Context& context) override;
    std::string Name(Context& context) override;
private:
    uint32_t value;
};

class CMSXI_API LongValue : public ConstantValue
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

class CMSXI_API ULongValue : public ConstantValue
{
public:
    ULongValue();
    ULongValue(uint64_t value_);
    Type* GetType(Context& context) override;
    std::string Name(Context& context) override;
private:
    uint64_t value;
};

class CMSXI_API FloatValue : public ConstantValue
{
public:
    FloatValue();
    FloatValue(float value_);
    Type* GetType(Context& context) override;
    std::string Name(Context& context) override;
private:
    float value;
};

class CMSXI_API DoubleValue : public ConstantValue
{
public:
    DoubleValue();
    DoubleValue(double value_);
    Type* GetType(Context& context) override;
    std::string Name(Context& context) override;
private:
    double value;
};

class CMSXI_API NullValue : public ConstantValue
{
public:
    NullValue(PtrType* ptrType_);
    Type* GetType(Context& context) override;
    std::string Name(Context& context) override;
private:
    PtrType* ptrType;
};

class CMSXI_API ArrayValue : public ConstantValue
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

class CMSXI_API StructureValue : public ConstantValue
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

class CMSXI_API StringValue : public ConstantValue
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

class CMSXI_API ConversionValue : public ConstantValue
{
public:
    ConversionValue(Type* type_, ConstantValue* from_);
    std::string Name(Context& context) override;
    Type* GetType(Context& context) override;
private:
    Type* type;
    ConstantValue* from;
};

class CMSXI_API ClsIdValue : public ConstantValue
{
public:
    ClsIdValue(const std::string& typeId_);
    std::string Name(Context& context) override;
    Type* GetType(Context& context) override;
private:
    std::string typeId;
};

} // namespace cmsxi

#endif // CMAJOR_CMSXI_VALUE_INCLUDED
