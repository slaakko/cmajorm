// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_STRING_FUNCTIONS_INCLUDED
#define CMAJOR_SYMBOLS_STRING_FUNCTIONS_INCLUDED
#include <cmajor/symbols/TypeSymbol.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>

namespace cmajor { namespace symbols {

class SYMBOLS_API StringFunctionContainerSymbol : public TypeSymbol
{
public:
    StringFunctionContainerSymbol();
    StringFunctionContainerSymbol(const Span& span_, const std::u32string& name_);
    void* IrType(Emitter& emitter) override;
    void* CreateDefaultIrValue(Emitter& emitter) override;
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    bool IsStringFunctionContainer() const override { return true; }
};

class SYMBOLS_API StringLengthFunction : public FunctionSymbol
{
public:
    StringLengthFunction(TypeSymbol* parentType);
    StringLengthFunction(const Span& span_, const std::u32string& name_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    std::unique_ptr<Value> ConstructValue(const std::vector<std::unique_ptr<Value>>& argumentValues, const Span& span, Value* receiver) const override;
    bool IsBasicTypeOperation() const override { return true; }
    bool IsCompileTimePrimitiveFunction() const override { return true; }
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_STRING_FUNCTIONS_INCLUDED
