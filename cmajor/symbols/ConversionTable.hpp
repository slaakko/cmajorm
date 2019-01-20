// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_CONVERSION_TABLE_INCLUDED
#define CMAJOR_SYMBOLS_CONVERSION_TABLE_INCLUDED
#include <cmajor/symbols/TypeSymbol.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <boost/functional/hash.hpp>

namespace cmajor { namespace symbols {

class Module;

struct ConversionTableEntry
{
    ConversionTableEntry(TypeSymbol* sourceType_, TypeSymbol* targetType_) : sourceType(sourceType_), targetType(targetType_) {}
    void CheckValid() const;
    TypeSymbol* sourceType;
    TypeSymbol* targetType;
};

inline bool operator==(const ConversionTableEntry& left, const ConversionTableEntry& right)
{
    return TypesEqual(left.sourceType, right.sourceType) && TypesEqual(left.targetType, right.targetType);
}

struct ConversionTableEntryHash
{
    size_t operator()(const ConversionTableEntry& entry) const 
    { 
        return boost::hash<boost::uuids::uuid>()(entry.sourceType->TypeId()) ^ boost::hash<boost::uuids::uuid>()(entry.targetType->TypeId()); 
    }
};

class ConversionTable
{
public:
    ConversionTable(Module* module_);
    void AddConversion(FunctionSymbol* conversion);
    void AddConversion(FunctionSymbol* conversion, Module* module);
    FunctionSymbol* GetConversion(TypeSymbol* sourceType, TypeSymbol* targetType, const Span& span) const;
    void AddGeneratedConversion(std::unique_ptr<FunctionSymbol>&& generatedConversion);
    void Add(const ConversionTable& that);
    void Check();
private:
    Module* module;
    std::unordered_map<ConversionTableEntry, FunctionSymbol*, ConversionTableEntryHash> conversionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> generatedConversions;
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_CONVERSION_TABLE_INCLUDED
