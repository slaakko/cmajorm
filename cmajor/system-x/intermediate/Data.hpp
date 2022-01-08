// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_DATA_INCLUDED
#define CMSX_INTERMEDIATE_DATA_INCLUDED
#include <system-x/intermediate/Types.hpp>

namespace cmsx::intermediate {

class Data;
class Function;
class GlobalVariable;

enum class ValueKind
{
    boolValue, sbyteValue, byteValue, shortValue, ushortValue, intValue, uintValue, longValue, ulongValue, floatValue, doubleValue, nullValue, addressValue,
    arrayValue, structureValue, stringValue, stringArrayValue, conversionValue, clsIdValue, symbolValue,
    globalVariable,
    regValue,
    instruction
};

class CMSX_INTERMEDIATE_API Value
{
public:
    Value(const SourcePos& sourcePos_, ValueKind kind_, Type* type_);
    virtual ~Value();
    bool IsRegValue() const { return kind == ValueKind::regValue; }
    bool IsInstruction() const { return kind == ValueKind::instruction; }
    bool IsSymbolValue() const { return kind == ValueKind::symbolValue; }
    bool IsAddressValue() const { return kind == ValueKind::addressValue; }
    bool IsGlobalVariable() const { return kind == ValueKind::globalVariable; }
    bool IsIntegerValue() const;
    int64_t GetIntegerValue() const;
    const SourcePos& GetSourcePos() const { return sourcePos; }
    ValueKind Kind() const { return kind; }
    std::string KindStr() const;
    Type* GetType() const { return type; }
    void SetType(Type* type_) { type = type_; }
private:
    SourcePos sourcePos;
    ValueKind kind;
    Type* type;
};

class CMSX_INTERMEDIATE_API ConstantValue : public Value
{
public:
    ConstantValue(const SourcePos& sourcePos_, ValueKind kind_, Type* type_);
    virtual void Accept(Visitor& visitor) = 0;
};

class CMSX_INTERMEDIATE_API BoolValue : public ConstantValue
{
public:
    BoolValue(bool value_, Type* type_);
    bool GetValue() const { return value; }
    void Accept(Visitor& visitor) override;
private:
    bool value;
};

class CMSX_INTERMEDIATE_API SByteValue : public ConstantValue
{
public:
    SByteValue(int8_t value_, Type* type_);
    int8_t GetValue() const { return value; }
    void Accept(Visitor& visitor) override;
private:
    int8_t value;
};

class CMSX_INTERMEDIATE_API ByteValue : public ConstantValue
{
public:
    ByteValue(uint8_t value_, Type* type_);
    uint8_t GetValue() const { return value; }
    void Accept(Visitor& visitor) override;
private:
    uint8_t value;
};

class CMSX_INTERMEDIATE_API ShortValue : public ConstantValue
{
public:
    ShortValue(int16_t value_, Type* type_);
    int16_t GetValue() const { return value; }
    void Accept(Visitor& visitor) override;
private:
    int16_t value;
};

class CMSX_INTERMEDIATE_API UShortValue : public ConstantValue
{
public:
    UShortValue(uint16_t value_, Type* type_);
    uint16_t GetValue() const { return value; }
    void Accept(Visitor& visitor) override;
private:
    uint16_t value;
};

class CMSX_INTERMEDIATE_API IntValue : public ConstantValue
{
public:
    IntValue(int32_t value_, Type* type_);
    int32_t GetValue() const { return value; }
    void Accept(Visitor& visitor) override;
private:
    int32_t value;
};

class CMSX_INTERMEDIATE_API UIntValue : public ConstantValue
{
public:
    UIntValue(uint32_t value_, Type* type_);
    uint32_t GetValue() const { return value; }
    void Accept(Visitor& visitor) override;
private:
    uint32_t value;
};

class CMSX_INTERMEDIATE_API LongValue : public ConstantValue
{
public:
    LongValue(int64_t value_, Type* type_);
    int64_t GetValue() const { return value; }
    void Accept(Visitor& visitor) override;
private:
    int64_t value;
};

class CMSX_INTERMEDIATE_API ULongValue : public ConstantValue
{
public:
    ULongValue(uint64_t value_, Type* type_);
    uint64_t GetValue() const { return value; }
    void Accept(Visitor& visitor) override;
private:
    uint64_t value;
};

class CMSX_INTERMEDIATE_API FloatValue : public ConstantValue
{
public:
    FloatValue(float value_, Type* type_);
    float GetValue() const { return value; }
    void Accept(Visitor& visitor) override;
private:
    float value;
};

class CMSX_INTERMEDIATE_API DoubleValue : public ConstantValue
{
public:
    DoubleValue(double value_, Type* type_);
    double GetValue() const { return value; }
    void Accept(Visitor& visitor) override;
private:
    double value;
};

class CMSX_INTERMEDIATE_API NullValue : public ConstantValue
{
public:
    NullValue(Type* type_);
    void Accept(Visitor& visitor) override;
};

class CMSX_INTERMEDIATE_API AddressValue : public ConstantValue
{
public:
    AddressValue(const SourcePos& sourcePos_, GlobalVariable* globalVariable_, Type* type);
    GlobalVariable* Value() const { return globalVariable; }
    void Accept(Visitor& visitor) override;
private:
    GlobalVariable* globalVariable;
};

class CMSX_INTERMEDIATE_API ArrayValue : public ConstantValue
{
public:
    ArrayValue(const SourcePos& sourcePos_, const std::vector<ConstantValue*>& elements_);
    const std::vector<ConstantValue*>& Elements() const { return elements; }
    void Accept(Visitor& visitor) override;
private:
    std::vector<ConstantValue*> elements;
};

class CMSX_INTERMEDIATE_API StructureValue : public ConstantValue
{
public:
    StructureValue(const SourcePos& sourcePos_, const std::vector<ConstantValue*>& fieldValues_);
    const std::vector<ConstantValue*>& FieldValues() const { return fieldValues; }
    void Accept(Visitor& visitor) override;
private:
    std::vector<ConstantValue*> fieldValues;
};

class CMSX_INTERMEDIATE_API StringValue : public ConstantValue
{
public:
    StringValue(const SourcePos& sourcePos_, const std::string& value_);
    const std::string& Value() const { return value; }
    void Accept(Visitor& visitor) override;
private:
    std::string value;
};

class CMSX_INTERMEDIATE_API StringArrayValue : public ConstantValue
{
public:
    StringArrayValue(const SourcePos& sourcePos_, char prefix_, const std::vector<ConstantValue*>& strings_);
    char Prefix() const { return prefix; }
    const std::vector<ConstantValue*>& Strings() const { return strings; }
    void Accept(Visitor& visitor) override;
private:
    char prefix;
    std::vector<ConstantValue*> strings;
};

class CMSX_INTERMEDIATE_API ConversionValue : public ConstantValue
{
public:
    ConversionValue(const SourcePos& sourcePos_, Type* type_, ConstantValue* from_);
    ConstantValue* From() const { return from; }
    void Accept(Visitor& visitor) override;
private:
    ConstantValue* from;
};

class CMSX_INTERMEDIATE_API ClsIdValue : public ConstantValue
{
public:
    ClsIdValue(const SourcePos& sourcePos_, Type* type_, const std::string& typeId_);
    const std::string& TypeId() const { return typeId; }
    void Accept(Visitor& visitor) override;
private:
    std::string typeId;
};

class CMSX_INTERMEDIATE_API SymbolValue : public ConstantValue
{
public:
    SymbolValue(const SourcePos& sourcePos_, Type* type_, const std::string& symbol_);
    const std::string& Symbol() const { return symbol; }
    Function* GetFunction() const { return function; }
    void SetFunction(Function* function_) { function = function_; }
    GlobalVariable* GetGlobalVariable() const { return globalVariable; }
    void SetGlobalVariable(GlobalVariable* globalVariable_) { globalVariable = globalVariable_; }
    void Accept(Visitor& visitor) override;
private:
    std::string symbol;
    Function* function;
    GlobalVariable* globalVariable;
};

class CMSX_INTERMEDIATE_API GlobalVariable : public Value
{
public:
    GlobalVariable(const SourcePos& sourcePos_, Type* type_, const std::string& name_, ConstantValue* initializer_, bool once_);
    virtual void Accept(Visitor& visitor);
    const std::string& Name() const { return name; }
    ConstantValue* Initializer() const { return initializer; }
    bool Once() const { return once; }
private:
    std::string name;
    ConstantValue* initializer;
    bool once;
};

template<class T>
class ConstantValueMap
{
public:
    ConstantValueMap();
    ConstantValue* Get(const T& value, Data* data, const Types& types);
private:
    std::map<T, ConstantValue*> valueMap;
};

class CMSX_INTERMEDIATE_API Data
{
public:
    Data();
    Data(const Data&) = delete;
    Data& operator=(const Data&) = delete;
    Context* GetContext() const { return context; }
    void SetContext(Context* context_) { context = context_; }
    void AddGlobalVariable(const SourcePos& sourcePos, Type* type, const std::string& variableName, ConstantValue* initializer, bool once, Context* context);
    ConstantValue* GetTrueValue(const Types& types);
    ConstantValue* GetFalseValue(const Types& types);
    ConstantValue* GetSByteValue(int8_t value, const Types& types);
    ConstantValue* GetByteValue(uint8_t value, const Types& types);
    ConstantValue* GetShortValue(int16_t value, const Types& types);
    ConstantValue* GetUShortValue(uint16_t value, const Types& types);
    ConstantValue* GetIntValue(int32_t value, const Types& types);
    ConstantValue* GetUIntValue(uint32_t value, const Types& types);
    ConstantValue* GetLongValue(int64_t value, const Types& types);
    ConstantValue* GetULongValue(uint64_t value, const Types& types);
    ConstantValue* GetFloatValue(float value, const Types& types);
    ConstantValue* GetDoubleValue(float value, const Types& types);
    ConstantValue* GetNullValue(Type* type);
    ConstantValue* MakeValue(int8_t value, const Types& types);
    ConstantValue* MakeValue(uint8_t value, const Types& types);
    ConstantValue* MakeValue(int16_t value, const Types& types);
    ConstantValue* MakeValue(uint16_t value, const Types& types);
    ConstantValue* MakeValue(int32_t value, const Types& types);
    ConstantValue* MakeValue(uint32_t value, const Types& types);
    ConstantValue* MakeValue(int64_t value, const Types& types);
    ConstantValue* MakeValue(uint64_t value, const Types& types);
    ConstantValue* MakeValue(float value, const Types& types);
    ConstantValue* MakeValue(double value, const Types& types);
    ConstantValue* MakeArrayValue(const SourcePos& sourcePos, const std::vector<ConstantValue*>& elements);
    ConstantValue* MakeStructureValue(const SourcePos& sourcePos, const std::vector<ConstantValue*>& fieldValues);
    ConstantValue* MakeStringValue(const SourcePos& sourcePos, const std::string& value);
    ConstantValue* MakeStringArrayValue(const SourcePos& sourcePos, char prefix, const std::vector<ConstantValue*>& strings);
    ConstantValue* MakeConversionValue(const SourcePos& sourcePos, Type* type, ConstantValue* from);
    ConstantValue* MakeClsIdValue(const SourcePos& sourcePos, Type* type, const std::string& clsIdStr);
    ConstantValue* MakeSymbolValue(const SourcePos& sourcePos, Type* type, const std::string& symbol);
    ConstantValue* MakeNumericLiteral(const SourcePos& sourcePos, Type* type, const std::string& strValue, const Types& types, Context* context);
    ConstantValue* MakeAddressLiteral(const SourcePos& sourcePos, Type* type, const std::string& id, Context* context);
    void VisitGlobalVariables(Visitor& visitor);
private:
    Context* context;
    std::vector<std::unique_ptr<Value>> values;
    std::vector<GlobalVariable*> globalVariables;
    std::map<std::string, GlobalVariable*> globalVariableMap;
    std::unique_ptr<BoolValue> trueValue;
    std::unique_ptr<BoolValue> falseValue;
    ConstantValueMap<int8_t> sbyteValueMap;
    ConstantValueMap<uint8_t> byteValueMap;
    ConstantValueMap<int16_t> shortValueMap;
    ConstantValueMap<uint16_t> ushortValueMap;
    ConstantValueMap<int32_t> intValueMap;
    ConstantValueMap<uint32_t> uintValueMap;
    ConstantValueMap<int64_t> longValueMap;
    ConstantValueMap<uint64_t> ulongValueMap;
    ConstantValueMap<float> floatValueMap;
    ConstantValueMap<double> doubleValueMap;
    std::map<Type*, NullValue*> nullValueMap;
};

template<class T>
ConstantValueMap<T>::ConstantValueMap()
{
}

template<class T>
ConstantValue* ConstantValueMap<T>::Get(const T& value, Data* data, const Types& types)
{
    auto it = valueMap.find(value);
    if (it != valueMap.cend())
    {
        return it->second;
    }
    else
    {
        ConstantValue* constantValue = data->MakeValue(value, types);
        valueMap[value] = constantValue;
        return constantValue;
    }
}

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_DATA_INCLUDED
