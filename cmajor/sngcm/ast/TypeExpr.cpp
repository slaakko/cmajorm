// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/TypeExpr.hpp>
#include <sngcm/ast/Visitor.hpp>
#include <sngcm/ast/AstWriter.hpp>
#include <sngcm/ast/AstReader.hpp>

namespace sngcm { namespace ast {

ConstNode::ConstNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::constNode, span_, moduleId_)
{
}

ConstNode::ConstNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_) : Node(NodeType::constNode, span_, moduleId_), subject(subject_)
{
    subject->SetParent(this);
}

Node* ConstNode::Clone(CloneContext& cloneContext) const
{
    ConstNode* clone = new ConstNode(GetSpan(), ModuleId(), subject->Clone(cloneContext));
    return clone;
}

void ConstNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void ConstNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.Write(subject.get());
}

void ConstNode::Read(AstReader& reader)
{
    Node::Read(reader);
    subject.reset(reader.ReadNode());
    subject->SetParent(this);
}

std::string ConstNode::ToString() const
{
    return "const " + subject->ToString();
}

LValueRefNode::LValueRefNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::lvalueRefNode, span_, moduleId_)
{
}

LValueRefNode::LValueRefNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_) : 
    Node(NodeType::lvalueRefNode, span_, moduleId_), subject(subject_)
{
    subject->SetParent(this);
}

Node* LValueRefNode::Clone(CloneContext& cloneContext) const
{
    LValueRefNode* clone = new LValueRefNode(GetSpan(), ModuleId(), subject->Clone(cloneContext));
    return clone;
}

void LValueRefNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void LValueRefNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.Write(subject.get());
}

void LValueRefNode::Read(AstReader& reader)
{
    Node::Read(reader);
    subject.reset(reader.ReadNode());
    subject->SetParent(this);
}

std::string LValueRefNode::ToString() const
{
    return subject->ToString() + "&";
}

RValueRefNode::RValueRefNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::rvalueRefNode, span_, moduleId_)
{
}

RValueRefNode::RValueRefNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_) : 
    Node(NodeType::rvalueRefNode, span_, moduleId_), subject(subject_)
{
    subject->SetParent(this);
}

Node* RValueRefNode::Clone(CloneContext& cloneContext) const
{
    RValueRefNode* clone = new RValueRefNode(GetSpan(), ModuleId(), subject->Clone(cloneContext));
    return clone;
}

void RValueRefNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void RValueRefNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.Write(subject.get());
}

void RValueRefNode::Read(AstReader& reader)
{
    Node::Read(reader);
    subject.reset(reader.ReadNode());
    subject->SetParent(this);
}

std::string RValueRefNode::ToString() const
{
    return subject->ToString() + "&&";
}

PointerNode::PointerNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::pointerNode, span_, moduleId_)
{
}

PointerNode::PointerNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_) : 
    Node(NodeType::pointerNode, span_, moduleId_), subject(subject_)
{
    subject->SetParent(this);
}

Node* PointerNode::Clone(CloneContext& cloneContext) const
{
    PointerNode* clone = new PointerNode(GetSpan(), ModuleId(), subject->Clone(cloneContext));
    return clone;
}

void PointerNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void PointerNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.Write(subject.get());
}

void PointerNode::Read(AstReader& reader)
{
    Node::Read(reader);
    subject.reset(reader.ReadNode());
    subject->SetParent(this);
}

std::string PointerNode::ToString() const
{
    return subject->ToString() + "*";
}

ArrayNode::ArrayNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::arrayNode, span_, moduleId_), subject(), size()
{
}

ArrayNode::ArrayNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_, Node* size_) : 
    Node(NodeType::arrayNode, span_, moduleId_), subject(subject_), size(size_)
{
    subject->SetParent(this);
    if (size)
    {
        size->SetParent(this);
    }
}

Node* ArrayNode::Clone(CloneContext& cloneContext) const
{
    Node* clonedSize = nullptr;
    if (size)
    {
        clonedSize = size->Clone(cloneContext);
    }
    ArrayNode* clone = new ArrayNode(GetSpan(), ModuleId(), subject->Clone(cloneContext), clonedSize);
    return clone;
}

void ArrayNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void ArrayNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.Write(subject.get());
    bool hasSize = size != nullptr;
    writer.GetBinaryWriter().Write(hasSize);
    if (hasSize)
    {
        writer.Write(size.get());
    }
}

void ArrayNode::Read(AstReader& reader)
{
    Node::Read(reader);
    subject.reset(reader.ReadNode());
    subject->SetParent(this);
    bool hasSize = reader.GetBinaryReader().ReadBool();
    if (hasSize)
    {
        size.reset(reader.ReadNode());
        size->SetParent(this);
    }
}

std::string ArrayNode::ToString() const
{
    std::string s = subject->ToString();
    s.append("[");
    if (size)
    {
        s.append(size->ToString());
    }
    s.append("]");
    return s;
}

} } // namespace sngcm::ast
