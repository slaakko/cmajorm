// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/TemplateSymbol.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/SymbolWriter.hpp>
#include <cmajor/symbols/SymbolReader.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Sha1.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace cmajor { namespace symbols {

using namespace soulng::unicode;

TemplateParameterSymbol::TemplateParameterSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) :
    TypeSymbol(SymbolType::templateParameterSymbol, span_, sourceModuleId_, name_), hasDefault(false), defaultType(nullptr)
{
}

void TemplateParameterSymbol::Write(SymbolWriter& writer)
{
    TypeSymbol::Write(writer);
    writer.GetBinaryWriter().Write(hasDefault);
    if (hasDefault)
    {
        boost::uuids::uuid defaultTypeId = boost::uuids::nil_uuid();
        if (defaultType != nullptr)
        {
            defaultTypeId = defaultType->TypeId();
        }
        writer.GetBinaryWriter().Write(defaultTypeId);
    }
}

void TemplateParameterSymbol::Read(SymbolReader& reader)
{
    TypeSymbol::Read(reader);
    hasDefault = reader.GetBinaryReader().ReadBool();
    if (hasDefault)
    {
        boost::uuids::uuid defaultTypeId;
        reader.GetBinaryReader().ReadUuid(defaultTypeId);
        if (!defaultTypeId.is_nil())
        {
            reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, defaultTypeId, 0);
        }
    }
}

void TemplateParameterSymbol::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    Assert(index == 0, "invalid emplace type index");
    defaultType = typeSymbol;
}

TypeSymbol* TemplateParameterSymbol::Unify(TypeSymbol* type, const Span& span, const boost::uuids::uuid& moduleId)
{
    return type;
}

TypeSymbol* TemplateParameterSymbol::UnifyTemplateArgumentType(SymbolTable& symbolTable, const std::unordered_map<TemplateParameterSymbol*, TypeSymbol*>& templateParameterMap, const Span& span, const boost::uuids::uuid& moduleId)
{
    auto it = templateParameterMap.find(this);
    if (it != templateParameterMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

BoundTemplateParameterSymbol::BoundTemplateParameterSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : 
    Symbol(SymbolType::boundTemplateParameterSymbol, span_, sourceModuleId_, name_), type(nullptr)
{
}

void BoundTemplateParameterSymbol::Write(SymbolWriter& writer)
{
    Symbol::Write(writer);
    writer.GetBinaryWriter().Write(type->TypeId());
}

void BoundTemplateParameterSymbol::Read(SymbolReader& reader)
{
    Symbol::Read(reader);
    boost::uuids::uuid typeId;
    reader.GetBinaryReader().ReadUuid(typeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, typeId, 0);
}

std::unique_ptr<sngxml::dom::Element> BoundTemplateParameterSymbol::CreateDomElement(TypeMap& typeMap)
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(U"BoundTemplateParameterSymbol"));
    if (type)
    {
        std::unique_ptr<sngxml::dom::Element> typeElement(new sngxml::dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(type);
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
    }
    return element;
}

void BoundTemplateParameterSymbol::Check()
{
    Symbol::Check();
    if (!type)
    {
        throw SymbolCheckException("bound template parameter symbol contains null type pointer", GetSpan(), SourceModuleId());
    }
}

} } // namespace cmajor::symbols
