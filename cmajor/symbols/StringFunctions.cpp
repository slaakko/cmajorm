// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/StringFunctions.hpp>
#include <cmajor/symbols/Value.hpp>
#include <cmajor/symbols/Module.hpp>

namespace cmajor { namespace symbols {

StringFunctionContainerSymbol::StringFunctionContainerSymbol() : TypeSymbol(SymbolType::stringFunctionContainerSymbol, Span(), U"@string_functions")
{
    AddMember(new StringLengthFunction(this));
}

void* StringFunctionContainerSymbol::IrType(Emitter& emitter)
{
    throw std::runtime_error("string function container symbol has no IR type");
}

void* StringFunctionContainerSymbol::CreateDefaultIrValue(Emitter& emitter)
{
    throw std::runtime_error("string function container symbol has no IR value");
}

StringFunctionContainerSymbol::StringFunctionContainerSymbol(const Span& span_, const std::u32string& name_) : TypeSymbol(SymbolType::stringFunctionContainerSymbol, span_, name_)
{
}

void StringFunctionContainerSymbol::Write(SymbolWriter& writer)
{
    TypeSymbol::Write(writer);
}

void StringFunctionContainerSymbol::Read(SymbolReader& reader)
{
    TypeSymbol::Read(reader);
}

StringLengthFunction::StringLengthFunction(TypeSymbol* parentType) : FunctionSymbol(SymbolType::stringLengthFunctionSymbol, Span(), U"Length")
{
    SetGroupName(U"Length");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* stringValueParam = new ParameterSymbol(Span(), U"stringValue");
    stringValueParam->SetType(parentType);
    AddMember(stringValueParam);
    TypeSymbol* longType = GetRootModuleForCurrentThread()->GetSymbolTable().GetTypeByName(U"long");
    SetReturnType(longType);
    ComputeName();
    GetRootModuleForCurrentThread()->GetSymbolTable().SetFunctionIdFor(this);
}

StringLengthFunction::StringLengthFunction(const Span& span_, const std::u32string& name_) : FunctionSymbol(span_, name_)
{
}

void StringLengthFunction::Write(SymbolWriter& writer)
{
    FunctionSymbol::Write(writer);
}

void StringLengthFunction::Read(SymbolReader& reader)
{
    FunctionSymbol::Read(reader);
}

void StringLengthFunction::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(false, "string length is compile time function only");
}

std::unique_ptr<Value> StringLengthFunction::ConstructValue(const std::vector<std::unique_ptr<Value>>& argumentValues, const Span& span, Value* receiver) const
{
    if (argumentValues.size() == 0)
    {
        if (receiver && receiver->IsScopedValue())
        {
            Value* value = receiver->GetSubject();
            if (value->IsStringReferenceValue())
            {
                value = value->GetSubject();
            }
            if (value)
            {
                switch (value->GetValueType())
                {
                    case ValueType::stringValue:
                    {
                        StringValue* stringValue = static_cast<StringValue*>(value);
                        return std::unique_ptr<Value>(new LongValue(span, stringValue->Str().length()));
                    }
                    case ValueType::wstringValue:
                    {
                        WStringValue* stringValue = static_cast<WStringValue*>(value);
                        return std::unique_ptr<Value>(new LongValue(span, stringValue->Str().length()));
                    }
                    case ValueType::ustringValue:
                    {
                        UStringValue* stringValue = static_cast<UStringValue*>(value);
                        return std::unique_ptr<Value>(new LongValue(span, stringValue->Str().length()));
                    }
                }
            }
        }
    }
    return std::unique_ptr<Value>();
}

} } // namespace cmajor::symbols
