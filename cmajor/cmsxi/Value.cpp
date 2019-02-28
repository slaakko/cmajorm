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

BoolValue::BoolValue() : ConstantValue(), value(false)
{
}

BoolValue::BoolValue(bool value_) : ConstantValue(), value(value_)
{
}

Type* BoolValue::GetType(Context& context)
{
    return context.GetBoolType();
}

std::string BoolValue::Name(Context& context)
{
    return value ? "true" : "false";
}

SByteValue::SByteValue() : ConstantValue(), value(0)
{
}

SByteValue::SByteValue(int8_t value_) : ConstantValue(), value(value_)
{
}

Type* SByteValue::GetType(Context& context)
{
    return context.GetSByteType();
}

std::string SByteValue::Name(Context& context)
{
    return std::to_string(value);
}

ByteValue::ByteValue() : ConstantValue(), value(0)
{
}

ByteValue::ByteValue(uint8_t value_) : ConstantValue(), value(value_)
{
}

Type* ByteValue::GetType(Context& context)
{
    return context.GetByteType();
}

std::string ByteValue::Name(Context& context)
{
    return std::to_string(value);
}

ShortValue::ShortValue() : ConstantValue(), value(0)
{
}

ShortValue::ShortValue(int16_t value_) : ConstantValue(), value(value_)
{
}

Type* ShortValue::GetType(Context& context)
{
    return context.GetShortType();
}

std::string ShortValue::Name(Context& context)
{
    return std::to_string(value);
}

UShortValue::UShortValue() : ConstantValue(), value(0)
{
}

UShortValue::UShortValue(uint16_t value_) : ConstantValue(), value(value_)
{
}

Type* UShortValue::GetType(Context& context)
{
    return context.GetUShortType();
}

std::string UShortValue::Name(Context& context)
{
    return std::to_string(value);
}

IntValue::IntValue() : ConstantValue(), value(0)
{
}

IntValue::IntValue(int32_t value_) : ConstantValue(), value(value_)
{
}

Type* IntValue::GetType(Context& context)
{
    return context.GetIntType();
}

std::string IntValue::Name(Context& context)
{
    return std::to_string(value);
}

UIntValue::UIntValue() : ConstantValue(), value(0)
{
}

UIntValue::UIntValue(uint32_t value_) : ConstantValue(), value(value_)
{
}

Type* UIntValue::GetType(Context& context)
{
    return context.GetUIntType();
}

std::string UIntValue::Name(Context& context)
{
    return std::to_string(value);
}

LongValue::LongValue() : ConstantValue(), value(0)
{
}

LongValue::LongValue(int64_t value_) : ConstantValue(), value(value_)
{
}

Type* LongValue::GetType(Context& context)
{
    return context.GetLongType();
}

std::string LongValue::Name(Context& context)
{
    return std::to_string(value);
}

ULongValue::ULongValue() : ConstantValue(), value(0)
{
}

ULongValue::ULongValue(uint64_t value_) : ConstantValue(), value(value_)
{
}

Type* ULongValue::GetType(Context& context)
{
    return context.GetULongType();
}

std::string ULongValue::Name(Context& context)
{
    return std::to_string(value);
}

FloatValue::FloatValue() : ConstantValue(), value(0.0)
{
}

FloatValue::FloatValue(float value_) : ConstantValue(), value(value_)
{
}

Type* FloatValue::GetType(Context& context)
{
    return context.GetFloatType();
}

std::string FloatValue::Name(Context& context)
{
    return std::to_string(value);
}

DoubleValue::DoubleValue() : ConstantValue(), value(0.0)
{
}

DoubleValue::DoubleValue(double value_) : value(value_)
{
}

Type* DoubleValue::GetType(Context& context)
{
    return context.GetDoubleType();
}

std::string DoubleValue::Name(Context& context)
{
    return std::to_string(value);
}

NullValue::NullValue(PtrType* ptrType_) : ConstantValue(), ptrType(ptrType_)
{
}

Type* NullValue::GetType(Context& context)
{
    return ptrType;
}

std::string NullValue::Name(Context& context)
{
    return "null";
}

ArrayValue::ArrayValue(Type* type_, const std::vector<ConstantValue*>& elements_) : ConstantValue(), type(type_), elements(elements_)
{
}

std::string ArrayValue::Name(Context& context)
{
    std::string name = "[ ";
    bool first = true;
    for (ConstantValue* element : elements)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            name.append(", ");
        }
        if (element->IsAggregateValue())
        {
            name.append(element->Name(context));
        }
        else
        {
            name.append(element->GetType(context)->Name()).append(" ").append(element->Name(context));
        }
    }
    name.append(" ]");
    return name;
}

void ArrayValue::AddElement(ConstantValue* element)
{
    elements.push_back(element);
}

StructureValue::StructureValue(Type* type_, const std::vector<ConstantValue*>& members_) : ConstantValue(), type(type_), members(members_)
{
}

Type* StructureValue::GetType(Context& context) 
{
    return type;
}

std::string StructureValue::Name(Context& context)
{
    std::string name = "{ ";
    bool first = true;
    for (ConstantValue* member : members)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            name.append(", ");
        }
        if (member->IsAggregateValue())
        {
            name.append(member->Name(context));
        }
        else
        {
            name.append(member->GetType(context)->Name()).append(" ").append(member->Name(context));
        }
    }
    name.append(" }");
    return name;
}

void StructureValue::AddMember(ConstantValue* member)
{
    members.push_back(member);
}

} // namespace cmsxi
