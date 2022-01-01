// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/Constant.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <sngcm/ast/Visitor.hpp>
#include <sngcm/ast/AstWriter.hpp>
#include <sngcm/ast/AstReader.hpp>

namespace sngcm { namespace ast {

ConstantNode::ConstantNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::constantNode, span_, moduleId_), specifiers(Specifiers::none)
{
}

ConstantNode::ConstantNode(const Span& span_, const boost::uuids::uuid& moduleId_, Specifiers specifiers_, Node* typeExpr_, IdentifierNode* id_, Node* value_) :
    Node(NodeType::constantNode, span_, moduleId_), specifiers(specifiers_), typeExpr(typeExpr_), id(id_), value(value_)
{
    typeExpr->SetParent(this);
    id->SetParent(this);
    if (value)
    {
        value->SetParent(this);
    }
}

Node* ConstantNode::Clone(CloneContext& cloneContext) const
{
    Node* clonedValue = nullptr; 
    if (value)
    {
        clonedValue = value->Clone(cloneContext);
    }
    ConstantNode* clone = new ConstantNode(GetSpan(), ModuleId(), specifiers, typeExpr->Clone(cloneContext), static_cast<IdentifierNode*>(id->Clone(cloneContext)), clonedValue);
    return clone;
}

void ConstantNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void ConstantNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.Write(specifiers);
    writer.Write(typeExpr.get());
    writer.Write(id.get());
    bool hasValue = value != nullptr;
    writer.GetBinaryWriter().Write(hasValue);
    if (hasValue)
    {
        writer.Write(value.get());
    }
    writer.GetBinaryWriter().Write(strValue);
}

void ConstantNode::Read(AstReader& reader)
{
    Node::Read(reader);
    specifiers = reader.ReadSpecifiers();
    typeExpr.reset(reader.ReadNode());
    typeExpr->SetParent(this);
    id.reset(reader.ReadIdentifierNode());
    id->SetParent(this);
    bool hasValue = reader.GetBinaryReader().ReadBool();
    if (hasValue)
    {
        value.reset(reader.ReadNode());
        value->SetParent(this);
    }
    strValue = reader.GetBinaryReader().ReadUtf32String();
}

} } // namespace sngcm::ast
