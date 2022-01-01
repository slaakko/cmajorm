// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_TYPE_MAP_INCLUDED
#define CMAJOR_SYMBOLS_TYPE_MAP_INCLUDED
#include <cmajor/symbols/SymbolsApi.hpp>
#include <sngxml/dom/Element.hpp>
#include <unordered_map>

namespace cmajor { namespace symbols {

class TypeSymbol;

class SYMBOLS_API TypeMap
{
public:
    TypeMap();
    TypeMap(const TypeMap&) = delete;
    TypeMap& operator=(const TypeMap&) = delete;
    int GetOrInsertType(TypeSymbol* type);
    std::vector<std::unique_ptr<sngxml::dom::Element>> TypeElements();
private:
    int nextClassTemplateSpecializationId;
    int nextOtherTypeId;
    std::unordered_map<TypeSymbol*, int> typeMap;
    std::map<int, std::unique_ptr<sngxml::dom::Element>> typeIdTypeElementMap;
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_TYPE_MAP_INCLUDED
