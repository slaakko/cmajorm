// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/ast/TypeExpr.hpp>
#include <cmajor/ast/Visitor.hpp>
#include <cmajor/ast/AstWriter.hpp>
#include <cmajor/ast/AstReader.hpp>

namespace cmajor { namespace ast {

ConstNode::ConstNode(const Span& span_) : Node(NodeType::constNode, span_)
{
}

ConstNode::ConstNode(const Span& span_, Node* subject_) : Node(NodeType::constNode, span_), subject(subject_)
{
    subject->SetParent(this);
}

Node* ConstNode::Clone(CloneContext& cloneContext) const
{
    return new ConstNode(GetSpan(), subject->Clone(cloneContext));
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

LValueRefNode::LValueRefNode(const Span& span_) : Node(NodeType::lvalueRefNode, span_)
{
}

LValueRefNode::LValueRefNode(const Span& span_, Node* subject_) : Node(NodeType::lvalueRefNode, span_), subject(subject_)
{
    subject->SetParent(this);
}

Node* LValueRefNode::Clone(CloneContext& cloneContext) const
{
    return new LValueRefNode(GetSpan(), subject->Clone(cloneContext));
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

RValueRefNode::RValueRefNode(const Span& span_) : Node(NodeType::rvalueRefNode, span_)
{
}

RValueRefNode::RValueRefNode(const Span& span_, Node* subject_) : Node(NodeType::rvalueRefNode, span_), subject(subject_)
{
    subject->SetParent(this);
}

Node* RValueRefNode::Clone(CloneContext& cloneContext) const
{
    return new RValueRefNode(GetSpan(), subject->Clone(cloneContext));
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

PointerNode::PointerNode(const Span& span_) : Node(NodeType::pointerNode, span_)
{
}

PointerNode::PointerNode(const Span& span_, Node* subject_) : Node(NodeType::pointerNode, span_), subject(subject_)
{
    subject->SetParent(this);
}

Node* PointerNode::Clone(CloneContext& cloneContext) const
{
    return new PointerNode(GetSpan(), subject->Clone(cloneContext));
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

ArrayNode::ArrayNode(const Span& span_) : Node(NodeType::arrayNode, span_), subject(), size()
{
}

ArrayNode::ArrayNode(const Span& span_, Node* subject_, Node* size_) : Node(NodeType::arrayNode, span_), subject(subject_), size(size_)
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
    return new ArrayNode(GetSpan(), subject->Clone(cloneContext), clonedSize);
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

} } // namespace cmajor::ast
