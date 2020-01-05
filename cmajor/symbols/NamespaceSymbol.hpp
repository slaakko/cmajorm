// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_NAMESPACE_SYMBOL_INCLUDED
#define CMAJOR_SYMBOLS_NAMESPACE_SYMBOL_INCLUDED
#include <cmajor/symbols/ContainerSymbol.hpp>

namespace cmajor { namespace symbols {

class SYMBOLS_API NamespaceSymbol : public ContainerSymbol
{
public:   
    NamespaceSymbol(const Span& span_, const std::u32string& name_);
    std::string TypeString() const override { return "namespace"; }
    std::u32string Id() const override { return U"ns_" + FullName(); }
    bool IsParentSymbol() const override { return true; }
    void Import(NamespaceSymbol* that, SymbolTable& symbolTable);
    bool IsGlobalNamespace() const { return Name().empty(); }
    std::unique_ptr<sngxml::dom::Element> CreateDomElement(TypeMap& typeMap) override;
    std::u32string Info() const override { return Name(); }
    const char* ClassName() const override { return "NamespaceSymbol"; }
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_NAMESPACE_SYMBOL_INCLUDED
