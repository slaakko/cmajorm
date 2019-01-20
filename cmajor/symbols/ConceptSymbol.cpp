// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/ConceptSymbol.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/SymbolWriter.hpp>
#include <cmajor/symbols/SymbolReader.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/SymbolCollector.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/util/Unicode.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace cmajor { namespace symbols {

using namespace cmajor::unicode;

ConceptGroupSymbol::ConceptGroupSymbol(const Span& span_, const std::u32string& name_) : Symbol(SymbolType::conceptGroupSymbol, span_, name_)
{
}

void ConceptGroupSymbol::AddConcept(ConceptSymbol* concept)
{
    Assert(concept->GroupName() == Name(), "wrong concept group");
    int arity = concept->Arity();
    auto it = arityConceptMap.find(arity);
    if (it != arityConceptMap.cend())
    {
        throw Exception(GetRootModuleForCurrentThread(), "concept group '" + ToUtf8(FullName()) + "' already has concept with arity " + std::to_string(arity), GetSpan());
    }
    arityConceptMap[arity] = concept;
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
        throw Exception(GetRootModuleForCurrentThread(), "concept with arity " + std::to_string(arity) + " not found from concept group '" + ToUtf8(FullName()) + "'", GetSpan());
    }
}

bool ConceptGroupSymbol::HasProjectMembers() const
{
    for (const auto& p : arityConceptMap)
    {
        ConceptSymbol* concept = p.second;
        if (concept->IsProject())
        {
            return true;
        }
    }
    return false;
}

void ConceptGroupSymbol::AppendChildElements(dom::Element* element, TypeMap& typeMap) const
{
    for (const auto& p : arityConceptMap)
    {
        ConceptSymbol* concept = p.second;
        if (concept->IsProject())
        {
            std::unique_ptr<dom::Element> conceptElement = concept->ToDomElement(typeMap);
            element->AppendChild(std::unique_ptr<dom::Node>(conceptElement.release()));
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
            throw SymbolCheckException(GetRootModuleForCurrentThread(), "concept group symbol has no concept symbol", GetSpan());
        }
    }
}

ConceptSymbol::ConceptSymbol(const Span& span_, const std::u32string& name_) : ContainerSymbol(SymbolType::conceptSymbol, span_, name_), refinedConcept(nullptr), 
    typeId(boost::uuids::nil_generator()()), hasSource(false)
{
}

void ConceptSymbol::Write(SymbolWriter& writer)
{
    ContainerSymbol::Write(writer);
    Assert(!typeId.is_nil(), "type id not initialized");
    writer.GetBinaryWriter().Write(typeId);
    writer.GetBinaryWriter().Write(groupName);
    boost::uuids::uuid refineConceptId = boost::uuids::nil_generator()();
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

void ConceptSymbol::EmplaceConcept(ConceptSymbol* concept)
{
    refinedConcept = concept;
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
            throw Exception(GetRootModuleForCurrentThread(), "invalid emplace type", GetSpan());
        }
    }
    else
    {
        throw Exception(GetRootModuleForCurrentThread(), "invalid emplace type index", GetSpan());
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
        throw Exception(GetRootModuleForCurrentThread(), "concept symbol cannot be static", GetSpan());
    }
    if ((specifiers & Specifiers::virtual_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "concept symbol cannot be virtual", GetSpan());
    }
    if ((specifiers & Specifiers::override_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "concept symbol cannot be override", GetSpan());
    }
    if ((specifiers & Specifiers::abstract_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "concept symbol cannot be abstract", GetSpan());
    }
    if ((specifiers & Specifiers::inline_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "concept symbol cannot be inline", GetSpan());
    }
    if ((specifiers & Specifiers::explicit_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "concept symbol cannot be explicit", GetSpan());
    }
    if ((specifiers & Specifiers::external_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "concept symbol cannot be external", GetSpan());
    }
    if ((specifiers & Specifiers::suppress_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "concept symbol cannot be suppressed", GetSpan());
    }
    if ((specifiers & Specifiers::default_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "concept symbol cannot be default", GetSpan());
    }
    if ((specifiers & Specifiers::constexpr_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "concept symbol cannot be constexpr", GetSpan());
    }
    if ((specifiers & Specifiers::cdecl_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "concept symbol cannot be decl", GetSpan());
    }
    if ((specifiers & Specifiers::nothrow_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "concept symbol cannot be nothrow", GetSpan());
    }
    if ((specifiers & Specifiers::throw_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "concept symbol cannot be throw", GetSpan());
    }
    if ((specifiers & Specifiers::new_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "concept symbol cannot be new", GetSpan());
    }
    if ((specifiers & Specifiers::const_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "concept symbol cannot be const", GetSpan());
    }
    if ((specifiers & Specifiers::unit_test_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "concept symbol cannot be unit_test", GetSpan());
    }
}

void ConceptSymbol::Check()
{
    ContainerSymbol::Check();
    if (typeId.is_nil())
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "concept symbol has empty type id", GetSpan());
    }
    if (groupName.empty())
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "concept symbol has empty group name", GetSpan());
    }
    for (TemplateParameterSymbol* templateParameter : templateParameters)
    {
        if (!templateParameter)
        {
            throw SymbolCheckException(GetRootModuleForCurrentThread(), "concept symbol has no template parameter", GetSpan());
        }
    }
}

} } // namespace cmajor::symbols
