// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSXI_VALUE_INCLUDED
#define CMAJOR_CMSXI_VALUE_INCLUDED
#include <cmajor/cmsxi/CmsxiApi.hpp>
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
    virtual std::string Name() = 0;
    virtual bool IsLongValue() const { return false; }
};

class CMSXI_API BoolValue : public Value
{
public:
    BoolValue();
    BoolValue(bool value_);
    Type* GetType(Context& context) override;
    std::string Name() override;
private:
    bool value;
};

class CMSXI_API SByteValue : public Value
{
public:
    SByteValue();
    SByteValue(int8_t value_);
    Type* GetType(Context& context) override;
    std::string Name() override;
private:
    int8_t value;
};

class CMSXI_API ByteValue : public Value
{
public:
    ByteValue();
    ByteValue(uint8_t value_);
    Type* GetType(Context& context) override;
    std::string Name() override;
private:
    uint8_t value;
};

class CMSXI_API ShortValue : public Value
{
public:
    ShortValue();
    ShortValue(int16_t value_);
    Type* GetType(Context& context) override;
    std::string Name() override;
private:
    int16_t value;
};

class CMSXI_API UShortValue : public Value
{
public:
    UShortValue();
    UShortValue(uint16_t value_);
    Type* GetType(Context& context) override;
    std::string Name() override;
private:
    uint16_t value;
};

class CMSXI_API IntValue : public Value
{
public:
    IntValue();
    IntValue(int32_t value_);
    Type* GetType(Context& context) override;
    std::string Name() override;
private:
    int32_t value;
};

class CMSXI_API UIntValue : public Value
{
public:
    UIntValue();
    UIntValue(uint32_t value_);
    Type* GetType(Context& context) override;
    std::string Name() override;
private:
    uint32_t value;
};

class CMSXI_API LongValue : public Value
{
public:
    LongValue();
    LongValue(int64_t value_);
    Type* GetType(Context& context) override;
    std::string Name() override;
    bool IsLongValue() const override { return true; }
    int64_t GetValue() const { return value; }
private:
    int64_t value;
};

class CMSXI_API ULongValue : public Value
{
public:
    ULongValue();
    ULongValue(uint64_t value_);
    Type* GetType(Context& context) override;
    std::string Name() override;
private:
    uint64_t value;
};

class CMSXI_API FloatValue : public Value
{
public:
    FloatValue();
    FloatValue(float value_);
    Type* GetType(Context& context) override;
    std::string Name() override;
private:
    float value;
};

class CMSXI_API DoubleValue : public Value
{
public:
    DoubleValue();
    DoubleValue(double value_);
    Type* GetType(Context& context) override;
    std::string Name() override;
private:
    double value;
};

class CMSXI_API NullValue : public Value
{
public:
    NullValue(PtrType* ptrType_);
    Type* GetType(Context& context) override;
    std::string Name() override;
private:
    PtrType* ptrType;
};
} // namespace cmsxi

#endif // CMAJOR_CMSXI_VALUE_INCLUDED
