// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/BasicType.hpp>
#include <sngcm/ast/Visitor.hpp>

namespace sngcm { namespace ast {

BoolNode::BoolNode(const Span& span_) : Node(NodeType::boolNode, span_)
{
}

Node* BoolNode::Clone(CloneContext& cloneContext) const
{
    return new BoolNode(cloneContext.MapSpan(GetSpan(), RootModuleId()));
}

void BoolNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

SByteNode::SByteNode(const Span& span_) : Node(NodeType::sbyteNode, span_)
{
}

Node* SByteNode::Clone(CloneContext& cloneContext) const
{
    return new SByteNode(cloneContext.MapSpan(GetSpan(), RootModuleId()));
}

void SByteNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ByteNode::ByteNode(const Span& span_) : Node(NodeType::byteNode, span_)
{
}

Node* ByteNode::Clone(CloneContext& cloneContext) const
{
    return new ByteNode(cloneContext.MapSpan(GetSpan(), RootModuleId()));
}

void ByteNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ShortNode::ShortNode(const Span& span_) : Node(NodeType::shortNode, span_)
{
}

Node* ShortNode::Clone(CloneContext& cloneContext) const
{
    return new ShortNode(cloneContext.MapSpan(GetSpan(), RootModuleId()));
}

void ShortNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

UShortNode::UShortNode(const Span& span_) : Node(NodeType::ushortNode, span_)
{
}

Node* UShortNode::Clone(CloneContext& cloneContext) const
{
    return new UShortNode(cloneContext.MapSpan(GetSpan(), RootModuleId()));
}

void UShortNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

IntNode::IntNode(const Span& span_) : Node(NodeType::intNode, span_)
{
}

Node* IntNode::Clone(CloneContext& cloneContext) const
{
    return new IntNode(cloneContext.MapSpan(GetSpan(), RootModuleId()));
}

void IntNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

UIntNode::UIntNode(const Span& span_) : Node(NodeType::uintNode, span_)
{
}

Node* UIntNode::Clone(CloneContext& cloneContext) const
{
    return new UIntNode(cloneContext.MapSpan(GetSpan(), RootModuleId()));
}

void UIntNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

LongNode::LongNode(const Span& span_) : Node(NodeType::longNode, span_)
{
}

Node* LongNode::Clone(CloneContext& cloneContext) const
{
    return new LongNode(cloneContext.MapSpan(GetSpan(), RootModuleId()));
}

void LongNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ULongNode::ULongNode(const Span& span_) : Node(NodeType::ulongNode, span_)
{
}

Node* ULongNode::Clone(CloneContext& cloneContext) const
{
    return new ULongNode(cloneContext.MapSpan(GetSpan(), RootModuleId()));
}

void ULongNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

FloatNode::FloatNode(const Span& span_) : Node(NodeType::floatNode, span_)
{
}

Node* FloatNode::Clone(CloneContext& cloneContext) const
{
    return new FloatNode(cloneContext.MapSpan(GetSpan(), RootModuleId()));
}

void FloatNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

DoubleNode::DoubleNode(const Span& span_) : Node(NodeType::doubleNode, span_)
{
}

Node* DoubleNode::Clone(CloneContext& cloneContext) const
{
    return new DoubleNode(cloneContext.MapSpan(GetSpan(), RootModuleId()));
}

void DoubleNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

CharNode::CharNode(const Span& span_) : Node(NodeType::charNode, span_)
{
}

Node* CharNode::Clone(CloneContext& cloneContext) const
{
    return new CharNode(cloneContext.MapSpan(GetSpan(), RootModuleId()));
}

void CharNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

WCharNode::WCharNode(const Span& span_) : Node(NodeType::wcharNode, span_)
{
}

Node* WCharNode::Clone(CloneContext& cloneContext) const
{
    return new WCharNode(cloneContext.MapSpan(GetSpan(), RootModuleId()));
}

void WCharNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

UCharNode::UCharNode(const Span& span_) : Node(NodeType::ucharNode, span_)
{
}

Node* UCharNode::Clone(CloneContext& cloneContext) const
{
    return new UCharNode(cloneContext.MapSpan(GetSpan(), RootModuleId()));
}

void UCharNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

VoidNode::VoidNode(const Span& span_) : Node(NodeType::voidNode, span_)
{
}

Node* VoidNode::Clone(CloneContext& cloneContext) const
{
    return new VoidNode(cloneContext.MapSpan(GetSpan(), RootModuleId()));
}

void VoidNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

} } // namespace sngcm::ast
