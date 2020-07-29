// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/DebugExpr.hpp>
#include <cmajor/cmdebug/DebugExprVisitor.hpp>

namespace cmajor { namespace debug {

DebugExprNode::DebugExprNode(Kind kind_) : kind(kind_)
{
}

DebugExprNode::~DebugExprNode()
{
}

IdentifierDebugExprNode::IdentifierDebugExprNode(const std::string& identifier_) : DebugExprNode(Kind::identifier), identifier(identifier_)
{
}

DebugExprNode* IdentifierDebugExprNode::Clone() const
{
    return new IdentifierDebugExprNode(identifier);
}

void IdentifierDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string IdentifierDebugExprNode::ToString() const
{
    return identifier;
}

IntegerDebugExprNode::IntegerDebugExprNode(int64_t value_) : DebugExprNode(Kind::integer), value(value_)
{
}

DebugExprNode* IntegerDebugExprNode::Clone() const
{
    return new IntegerDebugExprNode(value);
}

void IntegerDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string IntegerDebugExprNode::ToString() const
{
    return std::to_string(value);
}

AddDebugExprNode::AddDebugExprNode(DebugExprNode* left_, DebugExprNode* right_) : DebugExprNode(Kind::add), left(left_), right(right_)
{
}

DebugExprNode* AddDebugExprNode::Clone() const
{
    return new AddDebugExprNode(left->Clone(), right->Clone());
}

void AddDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string AddDebugExprNode::ToString() const
{
    return left->ToString() + "+" + right->ToString();
}

SubDebugExprNode::SubDebugExprNode(DebugExprNode* left_, DebugExprNode* right_) : DebugExprNode(Kind::sub), left(left_), right(right_)
{
}

DebugExprNode* SubDebugExprNode::Clone() const
{
    return new SubDebugExprNode(left->Clone(), right->Clone());
}

void SubDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string SubDebugExprNode::ToString() const
{
    return left->ToString() + "-" + right->ToString();
}

ParenthesizedDebugExprNode::ParenthesizedDebugExprNode(DebugExprNode* exprNode_) : DebugExprNode(Kind::parenExpr), exprNode(exprNode_)
{
}

DebugExprNode* ParenthesizedDebugExprNode::Clone() const
{
    return new ParenthesizedDebugExprNode(exprNode->Clone());
}

void ParenthesizedDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string ParenthesizedDebugExprNode::ToString() const
{
    return "(" + exprNode->ToString() + ")";
}

DerefDebugExprNode::DerefDebugExprNode(DebugExprNode* operand_) : DebugExprNode(Kind::deref), operand(operand_)
{
}

DebugExprNode* DerefDebugExprNode::Clone() const
{
    return new DerefDebugExprNode(operand->Clone());
}

void DerefDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string DerefDebugExprNode::ToString() const
{
    return "*" + operand->ToString();
}

AddrOfDebugExprNode::AddrOfDebugExprNode(DebugExprNode* operand_) : DebugExprNode(Kind::addrOf), operand(operand_)
{
}

DebugExprNode* AddrOfDebugExprNode::Clone() const
{
    return new AddrOfDebugExprNode(operand->Clone());
}

void AddrOfDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string AddrOfDebugExprNode::ToString() const
{
    return "&" + operand->ToString();
}

DotDebugExprNode::DotDebugExprNode(DebugExprNode* subject_, const std::string& member_) : DebugExprNode(Kind::dot), subject(subject_), member(member_)
{
}

DebugExprNode* DotDebugExprNode::Clone() const
{
    return new DotDebugExprNode(subject->Clone(), member);
}

void DotDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string DotDebugExprNode::ToString() const
{
    return subject->ToString() + "." + member;
}

SubscriptDebugExprNode::SubscriptDebugExprNode(DebugExprNode* subject_, DebugExprNode* index_) : DebugExprNode(Kind::subscript), subject(subject_), index(index_)
{
}

DebugExprNode* SubscriptDebugExprNode::Clone() const
{
    return new SubscriptDebugExprNode(subject->Clone(), index->Clone());
}

void SubscriptDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string SubscriptDebugExprNode::ToString() const
{
    return subject->ToString() + "[" + index->ToString() + "]";
}

RangeDebugExprNode::RangeDebugExprNode(DebugExprNode* subject_, DebugExprNode* rangeStart_, DebugExprNode* rangeEnd_) :
    DebugExprNode(Kind::range), subject(subject_), rangeStart(rangeStart_), rangeEnd(rangeEnd_)
{
}

DebugExprNode* RangeDebugExprNode::Clone() const
{
    return new RangeDebugExprNode(subject->Clone(), rangeStart->Clone(), rangeEnd->Clone());
}

void RangeDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string RangeDebugExprNode::ToString() const
{
    return subject->ToString() + "[" + rangeStart->ToString() + ", " + rangeEnd->ToString() + "]";
}

TypeIdDebugExprNode::TypeIdDebugExprNode(const std::string& typeId_) : DebugExprNode(Kind::typeId), typeId(typeId_)
{
}

DebugExprNode* TypeIdDebugExprNode::Clone() const
{
    return new TypeIdDebugExprNode(typeId);
}

void TypeIdDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string TypeIdDebugExprNode::ToString() const
{
    return "typeid(" + typeId + ")";
}

CastDebugExprNode::CastDebugExprNode(TypeIdDebugExprNode* typeIdNode_, DebugExprNode* exprNode_) : DebugExprNode(Kind::cast_), typeIdNode(typeIdNode_), exprNode(exprNode_)
{
}

DebugExprNode* CastDebugExprNode::Clone() const
{
    return new CastDebugExprNode(static_cast<TypeIdDebugExprNode*>(typeIdNode->Clone()), exprNode->Clone());
}

void CastDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string CastDebugExprNode::ToString() const
{
    return "cast<" + typeIdNode->ToString() + ">(" + exprNode->ToString() + ")";
}

} } // namespace cmajor::debug
