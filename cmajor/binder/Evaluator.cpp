// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/Evaluator.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/binder/TypeBinder.hpp>
#include <cmajor/binder/TypeResolver.hpp>
#include <cmajor/binder/OverloadResolution.hpp>
#include <cmajor/binder/StatementBinder.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/ast/Visitor.hpp>
#include <cmajor/ast/BasicType.hpp>
#include <cmajor/ast/Literal.hpp>
#include <cmajor/ast/Expression.hpp>
#include <cmajor/ast/Identifier.hpp>
#include <cmajor/util/Unicode.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::unicode;

void ThrowCannotEvaluateStatically(Module* module, const Span& defined)
{
    throw Exception(module, "cannot evaluate statically", defined);
}

void ThrowCannotEvaluateStatically(Module* module, const Span& defined, const Span& referenced)
{
    throw Exception(module, "cannot evaluate statically", defined, referenced);
}

typedef Value* (*BinaryOperatorFun)(Module* module, Value* left, Value* right, const Span& span, bool dontThrow);
typedef Value* (*UnaryOperatorFun)(Module* module, Value* operand, const Span& span, bool dontThrow);

class ScopedValue : public Value
{
public:
    ScopedValue(const Span& span_, ContainerSymbol* containerSymbol_);
    bool IsComplete() const override { return false; }
    bool IsScopedValue() const { return true; }
    const ContainerSymbol* GetContainerSymbol() const { return containerSymbol; }
    ContainerSymbol* GetContainerSymbol() { return containerSymbol; }
    Value* Clone() const override { Assert(false, "scoped value cannot be cloned"); return nullptr; }
    void Write(BinaryWriter& writer) override {}
    void Read(BinaryReader& reader) override {}
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override { Assert(false, "scoped value cannot be converted"); return nullptr; }
    void* IrValue(Emitter& emitter) override { Assert(false, "scoped value does not have ir value"); return nullptr; }
    TypeSymbol* GetType(SymbolTable* symbolTable) override { return type; }
    void SetType(TypeSymbol* type_) { type = type_; }
    Value* Subject() { return subject.get(); }
    void SetSubject(Value* subject_) { subject.reset(subject_); }
private:
    ContainerSymbol* containerSymbol;
    TypeSymbol* type;
    std::unique_ptr<Value> subject;
};

ScopedValue::ScopedValue(const Span& span_, ContainerSymbol* containerSymbol_) : Value(span_, ValueType::none), containerSymbol(containerSymbol_), type(nullptr)
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
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override { Assert(false, "function group value cannot be converted"); return nullptr; }
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

FunctionGroupValue::FunctionGroupValue(FunctionGroupSymbol* functionGroup_, ContainerScope* qualifiedScope_) : Value(Span(), ValueType::none), functionGroup(functionGroup_), qualifiedScope(qualifiedScope_)
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
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override { Assert(false, "array reference value cannot be converted"); return nullptr; }
    void* IrValue(Emitter& emitter) override { Assert(false, "array reference does not have ir value"); return nullptr; }
    TypeSymbol* GetType(SymbolTable* symbolTable) override { return arrayValue->GetType(symbolTable); }
    ArrayValue* GetArrayValue() const { return arrayValue; }
private:
    ArrayValue* arrayValue;
};

ArrayReferenceValue::ArrayReferenceValue(ArrayValue* arrayValue_) : Value(arrayValue_->GetSpan(), ValueType::none), arrayValue(arrayValue_)
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
    Value* As(TypeSymbol* targetType, bool cast, const Span& span, bool dontThrow) const override { Assert(false, "structured reference value cannot be converted"); return nullptr; }
    void* IrValue(Emitter& emitter) override { Assert(false, "structured reference does not have ir value"); return nullptr; }
    TypeSymbol* GetType(SymbolTable* symbolTable) override { return structuredValue->GetType(symbolTable); }
    StructuredValue* GetStructuredValue() const { return structuredValue; }
private:
    StructuredValue* structuredValue;
};

StructuredReferenceValue::StructuredReferenceValue(StructuredValue* structuredValue_) : Value(structuredValue_->GetSpan(), ValueType::none), structuredValue(structuredValue_)
{
}

class VariableValueSymbol : public VariableSymbol
{
public:
    VariableValueSymbol(const Span& span_, const std::u32string& name_, std::unique_ptr<Value>&& value_);
    Value* GetValue() { return value.get(); }
    void SetValue(Value* value_) { value.reset(value_); }
    const char* ClassName() const override { return "VariableValueSymbol"; }
private:
    std::unique_ptr<Value> value;
};

VariableValueSymbol::VariableValueSymbol(const Span& span_, const std::u32string& name_, std::unique_ptr<Value>&& value_) : VariableSymbol(SymbolType::variableValueSymbol, span_, name_), value(std::move(value_))
{
}

std::vector<std::unique_ptr<BoundExpression>> ValuesToLiterals(std::vector<std::unique_ptr<Value>>& values, SymbolTable* symbolTable, bool& error)
{
    std::vector<std::unique_ptr<BoundExpression>> arguments;
    for (std::unique_ptr<Value>& value : values)
    {
        ValueType valueType = value->GetValueType();
        TypeSymbol* type = value->GetType(symbolTable);
        BoundLiteral* literal = new BoundLiteral(symbolTable->GetModule(), std::move(value), type);
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
Value* BinaryEvaluate(Value* left, Value* right, Op op, const Span& span)
{
    ValueT* leftCasted = static_cast<ValueT*>(left);
    ValueT* rightCasted = static_cast<ValueT*>(right);
    return new ValueT(span, op(leftCasted->GetValue(), rightCasted->GetValue()));
}

template <typename ValueT, typename Op>
Value* BinaryPredEvaluate(Value* left, Value* right, Op op, const Span& span)
{
    ValueT* leftCasted = static_cast<ValueT*>(left);
    ValueT* rightCasted = static_cast<ValueT*>(right);
    return new BoolValue(span, op(leftCasted->GetValue(), rightCasted->GetValue()));
}

template<typename ValueT, typename Op>
Value* UnaryEvaluate(Value* subject, Op op, const Span& span)
{
    ValueT* subjectCasted = static_cast<ValueT*>(subject);
    return new ValueT(span, op(subjectCasted->GetValue()));
}

Value* NotSupported(Module* module, Value* subject, const Span& span, bool dontThrow)
{
    if (dontThrow)
    {
        return nullptr;
    }
    throw Exception(module, "operation not supported for type " + ValueTypeStr(subject->GetValueType()), span);
}

Value* NotSupported(Module* module, Value* left, Value* right, const Span& span, bool dontThrow)
{
    if (dontThrow)
    {
        return nullptr;
    }
    throw Exception(module, "operation not supported for types " + ValueTypeStr(left->GetValueType()) + " and " + ValueTypeStr(right->GetValueType()), span);
}

template<typename ValueT>
Value* Disjunction(Module* module, Value* left, Value* right, const Span& span, bool dontThrow)
{
    return BinaryEvaluate<ValueT>(left, right, std::logical_or<typename ValueT::OperandType>(), span);
}

BinaryOperatorFun disjunction[uint8_t(ValueType::maxValue)] =
{
    NotSupported, Disjunction<BoolValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Value* Conjunction(Module* module, Value* left, Value* right, const Span& span, bool dontThrow)
{
    return BinaryEvaluate<ValueT>(left, right, std::logical_and<typename ValueT::OperandType>(), span);
}

BinaryOperatorFun conjunction[uint8_t(ValueType::maxValue)] =
{
    NotSupported, Conjunction<BoolValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported,
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Value* BitOr(Module* module, Value* left, Value* right, const Span& span, bool dontThrow)
{
    return BinaryEvaluate<ValueT>(left, right, std::bit_or<typename ValueT::OperandType>(), span);
}

BinaryOperatorFun bitOr[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, BitOr<SByteValue>, BitOr<ByteValue>, BitOr<ShortValue>, BitOr<UShortValue>, BitOr<IntValue>, BitOr<UIntValue>,
    BitOr<LongValue>, BitOr<ULongValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Value* BitXor(Module* module, Value* left, Value* right, const Span& span, bool dontThrow)
{
    return BinaryEvaluate<ValueT>(left, right, std::bit_xor<typename ValueT::OperandType>(), span);
}

BinaryOperatorFun bitXor[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, BitXor<SByteValue>, BitXor<ByteValue>, BitXor<ShortValue>, BitXor<UShortValue>, BitXor<IntValue>, BitXor<UIntValue>,
    BitXor<LongValue>, BitXor<ULongValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Value* BitAnd(Module* module, Value* left, Value* right, const Span& span, bool dontThrow)
{
    return BinaryEvaluate<ValueT>(left, right, std::bit_and<typename ValueT::OperandType>(), span);
}

BinaryOperatorFun bitAnd[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, BitAnd<SByteValue>, BitAnd<ByteValue>, BitAnd<ShortValue>, BitAnd<UShortValue>, BitAnd<IntValue>, BitAnd<UIntValue>,
    BitAnd<LongValue>, BitAnd<ULongValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Value* Equal(Module* module, Value* left, Value* right, const Span& span, bool dontThrow)
{
    return BinaryPredEvaluate<ValueT>(left, right, std::equal_to<typename ValueT::OperandType>(), span);
}

BinaryOperatorFun equal[uint8_t(ValueType::maxValue)] =
{
    NotSupported, Equal<BoolValue>, Equal<SByteValue>, Equal<ByteValue>, Equal<ShortValue>, Equal<UShortValue>, Equal<IntValue>, Equal<UIntValue>,
    Equal<LongValue>, Equal<ULongValue>, Equal<FloatValue>, Equal<DoubleValue>, Equal<CharValue>, Equal<WCharValue>, Equal<UCharValue>, 
    NotSupported, NotSupported, NotSupported, NotSupported, Equal<PointerValue>, NotSupported, NotSupported
};

template<typename ValueT>
Value* NotEqual(Module* module, Value* left, Value* right, const Span& span, bool dontThrow)
{
    return BinaryPredEvaluate<ValueT>(left, right, std::not_equal_to<typename ValueT::OperandType>(), span);
}

BinaryOperatorFun notEqual[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotEqual<BoolValue>, NotEqual<SByteValue>, NotEqual<ByteValue>, NotEqual<ShortValue>, NotEqual<UShortValue>, NotEqual<IntValue>, NotEqual<UIntValue>,
    NotEqual<LongValue>, NotEqual<ULongValue>, NotEqual<FloatValue>, NotEqual<DoubleValue>, NotEqual<CharValue>, NotEqual<WCharValue>, NotEqual<UCharValue>, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotEqual<PointerValue>, NotSupported, NotSupported
};

template<typename ValueT>
Value* Less(Module* module, Value* left, Value* right, const Span& span, bool dontThrow)
{
    return BinaryPredEvaluate<ValueT>(left, right, std::less<typename ValueT::OperandType>(), span);
}

BinaryOperatorFun less[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, Less<SByteValue>, Less<ByteValue>, Less<ShortValue>, Less<UShortValue>, Less<IntValue>, Less<UIntValue>,
    Less<LongValue>, Less<ULongValue>, Less<FloatValue>, Less<DoubleValue>, Less<CharValue>, Less<WCharValue>, Less<UCharValue>, 
    NotSupported, NotSupported, NotSupported, NotSupported, Less<PointerValue>, NotSupported, NotSupported
};

template<typename ValueT>
Value* Greater(Module* module, Value* left, Value* right, const Span& span, bool dontThrow)
{
    return BinaryPredEvaluate<ValueT>(left, right, std::greater<typename ValueT::OperandType>(), span);
}

BinaryOperatorFun greater[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, Greater<SByteValue>, Greater<ByteValue>, Greater<ShortValue>, Greater<UShortValue>, Greater<IntValue>, Greater<UIntValue>,
    Greater<LongValue>, Greater<ULongValue>, Greater<FloatValue>, Greater<DoubleValue>, Greater<CharValue>, Greater<WCharValue>, Greater<UCharValue>, 
    NotSupported, NotSupported, NotSupported, NotSupported, Greater<PointerValue>, NotSupported, NotSupported
};

template<typename ValueT>
Value* LessEqual(Module* module, Value* left, Value* right, const Span& span, bool dontThrow)
{
    return BinaryPredEvaluate<ValueT>(left, right, std::less_equal<typename ValueT::OperandType>(), span);
}

BinaryOperatorFun lessEqual[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, LessEqual<SByteValue>, LessEqual<ByteValue>, LessEqual<ShortValue>, LessEqual<UShortValue>, LessEqual<IntValue>, LessEqual<UIntValue>,
    LessEqual<LongValue>, LessEqual<ULongValue>, LessEqual<FloatValue>, LessEqual<DoubleValue>, LessEqual<CharValue>, LessEqual<WCharValue>, LessEqual<UCharValue>, 
    NotSupported, NotSupported, NotSupported, NotSupported, LessEqual<PointerValue>, NotSupported, NotSupported
};

template<typename ValueT>
Value* GreaterEqual(Module* module, Value* left, Value* right, const Span& span, bool dontThrow)
{
    return BinaryPredEvaluate<ValueT>(left, right, std::greater_equal<typename ValueT::OperandType>(), span);
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
Value* ShiftLeft(Module* module, Value* left, Value* right, const Span& span, bool dontThrow)
{
    return BinaryEvaluate<ValueT>(left, right, shiftLeftFun<typename ValueT::OperandType>(), span);
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
Value* ShiftRight(Module* module, Value* left, Value* right, const Span& span, bool dontThrow)
{
    return BinaryEvaluate<ValueT>(left, right, shiftRightFun<typename ValueT::OperandType>(), span);
}

BinaryOperatorFun shiftRight[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, ShiftRight<SByteValue>, ShiftRight<ByteValue>, ShiftRight<ShortValue>, ShiftRight<UShortValue>, ShiftRight<IntValue>, ShiftRight<UIntValue>,
    ShiftRight<LongValue>, ShiftRight<ULongValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Value* Add(Module* module, Value* left, Value* right, const Span& span, bool dontThrow)
{
    return BinaryEvaluate<ValueT>(left, right, std::plus<typename ValueT::OperandType>(), span);
}

BinaryOperatorFun add[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, Add<SByteValue>, Add<ByteValue>, Add<ShortValue>, Add<UShortValue>, Add<IntValue>, Add<UIntValue>,
    Add<LongValue>, Add<ULongValue>, Add<FloatValue>, Add<DoubleValue>, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Value* Sub(Module* module, Value* left, Value* right, const Span& span, bool dontThrow)
{
    return BinaryEvaluate<ValueT>(left, right, std::minus<typename ValueT::OperandType>(), span);
}

BinaryOperatorFun sub[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, Sub<SByteValue>, Sub<ByteValue>, Sub<ShortValue>, Sub<UShortValue>, Sub<IntValue>, Sub<UIntValue>,
    Sub<LongValue>, Sub<ULongValue>, Sub<FloatValue>, Sub<DoubleValue>, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Value* Mul(Module* module, Value* left, Value* right, const Span& span, bool dontThrow)
{
    return BinaryEvaluate<ValueT>(left, right, std::multiplies<typename ValueT::OperandType>(), span);
}

BinaryOperatorFun mul[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, Mul<SByteValue>, Mul<ByteValue>, Mul<ShortValue>, Mul<UShortValue>, Mul<IntValue>, Mul<UIntValue>,
    Mul<LongValue>, Mul<ULongValue>, Mul<FloatValue>, Mul<DoubleValue>, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Value* Div(Module* module, Value* left, Value* right, const Span& span, bool dontThrow)
{
    return BinaryEvaluate<ValueT>(left, right, std::divides<typename ValueT::OperandType>(), span);
}

BinaryOperatorFun div[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, Div<SByteValue>, Div<ByteValue>, Div<ShortValue>, Div<UShortValue>, Div<IntValue>, Div<UIntValue>,
    Div<LongValue>, Div<ULongValue>, Div<FloatValue>, Div<DoubleValue>, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Value* Rem(Module* module, Value* left, Value* right, const Span& span, bool dontThrow)
{
    return BinaryEvaluate<ValueT>(left, right, std::modulus<typename ValueT::OperandType>(), span);
}

BinaryOperatorFun rem[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, Rem<SByteValue>, Rem<ByteValue>, Rem<ShortValue>, Rem<UShortValue>, Rem<IntValue>, Rem<UIntValue>,
    Rem<LongValue>, Rem<ULongValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Value* Not(Module* module, Value* subject, const Span& span, bool dontThrow)
{
    return UnaryEvaluate<ValueT>(subject, std::logical_not<typename ValueT::OperandType>(), span);
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
Value* UnaryPlus(Module* module, Value* subject, const Span& span, bool dontThrow)
{
    return UnaryEvaluate<ValueT>(subject, Identity<typename ValueT::OperandType>(), span);
}

UnaryOperatorFun unaryPlus[uint8_t(ValueType::maxValue)] =
{
    NotSupported, NotSupported, UnaryPlus<SByteValue>, UnaryPlus<ByteValue>, UnaryPlus<ShortValue>, UnaryPlus<UShortValue>, UnaryPlus<IntValue>, UnaryPlus<UIntValue>,
    UnaryPlus<LongValue>, UnaryPlus<ULongValue>, UnaryPlus<FloatValue>, UnaryPlus<DoubleValue>, NotSupported, NotSupported, NotSupported, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Value* UnaryMinus(Module* module, Value* subject, const Span& span, bool dontThrow)
{
    return UnaryEvaluate<ValueT>(subject, std::negate<typename ValueT::OperandType>(), span);
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
Value* Complement(Module* module, Value* subject, const Span& span, bool dontThrow)
{
    return UnaryEvaluate<ValueT>(subject, BitNot<typename ValueT::OperandType>(), span);
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
    Evaluator(BoundCompileUnit& boundCompileUnit_, ContainerScope* containerScope_, TypeSymbol* targetType_, ValueType targetValueType_, bool cast_, bool dontThrow_, BoundFunction* currentFunction_, const Span& span_);
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
    const Span& span_) :
    boundCompileUnit(boundCompileUnit_), symbolTable(&boundCompileUnit.GetSymbolTable()), module(&boundCompileUnit.GetModule()), 
    containerScope(containerScope_), qualifiedScope(nullptr), cast(cast_), dontThrow(dontThrow_), error(false),
    returned(false), broke(false), continued(false), lvalue(false), currentFunction(currentFunction_), currentDeclarationBlock(nullptr), currentFileScope(nullptr), currentClassType(nullptr), span(span_), value(),
    targetType(targetType_), targetValueType(targetValueType_), targetValueSymbol(nullptr)
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
            ThrowCannotEvaluateStatically(module, span, node.GetSpan());
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
            ThrowCannotEvaluateStatically(module, span, node.GetSpan());
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
                throw Exception(module, "incompatible pointer types for comparison", node.GetSpan());
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
            throw Exception(module, "conversion from " + ValueTypeStr(leftType) + " to " + ValueTypeStr(operationType) + " is not valid", span);
        }
    }
    std::unique_ptr<Value> leftConverted(left->As(type, cast, node.GetSpan(), dontThrow));
    std::unique_ptr<Value> rightConverted(right->As(type, cast, node.GetSpan(), dontThrow));
    if (dontThrow)
    {
        if (!leftConverted || !rightConverted)
        {
            error = true;
            return;
        }
    }
    BinaryOperatorFun operation = fun[uint8_t(operationType)];
    value.reset(operation(module, leftConverted.get(), rightConverted.get(), node.GetSpan(), dontThrow));
}

void Evaluator::EvaluateAdditivePointerOp(const Span& span, Operator op, const std::unique_ptr<Value>& left, const std::unique_ptr<Value>& right)
{
    if (op == Operator::add)
    {
        if (left->GetValueType() == ValueType::pointerValue)
        {
            std::unique_ptr<Value> rightConverted(right->As(symbolTable->GetTypeByName(U"long"), cast, span, dontThrow));
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
                    throw Exception(module, "invalid pointer operands", span);
                }
            }
        }
        else if (right->GetValueType() == ValueType::pointerValue)
        {
            std::unique_ptr<Value> leftConverted(right->As(symbolTable->GetTypeByName(U"long"), cast, span, dontThrow));
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
                    throw Exception(module, "invalid pointer operands", span);
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
                throw Exception(module, "invalid pointer operands", span);
            }
        }
    }
    else if (op == Operator::sub)
    {
        if (left->GetValueType() == ValueType::pointerValue && right->GetValueType() != ValueType::pointerValue)
        {
            std::unique_ptr<Value> rightConverted(right->As(symbolTable->GetTypeByName(U"long"), cast, span, dontThrow));
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
                    throw Exception(module, "invalid pointer operands", span);
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
                    throw Exception(module, "incompatible pointer operands", span);
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
                    throw Exception(module, "invalid pointer operands", span);
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
                throw Exception(module, "invalid pointer operands", span);
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
            ThrowCannotEvaluateStatically(module, span, node.GetSpan());
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
            throw Exception(module, "conversion from " + ValueTypeStr(subjectType) + " to " + ValueTypeStr(operationType) + " is not valid", span);
        }
    }
    std::unique_ptr<Value> subjectConverted(subject->As(type, cast, node.GetSpan(), dontThrow));
    if (dontThrow)
    {
        if (!subjectConverted)
        {
            error = true;
            return;
        }
    }
    UnaryOperatorFun operation = fun[uint8_t(operationType)];
    value.reset(operation(module, subjectConverted.get(), node.GetSpan(), dontThrow));
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
            FileScope* fileScope = new FileScope(&boundCompileUnit.GetModule());
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
    DeclarationBlock declarationBlock(span, U"functionBlock");
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
            throw Exception(module, "wrong number of function template type arguments", span);
        }
    }
    for (int i = 0; i < nt; ++i)
    {
        TemplateParameterNode* templateParameterNode = functionNode.TemplateParameters()[i];
        TypeSymbol* templateTypeArgument = templateTypeArguments[i];
        BoundTemplateParameterSymbol* boundTemplateParameter = new BoundTemplateParameterSymbol(span, templateParameterNode->Id()->Str());
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
            throw Exception(module, "wrong number of function arguments", span);
        }
    }
    for (int i = 0; i < n; ++i)
    {
        std::unique_ptr<Value> argumentValue = std::move(argumentValues[i]);
        TypeSymbol* argumentType = argumentValue->GetType(symbolTable);
        ParameterNode* parameterNode = functionNode.Parameters()[i];
        VariableValueSymbol* variableValueSymbol = new VariableValueSymbol(parameterNode->GetSpan(), parameterNode->Id()->Str(), std::move(argumentValue));
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
            FileScope* fileScope = new FileScope(&boundCompileUnit.GetModule());
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
    DeclarationBlock declarationBlock(span, U"constructorBlock");
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
            throw Exception(module, "wrong number of constructor arguments", span);
        }
    }
    for (int i = 0; i < n; ++i)
    {
        std::unique_ptr<Value> argumentValue = std::move(argumentValues[i]);
        TypeSymbol* argumentType = argumentValue->GetType(symbolTable);
        ParameterNode* parameterNode = constructorNode.Parameters()[i];
        VariableValueSymbol* variableValueSymbol = new VariableValueSymbol(parameterNode->GetSpan(), parameterNode->Id()->Str(), std::move(argumentValue));
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
                throw Exception(module, "this and base initializers not supported for a constexpr constructor", constructorNode.GetSpan());
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
                    throw Exception(module, "already has initializer for member variable '" + ToUtf8(memberName) + "'", initializer->GetSpan());
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
                        ThrowCannotEvaluateStatically(module, span, constructorNode.GetSpan());
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
                ThrowCannotEvaluateStatically(module, span, constructorNode.GetSpan());
            }
        }
        initializerArguments.insert(initializerArguments.begin(), std::unique_ptr<BoundExpression>(new BoundTypeExpression(module, span, memberVariableSymbol->GetType()->AddPointer(span))));
        OverloadResolutionFlags flags = OverloadResolutionFlags::dontInstantiate;
        if (dontThrow)
        {
            flags = flags | OverloadResolutionFlags::dontThrow;
        }
        std::vector<TypeSymbol*> templateArgumentTypes;
        std::unique_ptr<Exception> exception;
        std::unique_ptr<BoundFunctionCall> constructorCall = ResolveOverload(U"@constructor", containerScope, lookups, initializerArguments, boundCompileUnit, currentFunction, constructorNode.GetSpan(),
            flags, templateArgumentTypes, exception);
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
                ThrowCannotEvaluateStatically(module, span, constructorNode.GetSpan());
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
                ThrowCannotEvaluateStatically(module, span, constructorNode.GetSpan());
            }
        }
        FunctionSymbol* constructorSymbol = constructorCall->GetFunctionSymbol();
        if (constructorSymbol->IsCompileTimePrimitiveFunction())
        {
            value = constructorSymbol->ConstructValue(argumentValues, span);
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
                    ThrowCannotEvaluateStatically(module, span, constructorNode.GetSpan());
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
                    ThrowCannotEvaluateStatically(module, span, ctorNode->GetSpan());
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
                ThrowCannotEvaluateStatically(module, span, constructorNode.GetSpan());
            }
        }
        memberValues.push_back(std::move(value));
    }
    constructorNode.Body()->Accept(*this);
    value.reset(new StructuredValue(span, classType, std::move(memberValues)));
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
            FileScope* fileScope = new FileScope(&boundCompileUnit.GetModule());
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
    DeclarationBlock declarationBlock(span, U"functionBlock");
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
            throw Exception(module, "wrong number of function arguments", memberFunctionNode.GetSpan());
        }
    }
    for (int i = 0; i < n; ++i)
    {
        std::unique_ptr<Value> argumentValue = std::move(argumentValues[i]);
        TypeSymbol* argumentType = argumentValue->GetType(symbolTable);
        ParameterNode* parameterNode = memberFunctionNode.Parameters()[i];
        VariableValueSymbol* variableValueSymbol = new VariableValueSymbol(parameterNode->GetSpan(), parameterNode->Id()->Str(), std::move(argumentValue));
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
                throw Exception(module, "structured reference value expected", memberFunctionNode.GetSpan());
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
                throw Exception(module, "wrong number of structured value members", memberFunctionNode.GetSpan());
            }
        }
        for (int i = 0; i < n; ++i)
        {
            MemberVariableSymbol* memberVariableSymbol = currentClassType->MemberVariables()[i];
            Value* memberValue = structuredValue->Members()[i].get();
            ConstantSymbol* constantSymbol = new ConstantSymbol(span, memberVariableSymbol->Name());
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
        ThrowCannotEvaluateStatically(module, span, conversionFunctionNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, classNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, staticConstructorNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, destructorNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, interfaceNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, delegateNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, classDelegateNode.GetSpan());
    }
}

void Evaluator::Visit(CompoundStatementNode& compoundStatementNode)
{
    DeclarationBlock* prevDeclarationBlock = currentDeclarationBlock;
    DeclarationBlock declarationBlock(span, U"block");
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
            throw Exception(module, "Boolean expression expected", ifStatementNode.GetSpan());
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
                    throw Exception(module, "Boolean expression expected", whileStatementNode.GetSpan());
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
            throw Exception(module, "Boolean expression expected", whileStatementNode.GetSpan());
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
                throw Exception(module, "Boolean expression expected", doStatementNode.GetSpan());
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
    DeclarationBlock declarationBlock(span, U"forBlock");
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
                    throw Exception(module, "Boolean expression expected", forStatementNode.GetSpan());
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
            throw Exception(module, "Boolean expression expected", forStatementNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, gotoStatementNode.GetSpan());
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
            throw Exception(module, "internal error: current declaration block not set", constructionStatementNode.GetSpan());
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
                ThrowCannotEvaluateStatically(module, span, constructionStatementNode.GetSpan());
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
            ThrowCannotEvaluateStatically(module, span, constructionStatementNode.GetSpan());
        }
    }
    arguments.insert(arguments.begin(), std::unique_ptr<BoundExpression>(new BoundTypeExpression(module, span, type->AddPointer(span))));
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
    std::unique_ptr<BoundFunctionCall> constructorCall = ResolveOverload(U"@constructor", containerScope, scopeLookups, arguments, boundCompileUnit, currentFunction, span, flags, templateArgumentTypes, exception);
    if (!constructorCall)
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            ThrowCannotEvaluateStatically(module, span, constructionStatementNode.GetSpan());
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
            ThrowCannotEvaluateStatically(module, span, constructionStatementNode.GetSpan());
        }
    }
    FunctionSymbol* constructorSymbol = constructorCall->GetFunctionSymbol();
    if (constructorSymbol->IsCompileTimePrimitiveFunction())
    {
        value = constructorSymbol->ConstructValue(argumentValues, span);
        if (!value)
        {
            if (dontThrow)
            {
                error = true;
                return;
            }
            else
            {
                ThrowCannotEvaluateStatically(module, span, constructionStatementNode.GetSpan());
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
                ThrowCannotEvaluateStatically(module, span, ctorNode->GetSpan());
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
            ThrowCannotEvaluateStatically(module, span, constructionStatementNode.GetSpan());
        }
    }
    VariableValueSymbol* variableValue = new VariableValueSymbol(span, constructionStatementNode.Id()->Str(), std::move(value));
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
        ThrowCannotEvaluateStatically(module, span, deleteStatementNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, destroyStatementNode.GetSpan());
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
            ThrowCannotEvaluateStatically(module, span, assignmentStatementNode.GetSpan());
        }
    }
    arguments.insert(arguments.begin(), std::unique_ptr<BoundExpression>(new BoundTypeExpression(module, span, target->GetType()->AddPointer(span))));
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
    std::unique_ptr<BoundFunctionCall> assignmentCall = ResolveOverload(U"operator=", containerScope, scopeLookups, arguments, boundCompileUnit, currentFunction, span, flags, templateArgumentTypes, exception);
    if (!assignmentCall)
    {
        if (dontThrow)
        {
            error = true;
            return;
        }
        else
        {
            ThrowCannotEvaluateStatically(module, span, assignmentStatementNode.GetSpan());
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
            ThrowCannotEvaluateStatically(module, span, assignmentStatementNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, rangeForStatementNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, switchStatementNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, caseStatementNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, defaultStatementNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, gotoCaseStatementNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, gotoDefaultStatementNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, throwStatementNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, tryStatementNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, catchNode.GetSpan());
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
            ThrowCannotEvaluateStatically(module, span, assertStatementNode.GetSpan());
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
                throw Exception(module, "assertion '" + assertStatementNode.AssertExpr()->ToString() + "' failed", span, assertStatementNode.GetSpan());
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
            throw Exception(module, "assertion expression is not a Boolean-valued expression", span, assertStatementNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, conditionalCompilationPartNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, conditionalCompilationDisjunctionNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, conditionalCompilationConjunctionNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, conditionalCompilationNotNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, conditionalCompilationPrimaryNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, conditionalCompilationStatementNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, boolNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, sbyteNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, byteNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, shortNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, ushortNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, intNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, uintNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, longNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, ulongNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, floatNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, doubleNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, charNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, wcharNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, ucharNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, voidNode.GetSpan());
    }
}

void Evaluator::Visit(BooleanLiteralNode& booleanLiteralNode)
{
    value.reset(new BoolValue(booleanLiteralNode.GetSpan(), booleanLiteralNode.Value()));
}

void Evaluator::Visit(SByteLiteralNode& sbyteLiteralNode)
{
    value.reset(new SByteValue(sbyteLiteralNode.GetSpan(), sbyteLiteralNode.Value()));
}

void Evaluator::Visit(ByteLiteralNode& byteLiteralNode)
{
    value.reset(new ByteValue(byteLiteralNode.GetSpan(), byteLiteralNode.Value()));
}

void Evaluator::Visit(ShortLiteralNode& shortLiteralNode)
{
    value.reset(new ShortValue(shortLiteralNode.GetSpan(), shortLiteralNode.Value()));
}

void Evaluator::Visit(UShortLiteralNode& ushortLiteralNode)
{
    value.reset(new UShortValue(ushortLiteralNode.GetSpan(), ushortLiteralNode.Value()));
}

void Evaluator::Visit(IntLiteralNode& intLiteralNode)
{
    value.reset(new IntValue(intLiteralNode.GetSpan(), intLiteralNode.Value()));
}

void Evaluator::Visit(UIntLiteralNode& uintLiteralNode)
{
    value.reset(new UIntValue(uintLiteralNode.GetSpan(), uintLiteralNode.Value()));
}

void Evaluator::Visit(LongLiteralNode& longLiteralNode)
{
    value.reset(new LongValue(longLiteralNode.GetSpan(), longLiteralNode.Value()));
}

void Evaluator::Visit(ULongLiteralNode& ulongLiteralNode)
{
    value.reset(new ULongValue(ulongLiteralNode.GetSpan(), ulongLiteralNode.Value()));
}

void Evaluator::Visit(FloatLiteralNode& floatLiteralNode)
{
    value.reset(new FloatValue(floatLiteralNode.GetSpan(), floatLiteralNode.Value()));
}

void Evaluator::Visit(DoubleLiteralNode& doubleLiteralNode)
{
    value.reset(new DoubleValue(doubleLiteralNode.GetSpan(), doubleLiteralNode.Value()));
}

void Evaluator::Visit(CharLiteralNode& charLiteralNode)
{
    value.reset(new CharValue(charLiteralNode.GetSpan(), charLiteralNode.Value()));
}

void Evaluator::Visit(WCharLiteralNode& wcharLiteralNode)
{
    value.reset(new WCharValue(wcharLiteralNode.GetSpan(), wcharLiteralNode.Value()));
}

void Evaluator::Visit(UCharLiteralNode& ucharLiteralNode)
{
    value.reset(new UCharValue(ucharLiteralNode.GetSpan(), ucharLiteralNode.Value()));
}

void Evaluator::Visit(StringLiteralNode& stringLiteralNode)
{
    value.reset(new StringValue(stringLiteralNode.GetSpan(), boundCompileUnit.Install(stringLiteralNode.Value()), stringLiteralNode.Value()));
}

void Evaluator::Visit(WStringLiteralNode& wstringLiteralNode)
{
    value.reset(new WStringValue(wstringLiteralNode.GetSpan(), boundCompileUnit.Install(wstringLiteralNode.Value()), wstringLiteralNode.Value()));
}

void Evaluator::Visit(UStringLiteralNode& ustringLiteralNode)
{
    value.reset(new UStringValue(ustringLiteralNode.GetSpan(), boundCompileUnit.Install(ustringLiteralNode.Value()), ustringLiteralNode.Value()));
}

void Evaluator::Visit(NullLiteralNode& nullLiteralNode)
{
    value.reset(new NullValue(nullLiteralNode.GetSpan(), symbolTable->GetTypeByName(U"@nullptr_type")));
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
            throw Exception(module, "array type expected", span);
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
            throw Exception(module, "invalid length for array literal of type '" + ToUtf8(arrayType->FullName()) + "'", arrayLiteralNode.GetSpan());
        }
    }
    for (int i = 0; i < n; ++i)
    {
        value = Evaluate(arrayLiteralNode.Values()[i], elementType, containerScope, boundCompileUnit, dontThrow, currentFunction, arrayLiteralNode.GetSpan());
        if (error)
        {
            if (dontThrow)
            {
                return;
            }
            else
            {
                ThrowCannotEvaluateStatically(module, span, arrayLiteralNode.GetSpan());
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
                ThrowCannotEvaluateStatically(module, span, arrayLiteralNode.GetSpan());
            }
        }
        elementValues.push_back(std::move(value));
    }
    if (arrayType->Size() == -1)
    {
        arrayType = symbolTable->MakeArrayType(arrayType->ElementType(), n, arrayLiteralNode.GetSpan());
    }
    value.reset(new ArrayValue(arrayLiteralNode.GetSpan(), arrayType, std::move(elementValues)));
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
            throw Exception(module, "class type expected", span);
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
                throw Exception(module, "wrong number of members variables for class literal of type '" + ToUtf8(classType->FullName()) + "'", structuredLiteralNode.GetSpan());
            }
        }
        for (int i = 0; i < n; ++i)
        {
            TypeSymbol* memberType = classType->MemberVariables()[i]->GetType();
            value = Evaluate(structuredLiteralNode.Members()[i], memberType, containerScope, boundCompileUnit, dontThrow, currentFunction, structuredLiteralNode.GetSpan());
            if (error)
            {
                if (dontThrow)
                {
                    return;
                }
                else
                {
                    ThrowCannotEvaluateStatically(module, span, structuredLiteralNode.GetSpan());
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
                    ThrowCannotEvaluateStatically(module, span, structuredLiteralNode.GetSpan());
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
                ThrowCannotEvaluateStatically(module, span, structuredLiteralNode.GetSpan());
            }
        }
        arguments.insert(arguments.begin(), std::unique_ptr<BoundExpression>(new BoundTypeExpression(module, span, classType->AddPointer(span))));
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
        std::unique_ptr<BoundFunctionCall> constructorCall = ResolveOverload(U"@constructor", containerScope, scopeLookups, arguments, boundCompileUnit, currentFunction, span, flags, templateArgumentTypes, exception);
        if (!constructorCall)
        {
            if (dontThrow)
            {
                error = true;
                return;
            }
            else
            {
                ThrowCannotEvaluateStatically(module, span, structuredLiteralNode.GetSpan());
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
                ThrowCannotEvaluateStatically(module, span, structuredLiteralNode.GetSpan());
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
                ThrowCannotEvaluateStatically(module, span, structuredLiteralNode.GetSpan());
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
            throw Exception(module, "class '" + ToUtf8(classType->FullName()) + "' is not a literal class ", structuredLiteralNode.GetSpan());
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
            throw Exception(module, "symbol '" + ToUtf8(name) + "' not found", identifierNode.GetSpan());
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
            ThrowCannotEvaluateStatically(module, span, templateIdNode.GetSpan());
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
        value.reset(new ScopedValue(span, containerSymbol));
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
            ThrowCannotEvaluateStatically(module, span);
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
        throw Exception(module, "cyclic depenency detected", span);
    }
    Value* constantValue = constantSymbol->GetValue();
    if (constantValue)
    {
        if (constantValue->GetValueType() == ValueType::arrayValue)
        {
            value.reset(new ArrayReferenceValue(static_cast<ArrayValue*>(constantValue)));
        }
        else if (constantValue->GetValueType() == ValueType::structuredValue)
        {
            value.reset(new StructuredReferenceValue(static_cast<StructuredValue*>(constantValue)));
        }
        else
        {
            value.reset(constantValue->Clone());
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
            throw Exception(module, "node for constant symbol '" + ToUtf8(constantSymbol->FullName()) + "' not found from symbol table" , span);
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
        throw Exception(module, "cyclic depenency detected", span);
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
            ScopedValue* scopedValue = new ScopedValue(span, type);
            scopedValue->SetType(type);
            value.reset(scopedValue);
        }
        else if (value->IsStructuredReferenceValue())
        {
            TypeSymbol* type = static_cast<StructuredReferenceValue*>(value.get())->GetStructuredValue()->GetType(symbolTable);
            ScopedValue* scopedValue = new ScopedValue(span, type);
            scopedValue->SetType(type->AddPointer(span));
            scopedValue->SetSubject(value.release());
            value.reset(scopedValue);
        }
        else if (value->GetValueType() == ValueType::structuredValue)
        {
            TypeSymbol* type = static_cast<StructuredValue*>(value.get())->GetType(symbolTable);
            ScopedValue* scopedValue = new ScopedValue(span, type);
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
            if (scopedValue->GetType(symbolTable) && (scopedValue->GetType(symbolTable)->IsArrayType() || scopedValue->GetType(symbolTable)->BaseType()->IsClassTypeSymbol()))
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
                throw Exception(module, "symbol '" + ToUtf8(containerSymbol->FullName()) + "' does not have member '" + ToUtf8(memberName) + "'", dotNode.GetSpan());
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
            throw Exception(module, "expression '" + dotNode.Subject()->ToString() + "' must denote a namespace, class type or enumerated type", dotNode.Subject()->GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, arrowNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, equivalenceNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, implicationNode.GetSpan());
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
            ThrowCannotEvaluateStatically(module, span, prefixIncrementNode.GetSpan());
        }
    }
    bool unsignedType = value->GetType(symbolTable)->IsUnsignedType();
    CloneContext cloneContext;
    if (unsignedType)
    {
        AssignmentStatementNode assignmentStatementNode(prefixIncrementNode.GetSpan(), prefixIncrementNode.Subject()->Clone(cloneContext),
            new AddNode(prefixIncrementNode.GetSpan(), prefixIncrementNode.Subject()->Clone(cloneContext), new ByteLiteralNode(prefixIncrementNode.GetSpan(), 1)));
        assignmentStatementNode.Accept(*this);
    }
    else
    {
        AssignmentStatementNode assignmentStatementNode(prefixIncrementNode.GetSpan(), prefixIncrementNode.Subject()->Clone(cloneContext),
            new AddNode(prefixIncrementNode.GetSpan(), prefixIncrementNode.Subject()->Clone(cloneContext), new SByteLiteralNode(prefixIncrementNode.GetSpan(), 1)));
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
            ThrowCannotEvaluateStatically(module, span, prefixIncrementNode.GetSpan());
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
            ThrowCannotEvaluateStatically(module, span, prefixDecrementNode.GetSpan());
        }
    }
    bool unsignedType = value->GetType(symbolTable)->IsUnsignedType();
    CloneContext cloneContext;
    if (unsignedType)
    {
        AssignmentStatementNode assignmentStatementNode(prefixDecrementNode.GetSpan(), prefixDecrementNode.Subject()->Clone(cloneContext),
            new SubNode(prefixDecrementNode.GetSpan(), prefixDecrementNode.Subject()->Clone(cloneContext), new ByteLiteralNode(prefixDecrementNode.GetSpan(), 1)));
        assignmentStatementNode.Accept(*this);
    }
    else
    {
        AssignmentStatementNode assignmentStatementNode(prefixDecrementNode.GetSpan(), prefixDecrementNode.Subject()->Clone(cloneContext),
            new SubNode(prefixDecrementNode.GetSpan(), prefixDecrementNode.Subject()->Clone(cloneContext), new SByteLiteralNode(prefixDecrementNode.GetSpan(), 1)));
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
            ThrowCannotEvaluateStatically(module, span, prefixDecrementNode.GetSpan());
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
                throw Exception(module, "unsupported pointer value", derefNode.GetSpan());
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
            throw Exception(module, "pointer value expected", derefNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, addrOfNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, isNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, asNode.GetSpan());
    }
}

void Evaluator::Visit(IndexingNode& indexingNode)
{
    indexingNode.Subject()->Accept(*this);
    if (value && value->IsArrayReferenceValue())
    {
        ArrayValue* arrayValue = static_cast<ArrayReferenceValue*>(value.get())->GetArrayValue();
        value = Evaluate(indexingNode.Index(), symbolTable->GetTypeByName(U"long"), containerScope, boundCompileUnit, dontThrow, currentFunction, indexingNode.GetSpan());
        if (!value)
        {
            if (dontThrow)
            {
                error = true;
                return;
            }
            else
            {
                ThrowCannotEvaluateStatically(module, span, indexingNode.GetSpan());
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
                throw Exception(module, "array index out of range", indexingNode.GetSpan());
            }
        }
        Value* elementValue = arrayValue->Elements()[index].get();
        if (elementValue->GetValueType() == ValueType::arrayValue)
        {
            value.reset(new ArrayReferenceValue(static_cast<ArrayValue*>(elementValue)));
        }
        else
        {
            value = std::unique_ptr<Value>(elementValue->Clone());
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
            ThrowCannotEvaluateStatically(module, span, indexingNode.GetSpan());
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
                ThrowCannotEvaluateStatically(module, span, invokeNode.GetSpan());
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
                ThrowCannotEvaluateStatically(module, span, invokeNode.GetSpan());
            }
        }
        if (functionGroupValue->Receiver() && functionGroupValue->Receiver()->IsScopedValue())
        {
            TypeSymbol* type = static_cast<ScopedValue*>(functionGroupValue->Receiver())->GetType(symbolTable);
            if (type)
            {
                arguments.insert(arguments.begin(), std::unique_ptr<BoundExpression>(new BoundTypeExpression(module, span, type)));
            }
        }
        templateTypeArguments = std::move(functionGroupValue->TemplateTypeArguments());
        std::unique_ptr<Exception> exception;
        OverloadResolutionFlags flags = OverloadResolutionFlags::dontInstantiate;
        flags = flags | OverloadResolutionFlags::dontThrow;
        std::unique_ptr<BoundFunctionCall> functionCall = ResolveOverload(functionGroup->Name(), containerScope, functionScopeLookups, arguments, boundCompileUnit, currentFunction, span, flags,
            templateTypeArguments, exception);
        bool memberFunctionCall = false;
        if (!functionCall)
        {
            if (currentClassType)
            {
                arguments.insert(arguments.begin(), std::unique_ptr<BoundExpression>(new BoundTypeExpression(module, span, currentClassType->AddPointer(span))));
                functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, currentClassType->GetContainerScope()));
                OverloadResolutionFlags flags = OverloadResolutionFlags::dontInstantiate;
                if (dontThrow)
                {
                    flags = flags | OverloadResolutionFlags::dontThrow;
                }
                std::unique_ptr<Exception> exception;
                functionCall = ResolveOverload(functionGroup->Name(), containerScope, functionScopeLookups, arguments, boundCompileUnit, currentFunction, span, flags, templateTypeArguments, exception);
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
                    ThrowCannotEvaluateStatically(module, span, invokeNode.GetSpan());
                }
            }
        }
        FunctionSymbol* functionSymbol = functionCall->GetFunctionSymbol();
        if (functionSymbol->IsCompileTimePrimitiveFunction())
        {
            bool skipFirst = functionGroupValue->Receiver();
            argumentValues = ArgumentsToValues(functionCall->Arguments(), error, skipFirst, boundCompileUnit);
            if (error)
            {
                if (dontThrow)
                {
                    return;
                }
                else
                {
                    ThrowCannotEvaluateStatically(module, span, invokeNode.GetSpan());
                }
            }
            value = functionSymbol->ConstructValue(argumentValues, invokeNode.GetSpan());
            if (!value)
            {
                if (dontThrow)
                {
                    error = true;
                    return;
                }
                else
                {
                    ThrowCannotEvaluateStatically(module, span, invokeNode.GetSpan());
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
                    ThrowCannotEvaluateStatically(module, span, invokeNode.GetSpan());
                }
            }
            ClassTypeSymbol* prevClassType = currentClassType;
            if (functionGroupValue->Receiver() && functionGroupValue->Receiver()->IsScopedValue())
            {
                ScopedValue* receiver = static_cast<ScopedValue*>(functionGroupValue->Receiver());
                if (receiver->Subject() && receiver->Subject()->GetType(symbolTable)->IsClassTypeSymbol())
                {
                    currentClassType = static_cast<ClassTypeSymbol*>(receiver->Subject()->GetType(symbolTable));
                    structureReferenceValue = std::unique_ptr<Value>(receiver->Subject()->Clone());
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
                        ThrowCannotEvaluateStatically(module, span, invokeNode.GetSpan());
                    }
                }
                value = intrinsic->Evaluate(argumentValues, templateTypeArguments, invokeNode.GetSpan());
                if (!value)
                {
                    if (dontThrow)
                    {
                        error = true;
                        return;
                    }
                    else
                    {
                        ThrowCannotEvaluateStatically(module, span, invokeNode.GetSpan());
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
                    ThrowCannotEvaluateStatically(module, span, invokeNode.GetSpan());
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
            throw Exception(module, "function group expected", invokeNode.GetSpan());
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
            ThrowCannotEvaluateStatically(module, span, postfixIncrementNode.GetSpan());
        }
    }
    bool unsignedType = value->GetType(symbolTable)->IsUnsignedType();
    std::unique_ptr<Value> result = std::move(value);
    CloneContext cloneContext;
    if (unsignedType)
    {
        AssignmentStatementNode assignmentStatementNode(postfixIncrementNode.GetSpan(), postfixIncrementNode.Subject()->Clone(cloneContext),
            new AddNode(postfixIncrementNode.GetSpan(), postfixIncrementNode.Subject()->Clone(cloneContext), new ByteLiteralNode(postfixIncrementNode.GetSpan(), 1)));
        assignmentStatementNode.Accept(*this);
    }
    else
    {
        AssignmentStatementNode assignmentStatementNode(postfixIncrementNode.GetSpan(), postfixIncrementNode.Subject()->Clone(cloneContext),
            new AddNode(postfixIncrementNode.GetSpan(), postfixIncrementNode.Subject()->Clone(cloneContext), new SByteLiteralNode(postfixIncrementNode.GetSpan(), 1)));
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
            ThrowCannotEvaluateStatically(module, span, postfixDecrementNode.GetSpan());
        }
    }
    bool unsignedType = value->GetType(symbolTable)->IsUnsignedType();
    std::unique_ptr<Value> result = std::move(value);
    CloneContext cloneContext;
    if (unsignedType)
    {
        AssignmentStatementNode assignmentStatementNode(postfixDecrementNode.GetSpan(), postfixDecrementNode.Subject()->Clone(cloneContext),
            new SubNode(postfixDecrementNode.GetSpan(), postfixDecrementNode.Subject()->Clone(cloneContext), new ByteLiteralNode(postfixDecrementNode.GetSpan(), 1)));
        assignmentStatementNode.Accept(*this);
    }
    else
    {
        AssignmentStatementNode assignmentStatementNode(postfixDecrementNode.GetSpan(), postfixDecrementNode.Subject()->Clone(cloneContext),
            new SubNode(postfixDecrementNode.GetSpan(), postfixDecrementNode.Subject()->Clone(cloneContext), new SByteLiteralNode(postfixDecrementNode.GetSpan(), 1)));
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
        ThrowCannotEvaluateStatically(module, span, sizeOfNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, typeNameNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, typeIdNode.GetSpan());
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
            ThrowCannotEvaluateStatically(module, span, castNode.GetSpan());
        }
    }
    value.reset(value->As(type, true, castNode.GetSpan(), dontThrow));
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
        ThrowCannotEvaluateStatically(module, span, constructNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, newNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, thisNode.GetSpan());
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
        ThrowCannotEvaluateStatically(module, span, baseNode.GetSpan());
    }
}

void Evaluator::Visit(ParenthesizedExpressionNode& parenthesizedExpressionNode)
{
    parenthesizedExpressionNode.Subject()->Accept(*this);
}

std::unique_ptr<Value> Evaluate(Node* node, TypeSymbol* targetType, ContainerScope* containerScope, BoundCompileUnit& boundCompileUnit, bool dontThrow, BoundFunction* currentFunction, const Span& span)
{
    ValueType targetValueType = targetType->GetValueType();
    Evaluator evaluator(boundCompileUnit, containerScope, targetType, targetValueType, false, dontThrow, currentFunction, span);
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
            if (!TypesEqual(targetType->PlainType(span), value->GetType(&boundCompileUnit.GetSymbolTable())))
            {
                if (targetType->IsArrayType() && static_cast<ArrayTypeSymbol*>(targetType)->Size() == -1)
                {
                    return std::move(value);
                }
                value.reset(value->As(targetType->PlainType(span), false, node->GetSpan(), dontThrow));
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
                throw Exception(&boundCompileUnit.GetModule(), "value not complete", node->GetSpan());
            }
        }
    }
}

} } // namespace cmajor::binder
