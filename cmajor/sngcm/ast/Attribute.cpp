// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/Attribute.hpp>
#include <sngcm/ast/Visitor.hpp>
#include <sngcm/ast/AstWriter.hpp>
#include <sngcm/ast/AstReader.hpp>
#include <soulng/util/Unicode.hpp>

namespace sngcm { namespace ast {

using namespace soulng::unicode;

AttributeNode::AttributeNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::attributeNode, span_, moduleId_), name(), value()
{
}

AttributeNode::AttributeNode(const Span& span_, const boost::uuids::uuid& moduleId_, const std::u32string& name_, const std::u32string& value_) :
    Node(NodeType::attributeNode, span_, moduleId_), name(name_), value(value_)
{
}

Node* AttributeNode::Clone(CloneContext& cloneContext) const
{
    return new AttributeNode(GetSpan(), ModuleId(), name, value);
}

void AttributeNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void AttributeNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.GetBinaryWriter().Write(name);
    writer.GetBinaryWriter().Write(value);
}

void AttributeNode::Read(AstReader& reader)
{
    Node::Read(reader);
    name = reader.GetBinaryReader().ReadUtf32String();
    value = reader.GetBinaryReader().ReadUtf32String();
}

AttributesNode::AttributesNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::attributesNode, span_, moduleId_)
{
}

AttributeNode* AttributesNode::GetAttribute(const std::u32string& name) const
{
    std::map<std::u32string, AttributeNode*>::const_iterator it = attributeMap.find(name);
    if (it != attributeMap.cend())
    {
        return it->second;
    }
    return nullptr;
}

void AttributesNode::AddAttribute(const Span& span, const boost::uuids::uuid& moduleId, const std::u32string& name)
{
    AddAttribute(span, moduleId, name, U"true");
}

void AttributesNode::AddAttribute(const Span& span, const boost::uuids::uuid& moduleId, const std::u32string& name, const std::u32string& value)
{
    AttributeNode* prev = GetAttribute(name);
    if (prev != nullptr)
    {
        throw AttributeNotUniqueException("attribute '" + ToUtf8(name) + "' not unique", span, moduleId, prev->GetSpan(), prev->ModuleId());
    }
    AttributeNode* attribute = new AttributeNode(span, moduleId, name, value);
    AddAttribute(attribute);
}

void AttributesNode::AddAttribute(AttributeNode* attribute)
{
    attributes.push_back(std::unique_ptr<AttributeNode>(attribute));
    attributeMap[attribute->Name()] = attribute;
}

Node* AttributesNode::Clone(CloneContext& cloneContext) const
{
    std::unique_ptr<AttributesNode> clone(new AttributesNode(GetSpan(), ModuleId()));
    for (const std::unique_ptr<AttributeNode>& attribute : attributes)
    {
        clone->AddAttribute(attribute->GetSpan(), attribute->ModuleId(), attribute->Name(), attribute->Value());
    }
    return clone.release();
}

void AttributesNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void AttributesNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.GetBinaryWriter().WriteULEB128UInt(static_cast<uint32_t>(attributes.size()));
    for (const std::unique_ptr<AttributeNode>& attribute : attributes)
    {
        writer.Write(attribute.get());
    }
}

void AttributesNode::Read(AstReader& reader)
{
    Node::Read(reader);
    uint32_t n = reader.GetBinaryReader().ReadULEB128UInt();
    for (uint32_t i = 0u; i < n; ++i)
    {
        AttributeNode* attribute = reader.ReadAttributeNode();
        AddAttribute(attribute);
    }
}

AttributeNotUniqueException::AttributeNotUniqueException(const std::string& message_, const Span& span_, const boost::uuids::uuid& moduleId_,
    const Span& prevSpan_, const boost::uuids::uuid& prevModuleId_) : std::runtime_error(message_), span(span_), moduleId(moduleId_), prevSpan(prevSpan_), prevModuleId(prevModuleId_)
{
}

} } // namespace sngcm::ast
