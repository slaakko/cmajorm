// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/NamespaceSymbol.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/SymbolTable.hpp>

namespace cmajor { namespace symbols {

NamespaceSymbol::NamespaceSymbol(const Span& span_, const std::u32string& name_) : ContainerSymbol(SymbolType::namespaceSymbol, span_, name_)
{
}

void NamespaceSymbol::Import(NamespaceSymbol* that, SymbolTable& symbolTable)
{
    cmajor::symbols::SpanMapper spanMapper;
    Span thatSpan = that->GetSpan();
    Span span = spanMapper.MapSpan(thatSpan, GetRootModuleForCurrentThread()->Id());
    //NamespaceSymbol* ns = symbolTable.BeginNamespace(that->Name(), that->GetSpan());
    NamespaceSymbol* ns = symbolTable.BeginNamespace(that->Name(), span);
    symbolTable.MapNs(that, ns);
    for (const std::unique_ptr<Symbol>& symbol : that->Members())
    {
        if (symbol->GetSymbolType() == SymbolType::namespaceSymbol)
        {
            NamespaceSymbol* thatNs = static_cast<NamespaceSymbol*>(symbol.get());
            Import(thatNs, symbolTable);
        }
        else 
        {
            if (symbol->GetSymbolType() == SymbolType::functionGroupSymbol)
            {
                continue;
            }
            if (symbol->GetSymbolType() == SymbolType::conceptGroupSymbol)
            {
                continue;
            }
            if (symbol->GetSymbolType() == SymbolType::classGroupTypeSymbol)
            {
                continue;
            }
            if (symbol->GetSymbolType() == SymbolType::globalVariableGroupSymbol)
            {
                continue;
            }
            symbolTable.Container()->AddOwnedMember(symbol.get());
        }
    }
    symbolTable.EndNamespace();
}

std::unique_ptr<sngxml::dom::Element> NamespaceSymbol::CreateDomElement(TypeMap& typeMap)
{
    if (HasProjectMembers() && Access() != SymbolAccess::private_)
    {
        return std::unique_ptr<sngxml::dom::Element>(new sngxml::dom::Element(U"NamespaceSymbol"));
    }
    else
    {
        return std::unique_ptr<sngxml::dom::Element>();
    }
}

} } // namespace cmajor::symbols
