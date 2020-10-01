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

DisjunctionDebugExprNode::DisjunctionDebugExprNode(DebugExprNode* left_, DebugExprNode* right_) : DebugExprNode(Kind::disjunction), left(left_), right(right_)
{
}

DebugExprNode* DisjunctionDebugExprNode::Clone() const
{
    return new DisjunctionDebugExprNode(left->Clone(), right->Clone());
}

void DisjunctionDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string DisjunctionDebugExprNode::ToString() const
{
    return left->ToString() + "||" + right->ToString();
}

ConjunctionDebugExprNode::ConjunctionDebugExprNode(DebugExprNode* left_, DebugExprNode* right_) : DebugExprNode(Kind::conjunction), left(left_), right(right_)
{
}

DebugExprNode* ConjunctionDebugExprNode::Clone() const
{
    return new ConjunctionDebugExprNode(left->Clone(), right->Clone());
}

void ConjunctionDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string ConjunctionDebugExprNode::ToString() const
{
    return left->ToString() + "&&" + right->ToString();
}

BitOrDebugExprNode::BitOrDebugExprNode(DebugExprNode* left_, DebugExprNode* right_) : DebugExprNode(Kind::bitOr), left(left_), right(right_)
{
}

DebugExprNode* BitOrDebugExprNode::Clone() const
{
    return new BitOrDebugExprNode(left->Clone(), right->Clone());
}

void BitOrDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BitOrDebugExprNode::ToString() const
{
    return left->ToString() + "|" + right->ToString();
}

BitXorDebugExprNode::BitXorDebugExprNode(DebugExprNode* left_, DebugExprNode* right_) : DebugExprNode(Kind::bitXor), left(left_), right(right_)
{
}

DebugExprNode* BitXorDebugExprNode::Clone() const
{
    return new BitXorDebugExprNode(left->Clone(), right->Clone());
}

void BitXorDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BitXorDebugExprNode::ToString() const
{
    return left->ToString() + "^" + right->ToString();
}

BitAndDebugExprNode::BitAndDebugExprNode(DebugExprNode* left_, DebugExprNode* right_) : DebugExprNode(Kind::bitAnd), left(left_), right(right_)
{
}

DebugExprNode* BitAndDebugExprNode::Clone() const
{
    return new BitAndDebugExprNode(left->Clone(), right->Clone());
}

void BitAndDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BitAndDebugExprNode::ToString() const
{
    return left->ToString() + "&" + right->ToString();
}

EqualDebugExprNode::EqualDebugExprNode(DebugExprNode* left_, DebugExprNode* right_) : DebugExprNode(Kind::eq), left(left_), right(right_)
{
}

DebugExprNode* EqualDebugExprNode::Clone() const
{
    return new EqualDebugExprNode(left->Clone(), right->Clone());
}

void EqualDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string EqualDebugExprNode::ToString() const
{
    return left->ToString() + "==" + right->ToString();
}

NotEqualDebugExprNode::NotEqualDebugExprNode(DebugExprNode* left_, DebugExprNode* right_) : DebugExprNode(Kind::neq), left(left_), right(right_)
{
}

DebugExprNode* NotEqualDebugExprNode::Clone() const
{
    return new NotEqualDebugExprNode(left->Clone(), right->Clone());
}

void NotEqualDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string NotEqualDebugExprNode::ToString() const
{
    return left->ToString() + "!=" + right->ToString();
}

LessEqualDebugExprNode::LessEqualDebugExprNode(DebugExprNode* left_, DebugExprNode* right_) : DebugExprNode(Kind::leq), left(left_), right(right_)
{
}

DebugExprNode* LessEqualDebugExprNode::Clone() const
{
    return new LessEqualDebugExprNode(left->Clone(), right->Clone());
}

void LessEqualDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string LessEqualDebugExprNode::ToString() const
{
    return left->ToString() + "<=" + right->ToString();
}

GreaterEqualDebugExprNode::GreaterEqualDebugExprNode(DebugExprNode* left_, DebugExprNode* right_) : DebugExprNode(Kind::geq), left(left_), right(right_)
{
}

DebugExprNode* GreaterEqualDebugExprNode::Clone() const
{
    return new GreaterEqualDebugExprNode(left->Clone(), right->Clone());
}

void GreaterEqualDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string GreaterEqualDebugExprNode::ToString() const
{
    return left->ToString() + ">=" + right->ToString();
}

LessDebugExprNode::LessDebugExprNode(DebugExprNode* left_, DebugExprNode* right_) : DebugExprNode(Kind::less), left(left_), right(right_)
{
}

DebugExprNode* LessDebugExprNode::Clone() const
{
    return new LessDebugExprNode(left->Clone(), right->Clone());
}

void LessDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string LessDebugExprNode::ToString() const
{
    return left->ToString() + "<" + right->ToString();
}

GreaterDebugExprNode::GreaterDebugExprNode(DebugExprNode* left_, DebugExprNode* right_) : DebugExprNode(Kind::greater), left(left_), right(right_)
{
}

DebugExprNode* GreaterDebugExprNode::Clone() const
{
    return new GreaterDebugExprNode(left->Clone(), right->Clone());
}

void GreaterDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string GreaterDebugExprNode::ToString() const
{
    return left->ToString() + ">" + right->ToString();
}

ShiftLeftDebugExprNode::ShiftLeftDebugExprNode(DebugExprNode* left_, DebugExprNode* right_) : DebugExprNode(Kind::shiftLeft), left(left_), right(right_)
{
}

DebugExprNode* ShiftLeftDebugExprNode::Clone() const
{
    return new ShiftLeftDebugExprNode(left->Clone(), right->Clone());
}

void ShiftLeftDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string ShiftLeftDebugExprNode::ToString() const
{
    return left->ToString() + "<<" + right->ToString();
}

ShiftRightDebugExprNode::ShiftRightDebugExprNode(DebugExprNode* left_, DebugExprNode* right_) : DebugExprNode(Kind::shiftRight), left(left_), right(right_)
{
}

DebugExprNode* ShiftRightDebugExprNode::Clone() const
{
    return new ShiftRightDebugExprNode(left->Clone(), right->Clone());
}

void ShiftRightDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string ShiftRightDebugExprNode::ToString() const
{
    return left->ToString() + ">>" + right->ToString();
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

MulDebugExprNode::MulDebugExprNode(DebugExprNode* left_, DebugExprNode* right_) : DebugExprNode(Kind::mul), left(left_), right(right_)
{
}

DebugExprNode* MulDebugExprNode::Clone() const
{
    return new MulDebugExprNode(left->Clone(), right->Clone());
}

void MulDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string MulDebugExprNode::ToString() const
{
    return left->ToString() + "*" + right->ToString();
}

DivDebugExprNode::DivDebugExprNode(DebugExprNode* left_, DebugExprNode* right_) : DebugExprNode(Kind::div), left(left_), right(right_)
{
}

DebugExprNode* DivDebugExprNode::Clone() const
{
    return new DivDebugExprNode(left->Clone(), right->Clone());
}

void DivDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string DivDebugExprNode::ToString() const
{
    return left->ToString() + "/" + right->ToString();
}

ModDebugExprNode::ModDebugExprNode(DebugExprNode* left_, DebugExprNode* right_) : DebugExprNode(Kind::mod), left(left_), right(right_)
{
}

DebugExprNode* ModDebugExprNode::Clone() const
{
    return new ModDebugExprNode(left->Clone(), right->Clone());
}

void ModDebugExprNode::Accept(DebugExprVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string ModDebugExprNode::ToString() const
{
    return left->ToString() + "%" + right->ToString();
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
    return "(*" + operand->ToString() + ")";
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
    return "typeid(\"" + typeId + "\")";
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
