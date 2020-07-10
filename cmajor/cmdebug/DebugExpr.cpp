// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/DebugExpr.hpp>
#include <cmajor/cmdebug/DebugExprVisitor.hpp>

namespace cmajor { namespace debug {

DebugExprNode::DebugExprNode()
{
}

DebugExprNode::~DebugExprNode()
{
}

VariableNameDebugExprNode::VariableNameDebugExprNode(const std::string& identifier_) : identifier(identifier_)
{
}

void VariableNameDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

IntegerDebugExprNode::IntegerDebugExprNode(int64_t value_) : value(value_)
{
}

void IntegerDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

ParenthesizedDebugExprNode::ParenthesizedDebugExprNode(DebugExprNode* exprNode_) : exprNode(exprNode_)
{
}

void ParenthesizedDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

DerefDebugExprNode::DerefDebugExprNode(DebugExprNode* operand_) : operand(operand_)
{
}

void DerefDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

AddrOfDebugExprNode::AddrOfDebugExprNode(DebugExprNode* operand_) : operand(operand_)
{
}

void AddrOfDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

UnaryMinusDebugExprNode::UnaryMinusDebugExprNode(DebugExprNode* operand_) : operand(operand_)
{
}

void UnaryMinusDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

UnaryPlusDebugExprNode::UnaryPlusDebugExprNode(DebugExprNode* operand_) : operand(operand_)
{
}

void UnaryPlusDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

DotDebugExprNode::DotDebugExprNode(DebugExprNode* subject_, const std::string& member_) : subject(subject_), member(member_)
{
}

void DotDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

SubscriptDebugExprNode::SubscriptDebugExprNode(DebugExprNode* subject_, DebugExprNode* index_) : subject(subject_), index(index_)
{
}

void SubscriptDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

TypeIdDebugExprNode::TypeIdDebugExprNode(const std::string& typeId_) : typeId(typeId_)
{
}

void TypeIdDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

CastDebugExprNode::CastDebugExprNode(TypeIdDebugExprNode* typeIdNode_, DebugExprNode* exprNode_) : typeIdNode(typeIdNode_), exprNode(exprNode_)
{
}

void CastDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

BaseDebugExprNode::BaseDebugExprNode()
{
}

void BaseDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

} } // namespace cmajor::debug
