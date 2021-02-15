// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/ConversionTable.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <cmajor/symbols/ClassTemplateSpecializationSymbol.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/DebugFlags.hpp>

namespace cmajor { namespace symbols {

ConversionTableEntry::ConversionTableEntry(TypeSymbol* sourceType_, TypeSymbol* targetType_) : sourceType(sourceType_), targetType(targetType_)
{
#ifdef VALID_CONVERSION_TABLE_ENTRY_CHECK
    CheckValid();
#endif
}

ConversionTable::ConversionTable(Owner owner_, Module* module_) : owner(owner_), module(module_)
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

void ConversionTable::AddConversion(FunctionSymbol* conversion)
{
#ifdef IMMUTABLE_MODULE_CHECK
    if (module && module->IsImmutable())
    {
        throw ModuleImmutableException(GetRootModuleForCurrentThread(), module, Span(), Span());
    }
#endif
    ConversionTableEntry entry(conversion->ConversionSourceType(), conversion->ConversionTargetType());
#ifdef VALID_CONVERSION_TABLE_ENTRY_CHECK
    entry.CheckValid();
#endif
    conversionMap.erase(entry);
    conversionMap.insert(std::make_pair(entry, conversion));
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
        entry.CheckValid();
    }
}

FunctionSymbol* ConversionTable::GetConversion(TypeSymbol* sourceType, TypeSymbol* targetType, const Span& span, const boost::uuids::uuid& moduleId) const
{
    TypeSymbol* sourcePlainType = sourceType->PlainType(span, moduleId);
    TypeSymbol* targetPlainType = targetType->PlainType(span, moduleId);
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
