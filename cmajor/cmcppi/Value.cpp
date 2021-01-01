// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcppi/Value.hpp>
#include <cmajor/cmcppi/Context.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmcppi {

using namespace soulng::unicode;

Value::Value()
{
}

Value::~Value()
{
}

void Value::AddDependencies(GlobalVariable* variable, const std::unordered_map<std::string, GlobalVariable*>& nameMap, std::unordered_map<GlobalVariable*,
    std::set<GlobalVariable*>>& dependencies, Context& context)
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

CharValue::CharValue() : value('\0')
{
}

CharValue::CharValue(char8_t value_) :  value(value_)
{
}

Type* CharValue::GetType(Context& context)
{
    return context.GetCharType();
}

std::string CharValue::Name(Context& context)
{
    return CharStr((char)value);
}

WCharValue::WCharValue() : value(u'\0')
{
}

WCharValue::WCharValue(char16_t value_) : value(value_)
{
}

Type* WCharValue::GetType(Context& context)
{
    return context.GetWCharType();
}

std::string WCharValue::Name(Context& context)
{
    return ToUtf8(U"u'" + CharStr((char32_t)value) + U"'");
}

UCharValue::UCharValue() : value(U'\0')
{
}

UCharValue::UCharValue(char32_t value_) : value(value_)
{
}

Type* UCharValue::GetType(Context& context)
{
    return context.GetUCharType();
}

std::string UCharValue::Name(Context& context)
{
    return ToUtf8(U"U'" + CharStr((char32_t)value) + U"'");
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
    return "nullptr";
}

ArrayValue::ArrayValue(Type* type_, const std::vector<Value*>& elements_, const std::string& prefix_) : ConstantValue(), type(type_), elements(elements_), prefix(prefix_)
{
}

std::string ArrayValue::Name(Context& context)
{
    std::string name = prefix + "{ ";
    bool first = true;
    for (Value* element : elements)
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
            name.append(element->Name(context));
        }
    }
    name.append(" }");
    return name;
}

void ArrayValue::AddElement(ConstantValue* element)
{
    elements.push_back(element);
}

void ArrayValue::AddDependencies(GlobalVariable* variable, const std::unordered_map<std::string, GlobalVariable*>& nameMap,
    std::unordered_map<GlobalVariable*, std::set<GlobalVariable*>>& dependencies, Context& context)
{
    for (Value* element : elements)
    {
        if (element)
        {
            auto it = nameMap.find(element->Name(context));
            if (it != nameMap.cend())
            {
                dependencies[variable].insert(it->second);
            }
            element->AddDependencies(variable, nameMap, dependencies, context);
        }
    }
}

StructureValue::StructureValue(Type* type_, const std::vector<Value*>& members_) : ConstantValue(), type(type_), members(members_)
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
    for (Value* member : members)
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
            name.append(member->Name(context));
        }
    }
    name.append(" }");
    return name;
}

void StructureValue::AddMember(ConstantValue* member)
{
    members.push_back(member);
}

void StructureValue::AddDependencies(GlobalVariable* variable, const std::unordered_map<std::string, GlobalVariable*>& nameMap,
    std::unordered_map<GlobalVariable*, std::set<GlobalVariable*>>& dependencies, Context& context)
{
    for (Value* member : members)
    {
        auto it = nameMap.find(member->Name(context));
        if (it != nameMap.cend())
        {
            dependencies[variable].insert(it->second);
        }
        member->AddDependencies(variable, nameMap, dependencies, context);
    }
}

StringValue::StringValue(Type* type_, const std::string& value_) : ConstantValue(), type(type_), value(value_)
{
}

std::string StringValue::Name(Context& context)
{
    std::string name("\"");
    for (char c : value)
    {
        if (c == '"')
        {
            name.append("\\u00").append(soulng::util::ToHexString(static_cast<uint8_t>(c)));
        }
        else if (c == '\\')
        {
            name.append("\\u00").append(soulng::util::ToHexString(static_cast<uint8_t>(c)));
        }
        else if (c >= 32 && c < 127)
        {
            name.append(1, c);
        }
        else
        {
            name.append("\\u00").append(soulng::util::ToHexString(static_cast<uint8_t>(c)));
        }
    }
    name.append("\"");
    return name;
}

Type* StringValue::GetType(Context& context)
{
    return type;
}

WStringValue::WStringValue(Type* type_, const std::u16string& value_) : ConstantValue(), type(type_), value(value_)
{
}

std::string WStringValue::Name(Context& context)
{
    std::string name("u\"");
    for (char16_t c : value)
    {
        if (c == '"')
        {
            name.append("\\u00").append(soulng::util::ToHexString(static_cast<uint8_t>(c)));
        }
        else if (c == '\\')
        {
            name.append("\\u00").append(soulng::util::ToHexString(static_cast<uint8_t>(c)));
        }
        else if (c >= 32 && c < 127)
        {
            name.append(1, c);
        }
        else
        {
            name.append("\\u").append(soulng::util::ToHexString(static_cast<uint16_t>(c)));
        }
    }
    name.append("\"");
    return name;
}

Type* WStringValue::GetType(Context& context)
{
    return type;
}

UStringValue::UStringValue(Type* type_, const std::u32string& value_) : ConstantValue(), type(type_), value(value_)
{
}

std::string UStringValue::Name(Context& context)
{
    std::string name("U\"");
    for (char16_t c : value)
    {
        if (c == '"')
        {
            name.append("\\U000000").append(soulng::util::ToHexString(static_cast<uint8_t>(c)));
        }
        else if (c == '\\')
        {
            name.append("\\U000000").append(soulng::util::ToHexString(static_cast<uint8_t>(c)));
        }
        else if (c >= 32 && c < 127)
        {
            name.append(1, c);
        }
        else
        {
            name.append("\\U").append(soulng::util::ToHexString(static_cast<uint32_t>(c)));
        }
    }
    name.append("\"");
    return name;
}

Type* UStringValue::GetType(Context& context)
{
    return type;
}

ConversionValue::ConversionValue(Type* type_, ConstantValue* from_) : ConstantValue(), type(type_), from(from_)
{
}

std::string ConversionValue::Name(Context& context)
{
    std::string name = "(" + type->Name() + ")";
    name.append("(").append(from->GetType(context)->Name()).append(1, ')').append(from->Name(context));
    return name;
}

Type* ConversionValue::GetType(Context& context)
{
    return type;
}

void ConversionValue::AddDependencies(GlobalVariable* variable, const std::unordered_map<std::string, GlobalVariable*>& nameMap, std::unordered_map<GlobalVariable*, std::set<GlobalVariable*>>& dependencies,
    Context& context)
{
    auto it = nameMap.find(from->Name(context));
    if (it != nameMap.cend())
    {
        dependencies[variable].insert(it->second);
    }
}

ClsIdValue::ClsIdValue(const std::string& typeId_) : ConstantValue(), typeId(typeId_)
{
}

std::string ClsIdValue::Name(Context& context)
{
    std::string name = "(const void*)" + typeId.substr(0, 8) + ", (const void*)" + typeId.substr(8);
    return name;
}

Type* ClsIdValue::GetType(Context& context)
{
    return context.GetPtrType(context.GetVoidType());
}

} // namespace cmcppi
