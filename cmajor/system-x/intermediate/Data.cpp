// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/Data.hpp>
#include <system-x/intermediate/Context.hpp>
#include <system-x/intermediate/Error.hpp>
#include <system-x/intermediate/Visitor.hpp>

namespace cmsx::intermediate {

const char* valueKindStr[]
{
    "boolValue", "sbyteValue", "byteValue", "shortValue", "ushortValue", "intValue", "uintValue", "longValue", "ulongValue", "floatValue", "doubleValue", "nullValue", 
    "addressValue",
    "arrayValue", "structureValue", "stringValue", "stringArrayValue", "conversionValue", "clsIdValue", "symbolValue",
    "globalVariable",
    "regValue",
    "instruction"
};

Value::Value(const SourcePos& sourcePos_, ValueKind kind_, Type* type_) : sourcePos(sourcePos_), kind(kind_), type(type_)
{
}

Value::~Value()
{
}

bool Value::IsIntegerValue() const
{
    switch (kind)
    {
        case ValueKind::sbyteValue:
        case ValueKind::byteValue:
        case ValueKind::shortValue:
        case ValueKind::ushortValue:
        case ValueKind::intValue:
        case ValueKind::uintValue:
        case ValueKind::longValue:
        case ValueKind::ulongValue:
        {
            return true;
        }
        default:
        {
            return false;
        }
    }
}

int64_t Value::GetIntegerValue() const
{
    switch (kind)
    {
        case ValueKind::sbyteValue:
        {
            const SByteValue* sbyteValue = static_cast<const SByteValue*>(this);
            return sbyteValue->GetValue();
        }
        case ValueKind::byteValue:
        {
            const ByteValue* byteValue = static_cast<const ByteValue*>(this);
            return byteValue->GetValue();
        }
        case ValueKind::shortValue:
        {
            const ShortValue* shortValue = static_cast<const ShortValue*>(this);
            return shortValue->GetValue();
        }
        case ValueKind::ushortValue:
        {
            const UShortValue* ushortValue = static_cast<const UShortValue*>(this);
            return ushortValue->GetValue();
        }
        case ValueKind::intValue:
        {
            const IntValue* intValue = static_cast<const IntValue*>(this);
            return intValue->GetValue();
        }
        case ValueKind::uintValue:
        {
            const UIntValue* uintValue = static_cast<const UIntValue*>(this);
            return uintValue->GetValue();
        }
        case ValueKind::longValue:
        {
            const LongValue* longValue = static_cast<const LongValue*>(this);
            return longValue->GetValue();
        }
        case ValueKind::ulongValue:
        {
            const ULongValue* ulongValue = static_cast<const ULongValue*>(this);
            return static_cast<int64_t>(ulongValue->GetValue());
        }
        default:
        {
            return -1;
        }
    }
}

std::string Value::KindStr() const
{
    return valueKindStr[static_cast<int>(kind)];
}

ConstantValue::ConstantValue(const SourcePos& sourcePos_, ValueKind kind_, Type* type_) : Value(sourcePos_, kind_, type_)
{
}

BoolValue::BoolValue(bool value_, Type* type_) : ConstantValue(SourcePos(), ValueKind::boolValue, type_), value(value_)
{
}

void BoolValue::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

SByteValue::SByteValue(int8_t value_, Type* type_) : ConstantValue(SourcePos(), ValueKind::sbyteValue, type_), value(value_)
{
}

void SByteValue::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ByteValue::ByteValue(uint8_t value_, Type* type_) : ConstantValue(SourcePos(), ValueKind::byteValue, type_), value(value_)
{
}

void ByteValue::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ShortValue::ShortValue(int16_t value_, Type* type_) : ConstantValue(SourcePos(), ValueKind::shortValue, type_), value(value_)
{
}

void ShortValue::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

UShortValue::UShortValue(uint16_t value_, Type* type_) : ConstantValue(SourcePos(), ValueKind::ushortValue, type_), value(value_)
{
}

void UShortValue::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

IntValue::IntValue(int32_t value_, Type* type_) : ConstantValue(SourcePos(), ValueKind::intValue, type_), value(value_)
{
}

void IntValue::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

UIntValue::UIntValue(uint32_t value_, Type* type_) : ConstantValue(SourcePos(), ValueKind::uintValue, type_), value(value_)
{
}

void UIntValue::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

LongValue::LongValue(int64_t value_, Type* type_) : ConstantValue(SourcePos(), ValueKind::longValue, type_), value(value_)
{
}

void LongValue::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ULongValue::ULongValue(uint64_t value_, Type* type_) : ConstantValue(SourcePos(), ValueKind::ulongValue, type_), value(value_)
{
}

void ULongValue::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

FloatValue::FloatValue(float value_, Type* type_) : ConstantValue(SourcePos(), ValueKind::floatValue, type_), value(value_)
{
}

void FloatValue::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

DoubleValue::DoubleValue(double value_, Type* type_) : ConstantValue(SourcePos(), ValueKind::doubleValue, type_), value(value_)
{
}

void DoubleValue::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

NullValue::NullValue(Type* type_) : ConstantValue(SourcePos(), ValueKind::nullValue, type_)
{
}

void NullValue::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

AddressValue::AddressValue(const SourcePos& sourcePos_, GlobalVariable* globalVariable_, Type* type) :
    ConstantValue(sourcePos_, ValueKind::addressValue, type), globalVariable(globalVariable_)
{
}

void AddressValue::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ArrayValue::ArrayValue(const SourcePos& sourcePos_, const std::vector<ConstantValue*>& elements_) :
    ConstantValue(sourcePos_, ValueKind::arrayValue, nullptr), elements(elements_)
{
}

void ArrayValue::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

StructureValue::StructureValue(const SourcePos& sourcePos_, const std::vector<ConstantValue*>& fieldValues_) :
    ConstantValue(sourcePos_, ValueKind::structureValue, nullptr), fieldValues(fieldValues_)
{
}

void StructureValue::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

StringValue::StringValue(const SourcePos& sourcePos_, const std::string& value_) :
    ConstantValue(sourcePos_, ValueKind::stringValue, nullptr), value(value_)
{
}

void StringValue::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

StringArrayValue::StringArrayValue(const SourcePos& sourcePos_, char prefix_, const std::vector<ConstantValue*>& strings_) : 
    ConstantValue(sourcePos_, ValueKind::stringArrayValue, nullptr), prefix(prefix_), strings(strings_)
{
}

void StringArrayValue::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ConversionValue::ConversionValue(const SourcePos& sourcePos_, Type* type_, ConstantValue* from_) : 
    ConstantValue(sourcePos_, ValueKind::conversionValue, type_), from(from_)
{
}

void ConversionValue::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ClsIdValue::ClsIdValue(const SourcePos& sourcePos_, Type* type_, const std::string& typeId_) : 
    ConstantValue(sourcePos_, ValueKind::clsIdValue, type_), typeId(typeId_)
{
}

void ClsIdValue::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

SymbolValue::SymbolValue(const SourcePos& sourcePos_, Type* type_, const std::string& symbol_) : 
    ConstantValue(sourcePos_, ValueKind::symbolValue, type_), symbol(symbol_), function(nullptr), globalVariable(nullptr)
{
}

void SymbolValue::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

GlobalVariable::GlobalVariable(const SourcePos& sourcePos_, Type* type_, const std::string& name_, ConstantValue* initializer_, bool once_) :
    Value(sourcePos_, ValueKind::globalVariable, type_), name(name_), initializer(initializer_), once(once_)
{
    initializer->SetType(GetType());
}

void GlobalVariable::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

Data::Data() : context(nullptr)
{
}

void Data::AddGlobalVariable(const SourcePos& sourcePos, Type* type, const std::string& variableName, ConstantValue* initializer, bool once, Context* context)
{
    GlobalVariable* globalVariable = new GlobalVariable(sourcePos, type, variableName, initializer, once);
    values.push_back(std::unique_ptr<Value>(globalVariable));
    globalVariableMap[variableName] = globalVariable;
    globalVariables.push_back(globalVariable);
}

ConstantValue* Data::GetTrueValue(const Types& types)
{
    if (!trueValue)
    {
        trueValue.reset(new BoolValue(true, types.Get(boolTypeId)));
    }
    return trueValue.get();
}

ConstantValue* Data::GetFalseValue(const Types& types)
{
    if (!falseValue)
    {
        falseValue.reset(new BoolValue(false, types.Get(boolTypeId)));
    }
    return falseValue.get();
}

ConstantValue* Data::GetSByteValue(int8_t value, const Types& types)
{
    return sbyteValueMap.Get(value, this, types);
}

ConstantValue* Data::GetByteValue(uint8_t value, const Types& types)
{
    return byteValueMap.Get(value, this, types);
}

ConstantValue* Data::GetShortValue(int16_t value, const Types& types)
{
    return shortValueMap.Get(value, this, types);
}

ConstantValue* Data::GetUShortValue(uint16_t value, const Types& types)
{
    return ushortValueMap.Get(value, this, types);
}

ConstantValue* Data::GetIntValue(int32_t value, const Types& types)
{
    return intValueMap.Get(value, this, types);
}

ConstantValue* Data::GetUIntValue(uint32_t value, const Types& types)
{
    return uintValueMap.Get(value, this, types);
}

ConstantValue* Data::GetLongValue(int64_t value, const Types& types)
{
    return longValueMap.Get(value, this, types);
}

ConstantValue* Data::GetULongValue(uint64_t value, const Types& types)
{
    return ulongValueMap.Get(value, this, types);
}

ConstantValue* Data::GetFloatValue(float value, const Types& types)
{
    return floatValueMap.Get(value, this, types);
}

ConstantValue* Data::GetDoubleValue(float value, const Types& types)
{
    return doubleValueMap.Get(value, this, types);
}

ConstantValue* Data::GetNullValue(Type* type)
{
    auto it = nullValueMap.find(type);
    if (it != nullValueMap.cend())
    {
        return it->second;
    }
    else
    {
        NullValue* nullValue = new NullValue(type);
        nullValueMap[type] = nullValue;
        values.push_back(std::unique_ptr<Value>(nullValue));
        return nullValue;
    }
}

ConstantValue* Data::MakeValue(int8_t value, const Types& types)
{
    SByteValue* constantValue = new SByteValue(value, types.Get(sbyteTypeId));
    values.push_back(std::unique_ptr<Value>(constantValue));
    return constantValue;
}

ConstantValue* Data::MakeValue(uint8_t value, const Types& types)
{
    ByteValue* constantValue = new ByteValue(value, types.Get(byteTypeId));
    values.push_back(std::unique_ptr<Value>(constantValue));
    return constantValue;
}

ConstantValue* Data::MakeValue(int16_t value, const Types& types)
{
    ShortValue* constantValue = new ShortValue(value, types.Get(shortTypeId));
    values.push_back(std::unique_ptr<Value>(constantValue));
    return constantValue;
}

ConstantValue* Data::MakeValue(uint16_t value, const Types& types)
{
    UShortValue* constantValue = new UShortValue(value, types.Get(ushortTypeId));
    values.push_back(std::unique_ptr<Value>(constantValue));
    return constantValue;
}

ConstantValue* Data::MakeValue(int32_t value, const Types& types)
{
    IntValue* constantValue = new IntValue(value, types.Get(intTypeId));
    values.push_back(std::unique_ptr<Value>(constantValue));
    return constantValue;
}

ConstantValue* Data::MakeValue(uint32_t value, const Types& types)
{
    UIntValue* constantValue = new UIntValue(value, types.Get(uintTypeId));
    values.push_back(std::unique_ptr<Value>(constantValue));
    return constantValue;
}

ConstantValue* Data::MakeValue(int64_t value, const Types& types)
{
    LongValue* constantValue = new LongValue(value, types.Get(longTypeId));
    values.push_back(std::unique_ptr<Value>(constantValue));
    return constantValue;
}

ConstantValue* Data::MakeValue(uint64_t value, const Types& types)
{
    ULongValue* constantValue = new ULongValue(value, types.Get(ulongTypeId));
    values.push_back(std::unique_ptr<Value>(constantValue));
    return constantValue;
}

ConstantValue* Data::MakeValue(float value, const Types& types)
{
    FloatValue* constantValue = new FloatValue(value, types.Get(floatTypeId));
    values.push_back(std::unique_ptr<Value>(constantValue));
    return constantValue;
}

ConstantValue* Data::MakeValue(double value, const Types& types) 
{
    DoubleValue* constantValue = new DoubleValue(value, types.Get(doubleTypeId));
    values.push_back(std::unique_ptr<Value>(constantValue));
    return constantValue;
}

ConstantValue* Data::MakeArrayValue(const SourcePos& sourcePos, const std::vector<ConstantValue*>& elements)
{
    ArrayValue* arrayValue = new ArrayValue(sourcePos, elements);
    values.push_back(std::unique_ptr<Value>(arrayValue));
    return arrayValue;
}

ConstantValue* Data::MakeStructureValue(const SourcePos& sourcePos, const std::vector<ConstantValue*>& fieldValues)
{
    StructureValue* structureValue = new StructureValue(sourcePos, fieldValues);
    values.push_back(std::unique_ptr<Value>(structureValue));
    return structureValue;
}

ConstantValue* Data::MakeStringValue(const SourcePos& sourcePos, const std::string& value)
{
    StringValue* stringValue = new StringValue(sourcePos, value.substr(1, value.length() - 2));
    values.push_back(std::unique_ptr<Value>(stringValue));
    return stringValue;
}

ConstantValue* Data::MakeStringArrayValue(const SourcePos& sourcePos, char prefix, const std::vector<ConstantValue*>& strings)
{
    StringArrayValue* stringArrayValue = new StringArrayValue(sourcePos, prefix, strings);
    values.push_back(std::unique_ptr<Value>(stringArrayValue));
    return stringArrayValue;
}

ConstantValue* Data::MakeConversionValue(const SourcePos& sourcePos, Type* type, ConstantValue* from)
{
    ConversionValue* conversionValue = new ConversionValue(sourcePos, type, from);
    values.push_back(std::unique_ptr<Value>(conversionValue));
    return conversionValue;
}

ConstantValue* Data::MakeClsIdValue(const SourcePos& sourcePos, Type* type, const std::string& clsIdStr)
{
    // clsid(x)
    std::string typeId = clsIdStr.substr(6, clsIdStr.length() - 6 - 1);
    ClsIdValue* clsIdValue = new ClsIdValue(sourcePos, type, typeId);
    values.push_back(std::unique_ptr<Value>(clsIdValue));
    return clsIdValue;
}

ConstantValue* Data::MakeSymbolValue(const SourcePos& sourcePos, Type* type, const std::string& symbol)
{
    // @xxx
    SymbolValue* symbolValue = new SymbolValue(sourcePos, type, symbol);
    values.push_back(std::unique_ptr<Value>(symbolValue));
    return symbolValue;
}

ConstantValue* Data::MakeNumericLiteral(const SourcePos& sourcePos, Type* type, const std::string& strValue, const Types& types, Context* context)
{
    switch (type->Id())
    {
        case boolTypeId:
        {
            if (strValue == "true")
            {
                return GetTrueValue(types);
            }
            else if (strValue == "false")
            {
                return GetFalseValue(types);
            }
            else
            {
                Error("error making literal: Boolean value expected", sourcePos, context); 
            }
        }
        case sbyteTypeId:
        {
            int64_t value = boost::lexical_cast<int64_t>(strValue);
            if (value < std::numeric_limits<int8_t>::min() || value > std::numeric_limits<int8_t>::max())
            {
                Error("error making literal: range error: sbyte value expected", sourcePos, context);
            }
            return GetSByteValue(static_cast<int8_t>(value), types);
        }
        case byteTypeId:
        {
            int64_t value = boost::lexical_cast<int64_t>(strValue);
            if (value < std::numeric_limits<uint8_t>::min() || value > std::numeric_limits<uint8_t>::max())
            {
                Error("error making literal: range error: byte value expected", sourcePos, context);
            }
            return GetByteValue(static_cast<uint8_t>(value), types);
        }
        case shortTypeId:
        {
            int64_t value = boost::lexical_cast<int64_t>(strValue);
            if (value < std::numeric_limits<int16_t>::min() || value > std::numeric_limits<int16_t>::max())
            {
                Error("error making literal: range error: short value expected", sourcePos, context);
            }
            return GetShortValue(static_cast<int16_t>(value), types);
        }
        case ushortTypeId:
        {
            int64_t value = boost::lexical_cast<int64_t>(strValue);
            if (value < std::numeric_limits<uint16_t>::min() || value > std::numeric_limits<uint16_t>::max())
            {
                Error("error making literal: range error: ushort value expected", sourcePos, context);
            }
            return GetUShortValue(static_cast<uint16_t>(value), types);
        }
        case intTypeId:
        {
            int64_t value = boost::lexical_cast<int64_t>(strValue);
            if (value < std::numeric_limits<int32_t>::min() || value > std::numeric_limits<int32_t>::max())
            {
                Error("error making literal: range error: int value expected", sourcePos, context);
            }
            return GetIntValue(static_cast<int32_t>(value), types);
        }
        case uintTypeId:
        {
            int64_t value = boost::lexical_cast<int64_t>(strValue);
            if (value < std::numeric_limits<uint32_t>::min() || value > std::numeric_limits<uint32_t>::max())
            {
                Error("error making literal: range error: uint value expected", sourcePos, context);
            }
            return GetIntValue(static_cast<uint32_t>(value), types);
        }
        case longTypeId:
        {
            int64_t value = boost::lexical_cast<int64_t>(strValue);
            if (value < std::numeric_limits<int64_t>::min() || value > std::numeric_limits<int64_t>::max())
            {
                Error("error making literal: range error: long value expected", sourcePos, context);
            }
            return GetLongValue(static_cast<int64_t>(value), types);
        }
        case ulongTypeId:
        {
            uint64_t value = boost::lexical_cast<uint64_t>(strValue);
            if (value < std::numeric_limits<uint64_t>::min() || value > std::numeric_limits<uint64_t>::max())
            {
                Error("error making literal: range error: ulong value expected", sourcePos, context);
            }
            return GetULongValue(static_cast<int64_t>(value), types);
        }
        case floatTypeId:
        {
            float value = boost::lexical_cast<float>(strValue);
            return GetFloatValue(value, types);
        }
        case doubleTypeId:
        {
            double value = boost::lexical_cast<double>(strValue);
            return GetDoubleValue(value, types);
        }
        default:
        {
            Error("error making literal: invalid numeric value", sourcePos, context);
        }
    }
    return nullptr;
}

ConstantValue* Data::MakeAddressLiteral(const SourcePos& sourcePos, Type* type, const std::string& id, Context* context)
{
    auto it = globalVariableMap.find(id);
    if (it != globalVariableMap.cend())
    {
        GlobalVariable* globalVariable = it->second;
        AddressValue* addressValue = new AddressValue(sourcePos, globalVariable, globalVariable->GetType()->AddPointer(context));
        values.push_back(std::unique_ptr<Value>(addressValue));
        return addressValue;
    }
    else
    {
        Error("error making address literal: global variable id '" + id + "' not found", sourcePos, context);
    }
    return nullptr;
}

void Data::VisitGlobalVariables(Visitor& visitor)
{
    for (GlobalVariable* globalVariable : globalVariables)
    {
        globalVariable->Accept(visitor);
    }
}

} // cmsx::intermediate
