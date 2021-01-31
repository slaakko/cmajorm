// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/ConceptSymbol.hpp>
#include <cmajor/symbols/ModuleCache.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/SymbolWriter.hpp>
#include <cmajor/symbols/SymbolReader.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/SymbolCollector.hpp>
#include <cmajor/symbols/Module.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace cmajor { namespace symbols {

using namespace soulng::unicode;

ConceptGroupSymbol::ConceptGroupSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : Symbol(SymbolType::conceptGroupSymbol, span_, sourceModuleId_, name_)
{
}

void ConceptGroupSymbol::AddConcept(ConceptSymbol* conceptSymbol)
{
    Assert(conceptSymbol->GroupName() == Name(), "wrong concept group");
    int arity = conceptSymbol->Arity();
    auto it = arityConceptMap.find(arity);
    if (it != arityConceptMap.cend())
    {
        throw Exception("concept group '" + ToUtf8(FullName()) + "' already has concept with arity " + std::to_string(arity), GetSpan(), SourceModuleId());
    }
    arityConceptMap[arity] = conceptSymbol;
    conceptSymbol->SetConceptGroup(this);
}

void ConceptGroupSymbol::RemoveConcept(ConceptSymbol* conceptSymbol)
{
    int arity = conceptSymbol->Arity();
    arityConceptMap.erase(arity);
}

bool ConceptGroupSymbol::IsEmpty() const
{
    return arityConceptMap.empty();
}

ConceptSymbol* ConceptGroupSymbol::GetConcept(int arity)
{
    auto it = arityConceptMap.find(arity);
    if (it != arityConceptMap.cend())
    {
        return it->second;
    }
    else
    {
        throw Exception("concept with arity " + std::to_string(arity) + " not found from concept group '" + ToUtf8(FullName()) + "'", GetSpan(), SourceModuleId());
    }
}

bool ConceptGroupSymbol::HasProjectMembers() const
{
    for (const auto& p : arityConceptMap)
    {
        ConceptSymbol* conceptSymbol = p.second;
        if (conceptSymbol->IsProject())
        {
            return true;
        }
    }
    return false;
}

void ConceptGroupSymbol::AppendChildElements(sngxml::dom::Element* element, TypeMap& typeMap) const
{
    for (const auto& p : arityConceptMap)
    {
        ConceptSymbol* conceptSymbol = p.second;
        if (conceptSymbol->IsProject())
        {
            std::unique_ptr<sngxml::dom::Element> conceptElement = conceptSymbol->ToDomElement(typeMap);
            element->AppendChild(std::unique_ptr<sngxml::dom::Node>(conceptElement.release()));
        }
    }
}

void ConceptGroupSymbol::Check()
{
    Symbol::Check();
    for (const auto& p : arityConceptMap)
    {
        if (!p.second)
        {
            throw SymbolCheckException("concept group symbol has no concept symbol", GetSpan(), SourceModuleId());
        }
    }
}

ConceptSymbol::ConceptSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : 
    ContainerSymbol(SymbolType::conceptSymbol, span_, sourceModuleId_, name_), refinedConcept(nullptr), typeId(boost::uuids::nil_uuid()), hasSource(false), conceptGroup(nullptr)
{
}

void ConceptSymbol::Write(SymbolWriter& writer)
{
    ContainerSymbol::Write(writer);
    Assert(!typeId.is_nil(), "type id not initialized");
    writer.GetBinaryWriter().Write(typeId);
    writer.GetBinaryWriter().Write(groupName);
    boost::uuids::uuid refineConceptId = boost::uuids::nil_uuid();
    if (refinedConcept)
    {
        refineConceptId = refinedConcept->TypeId();
    }
    writer.GetBinaryWriter().Write(refineConceptId);
    uint32_t n = templateParameters.size();
    writer.GetBinaryWriter().WriteULEB128UInt(n);
    for (uint32_t i = 0; i < n; ++i)
    {
        TemplateParameterSymbol* templateParameter = templateParameters[i];
        Assert(!templateParameter->TypeId().is_nil(), "type id not initialized");
        writer.GetBinaryWriter().Write(templateParameter->TypeId());
    }
    Node* node = GetRootModuleForCurrentThread()->GetSymbolTable().GetNode(this);
    Assert(node->IsConceptNode(), "concept node expected");
    writer.GetAstWriter().Write(node);
    writer.GetBinaryWriter().Write(hasSource);
}

void ConceptSymbol::Read(SymbolReader& reader)
{
    ContainerSymbol::Read(reader);
    reader.GetBinaryReader().ReadUuid(typeId);
    reader.GetSymbolTable()->AddTypeOrConceptSymbolToTypeIdMap(this);
    groupName = reader.GetBinaryReader().ReadUtf32String();
    boost::uuids::uuid refinedConcepId;
    reader.GetBinaryReader().ReadUuid(refinedConcepId);
    if (!refinedConcepId.is_nil())
    {
        reader.GetSymbolTable()->EmplaceConceptRequest(reader, this, refinedConcepId);
    }
    uint32_t n = reader.GetBinaryReader().ReadULEB128UInt();
    templateParameters.resize(n);
    for (uint32_t i = 0; i < n; ++i)
    {
        boost::uuids::uuid templateParameterId;
        reader.GetBinaryReader().ReadUuid(templateParameterId);
        reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, templateParameterId, i);
    }
    conceptNode.reset(reader.GetAstReader().ReadConceptNode());
    hasSource = reader.GetBinaryReader().ReadBool();
}

void ConceptSymbol::EmplaceConcept(ConceptSymbol* conceptSymbol)
{
    refinedConcept = conceptSymbol;
}

void ConceptSymbol::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    if (index >= 0 && index < templateParameters.size())
    {
        if (typeSymbol->GetSymbolType() == SymbolType::templateParameterSymbol)
        {
            TemplateParameterSymbol* templateParameter = static_cast<TemplateParameterSymbol*>(typeSymbol);
            templateParameters[index] = templateParameter;
        }
        else
        {
            throw Exception("invalid emplace type", GetSpan(), SourceModuleId());
        }
    }
    else
    {
        throw Exception("invalid emplace type index", GetSpan(), SourceModuleId());
    }
}

void ConceptSymbol::Accept(SymbolCollector* collector)
{
    if (IsProject() && Access() == SymbolAccess::public_)
    {
        collector->AddConcept(this);
    }
}

void ConceptSymbol::Dump(CodeFormatter& formatter)
{
    formatter.WriteLine(ToUtf8(Name()));
    formatter.WriteLine("group name: " + ToUtf8(groupName));
    formatter.WriteLine("full name: " + ToUtf8(FullNameWithSpecifiers()));
    formatter.WriteLine("mangled name: " + ToUtf8(MangledName()));
    formatter.WriteLine("typeid: " + boost::uuids::to_string(typeId));
    if (refinedConcept)
    {
        formatter.WriteLine("refined concept: " + ToUtf8(refinedConcept->FullName()));
    }
}

void ConceptSymbol::AddMember(Symbol* member)
{
    ContainerSymbol::AddMember(member);
    if (member->GetSymbolType() == SymbolType::templateParameterSymbol)
    {
        templateParameters.push_back(static_cast<TemplateParameterSymbol*>(member));
    }
}

void ConceptSymbol::ComputeName()
{
    std::u32string name = groupName;
    bool first = true;
    name.append(1, '<');
    for (TemplateParameterSymbol* templateParameter : templateParameters)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            name.append(U", ");
        }
        name.append(templateParameter->Name());
    }
    name.append(1, '>');
    SetName(name);
    ComputeMangledName();
}

void ConceptSymbol::SetSpecifiers(Specifiers specifiers)
{
    Specifiers accessSpecifiers = specifiers & Specifiers::access_;
    SetAccess(accessSpecifiers);
    if ((specifiers & Specifiers::static_) != Specifiers::none)
    {
        throw Exception("concept symbol cannot be static", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::virtual_) != Specifiers::none)
    {
        throw Exception("concept symbol cannot be virtual", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::override_) != Specifiers::none)
    {
        throw Exception("concept symbol cannot be override", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::abstract_) != Specifiers::none)
    {
        throw Exception("concept symbol cannot be abstract", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::inline_) != Specifiers::none)
    {
        throw Exception("concept symbol cannot be inline", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::explicit_) != Specifiers::none)
    {
        throw Exception("concept symbol cannot be explicit", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::external_) != Specifiers::none)
    {
        throw Exception("concept symbol cannot be external", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::suppress_) != Specifiers::none)
    {
        throw Exception("concept symbol cannot be suppressed", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::default_) != Specifiers::none)
    {
        throw Exception("concept symbol cannot be default", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::constexpr_) != Specifiers::none)
    {
        throw Exception("concept symbol cannot be constexpr", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::cdecl_) != Specifiers::none)
    {
        throw Exception("concept symbol cannot be decl", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::nothrow_) != Specifiers::none)
    {
        throw Exception("concept symbol cannot be nothrow", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::throw_) != Specifiers::none)
    {
        throw Exception("concept symbol cannot be throw", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::new_) != Specifiers::none)
    {
        throw Exception("concept symbol cannot be new", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::const_) != Specifiers::none)
    {
        throw Exception("concept symbol cannot be const", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::unit_test_) != Specifiers::none)
    {
        throw Exception("concept symbol cannot be unit_test", GetSpan(), SourceModuleId());
    }
}

void ConceptSymbol::Check()
{
    ContainerSymbol::Check();
    if (typeId.is_nil())
    {
        throw SymbolCheckException("concept symbol has empty type id", GetSpan(), SourceModuleId());
    }
    if (groupName.empty())
    {
        throw SymbolCheckException("concept symbol has empty group name", GetSpan(), SourceModuleId());
    }
    for (TemplateParameterSymbol* templateParameter : templateParameters)
    {
        if (!templateParameter)
        {
            throw SymbolCheckException("concept symbol has no template parameter", GetSpan(), SourceModuleId());
        }
    }
}

std::unique_ptr<Symbol> ConceptSymbol::RemoveFromParent()
{
    std::unique_ptr<Symbol> symbol = ContainerSymbol::RemoveFromParent();
    if (conceptGroup)
    {
        conceptGroup->RemoveConcept(this);
        if (conceptGroup->IsEmpty())
        {
            std::unique_ptr<Symbol> conceptGroupSymbol = conceptGroup->RemoveFromParent();
        }
    }
    return symbol;
}

AxiomSymbol::AxiomSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId, const std::u32string& name_) : 
    ContainerSymbol(SymbolType::axiomSymbol, span_, sourceModuleId, name_)
{
}

} } // namespace cmajor::symbols
