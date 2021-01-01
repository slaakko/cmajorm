// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/Evaluator.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/binder/TypeBinder.hpp>
#include <cmajor/binder/TypeResolver.hpp>
#include <cmajor/binder/OverloadResolution.hpp>
#include <cmajor/binder/StatementBinder.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <sngcm/ast/Visitor.hpp>
#include <sngcm/ast/BasicType.hpp>
#include <sngcm/ast/Literal.hpp>
#include <sngcm/ast/Expression.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace binder {

using namespace soulng::unicode;

void ThrowCannotEvaluateStatically(const Span& defined, const boost::uuids::uuid& moduleId)
{
    throw Exception("cannot evaluate statically", defined, moduleId);
}

void ThrowCannotEvaluateStatically(const Span& defined, const boost::uuids::uuid& moduleId, const Span& referenced, const boost::uuids::uuid& referencedModuleId)
{
    throw Exception("cannot evaluate statically", defined, moduleId, referenced, referencedModuleId);
}

typedef Value* (*BinaryOperatorFun)(Value* left, Value* right, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow);
typedef Value* (*UnaryOperatorFun)(Value* operand, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow);

class ScopedValue : public Value
{
public:
    ScopedValue(const Span& span_, const boost::uuids::uuid& moduleId_, ContainerSymbol* containerSymbol_);
    bool IsComplete() const override { return false; }
    bool IsScopedValue() const override { return true; }
    const ContainerSymbol* GetContainerSymbol() const { return containerSymbol; }
    ContainerSymbol* GetContainerSymbol() { return containerSymbol; }
    Value* Clone() const override { Assert(false, "scoped value cannot be cloned"); return nullptr; }
    void Write(BinaryWriter& writer) override {}
    void Read(BinaryReader& reader) override {}
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow) const override { Assert(false, "scoped value cannot be converted"); return nullptr; }
    void* IrValue(Emitter& emitter) override { Assert(false, "scoped value does not have ir value"); return nullptr; }
    TypeSymbol* GetType(SymbolTable* symbolTable) override { return type; }
    void SetType(TypeSymbol* type_) override { type = type_; }
    Value* GetSubject() override { return subject.get(); }
    void SetSubject(Value* subject_) { subject.reset(subject_); }
private:
    ContainerSymbol* containerSymbol;
    TypeSymbol* type;
    std::unique_ptr<Value> subject;
};

ScopedValue::ScopedValue(const Span& span_, const boost::uuids::uuid& moduleId_, ContainerSymbol* containerSymbol_) : 
    Value(span_, moduleId_, ValueType::none), containerSymbol(containerSymbol_), type(nullptr)
{
}

class FunctionGroupValue : public Value
{
public:
    FunctionGroupValue(FunctionGroupSymbol* functionGroup_, ContainerScope* qualifiedScope_);
    bool IsComplete() const override { return false; }
    bool IsFunctionGroupValue() const override { return true; }
    Value* Clone() const override { Assert(false, "function group value cannot be cloned"); return nullptr; }
    void Write(BinaryWriter& writer) override {}
    void Read(BinaryReader& reader) override {}
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId_, bool dontThrow) const override { Assert(false, "function group value cannot be converted"); return nullptr; }
    void* IrValue(Emitter& emitter) override { Assert(false, "function group value does not have ir value"); return nullptr; }
    FunctionGroupSymbol* FunctionGroup() { return functionGroup; }
    ContainerScope* QualifiedScope() { return qualifiedScope; }
    TypeSymbol* GetType(SymbolTable* symbolTable) override { return nullptr; }
    void SetTemplateTypeArguments(std::vector<TypeSymbol*>&& templateTypeArguments_) { templateTypeArguments = std::move(templateTypeArguments_); }
    std::vector<TypeSymbol*> TemplateTypeArguments() { return std::move(templateTypeArguments); }
    void SetReceiver(std::unique_ptr<Value>&& receiver_) { receiver = std::move(receiver_); }
    Value* Receiver() { return receiver.get(); }
private:
    FunctionGroupSymbol* functionGroup;
    ContainerScope* qualifiedScope;
    std::vector<TypeSymbol*> templateTypeArguments;
    std::unique_ptr<Value> receiver;
};

FunctionGroupValue::FunctionGroupValue(FunctionGroupSymbol* functionGroup_, ContainerScope* qualifiedScope_) : Value(Span(), boost::uuids::nil_uuid(), ValueType::none), functionGroup(functionGroup_), qualifiedScope(qualifiedScope_)
{
}

class ArrayReferenceValue : public Value
{
public:
    ArrayReferenceValue(ArrayValue* arrayValue_);
    bool IsArrayReferenceValue() const override { return true; }
    Value* Clone() const override { return new ArrayReferenceValue(arrayValue); }
    void Write(BinaryWriter& writer) override {}
    void Read(BinaryReader& reader) override {}
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId_, bool dontThrow) const override { Assert(false, "array reference value cannot be converted"); return nullptr; }
    void* IrValue(Emitter& emitter) override { Assert(false, "array reference does not have ir value"); return nullptr; }
    TypeSymbol* GetType(SymbolTable* symbolTable) override { return arrayValue->GetType(symbolTable); }
    ArrayValue* GetArrayValue() const { return arrayValue; }
private:
    ArrayValue* arrayValue;
};

ArrayReferenceValue::ArrayReferenceValue(ArrayValue* arrayValue_) : Value(arrayValue_->GetSpan(), arrayValue_->ModuleId(), ValueType::none), arrayValue(arrayValue_)
{
}

class StructuredReferenceValue : public Value
{
public:
    StructuredReferenceValue(StructuredValue* structuredValue_);
    bool IsStructuredReferenceValue() const override { return true; }
    Value* Clone() const override { return new StructuredReferenceValue(structuredValue); }
    void Write(BinaryWriter& writer) override {}
    void Read(BinaryReader& reader) override {}
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId_, bool dontThrow) const override { Assert(false, "structured reference value cannot be converted"); return nullptr; }
    void* IrValue(Emitter& emitter) override { Assert(false, "structured reference does not have ir value"); return nullptr; }
    TypeSymbol* GetType(SymbolTable* symbolTable) override { return structuredValue->GetType(symbolTable); }
    StructuredValue* GetStructuredValue() const { return structuredValue; }
private:
    StructuredValue* structuredValue;
};

StructuredReferenceValue::StructuredReferenceValue(StructuredValue* structuredValue_) : Value(structuredValue_->GetSpan(), structuredValue_->ModuleId(), ValueType::none), structuredValue(structuredValue_)
{
}

class StringReferenceValue : public Value
{
public:
    StringReferenceValue(Value* stringValue_);
    bool IsStringReferenceValue() const override { return true; }
    Value* Clone() const override { return new StringReferenceValue(stringValue); }
    void Write(BinaryWriter& writer) override {}
    void Read(BinaryReader& reader) override {}
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, const boost::uuids::uuid& moduleId_, bool dontThrow) const override { Assert(false, "string reference value cannot be converted"); return nullptr; }
    void* IrValue(Emitter& emitter) override { return stringValue->IrValue(emitter); }
    TypeSymbol* GetType(SymbolTable * symbolTable) override { return stringValue->GetType(symbolTable); }
    Value* GetSubject() override { return stringValue; }
private:
    Value* stringValue;
};

StringReferenceValue::StringReferenceValue(Value* stringValue_) : Value(stringValue_->GetSpan(), stringValue_->ModuleId(), ValueType::none), stringValue(stringValue_)
{
}

class VariableValueSymbol : public VariableSymbol
{
public:
    VariableValueSymbol(const Span& span_, const boost::uuids::uuid& moduleId_, const std::u32string& name_, std::unique_ptr<Value>&& value_);
    Value* GetValue() { return value.get(); }
    void SetValue(Value* value_) { value.reset(value_); }
    const char* ClassName() const override { return "VariableValueSymbol"; }
private:
    std::unique_ptr<Value> value;
};

VariableValueSymbol::VariableValueSymbol(const Span& span_, const boost::uuids::uuid& moduleId_, const std::u32string& name_, std::unique_ptr<Value>&& value_) : 
    VariableSymbol(SymbolType::variableValueSymbol, span_, moduleId_, name_), value(std::move(value_))
{
}

std::vector<std::unique_ptr<BoundExpression>> ValuesToLiterals(std::vector<std::unique_ptr<Value>>& values, SymbolTable* symbolTable, bool& error)
{
    std::vector<std::unique_ptr<BoundExpression>> arguments;
    for (std::unique_ptr<Value>& value : values)
    {
        ValueType valueType = value->GetValueType();
        TypeSymbol* type = value->GetType(symbolTable);
        BoundLiteral* literal = new BoundLiteral(std::move(value), type);
        arguments.push_back(std::unique_ptr<BoundExpression>(literal));
    }
    return arguments;
}

std::vector<std::unique_ptr<Value>> ArgumentsToValues(const std::vector<std::unique_ptr<BoundExpression>>& arguments, bool& error, bool skipFirst, BoundCompileUnit& boundCompileUnit);

std::vector<std::unique_ptr<Value>> ArgumentsToValues(const std::vector<std::unique_ptr<BoundExpression>>& arguments, bool& error, BoundCompileUnit& boundCompileUnit)
{
    return ArgumentsToValues(arguments, error, false, boundCompileUnit);
}

std::vector<std::unique_ptr<Value>> ArgumentsToValues(const std::vector<std::unique_ptr<BoundExpression>>& arguments, bool& error, bool skipFirst, BoundCompileUnit& boundCompileUnit)
{
    std::vector<std::unique_ptr<Value>> values;
    bool first = true;
    for (const std::unique_ptr<BoundExpression>& argument : arguments)
    {
        if (first)
        {
            first = false;
            if (skipFirst)
            {
                continue;
            }
        }
        std::unique_ptr<Value> value = argument->ToValue(boundCompileUnit);
        if (value)
        {
            values.push_back(std::move(value));
        }
        else
        {
            error = true;
            return values;
        }
    }
    return values;
}

template <typename ValueT, typename Op>
Value* BinaryEvaluate(Value* left, Value* right, Op op, const Span& span, const boost::uuids::uuid& moduleId)
{
    ValueT* leftCasted = static_cast<ValueT*>(left);
    ValueT* rightCasted = static_cast<ValueT*>(right);
    return new ValueT(span, moduleId, op(leftCasted->GetValue(), rightCasted->GetValue()));
}

template <typename ValueT, typename Op>
Value* BinaryPredEvaluate(Value* left, Value* right, Op op, const Span& span, const boost::uuids::uuid& moduleId)
{
    ValueT* leftCasted = static_cast<ValueT*>(left);
    ValueT* rightCasted = static_cast<ValueT*>(right);
    return new BoolValue(span, moduleId, op(leftCasted->GetValue(), rightCasted->GetValue()));
}

template<typename ValueT, typename Op>
Value* UnaryEvaluate(Value* subject, Op op, const Span& span, const boost::uuids::uuid& moduleId)
{
    ValueT* subjectCasted = static_cast<ValueT*>(subject);
    return new ValueT(span, moduleId, op(subjectCasted->GetValue()));
}

Value* NotSupported(Value* subject, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow)
{
    if (dontThrow)
    {
        return nullptr;
    }
    throw Exception("operation not supported for type " + ValueTypeStr(subject->GetValueType()), span, moduleId);
}

Value* NotSupported(Value* left, Value* right, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow)
{
    if (dontThrow)
    {
        return nullptr;
    }
    throw Exception("operation not supported for types " + ValueTypeStr(left->GetValueType()) + " and " + ValueTypeStr(right->GetValueType()), span, moduleId);
}

template<typename ValueT>
Value* Disjunction(Value* left, Value* right, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow)
{
    return BinaryEvaluate<ValueT>(left, right, std::logical_or<typename ValueT::OperandType>(), span, moduleId);
}

BinaryOperatorFun disjunction[uint8_t(ValueType::maxValue)] =
{
    NotSupported, Disjunction<BoolValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Value* Conjunction(Value* left, Value* right, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow)
{
    return BinaryEvaluate<ValueT>(left, right, std::logical_and<typename ValueT::OperandType>(), span, moduleId);
}

BinaryOperatorFun conjunction[uint8_t(ValueType::maxValue)] =
{
    NotSupported, Conjunction<BoolValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported,
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Value* BitOr(Value* left, Value* right, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow)
{
    return BinaryEvaluate<ValueT>(left, right, std::bit_or<typename ValueT::OperandType>(), span, moduleId);
}

BinaryOperatorFun bitOr[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, BitOr<SByteValue>, BitOr<ByteValue>, BitOr<ShortValue>, BitOr<UShortValue>, BitOr<IntValue>, BitOr<UIntValue>,
    BitOr<LongValue>, BitOr<ULongValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Value* BitXor(Value* left, Value* right, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow)
{
    return BinaryEvaluate<ValueT>(left, right, std::bit_xor<typename ValueT::OperandType>(), span, moduleId);
}

BinaryOperatorFun bitXor[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, BitXor<SByteValue>, BitXor<ByteValue>, BitXor<ShortValue>, BitXor<UShortValue>, BitXor<IntValue>, BitXor<UIntValue>,
    BitXor<LongValue>, BitXor<ULongValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Value* BitAnd(Value* left, Value* right, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow)
{
    return BinaryEvaluate<ValueT>(left, right, std::bit_and<typename ValueT::OperandType>(), span, moduleId);
}

BinaryOperatorFun bitAnd[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, BitAnd<SByteValue>, BitAnd<ByteValue>, BitAnd<ShortValue>, BitAnd<UShortValue>, BitAnd<IntValue>, BitAnd<UIntValue>,
    BitAnd<LongValue>, BitAnd<ULongValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Value* Equal(Value* left, Value* right, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow)
{
    return BinaryPredEvaluate<ValueT>(left, right, std::equal_to<typename ValueT::OperandType>(), span, moduleId);
}

BinaryOperatorFun equal[uint8_t(ValueType::maxValue)] =
{
    NotSupported, Equal<BoolValue>, Equal<SByteValue>, Equal<ByteValue>, Equal<ShortValue>, Equal<UShortValue>, Equal<IntValue>, Equal<UIntValue>,
    Equal<LongValue>, Equal<ULongValue>, Equal<FloatValue>, Equal<DoubleValue>, Equal<CharValue>, Equal<WCharValue>, Equal<UCharValue>, 
    NotSupported, NotSupported, NotSupported, NotSupported, Equal<PointerValue>, NotSupported, NotSupported
};

template<typename ValueT>
Value* NotEqual(Value* left, Value* right, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow)
{
    return BinaryPredEvaluate<ValueT>(left, right, std::not_equal_to<typename ValueT::OperandType>(), span, moduleId);
}

BinaryOperatorFun notEqual[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotEqual<BoolValue>, NotEqual<SByteValue>, NotEqual<ByteValue>, NotEqual<ShortValue>, NotEqual<UShortValue>, NotEqual<IntValue>, NotEqual<UIntValue>,
    NotEqual<LongValue>, NotEqual<ULongValue>, NotEqual<FloatValue>, NotEqual<DoubleValue>, NotEqual<CharValue>, NotEqual<WCharValue>, NotEqual<UCharValue>, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotEqual<PointerValue>, NotSupported, NotSupported
};

template<typename ValueT>
Value* Less(Value* left, Value* right, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow)
{
    return BinaryPredEvaluate<ValueT>(left, right, std::less<typename ValueT::OperandType>(), span, moduleId);
}

BinaryOperatorFun less[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, Less<SByteValue>, Less<ByteValue>, Less<ShortValue>, Less<UShortValue>, Less<IntValue>, Less<UIntValue>,
    Less<LongValue>, Less<ULongValue>, Less<FloatValue>, Less<DoubleValue>, Less<CharValue>, Less<WCharValue>, Less<UCharValue>, 
    NotSupported, NotSupported, NotSupported, NotSupported, Less<PointerValue>, NotSupported, NotSupported
};

template<typename ValueT>
Value* Greater(Value* left, Value* right, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow)
{
    return BinaryPredEvaluate<ValueT>(left, right, std::greater<typename ValueT::OperandType>(), span, moduleId);
}

BinaryOperatorFun greater[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, Greater<SByteValue>, Greater<ByteValue>, Greater<ShortValue>, Greater<UShortValue>, Greater<IntValue>, Greater<UIntValue>,
    Greater<LongValue>, Greater<ULongValue>, Greater<FloatValue>, Greater<DoubleValue>, Greater<CharValue>, Greater<WCharValue>, Greater<UCharValue>, 
    NotSupported, NotSupported, NotSupported, NotSupported, Greater<PointerValue>, NotSupported, NotSupported
};

template<typename ValueT>
Value* LessEqual(Value* left, Value* right, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow)
{
    return BinaryPredEvaluate<ValueT>(left, right, std::less_equal<typename ValueT::OperandType>(), span, moduleId);
}

BinaryOperatorFun lessEqual[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, LessEqual<SByteValue>, LessEqual<ByteValue>, LessEqual<ShortValue>, LessEqual<UShortValue>, LessEqual<IntValue>, LessEqual<UIntValue>,
    LessEqual<LongValue>, LessEqual<ULongValue>, LessEqual<FloatValue>, LessEqual<DoubleValue>, LessEqual<CharValue>, LessEqual<WCharValue>, LessEqual<UCharValue>, 
    NotSupported, NotSupported, NotSupported, NotSupported, LessEqual<PointerValue>, NotSupported, NotSupported
};

template<typename ValueT>
Value* GreaterEqual(Value* left, Value* right, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow)
{
    return BinaryPredEvaluate<ValueT>(left, right, std::greater_equal<typename ValueT::OperandType>(), span, moduleId);
}

BinaryOperatorFun greaterEqual[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, GreaterEqual<SByteValue>, GreaterEqual<ByteValue>, GreaterEqual<ShortValue>, GreaterEqual<UShortValue>, GreaterEqual<IntValue>, GreaterEqual<UIntValue>,
    GreaterEqual<LongValue>, GreaterEqual<ULongValue>, GreaterEqual<FloatValue>, GreaterEqual<DoubleValue>, GreaterEqual<CharValue>, GreaterEqual<WCharValue>, GreaterEqual<UCharValue>, 
    NotSupported, NotSupported, NotSupported, NotSupported, GreaterEqual<PointerValue>, NotSupported, NotSupported
};

template<typename T>
struct shiftLeftFun : std::binary_function<T, T, T>
{
    T operator()(const T& left, const T& right) const
    {
        return left << right;
    }
};

template<typename ValueT>
Value* ShiftLeft(Value* left, Value* right, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow)
{
    return BinaryEvaluate<ValueT>(left, right, shiftLeftFun<typename ValueT::OperandType>(), span, moduleId);
}

BinaryOperatorFun shiftLeft[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, ShiftLeft<SByteValue>, ShiftLeft<ByteValue>, ShiftLeft<ShortValue>, ShiftLeft<UShortValue>, ShiftLeft<IntValue>, ShiftLeft<UIntValue>,
    ShiftLeft<LongValue>, ShiftLeft<ULongValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename T>
struct shiftRightFun : std::binary_function<T, T, T>
{
    T operator()(const T& left, const T& right) const
    {
        return left >> right;
    }
};

template<typename ValueT>
Value* ShiftRight(Value* left, Value* right, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow)
{
    return BinaryEvaluate<ValueT>(left, right, shiftRightFun<typename ValueT::OperandType>(), span, moduleId);
}

BinaryOperatorFun shiftRight[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, ShiftRight<SByteValue>, ShiftRight<ByteValue>, ShiftRight<ShortValue>, ShiftRight<UShortValue>, ShiftRight<IntValue>, ShiftRight<UIntValue>,
    ShiftRight<LongValue>, ShiftRight<ULongValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Value* Add(Value* left, Value* right, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow)
{
    return BinaryEvaluate<ValueT>(left, right, std::plus<typename ValueT::OperandType>(), span, moduleId);
}

BinaryOperatorFun add[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, Add<SByteValue>, Add<ByteValue>, Add<ShortValue>, Add<UShortValue>, Add<IntValue>, Add<UIntValue>,
    Add<LongValue>, Add<ULongValue>, Add<FloatValue>, Add<DoubleValue>, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Value* Sub(Value* left, Value* right, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow)
{
    return BinaryEvaluate<ValueT>(left, right, std::minus<typename ValueT::OperandType>(), span, moduleId);
}

BinaryOperatorFun sub[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, Sub<SByteValue>, Sub<ByteValue>, Sub<ShortValue>, Sub<UShortValue>, Sub<IntValue>, Sub<UIntValue>,
    Sub<LongValue>, Sub<ULongValue>, Sub<FloatValue>, Sub<DoubleValue>, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Value* Mul(Value* left, Value* right, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow)
{
    return BinaryEvaluate<ValueT>(left, right, std::multiplies<typename ValueT::OperandType>(), span, moduleId);
}

BinaryOperatorFun mul[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, Mul<SByteValue>, Mul<ByteValue>, Mul<ShortValue>, Mul<UShortValue>, Mul<IntValue>, Mul<UIntValue>,
    Mul<LongValue>, Mul<ULongValue>, Mul<FloatValue>, Mul<DoubleValue>, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Value* Div(Value* left, Value* right, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow)
{
    return BinaryEvaluate<ValueT>(left, right, std::divides<typename ValueT::OperandType>(), span, moduleId);
}

BinaryOperatorFun div[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, Div<SByteValue>, Div<ByteValue>, Div<ShortValue>, Div<UShortValue>, Div<IntValue>, Div<UIntValue>,
    Div<LongValue>, Div<ULongValue>, Div<FloatValue>, Div<DoubleValue>, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Value* Rem(Value* left, Value* right, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow)
{
    return BinaryEvaluate<ValueT>(left, right, std::modulus<typename ValueT::OperandType>(), span, moduleId);
}

BinaryOperatorFun rem[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, Rem<SByteValue>, Rem<ByteValue>, Rem<ShortValue>, Rem<UShortValue>, Rem<IntValue>, Rem<UIntValue>,
    Rem<LongValue>, Rem<ULongValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Value* Not(Value* subject, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow)
{
    return UnaryEvaluate<ValueT>(subject, std::logical_not<typename ValueT::OperandType>(), span, moduleId);
}

UnaryOperatorFun logicalNot[uint8_t(ValueType::maxValue)] =
{
    NotSupported, Not<BoolValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported,
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename T>
struct Identity
{
    const T& operator()(const T& value) const
    {
        return value;
    }
};

template<typename ValueT>
Value* UnaryPlus(Value* subject, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow)
{
    return UnaryEvaluate<ValueT>(subject, Identity<typename ValueT::OperandType>(), span, moduleId);
}

UnaryOperatorFun unaryPlus[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, UnaryPlus<SByteValue>, UnaryPlus<ByteValue>, UnaryPlus<ShortValue>, UnaryPlus<UShortValue>, UnaryPlus<IntValue>, UnaryPlus<UIntValue>,
    UnaryPlus<LongValue>, UnaryPlus<ULongValue>, UnaryPlus<FloatValue>, UnaryPlus<DoubleValue>, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Value* UnaryMinus(Value* subject, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow)
{
    return UnaryEvaluate<ValueT>(subject, std::negate<typename ValueT::OperandType>(), span, moduleId);
}

UnaryOperatorFun unaryMinus[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, UnaryMinus<SByteValue>, UnaryMinus<ByteValue>, UnaryMinus<ShortValue>, UnaryMinus<UShortValue>, UnaryMinus<IntValue>, UnaryMinus<UIntValue>,
    UnaryMinus<LongValue>, UnaryMinus<ULongValue>, UnaryMinus<FloatValue>, UnaryMinus<DoubleValue>, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template <typename T>
struct BitNot
{
    T operator()(const T& x) const
    {
        return ~x;
    }
};

template<typename ValueT>
Value* Complement(Value* subject, const Span& span, const boost::uuids::uuid& moduleId, bool dontThrow)
{
    return UnaryEvaluate<ValueT>(subject, BitNot<typename ValueT::OperandType>(), span, moduleId);
}

UnaryOperatorFun complement[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, Complement<SByteValue>, Complement<ByteValue>, Complement<ShortValue>, Complement<UShortValue>, Complement<IntValue>, Complement<UIntValue>,
    Complement<LongValue>, Complement<ULongValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

enum class Operator
{
    add, sub, comparison, other
};

class Evaluator : public Visitor
{
public:
    Evaluator(BoundCompileUnit& boundCompileUnit_, ContainerScope* containerScope_, TypeSymbol* targetType_, ValueType targetValueType_, bool cast_, bool dontThrow_, BoundFunction* currentFunction_, const Span& span_,
        const boost::uuids::uuid& moduleId_);
    bool Error() const { return error; }
    std::unique_ptr<Value> GetValue();

    void Visit(NamespaceImportNode& namespaceImportNode) override;
    void Visit(AliasNode& aliasNode) override;
    void Visit(FunctionNode& functionNode) override;
    void Visit(ClassNode& classNode) override;
    void Visit(StaticConstructorNode& staticConstructorNode) override;
    void Visit(ConstructorNode& constructorNode) override;
    void Visit(DestructorNode& destructorNode) override;
    void Visit(MemberFunctionNode& memberFunctionNode) override;
    void Visit(ConversionFunctionNode& conversionFunctionNode) override;
    void Visit(MemberVariableNode& memberVariableNode) override;
    void Visit(InterfaceNode& interfaceNode) override;
    void Visit(DelegateNode& delegateNode) override;
    void Visit(ClassDelegateNode& classDelegateNode) override;

    void Visit(CompoundStatementNode& compoundStatementNode) override;
    void Visit(ReturnStatementNode& returnStatementNode) override;
    void Visit(IfStatementNode& ifStatementNode) override;
    void Visit(WhileStatementNode& whileStatementNode) override;
    void Visit(DoStatementNode& doStatementNode) override;
    void Visit(ForStatementNode& forStatementNode) override;
    void Visit(BreakStatementNode& breakStatementNode) override;
    void Visit(ContinueStatementNode& continueStatementNode) override;
    void Visit(GotoStatementNode& gotoStatementNode) override;
    void Visit(ConstructionStatementNode& constructionStatementNode) override;
    void Visit(DeleteStatementNode& deleteStatementNode) override;
    void Visit(DestroyStatementNode& destroyStatementNode) override;
    void Visit(AssignmentStatementNode& assignmentStatementNode) override;
    void Visit(ExpressionStatementNode& expressionStatementNode) override;
    void Visit(EmptyStatementNode& emptyStatementNode) override;
    void Visit(RangeForStatementNode& rangeForStatementNode) override;
    void Visit(SwitchStatementNode& switchStatementNode) override;
    void Visit(CaseStatementNode& caseStatementNode) override;
    void Visit(DefaultStatementNode& defaultStatementNode) override;
    void Visit(GotoCaseStatementNode& gotoCaseStatementNode) override;
    void Visit(GotoDefaultStatementNode& gotoDefaultStatementNode) override;
    void Visit(ThrowStatementNode& throwStatementNode) override;
    void Visit(TryStatementNode& tryStatementNode) override;
    void Visit(CatchNode& catchNode) override;
    void Visit(AssertStatementNode& assertStatementNode) override;
    void Visit(ConditionalCompilationPartNode& conditionalCompilationPartNode) override;
    void Visit(ConditionalCompilationDisjunctionNode& conditionalCompilationDisjunctionNode) override;
    void Visit(ConditionalCompilationConjunctionNode& conditionalCompilationConjunctionNode) override;
    void Visit(ConditionalCompilationNotNode& conditionalCompilationNotNode) override;
    void Visit(ConditionalCompilationPrimaryNode& conditionalCompilationPrimaryNode) override;
    void Visit(ConditionalCompilationStatementNode& conditionalCompilationStatementNode) override;

    void Visit(BoolNode& boolNode) override;
    void Visit(SByteNode& sbyteNode) override;
    void Visit(ByteNode& byteNode) override;
    void Visit(ShortNode& shortNode) override;
    void Visit(UShortNode& ushortNode) override;
    void Visit(IntNode& intNode) override;
    void Visit(UIntNode& uintNode) override;
    void Visit(LongNode& longNode) override;
    void Visit(ULongNode& ulongNode) override;
    void Visit(FloatNode& floatNode) override;
    void Visit(DoubleNode& doubleNode) override;
    void Visit(CharNode& charNode) override;
    void Visit(WCharNode& wcharNode) override;
    void Visit(UCharNode& ucharNode) override;
    void Visit(VoidNode& voidNode) override;

    void Visit(BooleanLiteralNode& booleanLiteralNode) override;
    void Visit(SByteLiteralNode& sbyteLiteralNode) override;
    void Visit(ByteLiteralNode& byteLiteralNode) override;
    void Visit(ShortLiteralNode& shortLiteralNode) override;
    void Visit(UShortLiteralNode& ushortLiteralNode) override;
    void Visit(IntLiteralNode& intLiteralNode) override;
    void Visit(UIntLiteralNode& uintLiteralNode) override;
    void Visit(LongLiteralNode& longLiteralNode) override;
    void Visit(ULongLiteralNode& ulongLiteralNode) override;
    void Visit(FloatLiteralNode& floatLiteralNode) override;
    void Visit(DoubleLiteralNode& doubleLiteralNode) override;
    void Visit(CharLiteralNode& charLiteralNode) override;
    void Visit(WCharLiteralNode& wcharLiteralNode) override;
    void Visit(UCharLiteralNode& ucharLiteralNode) override;
    void Visit(StringLiteralNode& stringLiteralNode) override;
    void Visit(WStringLiteralNode& wstringLiteralNode) override;
    void Visit(UStringLiteralNode& ustringLiteralNode) override;
    void Visit(NullLiteralNode& nullLiteralNode) override;
    void Visit(ArrayLiteralNode& arrayLiteralNode) override;
    void Visit(StructuredLiteralNode& structuredLiteralNode) override;

    void Visit(IdentifierNode& identifierNode) override;
    void Visit(TemplateIdNode& templateIdNode) override;

    void Visit(DotNode& dotNode) override;
    void Visit(ArrowNode& arrowNode) override;
    void Visit(EquivalenceNode& equivalenceNode) override;
    void Visit(ImplicationNode& implicationNode) override;
    void Visit(DisjunctionNode& disjunctionNode) override;
    void Visit(ConjunctionNode& conjunctionNode) override;
    void Visit(BitOrNode& bitOrNode) override;
    void Visit(BitXorNode& bitXorNode) override;
    void Visit(BitAndNode& bitAndNode) override;
    void Visit(EqualNode& equalNode) override;
    void Visit(NotEqualNode& notEqualNode) override;
    void Visit(LessNode& lessNode) override;
    void Visit(GreaterNode& greaterNode) override;
    void Visit(LessOrEqualNode& lessOrEqualNode) override;
    void Visit(GreaterOrEqualNode& greaterOrEqualNode) override;
    void Visit(ShiftLeftNode& shiftLeftNode) override;
    void Visit(ShiftRightNode& shiftRightNode) override;
    void Visit(AddNode& addNode) override;
    void Visit(SubNode& subNode) override;
    void Visit(MulNode& mulNode) override;
    void Visit(DivNode& divNode) override;
    void Visit(RemNode& remNode) override;
    void Visit(NotNode& notNode) override;
    void Visit(UnaryPlusNode& unaryPlusNode) override;
    void Visit(UnaryMinusNode& unaryMinusNode) override;
    void Visit(PrefixIncrementNode& prefixIncrementNode) override;
    void Visit(PrefixDecrementNode& prefixDecrementNode) override;
    void Visit(DerefNode& derefNode) override;
    void Visit(AddrOfNode& addrOfNode) override;
    void Visit(ComplementNode& complementNode) override;
    void Visit(IsNode& isNode) override;
    void Visit(AsNode& asNode) override;
    void Visit(IndexingNode& indexingNode) override;
    void Visit(InvokeNode& invokeNode) override;
    void Visit(PostfixIncrementNode& postfixIncrementNode) override;
    void Visit(PostfixDecrementNode& postfixDecrementNode) override;
    void Visit(SizeOfNode& sizeOfNode) override;
    void Visit(TypeNameNode& typeNameNode) override;
    void Visit(TypeIdNode& typeIdNode) override;
    void Visit(CastNode& castNode) override;
    void Visit(ConstructNode& constructNode) override;
    void Visit(NewNode& newNode) override;
    void Visit(ThisNode& thisNode) override;
    void Visit(BaseNode& baseNode) override;
    void Visit(ParenthesizedExpressionNode& parenthesizedExpressionNode) override;
private:
    BoundCompileUnit& boundCompileUnit;
    SymbolTable* symbolTable;
    Module* module;
    ContainerScope* containerScope;
    ContainerScope* qualifiedScope;
    BoundFunction* currentFunction;
    DeclarationBlock* currentDeclarationBlock;
    FileScope* currentFileScope;
    ClassTypeSymbol* currentClassType;
    bool cast;
    bool dontThrow;
    bool error;
    bool returned;
    bool broke;
    bool continued;
    bool lvalue;
    Span span;
    boost::uuids::uuid moduleId;
    std::unique_ptr<Value> value;
    TypeSymbol* targetType;
    ValueType targetValueType;
    VariableValueSymbol* targetValueSymbol;
    std::vector<std::unique_ptr<Value>> argumentValues;
    std::unique_ptr<Value> structureReferenceValue;
    std::vector<TypeSymbol*> templateTypeArguments;
    void EvaluateBinOp(BinaryNode& node, BinaryOperatorFun* fun);
    void EvaluateBinOp(BinaryNode& node, BinaryOperatorFun* fun, Operator op);
    void EvaluateAdditivePointerOp(const Span& span, Operator op, const std::unique_ptr<Value>& left, const std::unique_ptr<Value>& right);
    void EvaluateUnaryOp(UnaryNode& node, UnaryOperatorFun* fun);
    void EvaluateSymbol(Symbol* symbol, const Span& span);
    void EvaluateConstantSymbol(ConstantSymbol* constantSymbol, const Span& span);
    void EvaluateEnumConstantSymbol(EnumConstantSymbol* enumConstantSymbol, const Span& span);
};

Evaluator::Evaluator(BoundCompileUnit& boundCompileUnit_, ContainerScope* containerScope_, TypeSymbol* targetType_, ValueType targetValueType_, bool cast_, bool dontThrow_, BoundFunction* currentFunction_, 
    const Span& span_, const boost::uuids::uuid& moduleId_) :
    boundCompileUnit(boundCompileUnit_), symbolTable(&boundCompileUnit.GetSymbolTable()), module(&boundCompileUnit.GetModule()), 
    containerScope(containerScope_), qualifiedScope(nullptr), cast(cast_), dontThrow(dontThrow_), error(false),
    returned(false), broke(false), continued(false), lvalue(false), currentFunction(currentFunction_), currentDeclarationBlock(nullptr), currentFileScope(nullptr), currentClassType(nullptr), 
    span(span_), moduleId(moduleId_), value(), targetType(targetType_), targetValueType(targetValueType_), targetValueSymbol(nullptr)
{
}

void Evaluator::EvaluateBinOp(BinaryNode& node, BinaryOperatorFun* fun)
{
    EvaluateBinOp(node, fun, Operator::other);
}

void Evaluator::EvaluateBinOp(BinaryNode& node, BinaryOperatorFun* fun, Operator op)
{
    node.Left()->Accept(*this);
    if (error)
    {
        return;
    }
    if (!value)
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            ThrowCannotEvaluateStatically(span, moduleId, node.GetSpan(), node.ModuleId());
        }
    }
    std::unique_ptr<Value> left(value.release());
    node.Right()->Accept(*this);
    if (error)
    {
        return;
    }
    if (!value)
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            ThrowCannotEvaluateStatically(span, moduleId, node.GetSpan(), node.ModuleId());
        }
    }
    std::unique_ptr<Value> right(value.release());
    if ((op == Operator::add || op == Operator::sub) && (left->GetValueType() == ValueType::pointerValue || right->GetValueType() == ValueType::pointerValue))
    {
        EvaluateAdditivePointerOp(node.GetSpan(), op, left, right);
        return;
    }
    if (op == Operator::comparison && left->GetValueType() == ValueType::pointerValue && right->GetValueType() == ValueType::pointerValue)
    {
        PointerValue* leftPtr = static_cast<PointerValue*>(left.get());
        PointerValue* rightPtr = static_cast<PointerValue*>(right.get());
        if (leftPtr->GetValue() != nullptr && rightPtr->GetValue() != nullptr && leftPtr->PointeeType() != rightPtr->PointeeType())
        {
            if (dontThrow)
            {
                error = true;
                return;
            }
            else
            {
                throw Exception("incompatible pointer types for comparison", node.GetSpan(), node.ModuleId());
            }
        }
    }
    ValueType leftType = left->GetValueType();
    ValueType rightType = right->GetValueType();
    ValueType commonType = CommonType(leftType, rightType);
    ValueType operationType = commonType;
    if (targetValueType > operationType)
    {
        operationType = targetValueType;
    }
    TypeSymbol* type = GetTypeFor(operationType, symbolTable);
    if (!type)
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            throw Exception("conversion from " + ValueTypeStr(leftType) + " to " + ValueTypeStr(operationType) + " is not valid", span, moduleId);
        }
    }
    std::unique_ptr<Value> leftConverted(left->As(type, cast, node.GetSpan(), node.ModuleId(), dontThrow));
    std::unique_ptr<Value> rightConverted(right->As(type, cast, node.GetSpan(), node.ModuleId(), dontThrow));
    if (dontThrow)
    {
        if (!leftConverted || !rightConverted)
        {
            error = true;
            return;
        }
    }
    BinaryOperatorFun operation = fun[uint8_t(operationType)];
    value.reset(operation(leftConverted.get(), rightConverted.get(), node.GetSpan(), node.ModuleId(), dontThrow));
}

void Evaluator::EvaluateAdditivePointerOp(const Span& span, Operator op, const std::unique_ptr<Value>& left, const std::unique_ptr<Value>& right)
{
    if (op == Operator::add)
    {
        if (left->GetValueType() == ValueType::pointerValue)
        {
            std::unique_ptr<Value> rightConverted(right->As(symbolTable->GetTypeByName(U"long"), cast, span, moduleId, dontThrow));
            if (dontThrow)
            {
                if (!rightConverted)
                {
                    error = true;
                    return;
                }
            }
            int64_t offset = static_cast<LongValue*>(rightConverted.get())->GetValue();
            PointerValue* leftPointerValue = static_cast<PointerValue*>(left.get());
            value.reset(leftPointerValue->Add(offset));
            if (!value)
            {
                if (dontThrow)
                {
                    error = true;
                    return;
                }
                else
                {
                    throw Exception("invalid pointer operands", span, moduleId);
                }
            }
        }
        else if (right->GetValueType() == ValueType::pointerValue)
        {
            std::unique_ptr<Value> leftConverted(right->As(symbolTable->GetTypeByName(U"long"), cast, span, moduleId, dontThrow));
            if (dontThrow)
            {
                if (!leftConverted)
                {
                    error = true;
                    return;
                }
            }
            int64_t offset = static_cast<LongValue*>(leftConverted.get())->GetValue();
            PointerValue* rightPointerValue = static_cast<PointerValue*>(right.get());
            value.reset(rightPointerValue->Add(offset));
            if (!value)
            {
                if (dontThrow)
                {
                    error = true;
                    return;
                }
                else
                {
                    throw Exception("invalid pointer operands", span, moduleId);
                }
            }
        }
        else
        {
            if (dontThrow)
            {
                error = true;
                return;
            }
            else
            {
                throw Exception("invalid pointer operands", span, moduleId);
            }
        }
    }
    else if (op == Operator::sub)
    {
        if (left->GetValueType() == ValueType::pointerValue && right->GetValueType() != ValueType::pointerValue)
        {
            std::unique_ptr<Value> rightConverted(right->As(symbolTable->GetTypeByName(U"long"), cast, span, moduleId, dontThrow));
            if (dontThrow)
            {
                if (!rightConverted)
                {
                    error = true;
                    return;
                }
            }
            int64_t offset = static_cast<LongValue*>(rightConverted.get())->GetValue();
            PointerValue* leftPointerValue = static_cast<PointerValue*>(left.get());
            value.reset(leftPointerValue->Sub(offset));
            if (!value)
            {
                if (dontThrow)
                {
                    error = true;
                    return;
                }
                else
                {
                    throw Exception("invalid pointer operands", span, moduleId);
                }
            }
        }
        else if (left->GetValueType() == ValueType::pointerValue && right->GetValueType() == ValueType::pointerValue)
        {
            PointerValue* leftPointerValue = static_cast<PointerValue*>(left.get());
            PointerValue* rightPointerValue = static_cast<PointerValue*>(right.get());
            if (leftPointerValue->PointeeType() != rightPointerValue->PointeeType())
            {
                if (dontThrow)
                {
                    error = true;
                    return;
                }
                else
                {
                    throw Exception("incompatible pointer operands", span, moduleId);
                }
            }
            value.reset(leftPointerValue->Sub(rightPointerValue->GetValue()));
            if (!value)
            {
                if (dontThrow)
                {
                    error = true;
                    return;
                }
                else
                {
                    throw Exception("invalid pointer operands", span, moduleId);
                }
            }
        }
        else
        {
            if (dontThrow)
            {
                error = true;
                return;
            }
            else
            {
                throw Exception("invalid pointer operands", span, moduleId);
            }
        }
    }
}

void Evaluator::EvaluateUnaryOp(UnaryNode& node, UnaryOperatorFun* fun)
{
    node.Subject()->Accept(*this);
    if (error)
    {
        return;
    }
    if (!value)
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            ThrowCannotEvaluateStatically(span, moduleId, node.GetSpan(), node.ModuleId());
        }
    }
    std::unique_ptr<Value> subject(value.release());
    ValueType subjectType = subject->GetValueType();
    ValueType operationType = subjectType;
    if (targetValueType > operationType)
    {
        operationType = targetValueType;
    }
    TypeSymbol* type = GetTypeFor(operationType, symbolTable);
    if (!type)
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            throw Exception("conversion from " + ValueTypeStr(subjectType) + " to " + ValueTypeStr(operationType) + " is not valid", span, moduleId);
        }
    }
    std::unique_ptr<Value> subjectConverted(subject->As(type, cast, node.GetSpan(), node.ModuleId(), dontThrow));
    if (dontThrow)
    {
        if (!subjectConverted)
        {
            error = true;
            return;
        }
    }
    UnaryOperatorFun operation = fun[uint8_t(operationType)];
    value.reset(operation(subjectConverted.get(), node.GetSpan(), node.ModuleId(), dontThrow));
}

std::unique_ptr<Value> Evaluator::GetValue()
{
    if (error)
    {
        return std::unique_ptr<Value>();
    }
    return std::move(value);
}

void Evaluator::Visit(FunctionNode& functionNode)
{
    bool fileScopeAdded = false;
    Symbol* symbol = symbolTable->GetSymbol(&functionNode);
    if (symbol->IsFunctionSymbol())
    {
        FunctionSymbol* functionSymbol = static_cast<FunctionSymbol*>(symbol);
        int n = functionSymbol->UsingNodes().Count();
        if (n > 0)
        {
            FileScope* fileScope = new FileScope();
            FileScope* prevFileScope = currentFileScope;
            currentFileScope = fileScope;
            boundCompileUnit.AddFileScope(fileScope);
            fileScopeAdded = true;
            for (int i = 0; i < n; ++i)
            {
                Node* usingNode = functionSymbol->UsingNodes()[i];
                usingNode->Accept(*this);
            }
            currentFileScope = prevFileScope;
        }
    }
    bool prevReturned = returned;
    DeclarationBlock* prevDeclarationBlock = currentDeclarationBlock;
    DeclarationBlock declarationBlock(span, moduleId, U"functionBlock");
    currentDeclarationBlock = &declarationBlock;
    ContainerScope* prevContainerScope = containerScope;
    containerScope = symbol->GetContainerScope();
    declarationBlock.GetContainerScope()->SetParentScope(containerScope);
    containerScope = declarationBlock.GetContainerScope();
    int nt = functionNode.TemplateParameters().Count();
    if (nt != templateTypeArguments.size())
    {
        if (dontThrow)
        {
            containerScope = prevContainerScope;
            currentDeclarationBlock = prevDeclarationBlock;
            returned = prevReturned;
            error = true;
            return;
        }
        else
        {
            throw Exception("wrong number of function template type arguments", span, moduleId);
        }
    }
    for (int i = 0; i < nt; ++i)
    {
        TemplateParameterNode* templateParameterNode = functionNode.TemplateParameters()[i];
        TypeSymbol* templateTypeArgument = templateTypeArguments[i];
        BoundTemplateParameterSymbol* boundTemplateParameter = new BoundTemplateParameterSymbol(span, moduleId, templateParameterNode->Id()->Str());
        boundTemplateParameter->SetType(templateTypeArgument);
        declarationBlock.AddMember(boundTemplateParameter);
    }
    int n = functionNode.Parameters().Count();
    if (n != argumentValues.size())
    {
        if (dontThrow)
        {
            containerScope = prevContainerScope;
            currentDeclarationBlock = prevDeclarationBlock;
            returned = prevReturned;
            error = true;
            return;
        }
        else
        {
            throw Exception("wrong number of function arguments", span, moduleId);
        }
    }
    for (int i = 0; i < n; ++i)
    {
        std::unique_ptr<Value> argumentValue = std::move(argumentValues[i]);
        TypeSymbol* argumentType = argumentValue->GetType(symbolTable);
        ParameterNode* parameterNode = functionNode.Parameters()[i];
        VariableValueSymbol* variableValueSymbol = new VariableValueSymbol(parameterNode->GetSpan(), parameterNode->ModuleId(), parameterNode->Id()->Str(), std::move(argumentValue));
        variableValueSymbol->SetType(argumentType);
        declarationBlock.AddMember(variableValueSymbol);
    }
    functionNode.Body()->Accept(*this);
    containerScope = prevContainerScope;
    currentDeclarationBlock = prevDeclarationBlock;
    returned = prevReturned;
    if (fileScopeAdded)
    {
        boundCompileUnit.RemoveLastFileScope();
    }
}

void Evaluator::Visit(ConstructorNode& constructorNode)
{
    bool fileScopeAdded = false;
    Symbol* symbol = symbolTable->GetSymbol(&constructorNode);
    ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(symbol->Parent());
    ClassTypeSymbol* prevClassType = currentClassType;
    currentClassType = classType;
    if (symbol->IsFunctionSymbol())
    {
        FunctionSymbol* functionSymbol = static_cast<FunctionSymbol*>(symbol);
        int n = functionSymbol->UsingNodes().Count();
        if (n > 0)
        {
            FileScope* fileScope = new FileScope();
            FileScope* prevFileScope = currentFileScope;
            currentFileScope = fileScope;
            boundCompileUnit.AddFileScope(fileScope);
            fileScopeAdded = true;
            for (int i = 0; i < n; ++i)
            {
                Node* usingNode = functionSymbol->UsingNodes()[i];
                usingNode->Accept(*this);
            }
            currentFileScope = prevFileScope;
        }
    }
    bool prevReturned = returned;
    DeclarationBlock* prevDeclarationBlock = currentDeclarationBlock;
    DeclarationBlock declarationBlock(span, moduleId, U"constructorBlock");
    currentDeclarationBlock = &declarationBlock;
    ContainerScope* prevContainerScope = containerScope;
    containerScope = symbol->GetContainerScope();
    declarationBlock.GetContainerScope()->SetParentScope(containerScope);
    containerScope = declarationBlock.GetContainerScope();
    int n = constructorNode.Parameters().Count();
    if (n != argumentValues.size())
    {
        if (dontThrow)
        {
            containerScope = prevContainerScope;
            currentDeclarationBlock = prevDeclarationBlock;
            returned = prevReturned;
            currentClassType = prevClassType;
            error = true;
            return;
        }
        else
        {
            throw Exception("wrong number of constructor arguments", span, moduleId);
        }
    }
    for (int i = 0; i < n; ++i)
    {
        std::unique_ptr<Value> argumentValue = std::move(argumentValues[i]);
        TypeSymbol* argumentType = argumentValue->GetType(symbolTable);
        ParameterNode* parameterNode = constructorNode.Parameters()[i];
        VariableValueSymbol* variableValueSymbol = new VariableValueSymbol(parameterNode->GetSpan(), parameterNode->ModuleId(), parameterNode->Id()->Str(), std::move(argumentValue));
        variableValueSymbol->SetType(argumentType);
        declarationBlock.AddMember(variableValueSymbol);
    }
    std::unordered_map<std::u32string, MemberInitializerNode*> memberInitializerMap;
    int ni = constructorNode.Initializers().Count();
    for (int i = 0; i < ni; ++i)
    {
        InitializerNode* initializer = constructorNode.Initializers()[i];
        if (initializer->GetNodeType() == NodeType::thisInitializerNode || initializer->GetNodeType() == NodeType::baseInitializerNode)
        {
            if (dontThrow)
            {
                containerScope = prevContainerScope;
                currentDeclarationBlock = prevDeclarationBlock;
                returned = prevReturned;
                currentClassType = prevClassType;
                error = true;
                return;
            }
            else
            {
                throw Exception("this and base initializers not supported for a constexpr constructor", constructorNode.GetSpan(), constructorNode.ModuleId());
            }
        }
        else
        {
            MemberInitializerNode* memberInitializer = static_cast<MemberInitializerNode*>(initializer);
            std::u32string memberName = memberInitializer->MemberId()->Str();
            auto it = memberInitializerMap.find(memberName);
            if (it != memberInitializerMap.cend())
            {
                if (dontThrow)
                {
                    containerScope = prevContainerScope;
                    currentDeclarationBlock = prevDeclarationBlock;
                    returned = prevReturned;
                    currentClassType = prevClassType;
                    error = true;
                    return;
                }
                else
                {
                    throw Exception("already has initializer for member variable '" + ToUtf8(memberName) + "'", initializer->GetSpan(), initializer->ModuleId());
                }
            }
            memberInitializerMap[memberName] = memberInitializer;
        }
    }
    std::vector<std::unique_ptr<Value>> memberValues;
    int nm = classType->MemberVariables().size();
    for (int i = 0; i < nm; ++i)
    {
        value.reset();
        MemberVariableSymbol* memberVariableSymbol = classType->MemberVariables()[i];
        std::vector<std::unique_ptr<Value>> initializerArgumentValues;
        auto it = memberInitializerMap.find(memberVariableSymbol->Name());
        if (it != memberInitializerMap.cend())
        {
            MemberInitializerNode* memberInitializer = it->second;
            int na = memberInitializer->Arguments().Count();
            for (int i = 0; i < na; ++i)
            {
                Node* argumentNode = memberInitializer->Arguments()[i];
                argumentNode->Accept(*this);
                if (error) return;
                if (!value)
                {
                    if (dontThrow)
                    {
                        containerScope = prevContainerScope;
                        currentDeclarationBlock = prevDeclarationBlock;
                        returned = prevReturned;
                        currentClassType = prevClassType;
                        error = true;
                        return;
                    }
                    else
                    {
                        ThrowCannotEvaluateStatically(span, moduleId, constructorNode.GetSpan(), constructorNode.ModuleId());
                    }
                }
                initializerArgumentValues.push_back(std::move(value));
            }
        }
        std::vector<FunctionScopeLookup> lookups;
        lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
        lookups.push_back(FunctionScopeLookup(ScopeLookup::this_, memberVariableSymbol->GetType()->BaseType()->GetContainerScope()));
        lookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
        std::vector<std::unique_ptr<BoundExpression>> initializerArguments = ValuesToLiterals(initializerArgumentValues, symbolTable, error);
        if (error)
        {
            if (dontThrow)
            {
                containerScope = prevContainerScope;
                currentDeclarationBlock = prevDeclarationBlock;
                returned = prevReturned;
                currentClassType = prevClassType;
                return;
            }
            else
            {
                ThrowCannotEvaluateStatically(span, moduleId, constructorNode.GetSpan(), constructorNode.ModuleId());
            }
        }
        initializerArguments.insert(initializerArguments.begin(), std::unique_ptr<BoundExpression>(new BoundTypeExpression(span, moduleId, memberVariableSymbol->GetType()->AddPointer(span, moduleId))));
        OverloadResolutionFlags flags = OverloadResolutionFlags::dontInstantiate;
        if (dontThrow)
        {
            flags = flags | OverloadResolutionFlags::dontThrow;
        }
        std::vector<TypeSymbol*> templateArgumentTypes;
        std::unique_ptr<Exception> exception;
        std::unique_ptr<BoundFunctionCall> constructorCall = ResolveOverload(U"@constructor", containerScope, lookups, initializerArguments, boundCompileUnit, currentFunction, 
            constructorNode.GetSpan(), constructorNode.ModuleId(), flags, templateArgumentTypes, exception);
        if (!constructorCall)
        {
            if (dontThrow)
            {
                containerScope = prevContainerScope;
                currentDeclarationBlock = prevDeclarationBlock;
                returned = prevReturned;
                currentClassType = prevClassType;
                error = true;
                return;
            }
            else
            {
                ThrowCannotEvaluateStatically(span, moduleId, constructorNode.GetSpan(), constructorNode.ModuleId());
            }
        }
        argumentValues = ArgumentsToValues(constructorCall->Arguments(), error, true, boundCompileUnit);
        if (error)
        {
            if (dontThrow)
            {
                return;
            }
            else
            {
                ThrowCannotEvaluateStatically(span, moduleId, constructorNode.GetSpan(), constructorNode.ModuleId());
            }
        }
        FunctionSymbol* constructorSymbol = constructorCall->GetFunctionSymbol();
        if (constructorSymbol->IsCompileTimePrimitiveFunction())
        {
            value = constructorSymbol->ConstructValue(argumentValues, span, moduleId, nullptr);
            if (!value)
            {
                if (dontThrow)
                {
                    containerScope = prevContainerScope;
                    currentDeclarationBlock = prevDeclarationBlock;
                    returned = prevReturned;
                    currentClassType = prevClassType;
                    error = true;
                    return;
                }
                else
                {
                    ThrowCannotEvaluateStatically(span, moduleId, constructorNode.GetSpan(), constructorNode.ModuleId());
                }
            }
        }
        else if (constructorSymbol->IsConstExpr())
        {
            FunctionNode* ctorNode = boundCompileUnit.GetFunctionNodeFor(constructorSymbol);
            ctorNode->Accept(*this);
            if (!value)
            {
                if (dontThrow)
                {
                    containerScope = prevContainerScope;
                    currentDeclarationBlock = prevDeclarationBlock;
                    returned = prevReturned;
                    currentClassType = prevClassType;
                    error = true;
                    return;
                }
                else
                {
                    ThrowCannotEvaluateStatically(span, moduleId, ctorNode->GetSpan(), ctorNode->ModuleId());
                }
            }
        }
        else
        {
            if (dontThrow)
            {
                containerScope = prevContainerScope;
                currentDeclarationBlock = prevDeclarationBlock;
                returned = prevReturned;
                currentClassType = prevClassType;
                error = true;
                return;
            }
            else
            {
                ThrowCannotEvaluateStatically(span, moduleId, constructorNode.GetSpan(), constructorNode.ModuleId());
            }
        }
        memberValues.push_back(std::move(value));
    }
    constructorNode.Body()->Accept(*this);
    value.reset(new StructuredValue(span, moduleId, classType, std::move(memberValues)));
    containerScope = prevContainerScope;
    currentDeclarationBlock = prevDeclarationBlock;
    returned = prevReturned;
    currentClassType = prevClassType;
    if (fileScopeAdded)
    {
        boundCompileUnit.RemoveLastFileScope();
    }
}

void Evaluator::Visit(MemberFunctionNode& memberFunctionNode)
{
    bool fileScopeAdded = false;
    Symbol* symbol = symbolTable->GetSymbol(&memberFunctionNode);
    if (symbol->IsFunctionSymbol())
    {
        FunctionSymbol* functionSymbol = static_cast<FunctionSymbol*>(symbol);
        int n = functionSymbol->UsingNodes().Count();
        if (n > 0)
        {
            FileScope* fileScope = new FileScope();
            FileScope* prevFileScope = currentFileScope;
            currentFileScope = fileScope;
            boundCompileUnit.AddFileScope(fileScope);
            fileScopeAdded = true;
            for (int i = 0; i < n; ++i)
            {
                Node* usingNode = functionSymbol->UsingNodes()[i];
                usingNode->Accept(*this);
            }
            currentFileScope = prevFileScope;
        }
    }
    bool prevReturned = returned;
    DeclarationBlock* prevDeclarationBlock = currentDeclarationBlock;
    DeclarationBlock declarationBlock(span, moduleId, U"functionBlock");
    currentDeclarationBlock = &declarationBlock;
    ContainerScope* prevContainerScope = containerScope;
    containerScope = symbol->GetContainerScope();
    declarationBlock.GetContainerScope()->SetParentScope(containerScope);
    containerScope = declarationBlock.GetContainerScope();
    int n = memberFunctionNode.Parameters().Count();
    if (n != argumentValues.size())
    {
        if (dontThrow)
        {
            containerScope = prevContainerScope;
            currentDeclarationBlock = prevDeclarationBlock;
            returned = prevReturned;
            error = true;
            return;
        }
        else
        {
            throw Exception("wrong number of function arguments", memberFunctionNode.GetSpan(), memberFunctionNode.ModuleId());
        }
    }
    for (int i = 0; i < n; ++i)
    {
        std::unique_ptr<Value> argumentValue = std::move(argumentValues[i]);
        TypeSymbol* argumentType = argumentValue->GetType(symbolTable);
        ParameterNode* parameterNode = memberFunctionNode.Parameters()[i];
        VariableValueSymbol* variableValueSymbol = new VariableValueSymbol(parameterNode->GetSpan(), parameterNode->ModuleId(), parameterNode->Id()->Str(), std::move(argumentValue));
        variableValueSymbol->SetType(argumentType);
        declarationBlock.AddMember(variableValueSymbol);
    }
    if (currentClassType && structureReferenceValue)
    {
        StructuredValue* structuredValue = nullptr;
        if (structureReferenceValue->IsStructuredReferenceValue())
        {
            structuredValue = static_cast<StructuredReferenceValue*>(structureReferenceValue.get())->GetStructuredValue();
        }
        else
        {
            if (dontThrow)
            {
                containerScope = prevContainerScope;
                currentDeclarationBlock = prevDeclarationBlock;
                returned = prevReturned;
                error = true;
                return;
            }
            else
            {
                throw Exception("structured reference value expected", memberFunctionNode.GetSpan(), memberFunctionNode.ModuleId());
            }
        }
        int n = currentClassType->MemberVariables().size();
        if (n != structuredValue->Members().size())
        {
            if (dontThrow)
            {
                containerScope = prevContainerScope;
                currentDeclarationBlock = prevDeclarationBlock;
                returned = prevReturned;
                error = true;
                return;
            }
            else
            {
                throw Exception("wrong number of structured value members", memberFunctionNode.GetSpan(), memberFunctionNode.ModuleId());
            }
        }
        for (int i = 0; i < n; ++i)
        {
            MemberVariableSymbol* memberVariableSymbol = currentClassType->MemberVariables()[i];
            Value* memberValue = structuredValue->Members()[i].get();
            ConstantSymbol* constantSymbol = new ConstantSymbol(span, moduleId, memberVariableSymbol->Name());
            constantSymbol->SetModule(module);
            constantSymbol->SetType(memberVariableSymbol->GetType());
            if (memberValue->GetValueType() == ValueType::arrayValue)
            {
                constantSymbol->SetValue(new ArrayReferenceValue(static_cast<ArrayValue*>(memberValue)));
            }
            else if (memberValue->GetValueType() == ValueType::structuredValue)
            {
                constantSymbol->SetValue(new StructuredReferenceValue(static_cast<StructuredValue*>(memberValue)));
            }
            else
            {
                constantSymbol->SetValue(memberValue->Clone());
            }
            declarationBlock.AddMember(constantSymbol);
        }
    }
    memberFunctionNode.Body()->Accept(*this);
    containerScope = prevContainerScope;
    currentDeclarationBlock = prevDeclarationBlock;
    returned = prevReturned;
    if (fileScopeAdded)
    {
        boundCompileUnit.RemoveLastFileScope();
    }
}

void Evaluator::Visit(ConversionFunctionNode& conversionFunctionNode)
{
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, conversionFunctionNode.GetSpan(), conversionFunctionNode.ModuleId());
    }
}

void Evaluator::Visit(NamespaceImportNode& namespaceImportNode)
{
    if (currentFileScope)
    {
        currentFileScope->InstallNamespaceImport(containerScope, &namespaceImportNode);
    }
}

void Evaluator::Visit(AliasNode& aliasNode)
{
    if (currentFileScope)
    {
        currentFileScope->InstallAlias(containerScope, &aliasNode);
    }
}

void Evaluator::Visit(ClassNode& classNode)
{
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, classNode.GetSpan(), classNode.ModuleId());
    }
}

void Evaluator::Visit(StaticConstructorNode& staticConstructorNode)
{
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, staticConstructorNode.GetSpan(), staticConstructorNode.ModuleId());
    }
}

void Evaluator::Visit(DestructorNode& destructorNode)
{
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, destructorNode.GetSpan(), destructorNode.ModuleId());
    }
}

void Evaluator::Visit(MemberVariableNode& memberVariableNode)
{
    memberVariableNode.Id()->Accept(*this);
}

void Evaluator::Visit(InterfaceNode& interfaceNode)
{
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, interfaceNode.GetSpan(), interfaceNode.ModuleId());
    }
}

void Evaluator::Visit(DelegateNode& delegateNode)
{
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, delegateNode.GetSpan(), delegateNode.ModuleId());
    }
}

void Evaluator::Visit(ClassDelegateNode& classDelegateNode)
{
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, classDelegateNode.GetSpan(), classDelegateNode.ModuleId());
    }
}

void Evaluator::Visit(CompoundStatementNode& compoundStatementNode)
{
    DeclarationBlock* prevDeclarationBlock = currentDeclarationBlock;
    DeclarationBlock declarationBlock(span, moduleId, U"block");
    currentDeclarationBlock = &declarationBlock;
    ContainerScope* prevContainerScope = containerScope;
    declarationBlock.GetContainerScope()->SetParentScope(containerScope);
    containerScope = declarationBlock.GetContainerScope();
    int n = compoundStatementNode.Statements().Count();
    for (int i = 0; i < n; ++i)
    {
        StatementNode* statementNode = compoundStatementNode.Statements()[i];
        statementNode->Accept(*this);
        if (error || returned || broke || continued)
        {
            currentDeclarationBlock = prevDeclarationBlock;
            containerScope = prevContainerScope;
            return;
        }
    }
    containerScope = prevContainerScope;
    currentDeclarationBlock = prevDeclarationBlock;
}

void Evaluator::Visit(ReturnStatementNode& returnStatementNode)
{
    if (returnStatementNode.Expression())
    {
        returnStatementNode.Expression()->Accept(*this);
        if (error) return;
    }
    returned = true;
}

void Evaluator::Visit(IfStatementNode& ifStatementNode)
{
    ifStatementNode.Condition()->Accept(*this);
    if (error) return;
    if (value && value->GetValueType() == ValueType::boolValue)
    {
        BoolValue* condition = static_cast<BoolValue*>(value.get());
        if (condition->GetValue())
        {
            ifStatementNode.ThenS()->Accept(*this);
        }
        else if (ifStatementNode.ElseS())
        {
            ifStatementNode.ElseS()->Accept(*this);
        }
    }
    else
    {
        if (dontThrow)
        {
            error = true;
        }
        else
        {
            throw Exception("Boolean expression expected", ifStatementNode.GetSpan(), ifStatementNode.ModuleId());
        }
    }
}

void Evaluator::Visit(WhileStatementNode& whileStatementNode)
{
    bool prevBroke = broke;
    bool prevContinued = continued;
    whileStatementNode.Condition()->Accept(*this);
    if (error)
    {
        broke = prevBroke;
        continued = prevContinued;
        return;
    }
    if (value && value->GetValueType() == ValueType::boolValue)
    {
        BoolValue* condition = static_cast<BoolValue*>(value.get());
        while (condition->GetValue())
        {
            whileStatementNode.Statement()->Accept(*this);
            if (error || returned)
            {
                broke = prevBroke;
                continued = prevContinued;
                return;
            }
            if (broke)
            {
                break;
            }
            if (continued)
            {
                continued = false;
            }
            whileStatementNode.Condition()->Accept(*this);
            if (error)
            {
                broke = prevBroke;
                continued = prevContinued;
                return;
            }
            if (value && value->GetValueType() == ValueType::boolValue)
            {
                condition = static_cast<BoolValue*>(value.get());
            }
            else
            {
                if (dontThrow)
                {
                    broke = prevBroke;
                    continued = prevContinued;
                    error = true;
                    return;
                }
                else
                {
                    throw Exception("Boolean expression expected", whileStatementNode.GetSpan(), whileStatementNode.ModuleId());
                }
            }
        }
    }
    else
    {
        if (dontThrow)
        {
            error = true;
        }
        else
        {
            throw Exception("Boolean expression expected", whileStatementNode.GetSpan(), whileStatementNode.ModuleId());
        }
    }
    broke = prevBroke;
    continued = prevContinued;
}

void Evaluator::Visit(DoStatementNode& doStatementNode)
{
    bool prevBroke = broke;
    bool prevContinued = continued;
    bool loop = true;
    while (loop)
    {
        doStatementNode.Statement()->Accept(*this);
        if (error || returned)
        {
            broke = prevBroke;
            continued = prevContinued;
            return;
        }
        if (broke)
        {
            break;
        }
        if (continued)
        {
            continued = false;
        }
        doStatementNode.Condition()->Accept(*this);
        if (error)
        {
            broke = prevBroke;
            continued = prevContinued;
            return;
        }
        if (value && value->GetValueType() == ValueType::boolValue)
        {
            BoolValue* condition = static_cast<BoolValue*>(value.get());
            loop = condition->GetValue();
        }
        else
        {
            if (dontThrow)
            {
                broke = prevBroke;
                continued = prevContinued;
                error = true;
                return;
            }
            else
            {
                throw Exception("Boolean expression expected", doStatementNode.GetSpan(), doStatementNode.ModuleId());
            }
        }
    }
    broke = prevBroke;
    continued = prevContinued;
}

void Evaluator::Visit(ForStatementNode& forStatementNode)
{
    bool prevBroke = broke;
    bool prevContinued = continued;
    DeclarationBlock* prevDeclarationBlock = currentDeclarationBlock;
    DeclarationBlock declarationBlock(span, moduleId, U"forBlock");
    currentDeclarationBlock = &declarationBlock;
    ContainerScope* prevContainerScope = containerScope;
    declarationBlock.GetContainerScope()->SetParentScope(containerScope);
    containerScope = declarationBlock.GetContainerScope();
    forStatementNode.InitS()->Accept(*this);
    if (error || returned)
    {
        containerScope = prevContainerScope;
        currentDeclarationBlock = prevDeclarationBlock;
        broke = prevBroke;
        continued = prevContinued;
        return;
    }
    forStatementNode.Condition()->Accept(*this);
    if (error)
    {
        containerScope = prevContainerScope;
        currentDeclarationBlock = prevDeclarationBlock;
        broke = prevBroke;
        continued = prevContinued;
        return;
    }
    if (value && value->GetValueType() == ValueType::boolValue)
    {
        BoolValue* condition = static_cast<BoolValue*>(value.get());
        bool loop = condition->GetValue();
        while (loop)
        {
            forStatementNode.ActionS()->Accept(*this);
            if (error || returned)
            {
                containerScope = prevContainerScope;
                currentDeclarationBlock = prevDeclarationBlock;
                broke = prevBroke;
                continued = prevContinued;
                return;
            }
            if (broke)
            {
                break;
            }
            if (continued)
            {
                continued = false;
            }
            forStatementNode.LoopS()->Accept(*this);
            if (error)
            {
                containerScope = prevContainerScope;
                currentDeclarationBlock = prevDeclarationBlock;
                broke = prevBroke;
                continued = prevContinued;
                return;
            }
            forStatementNode.Condition()->Accept(*this);
            if (error)
            {
                containerScope = prevContainerScope;
                currentDeclarationBlock = prevDeclarationBlock;
                broke = prevBroke;
                continued = prevContinued;
                return;
            }
            if (value && value->GetValueType() == ValueType::boolValue)
            {
                BoolValue* condition = static_cast<BoolValue*>(value.get());
                loop = condition->GetValue();
            }
            else
            {
                if (dontThrow)
                {
                    containerScope = prevContainerScope;
                    currentDeclarationBlock = prevDeclarationBlock;
                    broke = prevBroke;
                    continued = prevContinued;
                    error = true;
                    return;
                }
                else
                {
                    throw Exception("Boolean expression expected", forStatementNode.GetSpan(), forStatementNode.ModuleId());
                }
            }
        }
    }
    else
    {
        if (dontThrow)
        {
            containerScope = prevContainerScope;
            currentDeclarationBlock = prevDeclarationBlock;
            broke = prevBroke;
            continued = prevContinued;
            error = true;
            return;
        }
        else
        {
            throw Exception("Boolean expression expected", forStatementNode.GetSpan(), forStatementNode.ModuleId());
        }
    }
    containerScope = prevContainerScope;
    currentDeclarationBlock = prevDeclarationBlock;
    broke = prevBroke;
    continued = prevContinued;
}

void Evaluator::Visit(BreakStatementNode& breakStatementNode)
{
    broke = true;
}

void Evaluator::Visit(ContinueStatementNode& continueStatementNode)
{
    continued = true;
}

void Evaluator::Visit(GotoStatementNode& gotoStatementNode)
{
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, gotoStatementNode.GetSpan(), gotoStatementNode.ModuleId());
    }
}

void Evaluator::Visit(ConstructionStatementNode& constructionStatementNode)
{
    if (!currentDeclarationBlock)
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            throw Exception("internal error: current declaration block not set", constructionStatementNode.GetSpan(), constructionStatementNode.ModuleId());
        }
    }
    TypeSymbol* type = ResolveType(constructionStatementNode.TypeExpr(), boundCompileUnit, containerScope);
    std::vector<std::unique_ptr<Value>> values;
    int n = constructionStatementNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        Node* argumentNode = constructionStatementNode.Arguments()[i];
        argumentNode->Accept(*this);
        if (error) return;
        if (!value)
        {
            if (dontThrow)
            {
                error = true;
                return;
            }
            else
            {
                ThrowCannotEvaluateStatically(span, moduleId, constructionStatementNode.GetSpan(), constructionStatementNode.ModuleId());
            }
        }
        values.push_back(std::move(value));
    }
    std::vector<std::unique_ptr<BoundExpression>> arguments = ValuesToLiterals(values, symbolTable, error);
    if (error)
    {
        if (dontThrow)
        {
            return;
        }
        else
        {
            ThrowCannotEvaluateStatically(span, moduleId, constructionStatementNode.GetSpan(), constructionStatementNode.ModuleId());
        }
    }
    arguments.insert(arguments.begin(), std::unique_ptr<BoundExpression>(new BoundTypeExpression(span, moduleId, type->AddPointer(span, moduleId))));
    std::vector<FunctionScopeLookup> scopeLookups;
    scopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
    scopeLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
    std::unique_ptr<Exception> exception;
    OverloadResolutionFlags flags = OverloadResolutionFlags::dontInstantiate;
    if (dontThrow)
    {
        flags = flags | OverloadResolutionFlags::dontThrow;
    }
    std::vector<TypeSymbol*> templateArgumentTypes;
    std::unique_ptr<BoundFunctionCall> constructorCall = ResolveOverload(U"@constructor", containerScope, scopeLookups, arguments, boundCompileUnit, currentFunction, span, moduleId, 
        flags, templateArgumentTypes, exception);
    if (!constructorCall)
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            ThrowCannotEvaluateStatically(span, moduleId, constructionStatementNode.GetSpan(), constructionStatementNode.ModuleId());
        }
    }
    argumentValues = ArgumentsToValues(constructorCall->Arguments(), error, true, boundCompileUnit);
    if (error)
    {
        if (dontThrow)
        {
            return;
        }
        else
        {
            ThrowCannotEvaluateStatically(span, moduleId, constructionStatementNode.GetSpan(), constructionStatementNode.ModuleId());
        }
    }
    FunctionSymbol* constructorSymbol = constructorCall->GetFunctionSymbol();
    if (constructorSymbol->IsCompileTimePrimitiveFunction())
    {
        value = constructorSymbol->ConstructValue(argumentValues, span, moduleId, nullptr);
        if (!value)
        {
            if (dontThrow)
            {
                error = true;
                return;
            }
            else
            {
                ThrowCannotEvaluateStatically(span, moduleId, constructionStatementNode.GetSpan(), constructionStatementNode.ModuleId());
            }
        }
    }
    else if (constructorSymbol->IsConstExpr())
    {
        FunctionNode* ctorNode = boundCompileUnit.GetFunctionNodeFor(constructorSymbol);
        ctorNode->Accept(*this);
        if (!value)
        {
            if (dontThrow)
            {
                error = true;
                return;
            }
            else
            {
                ThrowCannotEvaluateStatically(span, moduleId, ctorNode->GetSpan(), ctorNode->ModuleId());
            }
        }
    }
    else
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            ThrowCannotEvaluateStatically(span, moduleId, constructionStatementNode.GetSpan(), constructionStatementNode.ModuleId());
        }
    }
    VariableValueSymbol* variableValue = new VariableValueSymbol(span, moduleId, constructionStatementNode.Id()->Str(), std::move(value));
    variableValue->SetType(type);
    currentDeclarationBlock->AddMember(variableValue);
}

void Evaluator::Visit(DeleteStatementNode& deleteStatementNode)
{
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, deleteStatementNode.GetSpan(), deleteStatementNode.ModuleId());
    }
}

void Evaluator::Visit(DestroyStatementNode& destroyStatementNode)
{
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, destroyStatementNode.GetSpan(), destroyStatementNode.ModuleId());
    }
}

void Evaluator::Visit(AssignmentStatementNode& assignmentStatementNode)
{
    bool prevLvalue = lvalue;
    lvalue = true;
    VariableValueSymbol* prevTargetValueSymbol = targetValueSymbol;
    assignmentStatementNode.TargetExpr()->Accept(*this);
    VariableValueSymbol* target = targetValueSymbol;
    targetValueSymbol = prevTargetValueSymbol;
    lvalue = prevLvalue;
    assignmentStatementNode.SourceExpr()->Accept(*this);
    std::vector<std::unique_ptr<Value>> values;
    values.push_back(std::move(value));
    std::vector<std::unique_ptr<BoundExpression>> arguments = ValuesToLiterals(values, symbolTable, error);
    if (error)
    {
        if (dontThrow)
        {
            return;
        }
        else
        {
            ThrowCannotEvaluateStatically(span, moduleId, assignmentStatementNode.GetSpan(), assignmentStatementNode.ModuleId());
        }
    }
    arguments.insert(arguments.begin(), std::unique_ptr<BoundExpression>(new BoundTypeExpression(span, moduleId, target->GetType()->AddPointer(span, moduleId))));
    std::vector<FunctionScopeLookup> scopeLookups;
    scopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
    scopeLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
    std::unique_ptr<Exception> exception;
    OverloadResolutionFlags flags = OverloadResolutionFlags::dontInstantiate;
    if (dontThrow)
    {
        flags = flags | OverloadResolutionFlags::dontThrow;
    }
    std::vector<TypeSymbol*> templateArgumentTypes;
    std::unique_ptr<BoundFunctionCall> assignmentCall = ResolveOverload(U"operator=", containerScope, scopeLookups, arguments, boundCompileUnit, currentFunction, span, moduleId, 
        flags, templateArgumentTypes, exception);
    if (!assignmentCall)
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            ThrowCannotEvaluateStatically(span, moduleId, assignmentStatementNode.GetSpan(), assignmentStatementNode.ModuleId());
        }
    }
    argumentValues = ArgumentsToValues(assignmentCall->Arguments(), error, true, boundCompileUnit);
    if (error)
    {
        if (dontThrow)
        {
            return;
        }
        else
        {
            ThrowCannotEvaluateStatically(span, moduleId, assignmentStatementNode.GetSpan(), assignmentStatementNode.ModuleId());
        }
    }
    target->SetValue(argumentValues.front().release());
}

void Evaluator::Visit(ExpressionStatementNode& expressionStatementNode)
{
    expressionStatementNode.Expression()->Accept(*this);
}

void Evaluator::Visit(EmptyStatementNode& emptyStatementNode)
{
}

void Evaluator::Visit(RangeForStatementNode& rangeForStatementNode)
{
    // todo
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, rangeForStatementNode.GetSpan(), rangeForStatementNode.ModuleId());
    }
}

void Evaluator::Visit(SwitchStatementNode& switchStatementNode)
{
    // todo
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, switchStatementNode.GetSpan(), switchStatementNode.ModuleId());
    }
}

void Evaluator::Visit(CaseStatementNode& caseStatementNode)
{
    // todo
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, caseStatementNode.GetSpan(), caseStatementNode.ModuleId());
    }
}

void Evaluator::Visit(DefaultStatementNode& defaultStatementNode)
{
    // todo
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, defaultStatementNode.GetSpan(), defaultStatementNode.ModuleId());
    }
}

void Evaluator::Visit(GotoCaseStatementNode& gotoCaseStatementNode)
{
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, gotoCaseStatementNode.GetSpan(), gotoCaseStatementNode.ModuleId());
    }
}

void Evaluator::Visit(GotoDefaultStatementNode& gotoDefaultStatementNode)
{
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, gotoDefaultStatementNode.GetSpan(), gotoDefaultStatementNode.ModuleId());
    }
}

void Evaluator::Visit(ThrowStatementNode& throwStatementNode)
{
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, throwStatementNode.GetSpan(), throwStatementNode.ModuleId());
    }
}

void Evaluator::Visit(TryStatementNode& tryStatementNode)
{
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, tryStatementNode.GetSpan(), tryStatementNode.ModuleId());
    }
}

void Evaluator::Visit(CatchNode& catchNode)
{
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, catchNode.GetSpan(), catchNode.ModuleId());
    }
}

void Evaluator::Visit(AssertStatementNode& assertStatementNode)
{
    assertStatementNode.AssertExpr()->Accept(*this);
    if (error) return;
    if (!value)
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            ThrowCannotEvaluateStatically(span, moduleId, assertStatementNode.GetSpan(), assertStatementNode.ModuleId());
        }
    }
    if (value->GetValueType() == ValueType::boolValue)
    {
        BoolValue* boolValue = static_cast<BoolValue*>(value.get());
        if (!boolValue->GetValue())
        {
            if (dontThrow)
            {
                error = true;
                return;
            }
            else
            {
                throw Exception("assertion '" + assertStatementNode.AssertExpr()->ToString() + "' failed", span, moduleId, assertStatementNode.GetSpan(), assertStatementNode.ModuleId());
            }
        }
    }
    else
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            throw Exception("assertion expression is not a Boolean-valued expression", span, moduleId, assertStatementNode.GetSpan(), assertStatementNode.ModuleId());
        }
    }
}

void Evaluator::Visit(ConditionalCompilationPartNode& conditionalCompilationPartNode)
{
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, conditionalCompilationPartNode.GetSpan(), conditionalCompilationPartNode.ModuleId());
    }
}

void Evaluator::Visit(ConditionalCompilationDisjunctionNode& conditionalCompilationDisjunctionNode)
{
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, conditionalCompilationDisjunctionNode.GetSpan(), conditionalCompilationDisjunctionNode.ModuleId());
    }
}

void Evaluator::Visit(ConditionalCompilationConjunctionNode& conditionalCompilationConjunctionNode)
{
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, conditionalCompilationConjunctionNode.GetSpan(), conditionalCompilationConjunctionNode.ModuleId());
    }
}

void Evaluator::Visit(ConditionalCompilationNotNode& conditionalCompilationNotNode)
{
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, conditionalCompilationNotNode.GetSpan(), conditionalCompilationNotNode.ModuleId());
    }
}

void Evaluator::Visit(ConditionalCompilationPrimaryNode& conditionalCompilationPrimaryNode)
{
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, conditionalCompilationPrimaryNode.GetSpan(), conditionalCompilationPrimaryNode.ModuleId());
    }
}

void Evaluator::Visit(ConditionalCompilationStatementNode& conditionalCompilationStatementNode)
{
    if (dontThrow)
    {
        error = true;
        return;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, conditionalCompilationStatementNode.GetSpan(), conditionalCompilationStatementNode.ModuleId());
    }
}

void Evaluator::Visit(BoolNode& boolNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, boolNode.GetSpan(), boolNode.ModuleId());
    }
}

void Evaluator::Visit(SByteNode& sbyteNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, sbyteNode.GetSpan(), sbyteNode.ModuleId());
    }
}

void Evaluator::Visit(ByteNode& byteNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, byteNode.GetSpan(), byteNode.ModuleId());
    }
}

void Evaluator::Visit(ShortNode& shortNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, shortNode.GetSpan(), shortNode.ModuleId());
    }
}

void Evaluator::Visit(UShortNode& ushortNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, ushortNode.GetSpan(), ushortNode.ModuleId());
    }
}

void Evaluator::Visit(IntNode& intNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, intNode.GetSpan(), intNode.ModuleId());
    }
}

void Evaluator::Visit(UIntNode& uintNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, uintNode.GetSpan(), uintNode.ModuleId());
    }
}

void Evaluator::Visit(LongNode& longNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, longNode.GetSpan(), longNode.ModuleId());
    }
}

void Evaluator::Visit(ULongNode& ulongNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, ulongNode.GetSpan(), ulongNode.ModuleId());
    }
}

void Evaluator::Visit(FloatNode& floatNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, floatNode.GetSpan(), floatNode.ModuleId());
    }
}

void Evaluator::Visit(DoubleNode& doubleNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, doubleNode.GetSpan(), doubleNode.ModuleId());
    }
}

void Evaluator::Visit(CharNode& charNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, charNode.GetSpan(), charNode.ModuleId());
    }
}

void Evaluator::Visit(WCharNode& wcharNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, wcharNode.GetSpan(), wcharNode.ModuleId());
    }
}

void Evaluator::Visit(UCharNode& ucharNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, ucharNode.GetSpan(), ucharNode.ModuleId());
    }
}

void Evaluator::Visit(VoidNode& voidNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, voidNode.GetSpan(), voidNode.ModuleId());
    }
}

void Evaluator::Visit(BooleanLiteralNode& booleanLiteralNode)
{
    value.reset(new BoolValue(booleanLiteralNode.GetSpan(), booleanLiteralNode.ModuleId(), booleanLiteralNode.Value()));
}

void Evaluator::Visit(SByteLiteralNode& sbyteLiteralNode)
{
    value.reset(new SByteValue(sbyteLiteralNode.GetSpan(), sbyteLiteralNode.ModuleId(), sbyteLiteralNode.Value()));
}

void Evaluator::Visit(ByteLiteralNode& byteLiteralNode)
{
    value.reset(new ByteValue(byteLiteralNode.GetSpan(), byteLiteralNode.ModuleId(), byteLiteralNode.Value()));
}

void Evaluator::Visit(ShortLiteralNode& shortLiteralNode)
{
    value.reset(new ShortValue(shortLiteralNode.GetSpan(), shortLiteralNode.ModuleId(), shortLiteralNode.Value()));
}

void Evaluator::Visit(UShortLiteralNode& ushortLiteralNode)
{
    value.reset(new UShortValue(ushortLiteralNode.GetSpan(), ushortLiteralNode.ModuleId(), ushortLiteralNode.Value()));
}

void Evaluator::Visit(IntLiteralNode& intLiteralNode)
{
    value.reset(new IntValue(intLiteralNode.GetSpan(), intLiteralNode.ModuleId(), intLiteralNode.Value()));
}

void Evaluator::Visit(UIntLiteralNode& uintLiteralNode)
{
    value.reset(new UIntValue(uintLiteralNode.GetSpan(), uintLiteralNode.ModuleId(), uintLiteralNode.Value()));
}

void Evaluator::Visit(LongLiteralNode& longLiteralNode)
{
    value.reset(new LongValue(longLiteralNode.GetSpan(), longLiteralNode.ModuleId(), longLiteralNode.Value()));
}

void Evaluator::Visit(ULongLiteralNode& ulongLiteralNode)
{
    value.reset(new ULongValue(ulongLiteralNode.GetSpan(), ulongLiteralNode.ModuleId(), ulongLiteralNode.Value()));
}

void Evaluator::Visit(FloatLiteralNode& floatLiteralNode)
{
    value.reset(new FloatValue(floatLiteralNode.GetSpan(), floatLiteralNode.ModuleId(), floatLiteralNode.Value()));
}

void Evaluator::Visit(DoubleLiteralNode& doubleLiteralNode)
{
    value.reset(new DoubleValue(doubleLiteralNode.GetSpan(), doubleLiteralNode.ModuleId(), doubleLiteralNode.Value()));
}

void Evaluator::Visit(CharLiteralNode& charLiteralNode)
{
    value.reset(new CharValue(charLiteralNode.GetSpan(), charLiteralNode.ModuleId(), charLiteralNode.Value()));
}

void Evaluator::Visit(WCharLiteralNode& wcharLiteralNode)
{
    value.reset(new WCharValue(wcharLiteralNode.GetSpan(), wcharLiteralNode.ModuleId(), wcharLiteralNode.Value()));
}

void Evaluator::Visit(UCharLiteralNode& ucharLiteralNode)
{
    value.reset(new UCharValue(ucharLiteralNode.GetSpan(), ucharLiteralNode.ModuleId(), ucharLiteralNode.Value()));
}

void Evaluator::Visit(StringLiteralNode& stringLiteralNode)
{
    value.reset(new StringValue(stringLiteralNode.GetSpan(), stringLiteralNode.ModuleId(), boundCompileUnit.Install(stringLiteralNode.Value()), stringLiteralNode.Value()));
}

void Evaluator::Visit(WStringLiteralNode& wstringLiteralNode)
{
    value.reset(new WStringValue(wstringLiteralNode.GetSpan(), wstringLiteralNode.ModuleId(), boundCompileUnit.Install(wstringLiteralNode.Value()), wstringLiteralNode.Value()));
}

void Evaluator::Visit(UStringLiteralNode& ustringLiteralNode)
{
    value.reset(new UStringValue(ustringLiteralNode.GetSpan(), ustringLiteralNode.ModuleId(), boundCompileUnit.Install(ustringLiteralNode.Value()), ustringLiteralNode.Value()));
}

void Evaluator::Visit(NullLiteralNode& nullLiteralNode)
{
    value.reset(new NullValue(nullLiteralNode.GetSpan(), nullLiteralNode.ModuleId(), symbolTable->GetTypeByName(U"@nullptr_type")));
}

void Evaluator::Visit(ArrayLiteralNode& arrayLiteralNode)
{
    if (targetValueType != ValueType::arrayValue)
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            throw Exception("array type expected", span, moduleId);
        }
    }
    ArrayTypeSymbol* arrayType = static_cast<ArrayTypeSymbol*>(targetType);
    TypeSymbol* elementType = arrayType->ElementType();
    std::vector<std::unique_ptr<Value>> elementValues;
    int n = arrayLiteralNode.Values().Count();
    if (arrayType->Size() != -1 && arrayType->Size() != n)
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            throw Exception("invalid length for array literal of type '" + ToUtf8(arrayType->FullName()) + "'", arrayLiteralNode.GetSpan(), arrayLiteralNode.ModuleId());
        }
    }
    for (int i = 0; i < n; ++i)
    {
        value = Evaluate(arrayLiteralNode.Values()[i], elementType, containerScope, boundCompileUnit, dontThrow, currentFunction, arrayLiteralNode.GetSpan(), arrayLiteralNode.ModuleId());
        if (error)
        {
            if (dontThrow)
            {
                return;
            }
            else
            {
                ThrowCannotEvaluateStatically(span, moduleId, arrayLiteralNode.GetSpan(), arrayLiteralNode.ModuleId());
            }
        }
        if (!value)
        {
            if (dontThrow)
            {
                error = true;
                return;
            }
            else
            {
                ThrowCannotEvaluateStatically(span, moduleId, arrayLiteralNode.GetSpan(), arrayLiteralNode.ModuleId());
            }
        }
        elementValues.push_back(std::move(value));
    }
    if (arrayType->Size() == -1)
    {
        arrayType = symbolTable->MakeArrayType(arrayType->ElementType(), n, arrayLiteralNode.GetSpan(), arrayLiteralNode.ModuleId());
    }
    value.reset(new ArrayValue(arrayLiteralNode.GetSpan(), arrayLiteralNode.ModuleId(), arrayType, std::move(elementValues)));
}

void Evaluator::Visit(StructuredLiteralNode& structuredLiteralNode)
{
    if (targetValueType != ValueType::structuredValue)
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            throw Exception("class type expected", span, moduleId);
        }
    }
    ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(targetType);
    if (!currentFunction)
    {
        if (classType->IsProject() && !classType->IsBound())
        {
            Node* node = boundCompileUnit.GetSymbolTable().GetNodeNoThrow(classType);
            if (node)
            {
                TypeBinder typeBinder(boundCompileUnit);
                typeBinder.SetContainerScope(containerScope);
                node->Accept(typeBinder);
            }
        }
    }
    if (classType->IsLiteralClassType())
    {
        std::vector<std::unique_ptr<Value>> memberValues;
        int n = structuredLiteralNode.Members().Count();
        if (classType->MemberVariables().size() != n)
        {
            if (dontThrow)
            {
                error = true;
                return;
            }
            else
            {
                throw Exception("wrong number of members variables for class literal of type '" + ToUtf8(classType->FullName()) + "'", structuredLiteralNode.GetSpan(), structuredLiteralNode.ModuleId());
            }
        }
        for (int i = 0; i < n; ++i)
        {
            TypeSymbol* memberType = classType->MemberVariables()[i]->GetType();
            value = Evaluate(structuredLiteralNode.Members()[i], memberType, containerScope, boundCompileUnit, dontThrow, currentFunction, structuredLiteralNode.GetSpan(), structuredLiteralNode.ModuleId());
            if (error)
            {
                if (dontThrow)
                {
                    return;
                }
                else
                {
                    ThrowCannotEvaluateStatically(span, moduleId, structuredLiteralNode.GetSpan(), structuredLiteralNode.ModuleId());
                }
            }
            if (!value)
            {
                if (dontThrow)
                {
                    error = true;
                    return;
                }
                else
                {
                    ThrowCannotEvaluateStatically(span, moduleId, structuredLiteralNode.GetSpan(), structuredLiteralNode.ModuleId());
                }
            }
            memberValues.push_back(std::move(value));
        }
        std::vector<std::unique_ptr<BoundExpression>> arguments = ValuesToLiterals(memberValues, symbolTable, error);
        if (error)
        {
            if (dontThrow)
            {
                return;
            }
            else
            {
                ThrowCannotEvaluateStatically(span, moduleId, structuredLiteralNode.GetSpan(), structuredLiteralNode.ModuleId());
            }
        }
        arguments.insert(arguments.begin(), std::unique_ptr<BoundExpression>(new BoundTypeExpression(span, moduleId, classType->AddPointer(span, moduleId))));
        std::vector<FunctionScopeLookup> scopeLookups;
        scopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, classType->ClassOrNsScope()));
        scopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
        scopeLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
        std::unique_ptr<Exception> exception;
        OverloadResolutionFlags flags = OverloadResolutionFlags::dontInstantiate;
        if (dontThrow)
        {
            flags = flags | OverloadResolutionFlags::dontThrow;
        }
        std::vector<TypeSymbol*> templateArgumentTypes;
        std::unique_ptr<BoundFunctionCall> constructorCall = ResolveOverload(U"@constructor", containerScope, scopeLookups, arguments, boundCompileUnit, currentFunction, span, moduleId, 
            flags, templateArgumentTypes, exception);
        if (!constructorCall)
        {
            if (dontThrow)
            {
                error = true;
                return;
            }
            else
            {
                ThrowCannotEvaluateStatically(span, moduleId, structuredLiteralNode.GetSpan(), structuredLiteralNode.ModuleId());
            }
        }
        argumentValues = ArgumentsToValues(constructorCall->Arguments(), error, true, boundCompileUnit);
        if (error)
        {
            if (dontThrow)
            {
                return;
            }
            else
            {
                ThrowCannotEvaluateStatically(span, moduleId, structuredLiteralNode.GetSpan(), structuredLiteralNode.ModuleId());
            }
        }
        FunctionSymbol* constructorSymbol = constructorCall->GetFunctionSymbol();
        if (constructorSymbol->IsConstExpr())
        {
            FunctionNode* constructorNode = boundCompileUnit.GetFunctionNodeFor(constructorSymbol);
            constructorNode->Accept(*this);
        }
        else
        {
            if (dontThrow)
            {
                error = true;
                return;
            }
            else
            {
                ThrowCannotEvaluateStatically(span, moduleId, structuredLiteralNode.GetSpan(), structuredLiteralNode.ModuleId());
            }
        }
    }
    else
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            throw Exception("class '" + ToUtf8(classType->FullName()) + "' is not a literal class ", structuredLiteralNode.GetSpan(), structuredLiteralNode.ModuleId());
        }
    }
}

void Evaluator::Visit(IdentifierNode& identifierNode)
{
    std::u32string name = identifierNode.Str();
    Symbol* symbol = containerScope->Lookup(name, ScopeLookup::this_and_base_and_parent);
    if (!symbol)
    {
        for (const std::unique_ptr<FileScope>& fileScope : boundCompileUnit.FileScopes())
        {
            symbol = fileScope->Lookup(name); 
            if (symbol) break;
        }
    }
    if (symbol)
    {
        qualifiedScope = nullptr;
        if (name.find('.') != std::u32string::npos)
        {
            qualifiedScope = symbol->Parent()->GetContainerScope();
        }
        EvaluateSymbol(symbol, identifierNode.GetSpan());
        if (error)
        {
            return;
        }
    }
    else
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            throw Exception("symbol '" + ToUtf8(name) + "' not found", identifierNode.GetSpan(), identifierNode.ModuleId());
        }
    }
}

void Evaluator::Visit(TemplateIdNode& templateIdNode)
{
    templateIdNode.Primary()->Accept(*this);
    if (error) return;
    if (value && value->IsFunctionGroupValue())
    {
        FunctionGroupValue* functionGroupValue = static_cast<FunctionGroupValue*>(value.get());
        FunctionGroupSymbol* functionGroup = functionGroupValue->FunctionGroup();
        std::vector<TypeSymbol*> templateTypeArguments;
        int n = templateIdNode.TemplateArguments().Count();
        for (int i = 0; i < n; ++i)
        {
            Node* templateArgumentNode = templateIdNode.TemplateArguments()[i];
            TypeSymbol* templateTypeArgument = ResolveType(templateArgumentNode, boundCompileUnit, containerScope);
            templateTypeArguments.push_back(templateTypeArgument);
        }
        functionGroupValue->SetTemplateTypeArguments(std::move(templateTypeArguments));
    }
    else
    {
        if (dontThrow)
        {
            error = true;
        }
        else
        {
            ThrowCannotEvaluateStatically(span, moduleId, templateIdNode.GetSpan(), templateIdNode.ModuleId());
        }
    }
}

void Evaluator::EvaluateSymbol(Symbol* symbol, const Span& span)
{
    if (symbol->GetSymbolType() == SymbolType::constantSymbol)
    {
        ConstantSymbol* constantSymbol = static_cast<ConstantSymbol*>(symbol);
        EvaluateConstantSymbol(constantSymbol, span);
    }
    else if (symbol->GetSymbolType() == SymbolType::enumConstantSymbol)
    {
        EnumConstantSymbol* enumConstantSymbol = static_cast<EnumConstantSymbol*>(symbol);
        EvaluateEnumConstantSymbol(enumConstantSymbol, span);
    }
    else if (symbol->IsContainerSymbol())
    {
        ContainerSymbol* containerSymbol = static_cast<ContainerSymbol*>(symbol);
        value.reset(new ScopedValue(span, moduleId, containerSymbol));
    }
    else if (symbol->GetSymbolType() == SymbolType::functionGroupSymbol)
    {
        FunctionGroupSymbol* functionGroup = static_cast<FunctionGroupSymbol*>(symbol);
        value.reset(new FunctionGroupValue(functionGroup, qualifiedScope));
    }
    else if (symbol->GetSymbolType() == SymbolType::variableValueSymbol)
    {
        VariableValueSymbol* variableValueSymbol = static_cast<VariableValueSymbol*>(symbol);
        if (lvalue)
        {
            targetValueSymbol = variableValueSymbol;
        }
        else
        {
            value.reset(variableValueSymbol->GetValue()->Clone());
        }
    }
    else
    {
        if (dontThrow)
        {
            error = true;
        }
        else
        {
            ThrowCannotEvaluateStatically(span, moduleId);
        }
    }
}

void Evaluator::EvaluateConstantSymbol(ConstantSymbol* constantSymbol, const Span& span)
{
    if (constantSymbol->Evaluating())
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        throw Exception("cyclic depenency detected", span, moduleId);
    }
    Value* constantValue = constantSymbol->GetValue();
    if (constantValue)
    {
        switch (constantValue->GetValueType())
        {
            case ValueType::arrayValue:
            {
                value.reset(new ArrayReferenceValue(static_cast<ArrayValue*>(constantValue)));
                break;
            }
            case ValueType::structuredValue:
            {
                value.reset(new StructuredReferenceValue(static_cast<StructuredValue*>(constantValue)));
                break;
            }
            case ValueType::stringValue: case ValueType::wstringValue: case ValueType::ustringValue:
            {
                value.reset(new StringReferenceValue(constantValue));
                break;
            }
            default:
            {
                value.reset(constantValue->Clone());
                break;
            }
        }
    }
    else
    {
        Node* node = symbolTable->GetNodeNoThrow(constantSymbol);
        if (!node)
        {
            if (dontThrow)
            {
                error = true;
                return;
            }
            throw Exception("node for constant symbol '" + ToUtf8(constantSymbol->FullName()) + "' not found from symbol table" , span, moduleId);
        }
        Assert(node->GetNodeType() == NodeType::constantNode, "constant node expected");
        ConstantNode* constantNode = static_cast<ConstantNode*>(node);
        constantSymbol->SetEvaluating();
        TypeBinder typeBinder(boundCompileUnit);
        typeBinder.SetContainerScope(containerScope);
        constantNode->Accept(typeBinder);
        constantSymbol->ResetEvaluating();
        Value* constantValue = constantSymbol->GetValue();
        Assert(constantValue, "constant value expected");
        value.reset(constantValue->Clone());
    }
}

void Evaluator::EvaluateEnumConstantSymbol(EnumConstantSymbol* enumConstantSymbol, const Span& span)
{
    if (enumConstantSymbol->Evaluating())
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        throw Exception("cyclic depenency detected", span, moduleId);
    }
    Value* enumConstantValue = enumConstantSymbol->GetValue();
    if (enumConstantValue)
    {
        value.reset(enumConstantValue->Clone());
    }
    else
    {
        Symbol* symbol = enumConstantSymbol->Parent();
        Assert(symbol->GetSymbolType() == SymbolType::enumTypeSymbol, "enum type symbol expected");
        EnumTypeSymbol* enumTypeSymbol = static_cast<EnumTypeSymbol*>(symbol);
        Node* node = boundCompileUnit.GetSymbolTable().GetNode(enumTypeSymbol);
        Assert(node->GetNodeType() == NodeType::enumTypeNode, "enum type node expected");
        EnumTypeNode* enumTypeNode = static_cast<EnumTypeNode*>(node);
        TypeBinder typeBinder(boundCompileUnit);
        typeBinder.SetContainerScope(containerScope);
        enumTypeNode->Accept(typeBinder);
        enumConstantSymbol->ResetEvaluating();
        Value* enumConstantValue = enumConstantSymbol->GetValue();
        Assert(enumConstantValue, "enum constant value expected");
        value.reset(enumConstantValue->Clone());
    }
}

void Evaluator::Visit(DotNode& dotNode)
{
    dotNode.Subject()->Accept(*this);
    if (error)
    {
        return;
    }
    if (value)
    {
        if (value->IsArrayReferenceValue())
        {
            TypeSymbol* type = static_cast<ArrayReferenceValue*>(value.get())->GetArrayValue()->GetType(symbolTable);
            ScopedValue* scopedValue = new ScopedValue(span, moduleId, type);
            scopedValue->SetType(type);
            value.reset(scopedValue);
        }
        else if (value->IsStructuredReferenceValue())
        {
            TypeSymbol* type = static_cast<StructuredReferenceValue*>(value.get())->GetStructuredValue()->GetType(symbolTable);
            ScopedValue* scopedValue = new ScopedValue(span, moduleId, type);
            scopedValue->SetType(type->AddPointer(span, moduleId));
            scopedValue->SetSubject(value.release());
            value.reset(scopedValue);
        }
        else if (value->IsStringReferenceValue())
        {
            TypeSymbol* type = symbolTable->GetTypeByName(U"@string_functions");
            ScopedValue* scopedValue = new ScopedValue(span, moduleId, type);
            scopedValue->SetType(type);
            scopedValue->SetSubject(value.release());
            value.reset(scopedValue);
        }
        else if (value->GetValueType() == ValueType::structuredValue)
        {
            TypeSymbol* type = static_cast<StructuredValue*>(value.get())->GetType(symbolTable);
            ScopedValue* scopedValue = new ScopedValue(span, moduleId, type);
            scopedValue->SetType(type);
            value.reset(scopedValue);
        }
    }
    if (value && value->IsScopedValue())
    {
        ScopedValue* scopedValue = static_cast<ScopedValue*>(value.get());
        ContainerSymbol* containerSymbol = scopedValue->GetContainerSymbol();
        if (containerSymbol->GetSymbolType() == SymbolType::classGroupTypeSymbol)
        {
            ClassGroupTypeSymbol* classGroupTypeSymbol = static_cast<ClassGroupTypeSymbol*>(containerSymbol);
            containerSymbol = classGroupTypeSymbol->GetClass(0);
        }
        ContainerScope* scope = containerSymbol->GetContainerScope();
        qualifiedScope = scope;
        std::u32string memberName = dotNode.MemberId()->Str();
        Symbol* symbol = scope->Lookup(memberName);
        if (symbol)
        {
            std::unique_ptr<Value> receiver;
            TypeSymbol* type = scopedValue->GetType(symbolTable);
            if (type && (type->IsArrayType() || type->BaseType()->IsClassTypeSymbol() || type->IsStringFunctionContainer()))
            {
                receiver = std::move(value);
            }
            EvaluateSymbol(symbol, dotNode.GetSpan());
            if (error) return;
            if (receiver && value->IsFunctionGroupValue())
            {
                FunctionGroupValue* functionGroupValue = static_cast<FunctionGroupValue*>(value.get());
                functionGroupValue->SetReceiver(std::move(receiver));
            }
        }
        else
        {
            if (dontThrow)
            {
                error = true;
                return;
            }
            else
            {
                throw Exception("symbol '" + ToUtf8(containerSymbol->FullName()) + "' does not have member '" + ToUtf8(memberName) + "'", dotNode.GetSpan(), dotNode.ModuleId());
            }
        }
    }
    else
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            throw Exception("expression '" + dotNode.Subject()->ToString() + "' must denote a namespace, class type or enumerated type", dotNode.Subject()->GetSpan(), dotNode.Subject()->ModuleId());
        }
    }
}

void Evaluator::Visit(ArrowNode& arrowNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, arrowNode.GetSpan(), arrowNode.ModuleId());
    }
}

void Evaluator::Visit(EquivalenceNode& equivalenceNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, equivalenceNode.GetSpan(), equivalenceNode.ModuleId());
    }
}

void Evaluator::Visit(ImplicationNode& implicationNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, implicationNode.GetSpan(), implicationNode.ModuleId());
    }
}

void Evaluator::Visit(DisjunctionNode& disjunctionNode)
{
    EvaluateBinOp(disjunctionNode, disjunction);
}

void Evaluator::Visit(ConjunctionNode& conjunctionNode)
{
    EvaluateBinOp(conjunctionNode, conjunction);
}

void Evaluator::Visit(BitOrNode& bitOrNode)
{
    EvaluateBinOp(bitOrNode, bitOr);
}

void Evaluator::Visit(BitXorNode& bitXorNode)
{
    EvaluateBinOp(bitXorNode, bitXor);
}

void Evaluator::Visit(BitAndNode& bitAndNode) 
{
    EvaluateBinOp(bitAndNode, bitAnd);
}

void Evaluator::Visit(EqualNode& equalNode)
{
    EvaluateBinOp(equalNode, equal, Operator::comparison);
}

void Evaluator::Visit(NotEqualNode& notEqualNode)
{
    EvaluateBinOp(notEqualNode, notEqual, Operator::comparison);
}

void Evaluator::Visit(LessNode& lessNode)
{
    EvaluateBinOp(lessNode, less, Operator::comparison);
}

void Evaluator::Visit(GreaterNode& greaterNode)
{
    EvaluateBinOp(greaterNode, greater, Operator::comparison);
}

void Evaluator::Visit(LessOrEqualNode& lessOrEqualNode)
{
    EvaluateBinOp(lessOrEqualNode, lessEqual, Operator::comparison);
}

void Evaluator::Visit(GreaterOrEqualNode& greaterOrEqualNode)
{
    EvaluateBinOp(greaterOrEqualNode, greaterEqual, Operator::comparison);
}

void Evaluator::Visit(ShiftLeftNode& shiftLeftNode)
{
    EvaluateBinOp(shiftLeftNode, shiftLeft);
}

void Evaluator::Visit(ShiftRightNode& shiftRightNode)
{
    EvaluateBinOp(shiftRightNode, shiftRight);
}

void Evaluator::Visit(AddNode& addNode)
{
    EvaluateBinOp(addNode, add, Operator::add);
}

void Evaluator::Visit(SubNode& subNode)
{
    EvaluateBinOp(subNode, sub, Operator::sub);
}

void Evaluator::Visit(MulNode& mulNode)
{
    EvaluateBinOp(mulNode, mul);
}

void Evaluator::Visit(DivNode& divNode)
{
    EvaluateBinOp(divNode, div);
}

void Evaluator::Visit(RemNode& remNode)
{
    EvaluateBinOp(remNode, rem);
}

void Evaluator::Visit(NotNode& notNode)
{
    EvaluateUnaryOp(notNode, logicalNot);
}

void Evaluator::Visit(UnaryPlusNode& unaryPlusNode)
{
    EvaluateUnaryOp(unaryPlusNode, unaryPlus);
}

void Evaluator::Visit(UnaryMinusNode& unaryMinusNode)
{
    EvaluateUnaryOp(unaryMinusNode, unaryMinus);
}

void Evaluator::Visit(PrefixIncrementNode& prefixIncrementNode)
{
    prefixIncrementNode.Subject()->Accept(*this);
    if (error)
    {
        return; 
    }
    if (!value)
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            ThrowCannotEvaluateStatically(span, moduleId, prefixIncrementNode.GetSpan(), prefixIncrementNode.ModuleId());
        }
    }
    bool unsignedType = value->GetType(symbolTable)->IsUnsignedType();
    CloneContext cloneContext;
    if (unsignedType)
    {
        AssignmentStatementNode assignmentStatementNode(prefixIncrementNode.GetSpan(), prefixIncrementNode.ModuleId(), prefixIncrementNode.Subject()->Clone(cloneContext),
            new AddNode(prefixIncrementNode.GetSpan(), prefixIncrementNode.ModuleId(), prefixIncrementNode.Subject()->Clone(cloneContext), 
                new ByteLiteralNode(prefixIncrementNode.GetSpan(), prefixIncrementNode.ModuleId(), 1)));
        assignmentStatementNode.Accept(*this);
    }
    else
    {
        AssignmentStatementNode assignmentStatementNode(prefixIncrementNode.GetSpan(), prefixIncrementNode.ModuleId(), prefixIncrementNode.Subject()->Clone(cloneContext),
            new AddNode(prefixIncrementNode.GetSpan(), prefixIncrementNode.ModuleId(), prefixIncrementNode.Subject()->Clone(cloneContext), 
                new SByteLiteralNode(prefixIncrementNode.GetSpan(), prefixIncrementNode.ModuleId(), 1)));
        assignmentStatementNode.Accept(*this);
    }
    prefixIncrementNode.Subject()->Accept(*this);
    if (error)
    {
        return;
    }
    if (!value)
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            ThrowCannotEvaluateStatically(span, moduleId, prefixIncrementNode.GetSpan(), prefixIncrementNode.ModuleId());
        }
    }
}

void Evaluator::Visit(PrefixDecrementNode& prefixDecrementNode)
{
    prefixDecrementNode.Subject()->Accept(*this);
    if (error)
    {
        return;
    }
    if (!value)
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            ThrowCannotEvaluateStatically(span, moduleId, prefixDecrementNode.GetSpan(), prefixDecrementNode.ModuleId());
        }
    }
    bool unsignedType = value->GetType(symbolTable)->IsUnsignedType();
    CloneContext cloneContext;
    if (unsignedType)
    {
        AssignmentStatementNode assignmentStatementNode(prefixDecrementNode.GetSpan(), prefixDecrementNode.ModuleId(), prefixDecrementNode.Subject()->Clone(cloneContext),
            new SubNode(prefixDecrementNode.GetSpan(), prefixDecrementNode.ModuleId(), prefixDecrementNode.Subject()->Clone(cloneContext), 
                new ByteLiteralNode(prefixDecrementNode.GetSpan(), prefixDecrementNode.ModuleId(), 1)));
        assignmentStatementNode.Accept(*this);
    }
    else
    {
        AssignmentStatementNode assignmentStatementNode(prefixDecrementNode.GetSpan(), prefixDecrementNode.ModuleId(), prefixDecrementNode.Subject()->Clone(cloneContext),
            new SubNode(prefixDecrementNode.GetSpan(), prefixDecrementNode.ModuleId(), prefixDecrementNode.Subject()->Clone(cloneContext), 
                new SByteLiteralNode(prefixDecrementNode.GetSpan(), prefixDecrementNode.ModuleId(), 1)));
        assignmentStatementNode.Accept(*this);
    }
    prefixDecrementNode.Subject()->Accept(*this);
    if (!value)
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            ThrowCannotEvaluateStatically(span, moduleId, prefixDecrementNode.GetSpan(), prefixDecrementNode.ModuleId());
        }
    }
}

void Evaluator::Visit(DerefNode& derefNode)
{
    derefNode.Subject()->Accept(*this);
    if (value && value->GetValueType() == ValueType::pointerValue)
    {
        PointerValue* pointerValue = static_cast<PointerValue*>(value.get());
        value.reset(pointerValue->Deref());
        if (!value)
        {
            if (dontThrow)
            {
                error = true;
            }
            else
            {
                throw Exception("unsupported pointer value", derefNode.GetSpan(), derefNode.ModuleId());
            }
        }
    }
    else
    {
        if (dontThrow)
        {
            error = true;
        }
        else
        {
            throw Exception("pointer value expected", derefNode.GetSpan(), derefNode.ModuleId());
        }
    }
}

void Evaluator::Visit(AddrOfNode& addrOfNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, addrOfNode.GetSpan(), addrOfNode.ModuleId());
    }
}

void Evaluator::Visit(ComplementNode& complementNode)
{
    EvaluateUnaryOp(complementNode, complement);
}

void Evaluator::Visit(IsNode& isNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, isNode.GetSpan(), isNode.ModuleId());
    }
}

void Evaluator::Visit(AsNode& asNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, asNode.GetSpan(), asNode.ModuleId());
    }
}

void Evaluator::Visit(IndexingNode& indexingNode)
{
    indexingNode.Subject()->Accept(*this);
    if (value && value->IsArrayReferenceValue())
    {
        ArrayValue* arrayValue = static_cast<ArrayReferenceValue*>(value.get())->GetArrayValue();
        value = Evaluate(indexingNode.Index(), symbolTable->GetTypeByName(U"long"), containerScope, boundCompileUnit, dontThrow, currentFunction, indexingNode.GetSpan(), indexingNode.ModuleId());
        if (!value)
        {
            if (dontThrow)
            {
                error = true;
                return;
            }
            else
            {
                ThrowCannotEvaluateStatically(span, moduleId, indexingNode.GetSpan(), indexingNode.ModuleId());
            }
        }
        LongValue* indexValue = static_cast<LongValue*>(value.get());
        int64_t index = indexValue->GetValue();
        if (index < 0 || index >= int64_t(arrayValue->Elements().size()))
        {
            if (dontThrow)
            {
                error = true;
                return;
            }
            else
            {
                throw Exception("array index out of range", indexingNode.GetSpan(), indexingNode.ModuleId());
            }
        }
        Value* elementValue = arrayValue->Elements()[index].get();
        switch (elementValue->GetValueType())
        {
            case ValueType::arrayValue:
            {
                value.reset(new ArrayReferenceValue(static_cast<ArrayValue*>(elementValue)));
                break;
            }
            case ValueType::stringValue: case ValueType::wstringValue: case ValueType::ustringValue:
            {
                value.reset(new StringReferenceValue(elementValue));
                break;
            }
            default:
            {
                value = std::unique_ptr<Value>(elementValue->Clone());
                break;
            }
        }
    }
    else
    {
        if (dontThrow)
        {
            error = true;
        }
        else
        {
            ThrowCannotEvaluateStatically(span, moduleId, indexingNode.GetSpan(), indexingNode.ModuleId());
        }
    }
}

void Evaluator::Visit(InvokeNode& invokeNode)
{
    if (error) return;
    std::vector<std::unique_ptr<Value>> values;
    int n = invokeNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        Node* arg = invokeNode.Arguments()[i];
        arg->Accept(*this);
        if (error) return;
        if (!value)
        {
            if (dontThrow)
            {
                error = true;
                return;
            }
            else
            {
                ThrowCannotEvaluateStatically(span, moduleId, invokeNode.GetSpan(), invokeNode.ModuleId());
            }
        }
        values.push_back(std::move(value));
    }
    invokeNode.Subject()->Accept(*this);
    if (error) return;
    if (value && value->IsFunctionGroupValue())
    {
        FunctionGroupValue* functionGroupValue = static_cast<FunctionGroupValue*>(value.get());
        FunctionGroupSymbol* functionGroup = functionGroupValue->FunctionGroup();
        std::vector<FunctionScopeLookup> functionScopeLookups;
        if (functionGroupValue->QualifiedScope())
        {
            FunctionScopeLookup qualifiedScopeLookup(ScopeLookup::this_and_base, functionGroupValue->QualifiedScope());
            functionScopeLookups.push_back(qualifiedScopeLookup);
        }
        else
        {
            functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
        }
        std::vector<std::unique_ptr<BoundExpression>> arguments = ValuesToLiterals(values, symbolTable, error);
        if (error)
        {
            if (dontThrow)
            {
                return;
            }
            else
            {
                ThrowCannotEvaluateStatically(span, moduleId, invokeNode.GetSpan(), invokeNode.ModuleId());
            }
        }
        if (functionGroupValue->Receiver() && functionGroupValue->Receiver()->IsScopedValue())
        {
            TypeSymbol* type = static_cast<ScopedValue*>(functionGroupValue->Receiver())->GetType(symbolTable);
            if (type)
            {
                arguments.insert(arguments.begin(), std::unique_ptr<BoundExpression>(new BoundTypeExpression(span, moduleId, type)));
            }
        }
        templateTypeArguments = std::move(functionGroupValue->TemplateTypeArguments());
        std::unique_ptr<Exception> exception;
        OverloadResolutionFlags flags = OverloadResolutionFlags::dontInstantiate;
        flags = flags | OverloadResolutionFlags::dontThrow;
        std::unique_ptr<BoundFunctionCall> functionCall = ResolveOverload(functionGroup->Name(), containerScope, functionScopeLookups, arguments, boundCompileUnit, currentFunction, span, moduleId, 
            flags, templateTypeArguments, exception);
        bool memberFunctionCall = false;
        if (!functionCall)
        {
            if (currentClassType)
            {
                arguments.insert(arguments.begin(), std::unique_ptr<BoundExpression>(new BoundTypeExpression(span, moduleId, currentClassType->AddPointer(span, moduleId))));
                functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, currentClassType->GetContainerScope()));
                OverloadResolutionFlags flags = OverloadResolutionFlags::dontInstantiate;
                if (dontThrow)
                {
                    flags = flags | OverloadResolutionFlags::dontThrow;
                }
                std::unique_ptr<Exception> exception;
                functionCall = ResolveOverload(functionGroup->Name(), containerScope, functionScopeLookups, arguments, boundCompileUnit, currentFunction, span, moduleId, 
                    flags, templateTypeArguments, exception);
                if (functionCall)
                {
                    memberFunctionCall = true;
                }
            }
            if (!functionCall)
            {
                if (dontThrow)
                {
                    error = true;
                    return;
                }
                else
                {
                    ThrowCannotEvaluateStatically(span, moduleId, invokeNode.GetSpan(), invokeNode.ModuleId());
                }
            }
        }
        FunctionSymbol* functionSymbol = functionCall->GetFunctionSymbol();
        if (functionSymbol->IsCompileTimePrimitiveFunction())
        {
            Value* receiver = functionGroupValue->Receiver();
            bool skipFirst = receiver != nullptr;;
            argumentValues = ArgumentsToValues(functionCall->Arguments(), error, skipFirst, boundCompileUnit);
            if (error)
            {
                if (dontThrow)
                {
                    return;
                }
                else
                {
                    ThrowCannotEvaluateStatically(span, moduleId, invokeNode.GetSpan(), invokeNode.ModuleId());
                }
            }
            value = functionSymbol->ConstructValue(argumentValues, invokeNode.GetSpan(), invokeNode.ModuleId(), receiver);
            if (!value)
            {
                if (dontThrow)
                {
                    error = true;
                    return;
                }
                else
                {
                    ThrowCannotEvaluateStatically(span, moduleId, invokeNode.GetSpan(), invokeNode.ModuleId());
                }
            }
        }
        else if (functionSymbol->IsConstExpr())
        {
            FunctionNode* functionNode = boundCompileUnit.GetFunctionNodeFor(functionSymbol);
            CheckFunctionReturnPaths(functionSymbol, *functionNode, containerScope, boundCompileUnit);
            bool skipFirst = memberFunctionCall || functionGroupValue->Receiver();
            argumentValues = ArgumentsToValues(functionCall->Arguments(), error, skipFirst, boundCompileUnit);
            if (error)
            {
                if (dontThrow)
                {
                    return;
                }
                else
                {
                    ThrowCannotEvaluateStatically(span, moduleId, invokeNode.GetSpan(), invokeNode.ModuleId());
                }
            }
            ClassTypeSymbol* prevClassType = currentClassType;
            if (functionGroupValue->Receiver() && functionGroupValue->Receiver()->IsScopedValue())
            {
                ScopedValue* receiver = static_cast<ScopedValue*>(functionGroupValue->Receiver());
                if (receiver->GetSubject() && receiver->GetSubject()->GetType(symbolTable)->IsClassTypeSymbol())
                {
                    currentClassType = static_cast<ClassTypeSymbol*>(receiver->GetSubject()->GetType(symbolTable));
                    structureReferenceValue = std::unique_ptr<Value>(receiver->GetSubject()->Clone());
                }
            }
            functionNode->Accept(*this);
            currentClassType = prevClassType;
        }
        else 
        {
            IntrinsicFunction* intrinsic = functionSymbol->GetIntrinsic();
            if (intrinsic)
            {
                argumentValues = ArgumentsToValues(functionCall->Arguments(), error, boundCompileUnit);
                if (error)
                {
                    if (dontThrow)
                    {
                        return;
                    }
                    else
                    {
                        ThrowCannotEvaluateStatically(span, moduleId, invokeNode.GetSpan(), invokeNode.ModuleId());
                    }
                }
                value = intrinsic->Evaluate(argumentValues, templateTypeArguments, invokeNode.GetSpan(), invokeNode.ModuleId());
                if (!value)
                {
                    if (dontThrow)
                    {
                        error = true;
                        return;
                    }
                    else
                    {
                        ThrowCannotEvaluateStatically(span, moduleId, invokeNode.GetSpan(), invokeNode.ModuleId());
                    }
                }
            }
            else
            {
                if (dontThrow)
                {
                    error = true;
                    return;
                }
                else
                {
                    ThrowCannotEvaluateStatically(span, moduleId, invokeNode.GetSpan(), invokeNode.ModuleId());
                }
            }
        }
    }
    else
    {
        if (dontThrow)
        {
            error = true;
        }
        else
        {
            throw Exception("function group expected", invokeNode.GetSpan(), invokeNode.ModuleId());
        }
    }
}

void Evaluator::Visit(PostfixIncrementNode& postfixIncrementNode)
{
    postfixIncrementNode.Subject()->Accept(*this);
    if (error) return;
    if (!value)
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            ThrowCannotEvaluateStatically(span, moduleId, postfixIncrementNode.GetSpan(), postfixIncrementNode.ModuleId());
        }
    }
    bool unsignedType = value->GetType(symbolTable)->IsUnsignedType();
    std::unique_ptr<Value> result = std::move(value);
    CloneContext cloneContext;
    if (unsignedType)
    {
        AssignmentStatementNode assignmentStatementNode(postfixIncrementNode.GetSpan(), postfixIncrementNode.ModuleId(), postfixIncrementNode.Subject()->Clone(cloneContext),
            new AddNode(postfixIncrementNode.GetSpan(), postfixIncrementNode.ModuleId(), postfixIncrementNode.Subject()->Clone(cloneContext), new ByteLiteralNode(postfixIncrementNode.GetSpan(), postfixIncrementNode.ModuleId(), 1)));
        assignmentStatementNode.Accept(*this);
    }
    else
    {
        AssignmentStatementNode assignmentStatementNode(postfixIncrementNode.GetSpan(), postfixIncrementNode.ModuleId(), postfixIncrementNode.Subject()->Clone(cloneContext),
            new AddNode(postfixIncrementNode.GetSpan(), postfixIncrementNode.ModuleId(), postfixIncrementNode.Subject()->Clone(cloneContext), new SByteLiteralNode(postfixIncrementNode.GetSpan(), postfixIncrementNode.ModuleId(), 1)));
        assignmentStatementNode.Accept(*this);
    }
    value = std::move(result);
}

void Evaluator::Visit(PostfixDecrementNode& postfixDecrementNode)
{
    postfixDecrementNode.Subject()->Accept(*this);
    if (error) return;
    if (!value)
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            ThrowCannotEvaluateStatically(span, moduleId, postfixDecrementNode.GetSpan(), postfixDecrementNode.ModuleId());
        }
    }
    bool unsignedType = value->GetType(symbolTable)->IsUnsignedType();
    std::unique_ptr<Value> result = std::move(value);
    CloneContext cloneContext;
    if (unsignedType)
    {
        AssignmentStatementNode assignmentStatementNode(postfixDecrementNode.GetSpan(), postfixDecrementNode.ModuleId(), postfixDecrementNode.Subject()->Clone(cloneContext),
            new SubNode(postfixDecrementNode.GetSpan(), postfixDecrementNode.ModuleId(), postfixDecrementNode.Subject()->Clone(cloneContext), 
                new ByteLiteralNode(postfixDecrementNode.GetSpan(), postfixDecrementNode.ModuleId(), 1)));
        assignmentStatementNode.Accept(*this);
    }
    else
    {
        AssignmentStatementNode assignmentStatementNode(postfixDecrementNode.GetSpan(), postfixDecrementNode.ModuleId(), postfixDecrementNode.Subject()->Clone(cloneContext),
            new SubNode(postfixDecrementNode.GetSpan(), postfixDecrementNode.ModuleId(), postfixDecrementNode.Subject()->Clone(cloneContext), 
                new SByteLiteralNode(postfixDecrementNode.GetSpan(), postfixDecrementNode.ModuleId(), 1)));
        assignmentStatementNode.Accept(*this);
    }
    value = std::move(result);
}

void Evaluator::Visit(SizeOfNode& sizeOfNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, sizeOfNode.GetSpan(), sizeOfNode.ModuleId());
    }
}

void Evaluator::Visit(TypeNameNode& typeNameNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, typeNameNode.GetSpan(), typeNameNode.ModuleId());
    }
}

void Evaluator::Visit(TypeIdNode& typeIdNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, typeIdNode.GetSpan(), typeIdNode.ModuleId());
    }
}

void Evaluator::Visit(CastNode& castNode)
{
    TypeSymbol* type = ResolveType(castNode.TargetTypeExpr(), boundCompileUnit, containerScope);
    bool prevCast = cast;
    cast = true;
    castNode.SourceExpr()->Accept(*this);
    if (error) return;
    if (!value)
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            ThrowCannotEvaluateStatically(span, moduleId, castNode.GetSpan(), castNode.ModuleId());
        }
    }
    value.reset(value->As(type, true, castNode.GetSpan(), castNode.ModuleId(), dontThrow));
    cast = prevCast;
}

void Evaluator::Visit(ConstructNode& constructNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, constructNode.GetSpan(), constructNode.ModuleId());
    }
}

void Evaluator::Visit(NewNode& newNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, newNode.GetSpan(), newNode.ModuleId());
    }
}

void Evaluator::Visit(ThisNode& thisNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, thisNode.GetSpan(), thisNode.ModuleId());
    }
}

void Evaluator::Visit(BaseNode& baseNode)
{
    if (dontThrow)
    {
        error = true;
    }
    else
    {
        ThrowCannotEvaluateStatically(span, moduleId, baseNode.GetSpan(), baseNode.ModuleId());
    }
}

void Evaluator::Visit(ParenthesizedExpressionNode& parenthesizedExpressionNode)
{
    parenthesizedExpressionNode.Subject()->Accept(*this);
}

std::unique_ptr<Value> Evaluate(Node* node, TypeSymbol* targetType, ContainerScope* containerScope, BoundCompileUnit& boundCompileUnit, bool dontThrow, BoundFunction* currentFunction, const Span& span,
    const boost::uuids::uuid& moduleId)
{
    ValueType targetValueType = targetType->GetValueType();
    Evaluator evaluator(boundCompileUnit, containerScope, targetType, targetValueType, false, dontThrow, currentFunction, span, moduleId);
    node->Accept(evaluator);
    if (evaluator.Error())
    {
        return std::unique_ptr<Value>();
    }
    else
    {
        std::unique_ptr<Value> value = evaluator.GetValue();
        if (value && value->IsComplete())
        {
            if (!TypesEqual(targetType->PlainType(span, moduleId), value->GetType(&boundCompileUnit.GetSymbolTable())))
            {
                if (targetType->IsArrayType() && static_cast<ArrayTypeSymbol*>(targetType)->Size() == -1)
                {
                    return std::move(value);
                }
                if (value->IsStringReferenceValue())
                {
                    return std::move(value);
                }
                value.reset(value->As(targetType->PlainType(span, moduleId), false, node->GetSpan(), node->ModuleId(), dontThrow));
            }
            return std::move(value);
        }
        else
        {
            if (dontThrow)
            {
                return std::unique_ptr<Value>();
            }
            else
            {
                throw Exception("value not complete", node->GetSpan(), node->ModuleId());
            }
        }
    }
}

} } // namespace cmajor::binder
