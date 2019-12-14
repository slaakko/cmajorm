// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/Interface.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <sngcm/ast/Visitor.hpp>

namespace sngcm { namespace ast {

InterfaceNode::InterfaceNode(const Span& span_) : Node(NodeType::interfaceNode, span_), specifiers(), id(), members()
{
}

InterfaceNode::InterfaceNode(const Span& span_, Specifiers specifiers_, IdentifierNode* id_, Attributes* attributes_) : 
    Node(NodeType::interfaceNode, span_), specifiers(specifiers_), id(id_), members(), attributes(attributes_)
{
    id->SetParent(this);
}

Node* InterfaceNode::Clone(CloneContext& cloneContext) const
{
    Attributes* clonedAttributes = nullptr;
    if (attributes)
    {
        clonedAttributes = attributes->Clone();
    }
    InterfaceNode* clone = new InterfaceNode(GetSpan(), specifiers, static_cast<IdentifierNode*>(id->Clone(cloneContext)), clonedAttributes);
    int n = members.Count();
    for (int i = 0; i < n; ++i)
    {
        clone->AddMember(members[i]->Clone(cloneContext));
    }
    clone->SetSpecifierSpan(specifierSpan);
    clone->SetBeginBraceSpan(beginBraceSpan);
    clone->SetEndBraceSpan(endBraceSpan);
    return clone;
}

void InterfaceNode::Accept(Visitor& visitor) 
{
    visitor.Visit(*this);
}

void InterfaceNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    bool hasAttributes = attributes != nullptr;
    writer.GetBinaryWriter().Write(hasAttributes);
    if (hasAttributes)
    {
        attributes->Write(writer);
    }
    writer.Write(id.get());
    members.Write(writer);
    writer.Write(specifierSpan);
    writer.Write(beginBraceSpan);
    writer.Write(endBraceSpan);
}

void InterfaceNode::Read(AstReader& reader)
{
    Node::Read(reader);
    bool hasAttributes = reader.GetBinaryReader().ReadBool();
    if (hasAttributes)
    {
        attributes.reset(new Attributes());
        attributes->Read(reader);
    }
    id.reset(reader.ReadIdentifierNode());
    id->SetParent(this);
    members.Read(reader);
    members.SetParent(this);
    specifierSpan = reader.ReadSpan();
    beginBraceSpan = reader.ReadSpan();
    endBraceSpan = reader.ReadSpan();
}

void InterfaceNode::AddMember(Node* member)
{
    member->SetParent(this);
    members.Add(member);
}

} } // namespace sngcm::ast
