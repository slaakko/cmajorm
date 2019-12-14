// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/TypeMap.hpp>
#include <cmajor/symbols/TypeSymbol.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace symbols {

using namespace soulng::unicode;

TypeMap::TypeMap() : nextClassTemplateSpecializationId(1), nextOtherTypeId(1000000)
{
}

int TypeMap::GetOrInsertType(TypeSymbol* type)
{
    auto it = typeMap.find(type);
    if (it != typeMap.cend())
    {
        return it->second;
    }
    else
    {
        int typeId = 0;
        if (type->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            typeId = nextClassTemplateSpecializationId++;
        }
        else
        {
            typeId = nextOtherTypeId++;
        }
        typeMap[type] = typeId;
        std::unique_ptr<sngxml::dom::Element> typeElement(new sngxml::dom::Element(U"type"));
        typeElement->SetAttribute(U"id", U"type_" + ToUtf32(std::to_string(typeId)));
        typeElement->AppendChild(std::move(type->ToDomElement(*this)));
        typeIdTypeElementMap[typeId] = std::move(typeElement);
        return typeId;
    }
}

std::vector<std::unique_ptr<sngxml::dom::Element>> TypeMap::TypeElements()
{
    std::vector<std::unique_ptr<sngxml::dom::Element>> typeElements;
    for (auto& p : typeIdTypeElementMap)
    {
        typeElements.push_back(std::move(p.second));
    }
    return typeElements;
}

} } // namespace cmajor::symbols
