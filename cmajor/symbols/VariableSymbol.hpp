// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_VARIABLE_SYMBOL_INCLUDED
#define CMAJOR_SYMBOLS_VARIABLE_SYMBOL_INCLUDED
#include <cmajor/symbols/Value.hpp>
#include <cmajor/symbols/TypeMap.hpp>
#include <cmajor/ir/Emitter.hpp>

namespace cmajor { namespace symbols {

using namespace cmajor::ir;

class SYMBOLS_API VariableSymbol : public Symbol
{
public:
    VariableSymbol(SymbolType symbolType_, const Span& span_, const std::u32string& name_);
    bool IsVariableSymbol() const override { return true; }
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    const TypeSymbol* GetType() const { return type; }
    TypeSymbol* GetType() { return type; }
    void SetType(TypeSymbol* typeSymbol) { type = typeSymbol; }
    std::u32string Info() const override { return Name(); }
    const char* ClassName() const override { return "VariableSymbol"; }
    void Check() override;
private:
    TypeSymbol* type;
};

class SYMBOLS_API ParameterSymbol : public VariableSymbol
{
public:    
    ParameterSymbol(const Span& span_, const std::u32string& name_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    std::string TypeString() const override { return "parameter"; }
    std::unique_ptr<dom::Element> CreateDomElement(TypeMap& typeMap) override;
    const char* ClassName() const override { return "ParameterSymbol"; }
    bool ArtificialName() const { return artificialName; }
    void SetArtificialName() { artificialName = true; }
    std::u32string CodeName() const override;
private:
    bool artificialName;
};

class SYMBOLS_API LocalVariableSymbol : public VariableSymbol
{
public:     
    LocalVariableSymbol(const Span& span_, const std::u32string& name_);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    bool IsExportSymbol() const override { return false; }
    std::unique_ptr<dom::Element> CreateDomElement(TypeMap& typeMap) override;
    const char* ClassName() const override { return "LocalVariableSymbol"; }
};

class SYMBOLS_API MemberVariableSymbol : public VariableSymbol
{
public:
    MemberVariableSymbol(const Span& span_, const std::u32string& name_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    bool IsExportSymbol() const override;
    void Accept(SymbolCollector* collector) override;
    void Dump(CodeFormatter& formatter) override;
    std::string TypeString() const override { return "member_variable"; }
    std::string Syntax() const override;
    void SetSpecifiers(Specifiers specifiers);
    int32_t LayoutIndex() const { return layoutIndex; }
    void SetLayoutIndex(int32_t layoutIndex_) { layoutIndex = layoutIndex_; }
    void* GetDIMemberType(Emitter& emitter, uint64_t offsetInBits);
    std::unique_ptr<dom::Element> CreateDomElement(TypeMap& typeMap) override;
    const char* ClassName() const override { return "MemberVariableSymbol"; }
    void Check() override;
private:
    int32_t layoutIndex;
};

class GlobalVariableSymbol;

class SYMBOLS_API GlobalVariableGroupSymbol : public Symbol
{
public:
    GlobalVariableGroupSymbol(const Span& span_, const std::u32string& name_);
    bool IsExportSymbol() const override { return false; }
    std::string TypeString() const override { return "global_variable_group"; }
    void ComputeMangledName() override;
    void AddGlobalVariable(GlobalVariableSymbol* globalVariableSymbol);
    std::u32string Info() const override { return Name(); }
    const char* ClassName() const override { return "GlobalVariableGroupSymbol"; }
    void CollectGlobalVariables(const std::string& compileUnitFilePath, std::vector<GlobalVariableSymbol*>& globalVariables) const;
private:
    std::vector<std::pair<GlobalVariableSymbol*, std::string>> globalVariableSymbols;
};

class SYMBOLS_API GlobalVariableSymbol : public VariableSymbol
{
public:
    GlobalVariableSymbol(const Span& span_, const std::u32string& groupName_, const std::string& compileUnitId, const std::string& compileUnitFilePath_);
    GlobalVariableSymbol(const Span& span_, const std::u32string& name_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    bool IsExportSymbol() const override;
    bool IsGlobalVariableSymbol() const override { return true; }
    void Accept(SymbolCollector* collector) override;
    void Dump(CodeFormatter& formatter) override;
    std::string TypeString() const override { return "global_variable"; }
    void ComputeMangledName() override;
    std::string Syntax() const override;
    void SetSpecifiers(Specifiers specifiers);
    const char* ClassName() const override { return "GlobalVariableSymbol"; }
    void SetInitializer(std::unique_ptr<Value>&& initializer_);
    Value* Initializer() const { return initializer.get(); }
    void* IrObject(Emitter& emitter) override;
    void CreateIrObject(Emitter& emitter);
    const std::u32string& GroupName() const { return groupName; }
    const std::string& CompileUnitFilePath() const { return compileUnitFilePath; }
private:
    std::u32string groupName;
    std::string compileUnitFilePath;
    std::unique_ptr<Value> initializer;
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_VARIABLE_SYMBOL_INCLUDED