// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/Value.hpp>
#include <cmajor/symbols/TypeSymbol.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/Module.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace symbols {

using namespace soulng::unicode;

const char* valueTypeStr[]
{
    "none", "bool", "sbyte", "byte", "short", "ushort", "int", "uint", "long", "ulong", "float", "double", "char", "wchar", "uchar", "string", "wstring", "ustring", "null", "pointer", "array", "structure", "uuid"
};

std::string ValueTypeStr(ValueType valueType)
{
    return valueTypeStr[uint8_t(valueType)];
}

ValueType commonType[uint8_t(ValueType::maxValue)][uint8_t(ValueType::maxValue)] = 
{
    // ValueType::none
    {   
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, 
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, 
        ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none
    },

    // ValueType::boolValue
    {   
        ValueType::none, ValueType::boolValue, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, 
        ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none
    },
    
    // ValueType::sbyteValue
    {   
        ValueType::none, ValueType::none, ValueType::sbyteValue, ValueType::shortValue, ValueType::shortValue, ValueType::intValue, ValueType::intValue, ValueType::longValue,
        ValueType::longValue, ValueType::none, ValueType::floatValue, ValueType::doubleValue, ValueType::none, ValueType::none, ValueType::none, 
        ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none
    },

    // ValueType::byteValue
    {
        ValueType::none, ValueType::none, ValueType::shortValue, ValueType::byteValue, ValueType::shortValue, ValueType::ushortValue, ValueType::intValue, ValueType::uintValue,
        ValueType::longValue, ValueType::ulongValue, ValueType::floatValue, ValueType::doubleValue, ValueType::none, ValueType::none, ValueType::none, 
        ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none
    },

    // ValueType::shortValue
    {
        ValueType::none, ValueType::none, ValueType::shortValue, ValueType::shortValue, ValueType::shortValue, ValueType::intValue, ValueType::intValue, ValueType::longValue,
        ValueType::longValue, ValueType::none, ValueType::floatValue, ValueType::doubleValue, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none
    },

    // ValueType::ushortValue
    {
        ValueType::none, ValueType::none, ValueType::intValue, ValueType::ushortValue, ValueType::intValue, ValueType::ushortValue, ValueType::intValue, ValueType::uintValue,
        ValueType::longValue, ValueType::ulongValue, ValueType::floatValue, ValueType::doubleValue, ValueType::none, ValueType::none, ValueType::none, 
        ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none
    },

    // ValueType::intValue
    {
        ValueType::none, ValueType::none, ValueType::intValue, ValueType::intValue, ValueType::intValue, ValueType::intValue, ValueType::intValue, ValueType::longValue,
        ValueType::longValue, ValueType::none, ValueType::floatValue, ValueType::doubleValue, ValueType::none, ValueType::none, ValueType::none, 
        ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none
    },

    // ValueType::uintValue
    {
        ValueType::none, ValueType::none, ValueType::longValue, ValueType::uintValue, ValueType::longValue, ValueType::uintValue, ValueType::longValue, ValueType::uintValue,
        ValueType::longValue, ValueType::ulongValue, ValueType::floatValue, ValueType::doubleValue, ValueType::none, ValueType::none, ValueType::none, 
        ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none
    },

    // ValueType::longValue
    {
        ValueType::none, ValueType::none, ValueType::longValue, ValueType::longValue, ValueType::longValue, ValueType::longValue, ValueType::longValue, ValueType::longValue,
        ValueType::longValue, ValueType::none, ValueType::floatValue, ValueType::doubleValue, ValueType::none, ValueType::none, ValueType::none, 
        ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none
    },

    // ValueType::ulongValue
    {
        ValueType::none, ValueType::none, ValueType::none, ValueType::ulongValue, ValueType::none, ValueType::ulongValue, ValueType::none, ValueType::ulongValue,
        ValueType::none, ValueType::ulongValue, ValueType::floatValue, ValueType::doubleValue, ValueType::none, ValueType::none, ValueType::none, 
        ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none
    },

    // ValueType::floatValue
    {
        ValueType::none, ValueType::none, ValueType::floatValue, ValueType::floatValue, ValueType::floatValue, ValueType::floatValue, ValueType::floatValue, ValueType::floatValue,
        ValueType::floatValue, ValueType::floatValue, ValueType::floatValue, ValueType::doubleValue, ValueType::none, ValueType::none, ValueType::none, 
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, 
        ValueType::none, ValueType::none, ValueType::none, ValueType::none
    },

    // ValueType::doubleValue
    {
        ValueType::none, ValueType::none, ValueType::doubleValue, ValueType::doubleValue, ValueType::doubleValue, ValueType::doubleValue, ValueType::doubleValue, ValueType::doubleValue,
        ValueType::doubleValue, ValueType::doubleValue, ValueType::doubleValue, ValueType::doubleValue, ValueType::none, ValueType::none, ValueType::none, 
        ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none
    },

    // ValueType::charValue
    {
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, 
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::charValue, ValueType::wcharValue, ValueType::ucharValue, 
        ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none
    },

    // ValueType::wcharValue
    {
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::wcharValue, ValueType::wcharValue, ValueType::ucharValue, 
        ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none
    },

    // ValueType::ucharValue
    {
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::ucharValue, ValueType::ucharValue, ValueType::ucharValue, 
        ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none
    },

    // ValueType::stringValue
    {
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, 
        ValueType::stringValue, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none
    },

    // ValueType::wstringValue
    {
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::wstringValue, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none
    },

    // ValueType::ustringValue
    {
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::ustringValue, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none
    },
        
    // ValueType::nullValue
    {
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, 
        ValueType::none, ValueType::none, ValueType::none, ValueType::nullValue,
        ValueType::pointerValue, ValueType::none, ValueType::none, ValueType::none
    },

    // ValueType::pointerValue
    {
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, 
        ValueType::none, ValueType::none, ValueType::none, ValueType::pointerValue,
        ValueType::pointerValue, ValueType::none, ValueType::none, ValueType::none
    }, 

    // ValueType::arrayValue
    {
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::arrayValue, ValueType::none, ValueType::none
    },

    // ValueType::structuredValue
    {
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::structuredValue, ValueType::none
    },

    // ValueType::uuidValue
    {
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::none,
        ValueType::none, ValueType::none, ValueType::none, ValueType::uuidValue
    }
};

ValueType CommonType(ValueType left, ValueType right)
{
    return commonType[uint8_t(left)][uint8_t(right)];
}

TypeSymbol* GetTypeFor(ValueType valueType, SymbolTable* symbolTable)
{
    switch (valueType)
    {
        case ValueType::boolValue: return symbolTable->GetTypeByName(U"bool");
        case ValueType::sbyteValue: return symbolTable->GetTypeByName(U"sbyte");
        case ValueType::byteValue: return symbolTable->GetTypeByName(U"byte");
        case ValueType::shortValue: return symbolTable->GetTypeByName(U"short");
        case ValueType::ushortValue: return symbolTable->GetTypeByName(U"ushort");
        case ValueType::intValue: return symbolTable->GetTypeByName(U"int");
        case ValueType::uintValue: return symbolTable->GetTypeByName(U"uint");
        case ValueType::longValue: return symbolTable->GetTypeByName(U"long");
        case ValueType::ulongValue: return symbolTable->GetTypeByName(U"ulong");
        case ValueType::floatValue: return symbolTable->GetTypeByName(U"float");
        case ValueType::doubleValue: return symbolTable->GetTypeByName(U"double");
        case ValueType::charValue: return symbolTable->GetTypeByName(U"char");
        case ValueType::wcharValue: return symbolTable->GetTypeByName(U"wchar");
        case ValueType::ucharValue: return symbolTable->GetTypeByName(U"uchar");
        case ValueType::nullValue: return symbolTable->GetTypeByName(U"@nullptr_type");
    }
    return nullptr;
}

Value::Value(const Span& span_, const boost::uuids::uuid& moduleId_, ValueType valueType_) : span(span_), moduleId(moduleId_), valueType(valueType_)
{
}

Value::~Value()
{
}

std::unique_ptr<sngxml::dom::Element> Value::ToDomElement()
{
    std::u32string className = ToUtf32(ClassName());
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(className));
    element->SetAttribute(U"info", ToUtf32(ToString()));
    return element;
}

BoolValue::BoolValue(const Span& span_, const boost::uuids::uuid& moduleId_, bool value_) : Value(span_, moduleId_, ValueType::boolValue), value(value_)
{
}

void* BoolValue::IrValue(Emitter& emitter)
{
    return emitter.CreateIrValueForBool(value);
}

void BoolValue::Write(BinaryWriter& writer)
{
    writer.Write(value);
}

void BoolValue::Read(BinaryReader& reader)
{
    reader.ReadBool();
}

Value* BoolValue::As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow) const
{
    switch (targetType->GetValueType())
    {
        case ValueType::boolValue: 
        {
            return new BoolValue(span, moduleId, value);
        }
        case ValueType::sbyteValue:
        {
            if (cast)
            {
                return new SByteValue(span, moduleId, static_cast<int8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::byteValue:
        {
            if (cast)
            {
                return new ByteValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::shortValue:
        {
            if (cast)
            {
                return new ShortValue(span, moduleId, static_cast<int16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ushortValue:
        {
            if (cast)
            {
                return new UShortValue(span, moduleId, static_cast<uint16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::intValue:
        {
            if (cast)
            {
                return new IntValue(span, moduleId, static_cast<int32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::uintValue:
        {
            if (cast)
            {
                return new UIntValue(span, moduleId, static_cast<uint32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::longValue:
        {
            if (cast)
            {
                return new LongValue(span, moduleId, static_cast<int64_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ulongValue:
        {
            if (cast)
            {
                return new ULongValue(span, moduleId, static_cast<uint64_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::floatValue:
        {
            if (cast)
            {
                return new FloatValue(span, moduleId, static_cast<float>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::doubleValue:
        {
            if (cast)
            {
                return new DoubleValue(span, moduleId, static_cast<double>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::charValue:
        {
            if (cast)
            {
                return new CharValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::wcharValue:
        {
            if (cast)
            {
                return new WCharValue(span, moduleId, static_cast<uint16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ucharValue:
        {
            if (cast)
            {
                return new UCharValue(span, moduleId, static_cast<uint32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        default:
        {
            if (dontThrow)
            {
                return nullptr;
            }
            else
            {
                throw Exception("conversion from " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " is not valid", span, moduleId);
            }
        }
    }
}

TypeSymbol* BoolValue::GetType(SymbolTable* symbolTable)
{
    return symbolTable->GetTypeByName(U"bool");
}

SByteValue::SByteValue(const Span& span_, const boost::uuids::uuid& moduleId_, int8_t value_) : Value(span_, moduleId_, ValueType::sbyteValue), value(value_)
{
}

void* SByteValue::IrValue(Emitter& emitter)
{
    return emitter.CreateIrValueForSByte(value);
}

void SByteValue::Write(BinaryWriter& writer)
{
    writer.Write(value);
}

void SByteValue::Read(BinaryReader& reader)
{
    value = reader.ReadSByte();
}

Value* SByteValue::As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow) const
{
    switch (targetType->GetValueType())
    {
        case ValueType::boolValue:
        {
            if (cast)
            {
                return new BoolValue(span, moduleId, static_cast<bool>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::sbyteValue:
        {
            return new SByteValue(span, moduleId, value);
        }
        case ValueType::byteValue:
        {
            if (cast)
            {
                return new ByteValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::shortValue:
        {
            return new ShortValue(span, moduleId, value);
        }
        case ValueType::ushortValue:
        {
            if (cast)
            {
                return new UShortValue(span, moduleId, static_cast<uint16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::intValue:
        {
            return new IntValue(span, moduleId, value);
        }
        case ValueType::uintValue:
        {
            if (cast)
            {
                return new UIntValue(span, moduleId, static_cast<uint32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::longValue:
        {
            return new LongValue(span, moduleId, value);
        }
        case ValueType::ulongValue:
        {
            if (cast)
            {
                return new ULongValue(span, moduleId, static_cast<uint64_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::floatValue:
        {
            return new FloatValue(span, moduleId, value);
        }
        case ValueType::doubleValue:
        {
            return new FloatValue(span, moduleId, value);
        }
        case ValueType::charValue:
        {
            if (cast)
            {
                return new CharValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::wcharValue:
        {
            if (cast)
            {
                return new CharValue(span, moduleId, static_cast<uint16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ucharValue:
        {
            if (cast)
            {
                return new CharValue(span, moduleId, static_cast<uint32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        default:
        {
            if (dontThrow)
            {
                return nullptr;
            }
            else
            {
                throw Exception("conversion from " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " is not valid", span, moduleId);
            }
        }
    }
}

TypeSymbol* SByteValue::GetType(SymbolTable* symbolTable)
{
    return symbolTable->GetTypeByName(U"sbyte");
}

ByteValue::ByteValue(const Span& span_, const boost::uuids::uuid& moduleId_, uint8_t value_) : Value(span_, moduleId_, ValueType::byteValue), value(value_)
{
}

void* ByteValue::IrValue(Emitter& emitter)
{
    return emitter.CreateIrValueForByte(value);
}

void ByteValue::Write(BinaryWriter& writer)
{
    writer.Write(value);
}

void ByteValue::Read(BinaryReader& reader)
{
    value = reader.ReadByte();
}

Value* ByteValue::As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow) const
{
    switch (targetType->GetValueType())
    {
        case ValueType::boolValue:
        {
            if (cast)
            {
                return new BoolValue(span, moduleId, static_cast<bool>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::sbyteValue:
        {
            if (cast)
            {
                return new SByteValue(span, moduleId, static_cast<int8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::byteValue:
        {
            return new ByteValue(span, moduleId, value);
        }
        case ValueType::shortValue:
        {
            return new ShortValue(span, moduleId, value);
        }
        case ValueType::ushortValue:
        {
            return new UShortValue(span, moduleId, value);
        }
        case ValueType::intValue:
        {
            return new IntValue(span, moduleId, value);
        }
        case ValueType::uintValue:
        {
            return new UIntValue(span, moduleId, value);
        }
        case ValueType::longValue:
        {
            return new LongValue(span, moduleId, value);
        }
        case ValueType::ulongValue:
        {
            return new ULongValue(span, moduleId, value);
        }
        case ValueType::floatValue:
        {
            return new FloatValue(span, moduleId, value);
        }
        case ValueType::doubleValue:
        {
            return new DoubleValue(span, moduleId, value);
        }
        case ValueType::charValue:
        {
            if (cast)
            {
                return new CharValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::wcharValue:
        {
            if (cast)
            {
                return new WCharValue(span, moduleId, static_cast<uint16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ucharValue:
        {
            if (cast)
            {
                return new UCharValue(span, moduleId, static_cast<uint32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        default:
        {
            if (dontThrow)
            {
                return nullptr;
            }
            else
            {
                throw Exception("conversion from " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " is not valid", span, moduleId);
            }
        }
    }
}

TypeSymbol* ByteValue::GetType(SymbolTable* symbolTable)
{
    return symbolTable->GetTypeByName(U"byte");
}

ShortValue::ShortValue(const Span& span_, const boost::uuids::uuid& moduleId_, int16_t value_) : Value(span_, moduleId_, ValueType::shortValue), value(value_)
{
}

void* ShortValue::IrValue(Emitter& emitter)
{
    return emitter.CreateIrValueForShort(value);
}

void ShortValue::Write(BinaryWriter& writer)
{
    writer.Write(value);
}

void ShortValue::Read(BinaryReader& reader)
{
    value = reader.ReadShort();
}

Value* ShortValue::As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow) const
{
    switch (targetType->GetValueType())
    {
        case ValueType::boolValue:
        {
            if (cast)
            {
                return new BoolValue(span, moduleId, static_cast<bool>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::sbyteValue:
        {
            if (cast)
            {
                return new SByteValue(span, moduleId, static_cast<int8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::byteValue:
        {
            if (cast)
            {
                return new ByteValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::shortValue:
        {
            return new ShortValue(span, moduleId, value);
        }
        case ValueType::ushortValue:
        {
            if (cast)
            {
                return new UShortValue(span, moduleId, static_cast<uint16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::intValue:
        {
            return new IntValue(span, moduleId, value);
        }
        case ValueType::uintValue:
        {
            if (cast)
            {
                return new UIntValue(span, moduleId, static_cast<uint32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::longValue:
        {
            return new LongValue(span, moduleId, value);
        }
        case ValueType::ulongValue:
        {
            if (cast)
            {
                return new ULongValue(span, moduleId, static_cast<uint64_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::floatValue:
        {
            return new FloatValue(span, moduleId, value);
        }
        case ValueType::doubleValue:
        {
            return new DoubleValue(span, moduleId, value);
        }
        case ValueType::charValue:
        {
            if (cast)
            {
                return new CharValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::wcharValue:
        {
            if (cast)
            {
                return new WCharValue(span, moduleId, static_cast<uint16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ucharValue:
        {
            if (cast)
            {
                return new UCharValue(span, moduleId, static_cast<uint32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        default:
        {
            if (dontThrow)
            {
                return nullptr;
            }
            else
            {
                throw Exception("conversion from " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " is not valid", span, moduleId);
            }
        }
    }
}

TypeSymbol* ShortValue::GetType(SymbolTable* symbolTable)
{
    return symbolTable->GetTypeByName(U"short");
}

UShortValue::UShortValue(const Span& span_, const boost::uuids::uuid& moduleId_, uint16_t value_) : Value(span_, moduleId_, ValueType::ushortValue), value(value_)
{
}

void* UShortValue::IrValue(Emitter& emitter)
{
    return emitter.CreateIrValueForUShort(value);
}

void UShortValue::Write(BinaryWriter& writer)
{
    writer.Write(value);
}

void UShortValue::Read(BinaryReader& reader)
{
    value = reader.ReadUShort();
}

Value* UShortValue::As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow) const
{
    switch (targetType->GetValueType())
    {
        case ValueType::boolValue:
        {
            if (cast)
            {
                return new BoolValue(span, moduleId, static_cast<bool>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::sbyteValue:
        {
            if (cast)
            {
                return new SByteValue(span, moduleId, static_cast<int8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::byteValue:
        {
            if (cast)
            {
                return new ByteValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::shortValue:
        {
            if (cast)
            {
                return new ShortValue(span, moduleId, static_cast<int16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ushortValue:
        {
            return new UShortValue(span, moduleId, value);
        }
        case ValueType::intValue:
        {
            return new IntValue(span, moduleId, value);
        }
        case ValueType::uintValue:
        {
            return new UIntValue(span, moduleId, value);
        }
        case ValueType::longValue:
        {
            return new LongValue(span, moduleId, value);
        }
        case ValueType::ulongValue:
        {
            return new ULongValue(span, moduleId,value);
        }
        case ValueType::floatValue:
        {
            return new FloatValue(span, moduleId, value);
        }
        case ValueType::doubleValue:
        {
            return new DoubleValue(span, moduleId, value);
        }
        case ValueType::charValue:
        {
            if (cast)
            {
                return new CharValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::wcharValue:
        {
            if (cast)
            {
                return new WCharValue(span, moduleId, static_cast<uint16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ucharValue:
        {
            if (cast)
            {
                return new UCharValue(span, moduleId, static_cast<uint32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        default:
        {
            if (dontThrow)
            {
                return nullptr;
            }
            else
            {
                throw Exception("conversion from " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " is not valid", span, moduleId);
            }
        }
    }
}

TypeSymbol* UShortValue::GetType(SymbolTable* symbolTable)
{
    return symbolTable->GetTypeByName(U"ushort");
}

IntValue::IntValue(const Span& span_, const boost::uuids::uuid& moduleId_, int32_t value_) : Value(span_, moduleId_, ValueType::intValue), value(value_)
{
}

void* IntValue::IrValue(Emitter& emitter)
{
    return emitter.CreateIrValueForInt(value);
}

void IntValue::Write(BinaryWriter& writer)
{
    writer.Write(value);
}

void IntValue::Read(BinaryReader& reader)
{
    value = reader.ReadInt();
}

Value* IntValue::As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow) const
{
    switch (targetType->GetValueType())
    {
        case ValueType::boolValue:
        {
            if (cast)
            {
                return new BoolValue(span, moduleId, static_cast<bool>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::sbyteValue:
        {
            if (cast)
            {
                return new SByteValue(span, moduleId, static_cast<int8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::byteValue:
        {
            if (cast)
            {
                return new ByteValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::shortValue:
        {
            if (cast)
            {
                return new ShortValue(span, moduleId, static_cast<int16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ushortValue:
        {
            if (cast)
            {
                return new UShortValue(span, moduleId, static_cast<uint16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::intValue:
        {
            return new IntValue(span, moduleId, value);
        }
        case ValueType::uintValue:
        {
            if (cast)
            {
                return new UIntValue(span, moduleId, static_cast<uint32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::longValue:
        {
            return new LongValue(span, moduleId, value);
        }
        case ValueType::ulongValue:
        {
            if (cast)
            {
                return new ULongValue(span, moduleId, static_cast<uint64_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::floatValue:
        {
            return new FloatValue(span, moduleId, value);
        }
        case ValueType::doubleValue:
        {
            return new DoubleValue(span, moduleId, value);
        }
        case ValueType::charValue:
        {
            if (cast)
            {
                return new CharValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::wcharValue:
        {
            if (cast)
            {
                return new WCharValue(span, moduleId, static_cast<uint16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ucharValue:
        {
            if (cast)
            {
                return new UCharValue(span, moduleId, static_cast<uint32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        default:
        {
            if (dontThrow)
            {
                return nullptr;
            }
            else
            {
                throw Exception("conversion from " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " is not valid", span, moduleId);
            }
        }
    }
}

TypeSymbol* IntValue::GetType(SymbolTable* symbolTable)
{
    return symbolTable->GetTypeByName(U"int");
}

UIntValue::UIntValue(const Span& span_, const boost::uuids::uuid& moduleId_, uint32_t value_) : Value(span_, moduleId_, ValueType::uintValue), value(value_)
{
}

void* UIntValue::IrValue(Emitter& emitter)
{
    return emitter.CreateIrValueForUInt(value);
}

void UIntValue::Write(BinaryWriter& writer)
{
    writer.Write(value);
}

void UIntValue::Read(BinaryReader& reader)
{
    value = reader.ReadUInt();
}

Value* UIntValue::As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow) const
{
    switch (targetType->GetValueType())
    {
        case ValueType::boolValue:
        {
            if (cast)
            {
                return new BoolValue(span, moduleId, static_cast<bool>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::sbyteValue:
        {
            if (cast)
            {
                return new SByteValue(span, moduleId, static_cast<int8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::byteValue:
        {
            if (cast)
            {
                return new ByteValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::shortValue:
        {
            if (cast)
            {
                return new ShortValue(span, moduleId, static_cast<int16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ushortValue:
        {
            if (cast)
            {
                return new UShortValue(span, moduleId, static_cast<uint16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::intValue:
        {
            if (cast)
            {
                return new IntValue(span, moduleId, static_cast<int32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::uintValue:
        {
            return new UIntValue(span, moduleId, value);
        }
        case ValueType::longValue:
        {
            return new LongValue(span, moduleId, value);
        }
        case ValueType::ulongValue:
        {
            return new ULongValue(span, moduleId, value);
        }
        case ValueType::floatValue:
        {
            return new FloatValue(span, moduleId, value);
        }
        case ValueType::doubleValue:
        {
            return new DoubleValue(span, moduleId, value);
        }
        case ValueType::charValue:
        {
            if (cast)
            {
                return new CharValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::wcharValue:
        {
            if (cast)
            {
                return new WCharValue(span, moduleId, static_cast<uint16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ucharValue:
        {
            if (cast)
            {
                return new UCharValue(span, moduleId, static_cast<uint32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        default:
        {
            if (dontThrow)
            {
                return nullptr;
            }
            else
            {
                throw Exception("conversion from " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " is not valid", span, moduleId);
            }
        }
    }
}

TypeSymbol* UIntValue::GetType(SymbolTable* symbolTable)
{
    return symbolTable->GetTypeByName(U"uint");
}

LongValue::LongValue(const Span& span_, const boost::uuids::uuid& moduleId_, int64_t value_) : Value(span_, moduleId_, ValueType::longValue), value(value_)
{
}

void* LongValue::IrValue(Emitter& emitter)
{
    return emitter.CreateIrValueForLong(value);
}

void LongValue::Write(BinaryWriter& writer)
{
    writer.Write(value);
}

void LongValue::Read(BinaryReader& reader)
{
    value = reader.ReadLong();
}

Value* LongValue::As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow) const
{
    switch (targetType->GetValueType())
    {
        case ValueType::boolValue:
        {
            if (cast)
            {
                return new BoolValue(span, moduleId, static_cast<bool>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::sbyteValue:
        {
            if (cast)
            {
                return new SByteValue(span, moduleId, static_cast<int8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::byteValue:
        {
            if (cast)
            {
                return new ByteValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::shortValue:
        {
            if (cast)
            {
                return new ShortValue(span, moduleId, static_cast<int16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ushortValue:
        {
            if (cast)
            {
                return new UShortValue(span, moduleId, static_cast<uint16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::intValue:
        {
            if (cast)
            {
                return new IntValue(span, moduleId, static_cast<int32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::uintValue:
        {
            if (cast)
            {
                return new UIntValue(span, moduleId, static_cast<uint32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::longValue:
        {
            return new LongValue(span, moduleId, value);
        }
        case ValueType::ulongValue:
        {
            if (cast)
            {
                return new ULongValue(span, moduleId, static_cast<uint64_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::floatValue:
        {
            return new FloatValue(span, moduleId, value);
        }
        case ValueType::doubleValue:
        {
            return new DoubleValue(span, moduleId, value);
        }
        case ValueType::charValue:
        {
            if (cast)
            {
                return new CharValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::wcharValue:
        {
            if (cast)
            {
                return new WCharValue(span, moduleId, static_cast<uint16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ucharValue:
        {
            if (cast)
            {
                return new UCharValue(span, moduleId, static_cast<uint32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        default:
        {
            if (dontThrow)
            {
                return nullptr;
            }
            else
            {
                throw Exception("conversion from " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " is not valid", span, moduleId);
            }
        }
    }
}

TypeSymbol* LongValue::GetType(SymbolTable* symbolTable)
{
    return symbolTable->GetTypeByName(U"long");
}

ULongValue::ULongValue(const Span& span_, const boost::uuids::uuid& moduleId_, uint64_t value_) : Value(span_, moduleId_, ValueType::ulongValue), value(value_)
{
}

void* ULongValue::IrValue(Emitter& emitter)
{
    return emitter.CreateIrValueForULong(value);
}

void ULongValue::Write(BinaryWriter& writer)
{
    writer.Write(value);
}

void ULongValue::Read(BinaryReader& reader)
{
    value = reader.ReadULong();
}

Value* ULongValue::As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow) const
{
    switch (targetType->GetValueType())
    {
        case ValueType::boolValue:
        {
            if (cast)
            {
                return new BoolValue(span, moduleId, static_cast<bool>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::sbyteValue:
        {
            if (cast)
            {
                return new SByteValue(span, moduleId, static_cast<int8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::byteValue:
        {
            if (cast)
            {
                return new ByteValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::shortValue:
        {
            if (cast)
            {
                return new ShortValue(span, moduleId, static_cast<int16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ushortValue:
        {
            if (cast)
            {
                return new UShortValue(span, moduleId, static_cast<uint16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::intValue:
        {
            if (cast)
            {
                return new IntValue(span, moduleId, static_cast<int32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::uintValue:
        {
            if (cast)
            {
                return new UIntValue(span, moduleId, static_cast<uint32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::longValue:
        {
            if (cast)
            {
                return new LongValue(span, moduleId, static_cast<int64_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ulongValue:
        {
            return new ULongValue(span, moduleId, value);
        }
        case ValueType::floatValue:
        {
            return new FloatValue(span, moduleId, value);
        }
        case ValueType::doubleValue:
        {
            return new DoubleValue(span, moduleId, value);
        }
        case ValueType::charValue:
        {
            if (cast)
            {
                return new CharValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::wcharValue:
        {
            if (cast)
            {
                return new WCharValue(span, moduleId, static_cast<uint16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ucharValue:
        {
            if (cast)
            {
                return new UCharValue(span, moduleId, static_cast<uint32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        default:
        {
            if (dontThrow)
            {
                return nullptr;
            }
            else
            {
                throw Exception("conversion from " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " is not valid", span, moduleId);
            }
        }
    }
}

TypeSymbol* ULongValue::GetType(SymbolTable* symbolTable)
{
    return symbolTable->GetTypeByName(U"ulong");
}

FloatValue::FloatValue(const Span& span_, const boost::uuids::uuid& moduleId_, float value_) : Value(span_, moduleId_, ValueType::floatValue), value(value_)
{
}

void* FloatValue::IrValue(Emitter& emitter)
{
    return emitter.CreateIrValueForFloat(value);
}

void FloatValue::Write(BinaryWriter& writer)
{
    writer.Write(value);
}

void FloatValue::Read(BinaryReader& reader)
{
    value = reader.ReadFloat();
}

Value* FloatValue::As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow) const
{
    switch (targetType->GetValueType())
    {
        case ValueType::boolValue:
        {
            if (cast)
            {
                return new BoolValue(span, moduleId, static_cast<bool>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::sbyteValue:
        {
            if (cast)
            {
                return new SByteValue(span, moduleId, static_cast<int8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::byteValue:
        {
            if (cast)
            {
                return new ByteValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::shortValue:
        {
            if (cast)
            {
                return new ShortValue(span, moduleId, static_cast<int16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ushortValue:
        {
            if (cast)
            {
                return new UShortValue(span, moduleId, static_cast<uint16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::intValue:
        {
            if (cast)
            {
                return new IntValue(span, moduleId, static_cast<int32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::uintValue:
        {
            if (cast)
            {
                return new UIntValue(span, moduleId, static_cast<uint32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::longValue:
        {
            if (cast)
            {
                return new LongValue(span, moduleId, static_cast<int64_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ulongValue:
        {
            if (cast)
            {
                return new ULongValue(span, moduleId, static_cast<uint64_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::floatValue:
        {
            return new FloatValue(span, moduleId, value);
        }
        case ValueType::doubleValue:
        {
            return new DoubleValue(span, moduleId, value);
        }
        case ValueType::charValue:
        {
            if (cast)
            {
                return new CharValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::wcharValue:
        {
            if (cast)
            {
                return new WCharValue(span, moduleId, static_cast<uint16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ucharValue:
        {
            if (cast)
            {
                return new UCharValue(span, moduleId, static_cast<uint32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        default:
        {
            if (dontThrow)
            {
                return nullptr;
            }
            else
            {
                throw Exception("conversion from " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " is not valid", span, moduleId);
            }
        }
    }
}

TypeSymbol* FloatValue::GetType(SymbolTable* symbolTable)
{
    return symbolTable->GetTypeByName(U"float");
}

DoubleValue::DoubleValue(const Span& span_, const boost::uuids::uuid& moduleId_, double value_) : Value(span_, moduleId_,ValueType::doubleValue), value(value_)
{
}

void* DoubleValue::IrValue(Emitter& emitter)
{
    return emitter.CreateIrValueForDouble(value);
}

void DoubleValue::Write(BinaryWriter& writer)
{
    writer.Write(value);
}

void DoubleValue::Read(BinaryReader& reader)
{
    value = reader.ReadDouble();
}

Value* DoubleValue::As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow) const
{
    switch (targetType->GetValueType())
    {
        case ValueType::boolValue:
        {
            if (cast)
            {
                return new BoolValue(span, moduleId, static_cast<bool>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::sbyteValue:
        {
            if (cast)
            {
                return new SByteValue(span, moduleId, static_cast<int8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::byteValue:
        {
            if (cast)
            {
                return new ByteValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::shortValue:
        {
            if (cast)
            {
                return new ShortValue(span, moduleId, static_cast<int16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ushortValue:
        {
            if (cast)
            {
                return new UShortValue(span, moduleId, static_cast<uint16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::intValue:
        {
            if (cast)
            {
                return new IntValue(span, moduleId, static_cast<int32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::uintValue:
        {
            if (cast)
            {
                return new UIntValue(span, moduleId, static_cast<uint32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::longValue:
        {
            if (cast)
            {
                return new LongValue(span, moduleId, static_cast<int64_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ulongValue:
        {
            if (cast)
            {
                return new ULongValue(span, moduleId, static_cast<uint64_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::floatValue:
        {
            if (cast)
            {
                return new FloatValue(span, moduleId, static_cast<float>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::doubleValue:
        {
            return new DoubleValue(span, moduleId, value);
        }
        case ValueType::charValue:
        {
            if (cast)
            {
                return new CharValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::wcharValue:
        {
            if (cast)
            {
                return new WCharValue(span, moduleId, static_cast<uint16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ucharValue:
        {
            if (cast)
            {
                return new UCharValue(span, moduleId, static_cast<uint32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        default:
        {
            if (dontThrow)
            {
                return nullptr;
            }
            else
            {
                throw Exception("conversion from " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " is not valid", span, moduleId);
            }
        }
    }
}

TypeSymbol* DoubleValue::GetType(SymbolTable* symbolTable)
{
    return symbolTable->GetTypeByName(U"double");
}

CharValue::CharValue(const Span& span_, const boost::uuids::uuid& moduleId_, unsigned char value_) : Value(span_, moduleId_, ValueType::charValue), value(value_)
{
}

void* CharValue::IrValue(Emitter& emitter)
{
    return emitter.CreateIrValueForChar(static_cast<uint8_t>(value));
}

void CharValue::Write(BinaryWriter& writer)
{
    writer.Write(value);
}

void CharValue::Read(BinaryReader& reader)
{
    value = reader.ReadChar();
}

Value* CharValue::As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow) const
{
    switch (targetType->GetValueType())
    {
        case ValueType::boolValue:
        {
            if (cast)
            {
                return new BoolValue(span, moduleId, static_cast<bool>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::sbyteValue:
        {
            if (cast)
            {
                return new SByteValue(span, moduleId, static_cast<int8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::byteValue:
        {
            if (cast)
            {
                return new ByteValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::shortValue:
        {
            if (cast)
            {
                return new ShortValue(span, moduleId, static_cast<int16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ushortValue:
        {
            if (cast)
            {
                return new UShortValue(span, moduleId, static_cast<uint16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::intValue:
        {
            if (cast)
            {
                return new IntValue(span, moduleId, static_cast<int32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::uintValue:
        {
            if (cast)
            {
                return new UIntValue(span, moduleId, static_cast<uint32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::longValue:
        {
            if (cast)
            {
                return new LongValue(span, moduleId, static_cast<int64_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ulongValue:
        {
            if (cast)
            {
                return new ULongValue(span, moduleId, static_cast<uint64_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::floatValue:
        {
            if (cast)
            {
                return new FloatValue(span, moduleId, static_cast<float>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::doubleValue:
        {
            if (cast)
            {
                return new DoubleValue(span, moduleId, static_cast<double>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::charValue:
        {
            return new CharValue(span, moduleId, value);
        }
        case ValueType::wcharValue:
        {
            return new WCharValue(span, moduleId, value);
        }
        case ValueType::ucharValue:
        {
            return new UCharValue(span, moduleId, value);
        }
        default:
        {
            if (dontThrow)
            {
                return nullptr;
            }
            else
            {
                throw Exception("conversion from " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " is not valid", span, moduleId);
            }
        }
    }
}

TypeSymbol* CharValue::GetType(SymbolTable* symbolTable)
{
    return symbolTable->GetTypeByName(U"char");
}

WCharValue::WCharValue(const Span& span_, const boost::uuids::uuid& moduleId_, char16_t value_) : Value(span_, moduleId_, ValueType::wcharValue), value(value_)
{
}

void* WCharValue::IrValue(Emitter& emitter)
{
    return emitter.CreateIrValueForWChar(static_cast<uint16_t>(value));
}

void WCharValue::Write(BinaryWriter& writer)
{
    writer.Write(value);
}

void WCharValue::Read(BinaryReader& reader)
{
    value = reader.ReadWChar();
}

Value* WCharValue::As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow) const
{
    switch (targetType->GetValueType())
    {
        case ValueType::boolValue:
        {
            if (cast)
            {
                return new BoolValue(span, moduleId, static_cast<bool>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::sbyteValue:
        {
            if (cast)
            {
                return new SByteValue(span, moduleId, static_cast<int8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::byteValue:
        {
            if (cast)
            {
                return new ByteValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::shortValue:
        {
            if (cast)
            {
                return new ShortValue(span, moduleId, static_cast<int16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ushortValue:
        {
            if (cast)
            {
                return new UShortValue(span, moduleId, static_cast<uint16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::intValue:
        {
            if (cast)
            {
                return new IntValue(span, moduleId, static_cast<int32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::uintValue:
        {
            if (cast)
            {
                return new UIntValue(span, moduleId, static_cast<uint32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::longValue:
        {
            if (cast)
            {
                return new LongValue(span, moduleId, static_cast<int64_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ulongValue:
        {
            if (cast)
            {
                return new ULongValue(span, moduleId, static_cast<uint64_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::floatValue:
        {
            if (cast)
            {
                return new FloatValue(span, moduleId, static_cast<float>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::doubleValue:
        {
            if (cast)
            {
                return new DoubleValue(span, moduleId, static_cast<double>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::charValue:
        {
            if (cast)
            {
                return new CharValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::wcharValue:
        {
            return new WCharValue(span, moduleId, value);
        }
        case ValueType::ucharValue:
        {
            return new UCharValue(span, moduleId, value);
        }
        default:
        {
            if (dontThrow)
            {
                return nullptr;
            }
            else
            {
                throw Exception("conversion from " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " is not valid", span, moduleId);
            }
        }
    }
}

TypeSymbol* WCharValue::GetType(SymbolTable* symbolTable)
{
    return symbolTable->GetTypeByName(U"wchar");
}

UCharValue::UCharValue(const Span& span_, const boost::uuids::uuid& moduleId_, char32_t value_) : Value(span_, moduleId_, ValueType::ucharValue), value(value_)
{
}

void* UCharValue::IrValue(Emitter& emitter)
{
    return emitter.CreateIrValueForUChar(static_cast<uint32_t>(value));
}

void UCharValue::Write(BinaryWriter& writer)
{
    writer.Write(value);
}

void UCharValue::Read(BinaryReader& reader)
{
    value = reader.ReadUChar();
}

Value* UCharValue::As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow) const
{
    switch (targetType->GetValueType())
    {
        case ValueType::boolValue:
        {
            if (cast)
            {
                return new BoolValue(span, moduleId, static_cast<bool>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::sbyteValue:
        {
            if (cast)
            {
                return new SByteValue(span, moduleId, static_cast<int8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::byteValue:
        {
            if (cast)
            {
                return new ByteValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::shortValue:
        {
            if (cast)
            {
                return new ShortValue(span, moduleId, static_cast<int16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ushortValue:
        {
            if (cast)
            {
                return new UShortValue(span, moduleId, static_cast<uint16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::intValue:
        {
            if (cast)
            {
                return new IntValue(span, moduleId, static_cast<int32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::uintValue:
        {
            if (cast)
            {
                return new UIntValue(span, moduleId, static_cast<uint32_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::longValue:
        {
            if (cast)
            {
                return new LongValue(span, moduleId, static_cast<int64_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ulongValue:
        {
            if (cast)
            {
                return new ULongValue(span, moduleId, static_cast<uint64_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::floatValue:
        {
            if (cast)
            {
                return new FloatValue(span, moduleId, static_cast<float>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::doubleValue:
        {
            if (cast)
            {
                return new DoubleValue(span, moduleId, static_cast<double>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::charValue:
        {
            if (cast)
            {
                return new CharValue(span, moduleId, static_cast<uint8_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::wcharValue:
        {
            if (cast)
            {
                return new WCharValue(span, moduleId, static_cast<uint16_t>(value));
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        case ValueType::ucharValue:
        {
            return new UCharValue(span, moduleId, value);
        }
        default:
        {
            if (dontThrow)
            {
                return nullptr;
            }
            else
            {
                throw Exception("conversion from " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " is not valid", span, moduleId);
            }
        }
    }
}

TypeSymbol* UCharValue::GetType(SymbolTable* symbolTable)
{
    return symbolTable->GetTypeByName(U"uchar");
}

StringValue::StringValue(const Span& span_, const boost::uuids::uuid& moduleId_, int stringId_, const std::string& str_) : Value(span_, moduleId_, ValueType::stringValue), stringId(stringId_), str(str_)
{
}

void* StringValue::IrValue(Emitter& emitter)
{
    if (stringId == -1)
    {
        stringId = emitter.Install(str);
    }
    return emitter.GetGlobalStringPtr(stringId);
}

void StringValue::Write(BinaryWriter& writer)
{
    writer.Write(str);
}

void StringValue::Read(BinaryReader& reader)
{
    str = reader.ReadUtf8String();
}

Value* StringValue::As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow) const
{
    switch (targetType->GetValueType())
    {
        case ValueType::stringValue:
        {
            return new StringValue(span, moduleId, stringId, str);
        }
        default:
        {
            if (dontThrow)
            {
                return nullptr;
            }
            else
            {
                throw Exception("conversion from " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " is not valid", span, moduleId);
            }
        }
    }
}

TypeSymbol* StringValue::GetType(SymbolTable* symbolTable)
{
    return symbolTable->GetTypeByName(U"char")->AddConst(GetSpan(), ModuleId())->AddPointer(GetSpan(), ModuleId());
}

WStringValue::WStringValue(const Span& span_, const boost::uuids::uuid& moduleId_, int stringId_, const std::u16string& str_) : Value(span_, moduleId_, ValueType::wstringValue), stringId(stringId_), str(str_)
{
}

void* WStringValue::IrValue(Emitter& emitter)
{ 
    if (stringId == -1)
    {
        stringId = emitter.Install(str);
    }
    void* wstringConstant = emitter.GetGlobalWStringConstant(stringId);
    return emitter.CreateIrValueForWString(wstringConstant);
}

void WStringValue::Write(BinaryWriter& writer)
{
}

void WStringValue::Read(BinaryReader& reader)
{
}

Value* WStringValue::As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow) const
{
    switch (targetType->GetValueType())
    {
        case ValueType::wstringValue:
        {
            return new WStringValue(span, moduleId, stringId, str);
        }
        default:
        {
            if (dontThrow)
            {
                return nullptr;
            }
            else
            {
                throw Exception("conversion from " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " is not valid", span, moduleId);
            }
        }
    }
}

TypeSymbol* WStringValue::GetType(SymbolTable* symbolTable)
{
    return symbolTable->GetTypeByName(U"wchar")->AddConst(GetSpan(), ModuleId())->AddPointer(GetSpan(), ModuleId());
}

UStringValue::UStringValue(const Span& span_, const boost::uuids::uuid& moduleId_, int stringId_, const std::u32string& str_) : Value(span_, moduleId_, ValueType::ustringValue), stringId(stringId_), str(str_)
{
}

void* UStringValue::IrValue(Emitter& emitter)
{
    if (stringId == -1)
    {
        stringId = emitter.Install(str);
    }
    void* ustringConstant = emitter.GetGlobalUStringConstant(stringId);
    return emitter.CreateIrValueForUString(ustringConstant);
}

void UStringValue::Write(BinaryWriter& writer)
{
}

void UStringValue::Read(BinaryReader& reader)
{
}

Value* UStringValue::As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow) const
{
    switch (targetType->GetValueType())
    {
        case ValueType::ustringValue:
        {
            return new UStringValue(span, moduleId, stringId, str);
        }
        default:
        {
            if (dontThrow)
            {
                return nullptr;
            }
            else
            {
                throw Exception("conversion from " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " is not valid", span, moduleId);
            }
        }
    }
}

TypeSymbol* UStringValue::GetType(SymbolTable* symbolTable)
{
    return symbolTable->GetTypeByName(U"uchar")->AddConst(GetSpan(), ModuleId())->AddPointer(GetSpan(), ModuleId());
}

NullValue::NullValue(const Span& span_, const boost::uuids::uuid& moduleId_, TypeSymbol* nullPtrType_) : Value(span_, moduleId_, ValueType::nullValue), nullPtrType(nullPtrType_)
{
}

void* NullValue::IrValue(Emitter& emitter)
{
    return emitter.CreateDefaultIrValueForPtrType(nullPtrType->IrType(emitter));
}

void NullValue::Write(BinaryWriter& writer)
{
}

void NullValue::Read(BinaryReader& reader)
{
}

Value* NullValue::As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow) const
{
    switch (targetType->GetValueType())
    {
        case ValueType::nullValue:
        {
            return new NullValue(span, moduleId, nullPtrType);
        }
        case ValueType::pointerValue:
        {
            if (targetType->IsPointerType())
            {
                return new PointerValue(span, moduleId, targetType, nullptr);
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("conversion from " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " is not valid", span, moduleId);
                }
            }
        }
        default:
        {
            if (dontThrow)
            {
                return nullptr;
            }
            else
            {
                throw Exception("conversion from " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " is not valid", span, moduleId);
            }
        }
    }
}

TypeSymbol* NullValue::GetType(SymbolTable* symbolTable)
{
    return nullPtrType;
}

PointerValue::PointerValue(const Span& span_, const boost::uuids::uuid& moduleId_, TypeSymbol* type_, const void* ptr_) : Value(span_, moduleId_, ValueType::pointerValue), type(type_), ptr(ptr_)
{
}

void* PointerValue::IrValue(Emitter& emitter)
{
    if (ptr)
    {
        throw std::runtime_error("IrValue for non-null pointers not supported");
    }
    else
    {
        return emitter.CreateDefaultIrValueForPtrType(type->IrType(emitter));
    }
}

void PointerValue::Write(BinaryWriter& writer)
{
}

void PointerValue::Read(BinaryReader& reader)
{
    ptr = nullptr;
}

Value* PointerValue::As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow) const
{
    switch (targetType->GetValueType())
    {
        case ValueType::pointerValue:
        {
            if (cast)
            {
                return new PointerValue(span, moduleId, targetType, ptr);
            }
            else
            {
                if (dontThrow)
                {
                    return nullptr;
                }
                else
                {
                    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " without a cast", span, moduleId);
                }
            }
        }
        default:
        {
            if (dontThrow)
            {
                return nullptr;
            }
            else
            {
                throw Exception("conversion from " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " is not valid", span, moduleId);
            }
        }
    }
}

TypeSymbol* PointerValue::GetType(SymbolTable* symbolTable)
{
    return type;
}

TypeSymbol* PointerValue::PointeeType() const
{ 
    return type->RemovePointer(GetSpan(), ModuleId());
}

Value* PointerValue::Add(int64_t offset) const
{
    ValueType pointeeValueType = PointeeType()->GetValueType();
    switch (pointeeValueType)
    {
        case ValueType::boolValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const BoolValue::OperandType*>(ptr) + offset);
        case ValueType::sbyteValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const SByteValue::OperandType*>(ptr) + offset);
        case ValueType::byteValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const ByteValue::OperandType*>(ptr) + offset);
        case ValueType::shortValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const ShortValue::OperandType*>(ptr) + offset);
        case ValueType::ushortValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const UShortValue::OperandType*>(ptr) + offset);
        case ValueType::intValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const IntValue::OperandType*>(ptr) + offset);
        case ValueType::uintValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const UIntValue::OperandType*>(ptr) + offset);
        case ValueType::longValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const LongValue::OperandType*>(ptr) + offset);
        case ValueType::ulongValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const ULongValue::OperandType*>(ptr) + offset);
        case ValueType::floatValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const FloatValue::OperandType*>(ptr) + offset);
        case ValueType::doubleValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const DoubleValue::OperandType*>(ptr) + offset);
        case ValueType::charValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const CharValue::OperandType*>(ptr) + offset);
        case ValueType::wcharValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const WCharValue::OperandType*>(ptr) + offset);
        case ValueType::ucharValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const UCharValue::OperandType*>(ptr) + offset);
        case ValueType::pointerValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const PointerValue::OperandType*>(ptr) + offset);
    }
    return nullptr;
}

Value* PointerValue::Sub(int64_t offset) const
{
    ValueType pointeeValueType = PointeeType()->GetValueType();
    switch (pointeeValueType)
    {
        case ValueType::boolValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const BoolValue::OperandType*>(ptr) - offset);
        case ValueType::sbyteValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const SByteValue::OperandType*>(ptr) - offset);
        case ValueType::byteValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const ByteValue::OperandType*>(ptr) - offset);
        case ValueType::shortValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const ShortValue::OperandType*>(ptr) - offset);
        case ValueType::ushortValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const UShortValue::OperandType*>(ptr) - offset);
        case ValueType::intValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const IntValue::OperandType*>(ptr) - offset);
        case ValueType::uintValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const UIntValue::OperandType*>(ptr) - offset);
        case ValueType::longValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const LongValue::OperandType*>(ptr) - offset);
        case ValueType::ulongValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const ULongValue::OperandType*>(ptr) - offset);
        case ValueType::floatValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const FloatValue::OperandType*>(ptr) - offset);
        case ValueType::doubleValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const DoubleValue::OperandType*>(ptr) - offset);
        case ValueType::charValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const CharValue::OperandType*>(ptr) - offset);
        case ValueType::wcharValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const WCharValue::OperandType*>(ptr) - offset);
        case ValueType::ucharValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const UCharValue::OperandType*>(ptr) - offset);
        case ValueType::pointerValue: return new PointerValue(GetSpan(), ModuleId(), type, static_cast<const PointerValue::OperandType*>(ptr) - offset);
    }
    return nullptr;
}

Value* PointerValue::Sub(const void* thatPtr) const
{
    ValueType pointeeValueType = PointeeType()->GetValueType();
    switch (pointeeValueType)
    {
        case ValueType::boolValue: return new LongValue(GetSpan(), ModuleId(), static_cast<const BoolValue::OperandType*>(ptr) - static_cast<const BoolValue::OperandType*>(thatPtr));
        case ValueType::sbyteValue: return new LongValue(GetSpan(), ModuleId(), static_cast<const SByteValue::OperandType*>(ptr) - static_cast<const SByteValue::OperandType*>(thatPtr));
        case ValueType::byteValue: return new LongValue(GetSpan(), ModuleId(), static_cast<const ByteValue::OperandType*>(ptr) - static_cast<const ByteValue::OperandType*>(thatPtr));
        case ValueType::shortValue: return new LongValue(GetSpan(), ModuleId(), static_cast<const ShortValue::OperandType*>(ptr) - static_cast<const ShortValue::OperandType*>(thatPtr));
        case ValueType::ushortValue: return new LongValue(GetSpan(), ModuleId(), static_cast<const UShortValue::OperandType*>(ptr) - static_cast<const UShortValue::OperandType*>(thatPtr));
        case ValueType::intValue: return new LongValue(GetSpan(), ModuleId(), static_cast<const IntValue::OperandType*>(ptr) - static_cast<const IntValue::OperandType*>(thatPtr));
        case ValueType::uintValue: return new LongValue(GetSpan(), ModuleId(), static_cast<const UIntValue::OperandType*>(ptr) - static_cast<const UIntValue::OperandType*>(thatPtr));
        case ValueType::longValue: return new LongValue(GetSpan(), ModuleId(), static_cast<const LongValue::OperandType*>(ptr) - static_cast<const LongValue::OperandType*>(thatPtr));
        case ValueType::ulongValue: return new LongValue(GetSpan(), ModuleId(), static_cast<const ULongValue::OperandType*>(ptr) - static_cast<const ULongValue::OperandType*>(thatPtr));
        case ValueType::floatValue: return new LongValue(GetSpan(), ModuleId(), static_cast<const FloatValue::OperandType*>(ptr) - static_cast<const FloatValue::OperandType*>(thatPtr));
        case ValueType::doubleValue: return new LongValue(GetSpan(), ModuleId(), static_cast<const DoubleValue::OperandType*>(ptr) - static_cast<const DoubleValue::OperandType*>(thatPtr));
        case ValueType::charValue: return new LongValue(GetSpan(), ModuleId(), static_cast<const CharValue::OperandType*>(ptr) - static_cast<const CharValue::OperandType*>(thatPtr));
        case ValueType::wcharValue: return new LongValue(GetSpan(), ModuleId(), static_cast<const WCharValue::OperandType*>(ptr) - static_cast<const WCharValue::OperandType*>(thatPtr));
        case ValueType::ucharValue: return new LongValue(GetSpan(), ModuleId(), static_cast<const UCharValue::OperandType*>(ptr) - static_cast<const UCharValue::OperandType*>(thatPtr));
    }
    return nullptr;
}

Value* PointerValue::Deref() const
{
    ValueType pointeeValueType = PointeeType()->GetValueType();
    switch (pointeeValueType)
    {
        case ValueType::boolValue: return new BoolValue(GetSpan(), ModuleId(), *static_cast<const BoolValue::OperandType*>(ptr));
        case ValueType::sbyteValue: return new SByteValue(GetSpan(), ModuleId(), *static_cast<const SByteValue::OperandType*>(ptr));
        case ValueType::byteValue: return new ByteValue(GetSpan(), ModuleId(), *static_cast<const ByteValue::OperandType*>(ptr));
        case ValueType::shortValue: return new ShortValue(GetSpan(), ModuleId(), *static_cast<const ShortValue::OperandType*>(ptr));
        case ValueType::ushortValue: return new UShortValue(GetSpan(), ModuleId(), *static_cast<const UShortValue::OperandType*>(ptr));
        case ValueType::intValue: return new IntValue(GetSpan(), ModuleId(), *static_cast<const IntValue::OperandType*>(ptr));
        case ValueType::uintValue: return new UIntValue(GetSpan(), ModuleId(), *static_cast<const UIntValue::OperandType*>(ptr));
        case ValueType::longValue: return new LongValue(GetSpan(), ModuleId(), *static_cast<const LongValue::OperandType*>(ptr));
        case ValueType::ulongValue: return new ULongValue(GetSpan(), ModuleId(), *static_cast<const ULongValue::OperandType*>(ptr));
        case ValueType::floatValue: return new FloatValue(GetSpan(), ModuleId(), *static_cast<const FloatValue::OperandType*>(ptr));
        case ValueType::doubleValue: return new DoubleValue(GetSpan(), ModuleId(), *static_cast<const DoubleValue::OperandType*>(ptr));
        case ValueType::charValue: return new CharValue(GetSpan(), ModuleId(), *static_cast<const CharValue::OperandType*>(ptr));
        case ValueType::wcharValue: return new WCharValue(GetSpan(), ModuleId(), *static_cast<const WCharValue::OperandType*>(ptr));
        case ValueType::ucharValue: return new UCharValue(GetSpan(), ModuleId(), *static_cast<const UCharValue::OperandType*>(ptr));
    }
    return nullptr;
}

ArrayValue::ArrayValue(const Span& span_, const boost::uuids::uuid& moduleId_, TypeSymbol* type_, std::vector<std::unique_ptr<Value>>&& elementValues_) :
    Value(span_, moduleId_, ValueType::arrayValue), type(type_), elementValues(std::move(elementValues_))
{
}

Value* ArrayValue::Clone() const
{
    std::vector<std::unique_ptr<Value>> clonedElementValues; 
    int64_t n = elementValues.size();
    for (int64_t i = 0; i < n; ++i)
    {
        clonedElementValues.push_back(std::unique_ptr<Value>(elementValues[i]->Clone()));
    }
    return new ArrayValue(GetSpan(), ModuleId(), type, std::move(clonedElementValues));
}

void* ArrayValue::IrValue(Emitter& emitter)
{
    std::vector<void*> elementConstants;
    int64_t n = elementValues.size();
    for (int64_t i = 0; i < n; ++i)
    {
        elementConstants.push_back(elementValues[i]->IrValue(emitter));
    }
    return emitter.CreateIrValueForConstantArray(type->IrType(emitter), elementConstants, std::string());
}

void ArrayValue::Write(BinaryWriter& writer)
{
    int64_t length = elementValues.size();
    writer.Write(length);
    for (int64_t i = 0; i < length; ++i)
    {
        Value* value = elementValues[i].get();
        value->Write(writer);
    }
}

void ArrayValue::Read(BinaryReader& reader)
{
    int64_t length = reader.ReadLong();
    for (int64_t i = 0; i < length; ++i)
    {
        Value* elementValue = static_cast<ArrayTypeSymbol*>(type)->ElementType()->MakeValue();
        elementValue->Read(reader);
        elementValues.push_back(std::unique_ptr<Value>(elementValue));
    }
}

Value* ArrayValue::As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow) const
{
    if (TypesEqual(targetType, type))
    {
        return Clone();
    }
    else
    {
        if (dontThrow)
        {
            return nullptr;
        }
        else
        {
            throw Exception("conversion from " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " is not valid", span, moduleId);
        }
    }
}

StructuredValue::StructuredValue(const Span& span_, const boost::uuids::uuid& moduleId_, TypeSymbol* type_, std::vector<std::unique_ptr<Value>>&& memberValues_) :
    Value(span_, moduleId_, ValueType::structuredValue), type(type_), memberValues(std::move(memberValues_))
{
}

Value* StructuredValue::Clone() const
{
    std::vector<std::unique_ptr<Value>> clonedMemberValues;
    int n = memberValues.size();
    for (int i = 0; i < n; ++i)
    {
        std::unique_ptr<Value> memberValue(memberValues[i]->Clone());
        clonedMemberValues.push_back(std::move(memberValue));
    }
    return new StructuredValue(GetSpan(), ModuleId(), type, std::move(clonedMemberValues));
}

void* StructuredValue::IrValue(Emitter& emitter) 
{
    std::vector<void*> memberConstants;
    int64_t n = memberValues.size();
    if (n == 0)
    {
        ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(type);
        const std::vector<TypeSymbol*>& objectLayout = classType->ObjectLayout();
        n = objectLayout.size();
        for (int64_t i = 0; i < n; ++i)
        {
            TypeSymbol* type = objectLayout[i];
            memberConstants.push_back(type->CreateDefaultIrValue(emitter));
        }
    }
    else
    {
        for (int64_t i = 0; i < n; ++i)
        {
            memberConstants.push_back(memberValues[i]->IrValue(emitter));
        }
    }
    return emitter.CreateIrValueForConstantStruct(type->IrType(emitter), memberConstants);
}

void StructuredValue::Write(BinaryWriter& writer)
{
    for (const std::unique_ptr<Value>& memberValue : memberValues)
    {
        memberValue->Write(writer);
    }
}

void StructuredValue::Read(BinaryReader& reader)
{
    for (MemberVariableSymbol* memberVariable : static_cast<ClassTypeSymbol*>(type)->MemberVariables())
    {
        Value* memberValue = memberVariable->GetType()->MakeValue();
        memberValue->Read(reader);
        memberValues.push_back(std::unique_ptr<Value>(memberValue));
    }
}

Value* StructuredValue::As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow) const
{
    if (TypesEqual(targetType, type))
    {
        return Clone();
    }
    else
    {
        if (dontThrow)
        {
            return nullptr;
        }
        else
        {
            throw Exception("conversion from " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " is not valid", span, moduleId);
        }
    }
}

UuidValue::UuidValue(const Span& span_, const boost::uuids::uuid& moduleId_, int uuidId_) : Value(span_, moduleId_, ValueType::uuidValue), uuidId(uuidId_)
{
}

void* UuidValue::IrValue(Emitter& emitter)
{
    void* uuidConstant = emitter.GetGlobalUuidConstant(uuidId);
    return emitter.CreateIrValueForUuid(uuidConstant);
}

void UuidValue::Write(BinaryWriter& writer) 
{
    Assert(false, "write for uuid value not supported");
}

void UuidValue::Read(BinaryReader& reader)
{
    Assert(false, "read for uuid value not supported");
}

Value* UuidValue::As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow) const
{
    switch (targetType->GetValueType())
    {
        case ValueType::uuidValue:
        {
            return new UuidValue(span, moduleId, uuidId);
        }
        default:
        {
            if (dontThrow)
            {
                return nullptr;
            }
            else
            {
                throw Exception("conversion from " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType->GetValueType()) + " is not valid", span, moduleId);
            }
        }
    }
}

TypeSymbol* UuidValue::GetType(SymbolTable* symbolTable)
{
    return symbolTable->GetTypeByName(U"void")->AddPointer(GetSpan(), ModuleId());
}

bool operator==(IntegralValue left, IntegralValue right)
{
    if (left.value->GetValueType() != right.value->GetValueType()) return false;
    switch (left.value->GetValueType())
    {
        case ValueType::boolValue: return ValuesEqual(*static_cast<BoolValue*>(left.value), *static_cast<BoolValue*>(right.value));
        case ValueType::sbyteValue: return ValuesEqual(*static_cast<SByteValue*>(left.value), *static_cast<SByteValue*>(right.value));
        case ValueType::byteValue: return ValuesEqual(*static_cast<ByteValue*>(left.value), *static_cast<ByteValue*>(right.value));
        case ValueType::shortValue: return ValuesEqual(*static_cast<ShortValue*>(left.value), *static_cast<ShortValue*>(right.value));
        case ValueType::ushortValue: return ValuesEqual(*static_cast<UShortValue*>(left.value), *static_cast<UShortValue*>(right.value));
        case ValueType::intValue: return ValuesEqual(*static_cast<IntValue*>(left.value), *static_cast<IntValue*>(right.value));
        case ValueType::uintValue: return ValuesEqual(*static_cast<UIntValue*>(left.value), *static_cast<UIntValue*>(right.value));
        case ValueType::longValue: return ValuesEqual(*static_cast<LongValue*>(left.value), *static_cast<LongValue*>(right.value));
        case ValueType::ulongValue: return ValuesEqual(*static_cast<ULongValue*>(left.value), *static_cast<ULongValue*>(right.value));
        case ValueType::charValue: return ValuesEqual(*static_cast<CharValue*>(left.value), *static_cast<CharValue*>(right.value));
        case ValueType::wcharValue: return ValuesEqual(*static_cast<WCharValue*>(left.value), *static_cast<WCharValue*>(right.value));
        case ValueType::ucharValue: return ValuesEqual(*static_cast<UCharValue*>(left.value), *static_cast<UCharValue*>(right.value));
    }
    return false;
}

size_t IntegralValueHash::operator()(IntegralValue integralValue) const
{
    switch (integralValue.value->GetValueType())
    {
        case ValueType::boolValue: return GetHashCode(*static_cast<BoolValue*>(integralValue.value));
        case ValueType::sbyteValue: return GetHashCode(*static_cast<SByteValue*>(integralValue.value));
        case ValueType::byteValue: return GetHashCode(*static_cast<ByteValue*>(integralValue.value));
        case ValueType::shortValue: return GetHashCode(*static_cast<ShortValue*>(integralValue.value));
        case ValueType::ushortValue: return GetHashCode(*static_cast<UShortValue*>(integralValue.value));
        case ValueType::intValue: return GetHashCode(*static_cast<IntValue*>(integralValue.value));
        case ValueType::uintValue: return GetHashCode(*static_cast<UIntValue*>(integralValue.value));
        case ValueType::longValue: return GetHashCode(*static_cast<LongValue*>(integralValue.value));
        case ValueType::ulongValue: return GetHashCode(*static_cast<ULongValue*>(integralValue.value));
        case ValueType::charValue: return GetHashCode(*static_cast<CharValue*>(integralValue.value));
        case ValueType::wcharValue: return GetHashCode(*static_cast<WCharValue*>(integralValue.value));
        case ValueType::ucharValue: return GetHashCode(*static_cast<UCharValue*>(integralValue.value));
    }
    return 0;
}

void WriteValue(Value* value, BinaryWriter& writer)
{
    writer.Write(static_cast<uint8_t>(value->GetValueType()));
    value->Write(writer);
}

std::unique_ptr<Value> ReadValue(BinaryReader& reader, const Span& span, const boost::uuids::uuid& moduleId)
{
    ValueType valueType = static_cast<ValueType>(reader.ReadByte());
    std::unique_ptr<Value> value;
    switch (valueType)
    {
        case ValueType::boolValue: value.reset(new BoolValue(Span(), boost::uuids::nil_uuid(), false)); break;
        case ValueType::sbyteValue: value.reset(new SByteValue(Span(), boost::uuids::nil_uuid(), 0)); break;
        case ValueType::byteValue: value.reset(new ByteValue(Span(), boost::uuids::nil_uuid(), 0)); break;
        case ValueType::shortValue: value.reset(new ShortValue(Span(), boost::uuids::nil_uuid(), 0)); break;
        case ValueType::ushortValue: value.reset(new UShortValue(Span(), boost::uuids::nil_uuid(), 0)); break;
        case ValueType::intValue: value.reset(new IntValue(Span(), boost::uuids::nil_uuid(), 0)); break;
        case ValueType::uintValue: value.reset(new UIntValue(Span(), boost::uuids::nil_uuid(), 0)); break;
        case ValueType::longValue: value.reset(new LongValue(Span(), boost::uuids::nil_uuid(), 0)); break;
        case ValueType::ulongValue: value.reset(new ULongValue(Span(), boost::uuids::nil_uuid(), 0)); break;
        case ValueType::floatValue: value.reset(new FloatValue(Span(), boost::uuids::nil_uuid(), 0.0)); break;
        case ValueType::doubleValue: value.reset(new DoubleValue(Span(), boost::uuids::nil_uuid(), 0.0)); break;
        case ValueType::charValue: value.reset(new CharValue(Span(), boost::uuids::nil_uuid(), '\0')); break;
        case ValueType::wcharValue: value.reset(new WCharValue(Span(), boost::uuids::nil_uuid(), '\0')); break;
        case ValueType::ucharValue: value.reset(new UCharValue(Span(), boost::uuids::nil_uuid(), '\0')); break;
        case ValueType::pointerValue: value.reset(new PointerValue(Span(), boost::uuids::nil_uuid(), nullptr, nullptr)); break;
        case ValueType::stringValue: value.reset(new StringValue(Span(), boost::uuids::nil_uuid(), -1, ""));
        case ValueType::wstringValue: value.reset(new WStringValue(Span(), boost::uuids::nil_uuid(), -1, u""));
        case ValueType::ustringValue: value.reset(new UStringValue(Span(), boost::uuids::nil_uuid(), -1, U""));
    }
    if (value)
    {
        value->Read(reader);
        return value;
    }
    else
    {
        throw Exception( "internal error: could not read value of type '" + ValueTypeStr(valueType) + "'", span, moduleId);
    }
}

} } // namespace cmajor::symbols
