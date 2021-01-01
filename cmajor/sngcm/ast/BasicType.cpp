// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/BasicType.hpp>
#include <sngcm/ast/Visitor.hpp>

namespace sngcm { namespace ast {

BoolNode::BoolNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::boolNode, span_, moduleId_)
{
}

Node* BoolNode::Clone(CloneContext& cloneContext) const
{
    BoolNode* clone = new BoolNode(GetSpan(), ModuleId());
    return clone;
}

void BoolNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

SByteNode::SByteNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::sbyteNode, span_, moduleId_)
{
}

Node* SByteNode::Clone(CloneContext& cloneContext) const
{
    SByteNode* clone = new SByteNode(GetSpan(), ModuleId());
    return clone;
}

void SByteNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ByteNode::ByteNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::byteNode, span_, moduleId_)
{
}

Node* ByteNode::Clone(CloneContext& cloneContext) const
{
    ByteNode* clone = new ByteNode(GetSpan(), ModuleId());
    return clone;
}

void ByteNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ShortNode::ShortNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::shortNode, span_, moduleId_)
{
}

Node* ShortNode::Clone(CloneContext& cloneContext) const
{
    ShortNode* clone = new ShortNode(GetSpan(), ModuleId());
    return clone;
}

void ShortNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

UShortNode::UShortNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::ushortNode, span_, moduleId_)
{
}

Node* UShortNode::Clone(CloneContext& cloneContext) const
{
    UShortNode* clone = new UShortNode(GetSpan(), ModuleId());
    return clone;
}

void UShortNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

IntNode::IntNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::intNode, span_, moduleId_)
{
}

Node* IntNode::Clone(CloneContext& cloneContext) const
{
    IntNode* clone = new IntNode(GetSpan(), ModuleId());
    return clone;
}

void IntNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

UIntNode::UIntNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::uintNode, span_, moduleId_)
{
}

Node* UIntNode::Clone(CloneContext& cloneContext) const
{
    UIntNode* clone = new UIntNode(GetSpan(), ModuleId());
    return clone;
}

void UIntNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

LongNode::LongNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::longNode, span_, moduleId_)
{
}

Node* LongNode::Clone(CloneContext& cloneContext) const
{
    LongNode* clone = new LongNode(GetSpan(), ModuleId());
    return clone;
}

void LongNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ULongNode::ULongNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::ulongNode, span_, moduleId_)
{
}

Node* ULongNode::Clone(CloneContext& cloneContext) const
{
    ULongNode* clone = new ULongNode(GetSpan(), ModuleId());
    return clone;
}

void ULongNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

FloatNode::FloatNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::floatNode, span_, moduleId_)
{
}

Node* FloatNode::Clone(CloneContext& cloneContext) const
{
    FloatNode* clone = new FloatNode(GetSpan(), ModuleId());
    return clone;
}

void FloatNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

DoubleNode::DoubleNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::doubleNode, span_, moduleId_)
{
}

Node* DoubleNode::Clone(CloneContext& cloneContext) const
{
    DoubleNode* clone = new DoubleNode(GetSpan(), ModuleId());
    return clone;
}

void DoubleNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

CharNode::CharNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::charNode, span_, moduleId_)
{
}

Node* CharNode::Clone(CloneContext& cloneContext) const
{
    CharNode* clone = new CharNode(GetSpan(), ModuleId());
    return clone;
}

void CharNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

WCharNode::WCharNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::wcharNode, span_, moduleId_)
{
}

Node* WCharNode::Clone(CloneContext& cloneContext) const
{
    WCharNode* clone = new WCharNode(GetSpan(), ModuleId());
    return clone;
}

void WCharNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

UCharNode::UCharNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::ucharNode, span_, moduleId_)
{
}

Node* UCharNode::Clone(CloneContext& cloneContext) const
{
    UCharNode* clone = new UCharNode(GetSpan(), ModuleId());
    return clone;
}

void UCharNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

VoidNode::VoidNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::voidNode, span_, moduleId_)
{
}

Node* VoidNode::Clone(CloneContext& cloneContext) const
{
    VoidNode* clone = new VoidNode(GetSpan(), ModuleId());
    return clone;
}

void VoidNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

} } // namespace sngcm::ast
