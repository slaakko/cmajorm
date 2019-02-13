// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsxi/Value.hpp>
#include <cmajor/cmsxi/Context.hpp>

namespace cmsxi {

Value::Value()
{
}

Value::~Value()
{
}

BoolValue::BoolValue() : Value(), value(false)
{
}

BoolValue::BoolValue(bool value_) : Value(), value(value_)
{
}

Type* BoolValue::GetType(Context& context)
{
    return context.GetBoolType();
}

std::string BoolValue::Name()
{
    return value ? "true" : "false";
}

SByteValue::SByteValue() : value(0)
{
}

SByteValue::SByteValue(int8_t value_) : Value(), value(value_)
{
}

Type* SByteValue::GetType(Context& context)
{
    return context.GetSByteType();
}

std::string SByteValue::Name()
{
    return std::to_string(value);
}

ByteValue::ByteValue() : Value(), value(0)
{
}

ByteValue::ByteValue(uint8_t value_) : Value(), value(value_)
{
}

Type* ByteValue::GetType(Context& context)
{
    return context.GetByteType();
}

std::string ByteValue::Name()
{
    return std::to_string(value);
}

ShortValue::ShortValue() : Value(), value(0)
{
}

ShortValue::ShortValue(int16_t value_) : Value(), value(value_)
{
}

Type* ShortValue::GetType(Context& context)
{
    return context.GetShortType();
}

std::string ShortValue::Name()
{
    return std::to_string(value);
}

UShortValue::UShortValue() : Value(), value(0)
{
}

UShortValue::UShortValue(uint16_t value_) : Value(), value(value_)
{
}

Type* UShortValue::GetType(Context& context)
{
    return context.GetUShortType();
}

std::string UShortValue::Name()
{
    return std::to_string(value);
}

IntValue::IntValue() : Value(), value(0)
{
}

IntValue::IntValue(int32_t value_) : Value(), value(value_)
{
}

Type* IntValue::GetType(Context& context)
{
    return context.GetIntType();
}

std::string IntValue::Name()
{
    return std::to_string(value);
}

UIntValue::UIntValue() : Value(), value(0)
{
}

UIntValue::UIntValue(uint32_t value_) : Value(), value(value_)
{
}

Type* UIntValue::GetType(Context& context)
{
    return context.GetUIntType();
}

std::string UIntValue::Name()
{
    return std::to_string(value);
}

LongValue::LongValue() : Value(), value(0)
{
}

LongValue::LongValue(int64_t value_) : Value(), value(value_)
{
}

Type* LongValue::GetType(Context& context)
{
    return context.GetLongType();
}

std::string LongValue::Name()
{
    return std::to_string(value);
}

ULongValue::ULongValue() : Value(), value(0)
{
}

ULongValue::ULongValue(uint64_t value_) : Value(), value(value_)
{
}

Type* ULongValue::GetType(Context& context)
{
    return context.GetULongType();
}

std::string ULongValue::Name()
{
    return std::to_string(value);
}

FloatValue::FloatValue() : Value(), value(0.0)
{
}

FloatValue::FloatValue(float value_) : Value(), value(value_)
{
}

Type* FloatValue::GetType(Context& context)
{
    return context.GetFloatType();
}

std::string FloatValue::Name()
{
    return std::to_string(value);
}

DoubleValue::DoubleValue() : Value(), value(0.0)
{
}

DoubleValue::DoubleValue(double value_) : value(value_)
{
}

Type* DoubleValue::GetType(Context& context)
{
    return context.GetDoubleType();
}

std::string DoubleValue::Name()
{
    return std::to_string(value);
}

NullValue::NullValue(PtrType* ptrType_) : Value(), ptrType(ptrType_)
{
}

Type* NullValue::GetType(Context& context)
{
    return ptrType;
}

std::string NullValue::Name()
{
    return "null";
}

} // namespace cmsxi
