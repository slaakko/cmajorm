// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/ast/Attribute.hpp>
#include <cmajor/ast/AstWriter.hpp>
#include <cmajor/ast/AstReader.hpp>
#include <cmajor/util/Unicode.hpp>

namespace cmajor { namespace ast {

using namespace cmajor::unicode;

Attribute::Attribute() : span(), name(), value()
{
}

Attribute::Attribute(const Span& span_, const std::u32string& name_, const std::u32string& value_) : span(span_), name(name_), value(value_)
{
}

void Attribute::Write(AstWriter& writer)
{
    writer.Write(span);
    writer.GetBinaryWriter().Write(name);
    writer.GetBinaryWriter().Write(value);
}

void Attribute::Read(AstReader& reader)
{
    span = reader.ReadSpan();
    name = reader.GetBinaryReader().ReadUtf32String();
    value = reader.GetBinaryReader().ReadUtf32String();
}

Attribute* Attributes::GetAttribute(const std::u32string& name) const
{
    auto it = attributeMap.find(name);
    if (it != attributeMap.cend())
    {
        return it->second;
    }
    return nullptr;
}

void Attributes::AddAttribute(const Span& span, const std::u32string& name)
{
    AddAttribute(span, name, U"true");
}

void Attributes::AddAttribute(const Span& span, const std::u32string& name, const std::u32string& value)
{
    Attribute* prev = GetAttribute(name);
    if (prev != nullptr)
    {
        throw AttributeNotUniqueException("attribute '" + ToUtf8(name) + "' not unique", span, prev->GetSpan());
    }
    Attribute* attribute = new Attribute(span, name, value);
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
        clone->AddAttribute(attribute->GetSpan(), attribute->Name(), attribute->Value());
    }
    return clone.release();
}

void Attributes::Write(AstWriter& writer)
{
    writer.GetBinaryWriter().WriteULEB128UInt(attributes.size());
    for (const std::unique_ptr<Attribute>& attribute : attributes)
    {
        attribute->Write(writer);
    }
}

void Attributes::Read(AstReader& reader)
{
    uint32_t n = reader.GetBinaryReader().ReadULEB128UInt();
    for (uint32_t i = 0; i < n; ++i)
    {
        Attribute* attribute = new Attribute();
        attribute->Read(reader);
        AddAttribute(attribute);
    }
}

AttributeNotUniqueException::AttributeNotUniqueException(const std::string& message_, const Span& span_, const Span& prevSpan_) : std::runtime_error(message_), span(span_), prevSpan(prevSpan_)
{
}

} } // namespace cmajor::ast
