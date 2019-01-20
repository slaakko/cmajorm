// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_TYPEDEF_SYMBOL_INCLUDED
#define CMAJOR_SYMBOLS_TYPEDEF_SYMBOL_INCLUDED
#include <cmajor/symbols/Symbol.hpp>

namespace cmajor { namespace symbols {

class TypedefSymbol : public Symbol
{
public:
    TypedefSymbol(const Span& span_, const std::u32string& name_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    bool IsExportSymbol() const override;
    void Accept(SymbolCollector* collector) override;
    void Dump(CodeFormatter& formatter) override;
    std::string TypeString() const override { return "typedef"; }
    std::string Syntax() const override;
    void SetSpecifiers(Specifiers specifiers);
    const TypeSymbol* GetType() const { return type; }
    TypeSymbol* GetType() { return type; }
    void SetType(TypeSymbol* typeSymbol) { type = typeSymbol; }
    std::unique_ptr<dom::Element> CreateDomElement(TypeMap& typeMap) override;
    std::u32string Info() const override { return Name(); }
    const char* ClassName() const override { return "TypedefSymbol"; }
    void Check() override;
private:
    TypeSymbol* type;
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_TYPEDEF_SYMBOL_INCLUDED
