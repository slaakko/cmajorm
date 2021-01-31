// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_CONCEPT_SYMBOL_INCLUDED
#define CMAJOR_SYMBOLS_CONCEPT_SYMBOL_INCLUDED
#include <cmajor/symbols/ContainerSymbol.hpp>
#include <cmajor/symbols/TemplateSymbol.hpp>
#include <unordered_map>

namespace cmajor { namespace symbols {

class ConceptSymbol;

class SYMBOLS_API ConceptGroupSymbol : public Symbol
{
public:
    ConceptGroupSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    std::string TypeString() const override { return "concept_group"; }
    bool IsExportSymbol() const override { return false; }
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void AddConcept(ConceptSymbol* conceptSymbol);
    void RemoveConcept(ConceptSymbol* conceptSymbol);
    bool IsEmpty() const;
    ConceptSymbol* GetConcept(int arity);
    bool HasProjectMembers() const override;
    void AppendChildElements(sngxml::dom::Element* element, TypeMap& typeMap) const override;
    std::u32string Info() const override { return Name(); }
    const char* ClassName() const override { return "ConceptGroupSymbol"; }
    void Check() override;
private:
    std::unordered_map<int, ConceptSymbol*> arityConceptMap;
};

class SYMBOLS_API ConceptSymbol : public ContainerSymbol
{
public:
    ConceptSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    bool IsParentSymbol() const override { return true; }
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceConcept(ConceptSymbol* conceptSymbol) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    void Accept(SymbolCollector* collector) override;
    void Dump(CodeFormatter& formatter) override;
    void AddMember(Symbol* member) override;
    std::string TypeString() const override { return "concept"; }
    std::u32string SimpleName() const override { return groupName; }
    void ComputeName();
    void SetSpecifiers(Specifiers specifiers);
    void SetTypeId(const boost::uuids::uuid& typeId_) { typeId = typeId_; }
    const boost::uuids::uuid& TypeId() const { return typeId; }
    const std::u32string& GroupName() const { return groupName; }
    void SetGroupName(const std::u32string& groupName_) { groupName = groupName_; }
    std::u32string CodeName() const override { return groupName; }
    int Arity() const { return templateParameters.size(); }
    ConceptNode* GetConceptNode() { return conceptNode.get(); }
    ConceptSymbol* RefinedConcept() const { return refinedConcept; }
    void SetRefinedConcept(ConceptSymbol* refinedConcept_) { refinedConcept = refinedConcept_; }
    const std::vector<TemplateParameterSymbol*>& TemplateParameters() const { return templateParameters; }
    std::u32string Info() const override { return groupName; }
    const char* ClassName() const override { return "ConceptSymbol"; }
    bool HasSource() const { return hasSource; }
    void SetHasSource() { hasSource = true; }
    void Check() override;
    void SetConceptGroup(ConceptGroupSymbol* conceptGroup_) { conceptGroup = conceptGroup_; }
    std::unique_ptr<Symbol> RemoveFromParent() override;
private:
    boost::uuids::uuid typeId;
    std::u32string groupName;
    std::vector<TemplateParameterSymbol*> templateParameters;
    std::unique_ptr<ConceptNode> conceptNode;
    ConceptSymbol* refinedConcept;
    bool hasSource;
    ConceptGroupSymbol* conceptGroup;
};

class AxiomSymbol : public ContainerSymbol
{
public:
    AxiomSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId, const std::u32string& name_);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    bool IsExportSymbol() const override { return false; }
    std::u32string Info() const override { return Name(); }
    const char* ClassName() const override { return "AxiomSymbol"; }
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_CONCEPT_SYMBOL_INCLUDED
