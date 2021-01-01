// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/Attribute.hpp>
#include <sngcm/ast/AstWriter.hpp>
#include <sngcm/ast/AstReader.hpp>
#include <soulng/util/Unicode.hpp>

namespace sngcm { namespace ast {

using namespace soulng::unicode;

Attribute::Attribute() : span(), moduleId(), name(), value()
{
}

Attribute::Attribute(const Span& span_, const boost::uuids::uuid& moduleId_, const std::u32string& name_, const std::u32string& value_) : 
    span(span_), moduleId(moduleId_), name(name_), value(value_)
{
}

void Attribute::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void Attribute::Write(AstWriter& writer)
{
    if (moduleId == writer.SpanConversionModuleId())
    {
        writer.Write(span, true);
    }
    else
    {
        writer.Write(span, false);
    }
    writer.GetBinaryWriter().Write(moduleId);
    writer.GetBinaryWriter().Write(name);
    writer.GetBinaryWriter().Write(value);
}

void Attribute::Read(AstReader& reader)
{
    span = reader.ReadSpan();
    reader.GetBinaryReader().ReadUuid(moduleId);
    name = reader.GetBinaryReader().ReadUtf32String();
    value = reader.GetBinaryReader().ReadUtf32String();
}

Attributes::Attributes()
{
}

Attribute* Attributes::GetAttribute(const std::u32string& name) const
{
    std::map<std::u32string, Attribute*>::const_iterator it = attributeMap.find(name);
    if (it != attributeMap.cend())
    {
        return it->second;
    }
    return nullptr;
}

void Attributes::AddAttribute(const Span& span, const boost::uuids::uuid& moduleId, const std::u32string& name)
{
    AddAttribute(span, moduleId, name, U"true");
}

void Attributes::AddAttribute(const Span& span, const boost::uuids::uuid& moduleId, const std::u32string& name, const std::u32string& value)
{
    Attribute* prev = GetAttribute(name);
    if (prev != nullptr)
    {
        throw AttributeNotUniqueException("attribute '" + ToUtf8(name) + "' not unique", span, moduleId, prev->GetSpan(), prev->ModuleId());
    }
    Attribute* attribute = new Attribute(span, moduleId, name, value);
    AddAttribute(attribute);
}

void Attributes::AddAttribute(Attribute* attribute)
{
    attributes.push_back(std::unique_ptr<Attribute>(attribute));
    attributeMap[attribute->Name()] = attribute;
}

Attributes* Attributes::Clone() const
{
    std::unique_ptr<Attributes> clone(new Attributes());
    for (const std::unique_ptr<Attribute>& attribute : attributes)
    {
        clone->AddAttribute(attribute->GetSpan(), attribute->ModuleId(), attribute->Name(), attribute->Value());
    }
    return clone.release();
}

void Attributes::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void Attributes::Write(AstWriter& writer)
{
    writer.GetBinaryWriter().WriteULEB128UInt(static_cast<uint32_t>(attributes.size()));
    for (const std::unique_ptr<Attribute>& attribute : attributes)
    {
        attribute->Write(writer);
    }
}

void Attributes::Read(AstReader& reader)
{
    uint32_t n = reader.GetBinaryReader().ReadULEB128UInt();
    for (uint32_t i = 0u; i < n; ++i)
    {
        Attribute* attribute = new Attribute();
        attribute->Read(reader);
        AddAttribute(attribute);
    }
}

AttributeNotUniqueException::AttributeNotUniqueException(const std::string& message_, const Span& span_, const boost::uuids::uuid& moduleId_,
    const Span& prevSpan_, const boost::uuids::uuid& prevModuleId_) : std::runtime_error(message_), span(span_), moduleId(moduleId_), prevSpan(prevSpan_), prevModuleId(prevModuleId_)
{
}

} } // namespace sngcm::ast
