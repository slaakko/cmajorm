// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/ConversionTable.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <cmajor/symbols/ClassTemplateSpecializationSymbol.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/DebugFlags.hpp>

namespace cmajor { namespace symbols {

ConversionTable::ConversionTable(Module* module_) : module(module_)
{
}

void ConversionTableEntry::CheckValid() const
{
    if (!sourceType || !targetType)
    {
        throw std::runtime_error("invalid conversion table entry: source type is null or target type is null");
    }
    if (!sourceType->BaseType() || !targetType->BaseType())
    {
        throw std::runtime_error("invalid conversion table entry: source base type is null or target base type is null");
    }
}

void ConversionTable::AddConversion(FunctionSymbol* conversion, Module* module)
{
#ifdef IMMUTABLE_MODULE_CHECK
    if (module && module->IsImmutable())
    {
        throw ModuleImmutableException(GetRootModuleForCurrentThread(), module, Span(), Span());
    }
#endif
    ConversionTableEntry entry(conversion->ConversionSourceType()->PlainType(conversion->GetSpan(), module),
        conversion->ConversionTargetType()->PlainType(conversion->GetSpan(), module));
#ifdef VALID_CONVERSION_TABLE_ENTRY_CHECK
    entry.CheckValid();
#endif
    conversionMap.erase(entry);
    conversionMap.insert(std::make_pair(entry, conversion));
}

void ConversionTable::AddConversion(FunctionSymbol* conversion)
{
    AddConversion(conversion, nullptr);
}

void ConversionTable::Add(const ConversionTable& that)
{
#ifdef IMMUTABLE_MODULE_CHECK
    if (module && module->IsImmutable())
    {
        throw ModuleImmutableException(GetRootModuleForCurrentThread(), module, Span(), Span());
    }
#endif
    for (const auto& p : that.conversionMap)
    {
        ConversionTableEntry entry(p.first);
#ifdef VALID_CONVERSION_TABLE_ENTRY_CHECK
        entry.CheckValid();
#endif
        conversionMap[entry] = p.second;
    }
}

void ConversionTable::Check()
{
    for (const auto& p : conversionMap)
    {
        const ConversionTableEntry& entry = p.first;
        entry.sourceType->SetFlag(SymbolFlags::inConversionTable);
        entry.sourceType->BaseType()->SetFlag(SymbolFlags::inConversionTable);
        entry.targetType->SetFlag(SymbolFlags::inConversionTable);
        entry.targetType->BaseType()->SetFlag(SymbolFlags::inConversionTable);
        entry.CheckValid();
    }
}

FunctionSymbol* ConversionTable::GetConversion(TypeSymbol* sourceType, TypeSymbol* targetType, const Span& span) const
{
    TypeSymbol* sourcePlainType = sourceType->PlainType(span);
    TypeSymbol* targetPlainType = targetType->PlainType(span);
    ConversionTableEntry entry(sourcePlainType, targetPlainType);
    auto it = conversionMap.find(entry);
    if (it != conversionMap.cend())
    {
        return it->second;
    }
    return nullptr;
}

void ConversionTable::AddGeneratedConversion(std::unique_ptr<FunctionSymbol>&& generatedConversion)
{
    generatedConversions.push_back(std::move(generatedConversion));
}

} } // namespace cmajor::symbols
