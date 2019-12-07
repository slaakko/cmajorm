// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_VALUE_INCLUDED
#define CMAJOR_SYMBOLS_VALUE_INCLUDED
#include <cmajor/symbols/Symbol.hpp>
#include <cmajor/parsing/Scanner.hpp>
#include <cmajor/ir/Emitter.hpp>

namespace cmajor { namespace symbols {

using cmajor::parsing::Span;
using namespace cmajor::ir;

class TypeSymbol;

enum class ValueType : uint8_t
{
    none, boolValue, sbyteValue, byteValue, shortValue, ushortValue, intValue, uintValue, 
    longValue, ulongValue, floatValue, doubleValue, charValue, wcharValue, ucharValue, 
    stringValue, wstringValue, ustringValue, nullValue, pointerValue, arrayValue, structuredValue, 
    uuidValue,
    maxValue
};

SYMBOLS_API std::string ValueTypeStr(ValueType valueType);

SYMBOLS_API ValueType CommonType(ValueType left, ValueType right);

SYMBOLS_API TypeSymbol* GetTypeFor(ValueType valueType, SymbolTable* symbolTable);

class SYMBOLS_API Value
{
public:
    Value(const Span& span_, ValueType valueType_);
    virtual ~Value();
    virtual Value* Clone() const = 0;
    virtual Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const = 0;
    virtual void* IrValue(Emitter& emitter) = 0;
    virtual void Write(BinaryWriter& writer) = 0;
    virtual void Read(BinaryReader& reader) = 0;
    virtual bool IsComplete() const { return true; }
    virtual bool IsScopedValue() const { return false; }
    virtual bool IsFunctionGroupValue() const { return false; }
    virtual bool IsArrayReferenceValue() const { return false; }
    virtual bool IsStructuredReferenceValue() const { return false; }
    virtual bool IsComplexValue() const { return false; }
    virtual std::string ToString() const { return std::string(); }
    virtual TypeSymbol* GetType(SymbolTable* symbolTable) = 0;
    virtual void SetType(TypeSymbol* type_) {}
    const Span& GetSpan() const { return span; }
    ValueType GetValueType() const { return valueType; }
    std::unique_ptr<dom::Element> ToDomElement();
    virtual const char* ClassName() const { return "Value";  }
private:
    Span span;
    ValueType valueType;
};

class SYMBOLS_API BoolValue : public Value
{
public:
    typedef bool OperandType;
    BoolValue(const Span& span_, bool value_);
    Value* Clone() const override  { return new BoolValue(GetSpan(), value); }
    void* IrValue(Emitter& emitter) override;
    void Write(BinaryWriter& writer) override;
    void Read(BinaryReader& reader) override;
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override;
    std::string ToString() const override { return value ? "true" : "false"; }
    TypeSymbol* GetType(SymbolTable* symbolTable) override;
    bool GetValue() const { return value; }
    const char* ClassName() const override { return "BoolValue"; }
private:
    bool value;
};

class SYMBOLS_API SByteValue : public Value
{
public:
    typedef int8_t OperandType;
    SByteValue(const Span& span_, int8_t value_);
    Value* Clone() const override  { return new SByteValue(GetSpan(), value); }
    void* IrValue(Emitter& emitter) override;
    void Write(BinaryWriter& writer) override;
    void Read(BinaryReader& reader) override;
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override;
    std::string ToString() const override { return std::to_string(value); }
    TypeSymbol* GetType(SymbolTable* symbolTable) override;
    int8_t GetValue() const { return value; }
    const char* ClassName() const override { return "SByteValue"; }
private:
    int8_t value;
};

class SYMBOLS_API ByteValue : public Value
{
public:
    typedef uint8_t OperandType;
    ByteValue(const Span& span_, uint8_t value_);
    Value* Clone() const override  { return new ByteValue(GetSpan(), value); }
    void* IrValue(Emitter& emitter) override;
    void Write(BinaryWriter& writer) override;
    void Read(BinaryReader& reader) override;
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override;
    std::string ToString() const override { return std::to_string(value); }
    TypeSymbol* GetType(SymbolTable* symbolTable) override;
    uint8_t GetValue() const { return value; }
    const char* ClassName() const override { return "ByteValue"; }
private:
    uint8_t value;
};

class SYMBOLS_API ShortValue : public Value
{
public:
    typedef int16_t OperandType;
    ShortValue(const Span& span_, int16_t value_);
    Value* Clone() const override  { return new ShortValue(GetSpan(), value); }
    void* IrValue(Emitter& emitter) override;
    void Write(BinaryWriter& writer) override;
    void Read(BinaryReader& reader) override;
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override;
    std::string ToString() const override { return std::to_string(value); }
    TypeSymbol* GetType(SymbolTable* symbolTable) override;
    int16_t GetValue() const { return value; }
    const char* ClassName() const override { return "ShortValue"; }
private:
    int16_t value;
};

class SYMBOLS_API UShortValue : public Value
{
public:
    typedef uint16_t OperandType;
    UShortValue(const Span& span_, uint16_t value_);
    Value* Clone() const override  { return new UShortValue(GetSpan(), value); }
    void* IrValue(Emitter& emitter) override;
    void Write(BinaryWriter& writer) override;
    void Read(BinaryReader& reader) override;
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override;
    std::string ToString() const override { return std::to_string(value); }
    TypeSymbol* GetType(SymbolTable* symbolTable) override;
    uint16_t GetValue() const { return value; }
    const char* ClassName() const override { return "UShortValue"; }
private:
    uint16_t value;
};

class SYMBOLS_API IntValue : public Value
{
public:
    typedef int32_t OperandType;
    IntValue(const Span& span_, int32_t value_);
    Value* Clone() const override  { return new IntValue(GetSpan(), value); }
    void* IrValue(Emitter& emitter) override;
    void Write(BinaryWriter& writer) override;
    void Read(BinaryReader& reader) override;
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override;
    std::string ToString() const override { return std::to_string(value); }
    TypeSymbol* GetType(SymbolTable* symbolTable) override;
    int32_t GetValue() const { return value; }
    const char* ClassName() const override { return "IntValue"; }
private:
    int32_t value;
};

class SYMBOLS_API UIntValue : public Value
{
public:
    typedef uint32_t OperandType;
    UIntValue(const Span& span_, uint32_t value_);
    Value* Clone() const override  { return new UIntValue(GetSpan(), value); }
    void* IrValue(Emitter& emitter) override;
    void Write(BinaryWriter& writer) override;
    void Read(BinaryReader& reader) override;
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override;
    std::string ToString() const override { return std::to_string(value); }
    TypeSymbol* GetType(SymbolTable* symbolTable) override;
    uint32_t GetValue() const { return value; }
    const char* ClassName() const override { return "UIntValue"; }
private:
    uint32_t value;
};

class SYMBOLS_API LongValue : public Value
{
public:
    typedef int64_t OperandType;
    LongValue(const Span& span_, int64_t value_);
    Value* Clone() const override  { return new LongValue(GetSpan(), value); }
    void* IrValue(Emitter& emitter) override;
    void Write(BinaryWriter& writer) override;
    void Read(BinaryReader& reader) override;
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override;
    std::string ToString() const override { return std::to_string(value); }
    TypeSymbol* GetType(SymbolTable* symbolTable) override;
    int64_t GetValue() const { return value; }
    const char* ClassName() const override { return "LongValue"; }
private:
    int64_t value;
};

class SYMBOLS_API ULongValue : public Value
{
public:
    typedef uint64_t OperandType;
    ULongValue(const Span& span_, uint64_t value_);
    Value* Clone() const override  { return new ULongValue(GetSpan(), value); }
    void* IrValue(Emitter& emitter) override;
    void Write(BinaryWriter& writer) override;
    void Read(BinaryReader& reader) override;
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override;
    std::string ToString() const override { return std::to_string(value); }
    TypeSymbol* GetType(SymbolTable* symbolTable) override;
    uint64_t GetValue() const { return value; }
    const char* ClassName() const override { return "ULongValue"; }
private:
    uint64_t value;
};

class SYMBOLS_API FloatValue : public Value
{
public:
    typedef float OperandType;
    FloatValue(const Span& span_, float value_);
    Value* Clone() const override  { return new FloatValue(GetSpan(), value); }
    void* IrValue(Emitter& emitter) override;
    void Write(BinaryWriter& writer) override;
    void Read(BinaryReader& reader) override;
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override;
    std::string ToString() const override { return std::to_string(value); }
    TypeSymbol* GetType(SymbolTable* symbolTable) override;
    float GetValue() const { return value; }
    const char* ClassName() const override { return "FloatValue"; }
private:
    float value;
};

class SYMBOLS_API DoubleValue : public Value
{
public:
    typedef double OperandType;
    DoubleValue(const Span& span_, double value_);
    Value* Clone() const override  { return new DoubleValue(GetSpan(), value); }
    void* IrValue(Emitter& emitter) override;
    void Write(BinaryWriter& writer) override;
    void Read(BinaryReader& reader) override;
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override;
    std::string ToString() const override { return std::to_string(value); }
    TypeSymbol* GetType(SymbolTable* symbolTable) override;
    double GetValue() const { return value; }
    const char* ClassName() const override { return "DoubleValue"; }
private:
    double value;
};

class SYMBOLS_API CharValue : public Value
{
public:
    typedef unsigned char OperandType;
    CharValue(const Span& span_, unsigned char value_);
    Value* Clone() const override  { return new CharValue(GetSpan(), value); }
    void* IrValue(Emitter& emitter) override;
    void Write(BinaryWriter& writer) override;
    void Read(BinaryReader& reader) override;
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override;
    std::string ToString() const override { return std::to_string(value); }
    TypeSymbol* GetType(SymbolTable* symbolTable) override;
    unsigned char GetValue() const { return value; }
    const char* ClassName() const override { return "CharValue"; }
private:
    unsigned char value;
};

class SYMBOLS_API WCharValue : public Value
{
public:
    typedef char16_t OperandType;
    WCharValue(const Span& span_, char16_t value_);
    Value* Clone() const override  { return new WCharValue(GetSpan(), value); }
    void* IrValue(Emitter& emitter) override;
    void Write(BinaryWriter& writer) override;
    void Read(BinaryReader& reader) override;
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override;
    std::string ToString() const override { return std::to_string(value); }
    TypeSymbol* GetType(SymbolTable* symbolTable) override;
    char16_t GetValue() const { return value; }
    const char* ClassName() const override { return "WCharValue"; }
private:
    char16_t value;
};

class SYMBOLS_API UCharValue : public Value
{
public:
    typedef char32_t OperandType;
    UCharValue(const Span& span_, char32_t value_);
    Value* Clone() const override  { return new UCharValue(GetSpan(), value); }
    void* IrValue(Emitter& emitter) override;
    void Write(BinaryWriter& writer) override;
    void Read(BinaryReader& reader) override;
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override;
    std::string ToString() const override { return std::to_string(value); }
    TypeSymbol* GetType(SymbolTable* symbolTable) override;
    char32_t GetValue() const { return value; }
    const char* ClassName() const override { return "UCharValue"; }
private:
    char32_t value;
};

class SYMBOLS_API StringValue : public Value
{
public:
    StringValue(const Span& span_, int stringId_, const std::string& str_);
    Value* Clone() const override  { return new StringValue(GetSpan(), stringId, str); }
    void* IrValue(Emitter& emitter) override;
    void Write(BinaryWriter& writer) override;
    void Read(BinaryReader& reader) override;
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override;
    TypeSymbol* GetType(SymbolTable* symbolTable) override;
    int StringId() const { return stringId; }
    const char* ClassName() const override { return "StringValue"; }
private:
    int stringId;
    std::string str;
};

class SYMBOLS_API WStringValue : public Value
{
public:
    WStringValue(const Span& span_, int stringId_, const std::u16string& str_);
    Value* Clone() const override  { return new WStringValue(GetSpan(), stringId, str); }
    void* IrValue(Emitter& emitter) override;
    void Write(BinaryWriter& writer) override;
    void Read(BinaryReader& reader) override;
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override;
    TypeSymbol* GetType(SymbolTable* symbolTable) override;
    int StringId() const { return stringId; }
    const char* ClassName() const override { return "WStringValue"; }
private:
    int stringId;
    std::u16string str;
};

class SYMBOLS_API UStringValue : public Value
{
public:
    UStringValue(const Span& span_, int stringId_, const std::u32string& str_);
    Value* Clone() const override  { return new UStringValue(GetSpan(), stringId, str); }
    void* IrValue(Emitter& emitter) override;
    void Write(BinaryWriter& writer) override;
    void Read(BinaryReader& reader) override;
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override;
    TypeSymbol* GetType(SymbolTable* symbolTable) override;
    int StringId() const { return stringId; }
    const char* ClassName() const override { return "UStringValue"; }
private:
    int stringId;
    std::u32string str;
};

class SYMBOLS_API NullValue : public Value
{
public:
    NullValue(const Span& span_, TypeSymbol* nullPtrType_);
    Value* Clone() const override  { return new NullValue(GetSpan(), nullPtrType); }
    void* IrValue(Emitter& emitter) override;
    void Write(BinaryWriter& writer) override;
    void Read(BinaryReader& reader) override;
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override;
    std::string ToString() const override { return "null"; }
    TypeSymbol* GetType(SymbolTable* symbolTable) override;
    const char* ClassName() const override { return "NullValue"; }
private:
    TypeSymbol* nullPtrType;
};

class SYMBOLS_API PointerValue : public Value
{
public:
    typedef const void* OperandType;
    PointerValue(const Span& span_, TypeSymbol* type, const void* ptr_);
    Value* Clone() const override  { return new PointerValue(GetSpan(), type, ptr); }
    void* IrValue(Emitter& emitter) override;
    void Write(BinaryWriter& writer) override;
    void Read(BinaryReader& reader) override;
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override;
    TypeSymbol* GetType(SymbolTable* symbolTable) override;
    void SetType(TypeSymbol* type_) override { type = type_; }
    TypeSymbol* PointeeType() const;
    const void* GetValue() const { return ptr; }
    Value* Add(int64_t offset) const;
    Value* Sub(int64_t offset) const;
    Value* Sub(const void* thatPtr) const;
    Value* Deref() const;
    const char* ClassName() const override { return "PointerValue"; }
private:
    TypeSymbol* type;
    const void* ptr;
};

class SYMBOLS_API ArrayValue : public Value
{
public:
    ArrayValue(const Span& span_, TypeSymbol* type_, std::vector<std::unique_ptr<Value>>&& elementValues_);
    ArrayValue(const ArrayValue&) = delete;
    ArrayValue& operator=(const ArrayValue&) = delete;
    Value* Clone() const override ;
    void* IrValue(Emitter& emitter) override;
    void Write(BinaryWriter& writer) override;
    void Read(BinaryReader& reader) override;
    bool IsComplexValue() const override { return true; }
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override;
    TypeSymbol* GetType(SymbolTable* symbolTable) override { return type; }
    void SetType(TypeSymbol* type_) { type = type_; }
    const std::vector<std::unique_ptr<Value>>& Elements() const { return elementValues; }
    std::vector<std::unique_ptr<Value>>& Elements() { return elementValues; }
    const char* ClassName() const override { return "ArrayValue"; }
private:
    std::vector<std::unique_ptr<Value>> elementValues;
    TypeSymbol* type;
};

class SYMBOLS_API StructuredValue : public Value
{
public:
    StructuredValue(const Span& span_, TypeSymbol* type_, std::vector<std::unique_ptr<Value>>&& memberValues_);
    StructuredValue(const StructuredValue&) = delete;
    StructuredValue& operator=(const StructuredValue&) = delete;
    Value* Clone() const override ;
    void* IrValue(Emitter& emitter) override;
    void Write(BinaryWriter& writer) override;
    void Read(BinaryReader& reader) override;
    bool IsComplexValue() const override { return true; }
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override;
    void SetType(TypeSymbol* type_) { type = type_; }
    TypeSymbol* GetType(SymbolTable* symbolTable) override { return type; }
    const std::vector<std::unique_ptr<Value>>& Members() const { return memberValues; }
    std::vector<std::unique_ptr<Value>>& Members() { return memberValues; }
    const char* ClassName() const override { return "StructuredValue"; }
private:
    std::vector<std::unique_ptr<Value>> memberValues;
    TypeSymbol* type;
};

class SYMBOLS_API UuidValue : public Value
{
public:
    UuidValue(const Span& span_, int uuidId_);
    Value* Clone() const override { return new UuidValue(GetSpan(), uuidId); }
    void* IrValue(Emitter& emitter) override;
    void Write(BinaryWriter& writer) override;
    void Read(BinaryReader& reader) override;
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override;
    TypeSymbol* GetType(SymbolTable* symbolTable) override;
    int UuidId() const { return uuidId; }
    const char* ClassName() const override { return "UuidValue"; }
private:
    int uuidId;
};


template<typename ValueT>
inline bool ValuesEqual(const ValueT& left, const ValueT& right)
{
    return left.GetValue() == right.GetValue();
}

template<typename ValueT>
inline size_t GetHashCode(const ValueT& value)
{
    return static_cast<size_t>(value.GetValue());
}

struct SYMBOLS_API IntegralValue
{
    IntegralValue(Value* value_) : value(value_) {}
    Value* value;
};

SYMBOLS_API bool operator==(IntegralValue left, IntegralValue right);

inline bool operator!=(IntegralValue left, IntegralValue right)
{
    return !(left == right);
}

struct SYMBOLS_API IntegralValueHash
{
    size_t operator()(IntegralValue integralValue) const;
};

SYMBOLS_API void WriteValue(Value* value, BinaryWriter& writer);
SYMBOLS_API std::unique_ptr<Value> ReadValue(BinaryReader& reader, const Span& span);

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_VALUE_INCLUDED
