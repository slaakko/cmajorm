// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_TEMPLATE_SYMBOL_INCLUDED
#define CMAJOR_SYMBOLS_TEMPLATE_SYMBOL_INCLUDED
#include <cmajor/symbols/TypeSymbol.hpp>

namespace cmajor { namespace symbols {

class SYMBOLS_API TemplateParameterSymbol : public TypeSymbol
{
public:
    TemplateParameterSymbol(const Span& span_, const std::u32string& name_);
    std::u32string FullName() const override { return Name(); }
    void Write(SymbolWriter& writer);
    void Read(SymbolReader& reader);
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    void* IrType(Emitter& emitter) override { Assert(false, "tried to get ir type of template parameter"); return nullptr; }
    void* CreateDefaultIrValue(Emitter& emitter) override { Assert(false, "tried to create defualt ir value of template parameter"); return nullptr; }
    TypeSymbol* Unify(TypeSymbol* type, const Span& span) override;
    bool ContainsTemplateParameter() const override { return true; }
    bool HasDefault() const { return hasDefault; }
    void SetHasDefault() { hasDefault = true; }
    void SetDefaultType(TypeSymbol* defaultType_) { defaultType = defaultType_; }
    TypeSymbol* DefaultType() { return defaultType; }
    TypeSymbol* UnifyTemplateArgumentType(SymbolTable& symbolTable, const std::unordered_map<TemplateParameterSymbol*, TypeSymbol*>& templateParameterMap, const Span& span) override;
    std::u32string Info() const override { return Name(); }
    const char* ClassName() const override { return "TemplateParameterSymbol"; }
private:
    bool hasDefault;
    TypeSymbol* defaultType;
};

class SYMBOLS_API BoundTemplateParameterSymbol : public Symbol
{
public:
    BoundTemplateParameterSymbol(const Span& span_, const std::u32string& name_);
    std::u32string FullName() const override { return Name(); }
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    TypeSymbol* GetType() const { return type; }
    void SetType(TypeSymbol* type_) { type = type_; }
    std::unique_ptr<sngxml::dom::Element> CreateDomElement(TypeMap& typeMap) override;
    std::u32string Info() const override { return Name(); }
    const char* ClassName() const override { return "BoundTemplateParameterSymbol"; }
    void Check() override;
private:
    TypeSymbol* type;
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_TEMPLATE_SYMBOL_INCLUDED
