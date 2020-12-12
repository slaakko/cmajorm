// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/Expression.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <sngcm/ast/Visitor.hpp>

namespace sngcm { namespace ast {

DotNode::DotNode(const Span& span_, const boost::uuids::uuid& moduleId_) : UnaryNode(NodeType::dotNode, span_, moduleId_), memberId()
{
}

DotNode::DotNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_, IdentifierNode* memberId_) : UnaryNode(NodeType::dotNode, span_, moduleId_, subject_), memberId(memberId_)
{
    memberId->SetParent(this);
}

Node* DotNode::Clone(CloneContext& cloneContext) const
{
    DotNode* clone = new DotNode(GetSpan(), ModuleId(), Subject()->Clone(cloneContext), static_cast<IdentifierNode*>(memberId->Clone(cloneContext)));
    return clone;
}

void DotNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void DotNode::Write(AstWriter& writer)
{
    UnaryNode::Write(writer);
    writer.Write(memberId.get());
}

void DotNode::Read(AstReader& reader)
{
    UnaryNode::Read(reader);
    memberId.reset(reader.ReadIdentifierNode());
    memberId->SetParent(this);
}

std::string DotNode::ToString() const
{
    return Subject()->ToString() + "." + memberId->ToString();
}

ArrowNode::ArrowNode(const Span& span_, const boost::uuids::uuid& moduleId_) : UnaryNode(NodeType::arrowNode, span_, moduleId_), memberId()
{
}

ArrowNode::ArrowNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_, IdentifierNode* memberId_) : 
    UnaryNode(NodeType::arrowNode, span_, moduleId_, subject_), memberId(memberId_)
{
    memberId->SetParent(this);
}

Node* ArrowNode::Clone(CloneContext& cloneContext) const
{
    ArrowNode* clone = new ArrowNode(GetSpan(), ModuleId(), Subject()->Clone(cloneContext), static_cast<IdentifierNode*>(memberId->Clone(cloneContext)));
    return clone;
}

void ArrowNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void ArrowNode::Write(AstWriter& writer)
{
    UnaryNode::Write(writer);
    writer.Write(memberId.get());
}

void ArrowNode::Read(AstReader& reader)
{
    UnaryNode::Read(reader);
    memberId.reset(reader.ReadIdentifierNode());
    memberId->SetParent(this);
}

std::string ArrowNode::ToString() const
{
    return Subject()->ToString() + "->" + memberId->ToString();
}

EquivalenceNode::EquivalenceNode(const Span& span_, const boost::uuids::uuid& moduleId_) : BinaryNode(NodeType::equivalenceNode, span_, moduleId_)
{
}

EquivalenceNode::EquivalenceNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_) : 
    BinaryNode(NodeType::equivalenceNode, span_, moduleId_, left_, right_)
{
}

Node* EquivalenceNode::Clone(CloneContext& cloneContext) const
{
    EquivalenceNode* clone = new EquivalenceNode(GetSpan(), ModuleId(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
    return clone;
}

void EquivalenceNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string EquivalenceNode::ToString() const
{
    return Left()->ToString() + "<=>" + Right()->ToString();
}

ImplicationNode::ImplicationNode(const Span& span_, const boost::uuids::uuid& moduleId_) : BinaryNode(NodeType::implicationNode, span_, moduleId_)
{
}

ImplicationNode::ImplicationNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_) : 
    BinaryNode(NodeType::implicationNode, span_, moduleId_, left_, right_)
{
}

Node* ImplicationNode::Clone(CloneContext& cloneContext) const
{
    ImplicationNode* clone = new ImplicationNode(GetSpan(), ModuleId(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
    return clone;
}

void ImplicationNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string ImplicationNode::ToString() const
{
    return Left()->ToString() + "=>" + Right()->ToString();
}

DisjunctionNode::DisjunctionNode(const Span& span_, const boost::uuids::uuid& moduleId_) : BinaryNode(NodeType::disjunctionNode, span_, moduleId_)
{
}

DisjunctionNode::DisjunctionNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_) : 
    BinaryNode(NodeType::disjunctionNode, span_, moduleId_, left_, right_)
{
}

Node* DisjunctionNode::Clone(CloneContext& cloneContext) const
{
    DisjunctionNode* clone = new DisjunctionNode(GetSpan(), ModuleId(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
    return clone;
}

void DisjunctionNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string DisjunctionNode::ToString() const
{
    return Left()->ToString() + " || " + Right()->ToString();
}

ConjunctionNode::ConjunctionNode(const Span& span_, const boost::uuids::uuid& moduleId_) : BinaryNode(NodeType::conjunctionNode, span_, moduleId_)
{
}

ConjunctionNode::ConjunctionNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_) : 
    BinaryNode(NodeType::conjunctionNode, span_, moduleId_, left_, right_)
{
}

Node* ConjunctionNode::Clone(CloneContext& cloneContext) const
{
    ConjunctionNode* clone = new ConjunctionNode(GetSpan(), ModuleId(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
    return clone;
}

void ConjunctionNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string ConjunctionNode::ToString() const
{
    return Left()->ToString() + " && " + Right()->ToString();
}

BitOrNode::BitOrNode(const Span& span_, const boost::uuids::uuid& moduleId_) : BinaryNode(NodeType::bitOrNode, span_, moduleId_)
{
}

BitOrNode::BitOrNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_) : BinaryNode(NodeType::bitOrNode, span_, moduleId_, left_, right_)
{
}

Node* BitOrNode::Clone(CloneContext& cloneContext) const
{
    BitOrNode* clone = new BitOrNode(GetSpan(), ModuleId(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
    return clone;
}

void BitOrNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string BitOrNode::ToString() const
{
    return Left()->ToString() + " | " + Right()->ToString();
}

BitXorNode::BitXorNode(const Span& span_, const boost::uuids::uuid& moduleId_) : BinaryNode(NodeType::bitXorNode, span_, moduleId_)
{
}

BitXorNode::BitXorNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_) : 
    BinaryNode(NodeType::bitXorNode, span_, moduleId_, left_, right_)
{
}

Node* BitXorNode::Clone(CloneContext& cloneContext) const
{
    BitXorNode* clone = new BitXorNode(GetSpan(), ModuleId(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
    return clone;
}

void BitXorNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string BitXorNode::ToString() const
{
    return Left()->ToString() + " ^ " + Right()->ToString();
}

BitAndNode::BitAndNode(const Span& span_, const boost::uuids::uuid& moduleId_) : BinaryNode(NodeType::bitAndNode, span_, moduleId_)
{
}

BitAndNode::BitAndNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_) : 
    BinaryNode(NodeType::bitAndNode, span_, moduleId_, left_, right_)
{
}

Node* BitAndNode::Clone(CloneContext& cloneContext) const
{
    BitAndNode* clone = new BitAndNode(GetSpan(), ModuleId(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
    return clone;
}

void BitAndNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string BitAndNode::ToString() const
{
    return Left()->ToString() + " & " + Right()->ToString();
}

EqualNode::EqualNode(const Span& span_, const boost::uuids::uuid& moduleId_) : BinaryNode(NodeType::equalNode, span_, moduleId_)
{
}

EqualNode::EqualNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_) : 
    BinaryNode(NodeType::equalNode, span_, moduleId_, left_, right_)
{
}

Node* EqualNode::Clone(CloneContext& cloneContext) const
{
    EqualNode* clone = new EqualNode(GetSpan(), ModuleId(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
    return clone;
}

void EqualNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string EqualNode::ToString() const
{
    return Left()->ToString() + " == " + Right()->ToString();
}

NotEqualNode::NotEqualNode(const Span& span_, const boost::uuids::uuid& moduleId_) : BinaryNode(NodeType::notEqualNode, span_, moduleId_)
{
}

NotEqualNode::NotEqualNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_) : BinaryNode(NodeType::notEqualNode, span_, moduleId_, left_, right_)
{
}

Node* NotEqualNode::Clone(CloneContext& cloneContext) const
{
    NotEqualNode* clone = new NotEqualNode(GetSpan(), ModuleId(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
    return clone;
}

void NotEqualNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string NotEqualNode::ToString() const
{
    return Left()->ToString() + " != " + Right()->ToString();
}

LessNode::LessNode(const Span& span_, const boost::uuids::uuid& moduleId_) : BinaryNode(NodeType::lessNode, span_, moduleId_)
{
}

LessNode::LessNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_) : BinaryNode(NodeType::lessNode, span_, moduleId_, left_, right_)
{
}

Node* LessNode::Clone(CloneContext& cloneContext) const
{
    LessNode* clone = new LessNode(GetSpan(), ModuleId(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
    return clone;
}

void LessNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string LessNode::ToString() const
{
    return Left()->ToString() + " < " + Right()->ToString();
}

GreaterNode::GreaterNode(const Span& span_, const boost::uuids::uuid& moduleId_) : BinaryNode(NodeType::greaterNode, span_, moduleId_)
{
}

GreaterNode::GreaterNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_) : BinaryNode(NodeType::greaterNode, span_, moduleId_, left_, right_)
{
}

Node* GreaterNode::Clone(CloneContext& cloneContext) const
{
    GreaterNode* clone = new GreaterNode(GetSpan(), ModuleId(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
    return clone;
}

void GreaterNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string GreaterNode::ToString() const
{
    return Left()->ToString() + " > " + Right()->ToString();
}

LessOrEqualNode::LessOrEqualNode(const Span& span_, const boost::uuids::uuid& moduleId_) : BinaryNode(NodeType::lessOrEqualNode, span_, moduleId_)
{
}

LessOrEqualNode::LessOrEqualNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_) :
    BinaryNode(NodeType::lessOrEqualNode, span_, moduleId_, left_, right_)
{
}

Node* LessOrEqualNode::Clone(CloneContext& cloneContext) const
{
    LessOrEqualNode* clone = new LessOrEqualNode(GetSpan(), ModuleId(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
    return clone;
}

void LessOrEqualNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string LessOrEqualNode::ToString() const
{
    return Left()->ToString() + " <= " + Right()->ToString();
}

GreaterOrEqualNode::GreaterOrEqualNode(const Span& span_, const boost::uuids::uuid& moduleId_) : BinaryNode(NodeType::greaterOrEqualNode, span_, moduleId_)
{
}

GreaterOrEqualNode::GreaterOrEqualNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_) : 
    BinaryNode(NodeType::greaterOrEqualNode, span_, moduleId_, left_, right_)
{
}

Node* GreaterOrEqualNode::Clone(CloneContext& cloneContext) const
{
    GreaterOrEqualNode* clone = new GreaterOrEqualNode(GetSpan(), ModuleId(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
    return clone;
}

void GreaterOrEqualNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string GreaterOrEqualNode::ToString() const
{
    return Left()->ToString() + " >= " + Right()->ToString();
}

ShiftLeftNode::ShiftLeftNode(const Span& span_, const boost::uuids::uuid& moduleId_) : BinaryNode(NodeType::shiftLeftNode, span_, moduleId_)
{
}

ShiftLeftNode::ShiftLeftNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_) : 
    BinaryNode(NodeType::shiftLeftNode, span_, moduleId_, left_, right_)
{
}

Node* ShiftLeftNode::Clone(CloneContext& cloneContext) const
{
    ShiftLeftNode* clone = new ShiftLeftNode(GetSpan(), ModuleId(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
    return clone;
}

void ShiftLeftNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string ShiftLeftNode::ToString() const
{
    return Left()->ToString() + " << " + Right()->ToString();
}

ShiftRightNode::ShiftRightNode(const Span& span_, const boost::uuids::uuid& moduleId_) : BinaryNode(NodeType::shiftRightNode, span_, moduleId_)
{
}

ShiftRightNode::ShiftRightNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_) : 
    BinaryNode(NodeType::shiftRightNode, span_, moduleId_, left_, right_)
{
}

Node* ShiftRightNode::Clone(CloneContext& cloneContext) const
{
    ShiftRightNode* clone = new ShiftRightNode(GetSpan(), ModuleId(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
    return clone;
}

void ShiftRightNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string ShiftRightNode::ToString() const
{
    return Left()->ToString() + " >> " + Right()->ToString();
}

AddNode::AddNode(const Span& span_, const boost::uuids::uuid& moduleId_) : BinaryNode(NodeType::addNode, span_, moduleId_)
{
}

AddNode::AddNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_) : BinaryNode(NodeType::addNode, span_, moduleId_, left_, right_)
{
}

Node* AddNode::Clone(CloneContext& cloneContext) const
{
    AddNode* clone = new AddNode(GetSpan(), ModuleId(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
    return clone;
}

void AddNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string AddNode::ToString() const
{
    return Left()->ToString() + " + " + Right()->ToString();
}

SubNode::SubNode(const Span& span_, const boost::uuids::uuid& moduleId_) : BinaryNode(NodeType::subNode, span_, moduleId_)
{
}

SubNode::SubNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_) : 
    BinaryNode(NodeType::subNode, span_, moduleId_, left_, right_)
{
}

Node* SubNode::Clone(CloneContext& cloneContext) const
{
    SubNode* clone = new SubNode(GetSpan(), ModuleId(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
    return clone;
}

void SubNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string SubNode::ToString() const
{
    return Left()->ToString() + " - " + Right()->ToString();
}

MulNode::MulNode(const Span& span_, const boost::uuids::uuid& moduleId_) : BinaryNode(NodeType::mulNode, span_, moduleId_)
{
}

MulNode::MulNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_) : 
    BinaryNode(NodeType::mulNode, span_, moduleId_, left_, right_)
{
}

Node* MulNode::Clone(CloneContext& cloneContext) const
{
    MulNode* clone = new MulNode(GetSpan(), ModuleId(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
    return clone;
}

void MulNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string MulNode::ToString() const
{
    return Left()->ToString() + " * " + Right()->ToString();
}

DivNode::DivNode(const Span& span_, const boost::uuids::uuid& moduleId_) : BinaryNode(NodeType::divNode, span_, moduleId_)
{
}

DivNode::DivNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_) : 
    BinaryNode(NodeType::divNode, span_, moduleId_, left_, right_)
{
}

Node* DivNode::Clone(CloneContext& cloneContext) const
{
    DivNode* clone = new DivNode(GetSpan(), ModuleId(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
    return clone;
}

void DivNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string DivNode::ToString() const
{
    return Left()->ToString() + " / " + Right()->ToString();
}

RemNode::RemNode(const Span& span_, const boost::uuids::uuid& moduleId_) : BinaryNode(NodeType::remNode, span_, moduleId_)
{
}

RemNode::RemNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_) : BinaryNode(NodeType::remNode, span_, moduleId_, left_, right_)
{
}

Node* RemNode::Clone(CloneContext& cloneContext) const
{
    RemNode* clone = new RemNode(GetSpan(), ModuleId(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
    return clone;
}

void RemNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string RemNode::ToString() const
{
    return Left()->ToString() + " % " + Right()->ToString();
}

NotNode::NotNode(const Span& span_, const boost::uuids::uuid& moduleId_) : UnaryNode(NodeType::notNode, span_, moduleId_)
{
}

NotNode::NotNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_) : UnaryNode(NodeType::notNode, span_, moduleId_, subject_)
{
}

Node* NotNode::Clone(CloneContext& cloneContext) const
{
    NotNode* clone = new NotNode(GetSpan(), ModuleId(), Subject()->Clone(cloneContext));
    return clone;
}

void NotNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string NotNode::ToString() const
{
    return "!" + Subject()->ToString();
}

UnaryPlusNode::UnaryPlusNode(const Span& span_, const boost::uuids::uuid& moduleId_) : UnaryNode(NodeType::unaryPlusNode, span_, moduleId_)
{
}

UnaryPlusNode::UnaryPlusNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_) : UnaryNode(NodeType::unaryPlusNode, span_, moduleId_, subject_)
{
}

Node* UnaryPlusNode::Clone(CloneContext& cloneContext) const
{
    UnaryPlusNode* clone = new UnaryPlusNode(GetSpan(), ModuleId(), Subject()->Clone(cloneContext));
    return clone;
}

void UnaryPlusNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string UnaryPlusNode::ToString() const
{
    return "+" + Subject()->ToString();
}

UnaryMinusNode::UnaryMinusNode(const Span& span_, const boost::uuids::uuid& moduleId_) : UnaryNode(NodeType::unaryMinusNode, span_, moduleId_)
{
}

UnaryMinusNode::UnaryMinusNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_) : UnaryNode(NodeType::unaryMinusNode, span_, moduleId_, subject_)
{
}

Node* UnaryMinusNode::Clone(CloneContext& cloneContext) const
{
    UnaryMinusNode* clone = new UnaryMinusNode(GetSpan(), ModuleId(), Subject()->Clone(cloneContext));
    return clone;
}

void UnaryMinusNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string UnaryMinusNode::ToString() const
{
    return "-" + Subject()->ToString();
}

PrefixIncrementNode::PrefixIncrementNode(const Span& span_, const boost::uuids::uuid& moduleId_) : UnaryNode(NodeType::prefixIncrementNode, span_, moduleId_)
{
}

PrefixIncrementNode::PrefixIncrementNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_) : 
    UnaryNode(NodeType::prefixIncrementNode, span_, moduleId_, subject_)
{
}

Node* PrefixIncrementNode::Clone(CloneContext& cloneContext) const
{
    PrefixIncrementNode* clone = new PrefixIncrementNode(GetSpan(), ModuleId(), Subject()->Clone(cloneContext));
    return clone;
}

void PrefixIncrementNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string PrefixIncrementNode::ToString() const
{
    return "++" + Subject()->ToString();
}

PrefixDecrementNode::PrefixDecrementNode(const Span& span_, const boost::uuids::uuid& moduleId_) : UnaryNode(NodeType::prefixDecrementNode, span_, moduleId_)
{
}

PrefixDecrementNode::PrefixDecrementNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_) : 
    UnaryNode(NodeType::prefixDecrementNode, span_, moduleId_, subject_)
{
}

Node* PrefixDecrementNode::Clone(CloneContext& cloneContext) const
{
    PrefixDecrementNode* clone = new PrefixDecrementNode(GetSpan(), ModuleId(), Subject()->Clone(cloneContext));
    return clone;
}

void PrefixDecrementNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string PrefixDecrementNode::ToString() const
{
    return "--" + Subject()->ToString();
}

ComplementNode::ComplementNode(const Span& span_, const boost::uuids::uuid& moduleId_) : UnaryNode(NodeType::complementNode, span_, moduleId_)
{
}

ComplementNode::ComplementNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_) : UnaryNode(NodeType::complementNode, span_, moduleId_, subject_)
{
}

Node* ComplementNode::Clone(CloneContext& cloneContext) const
{
    ComplementNode* clone = new ComplementNode(GetSpan(), ModuleId(), Subject()->Clone(cloneContext));
    return clone;
}

void ComplementNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string ComplementNode::ToString() const
{
    return "~" + Subject()->ToString();
}

DerefNode::DerefNode(const Span& span_, const boost::uuids::uuid& moduleId_) : UnaryNode(NodeType::derefNode, span_, moduleId_)
{
}

DerefNode::DerefNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_) : UnaryNode(NodeType::derefNode, span_, moduleId_, subject_)
{
}

Node* DerefNode::Clone(CloneContext& cloneContext) const
{
    DerefNode* clone = new DerefNode(GetSpan(), ModuleId(), Subject()->Clone(cloneContext));
    return clone;
}

void DerefNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string DerefNode::ToString() const
{
    return "*" + Subject()->ToString();
}

AddrOfNode::AddrOfNode(const Span& span_, const boost::uuids::uuid& moduleId_) : UnaryNode(NodeType::addrOfNode, span_, moduleId_)
{
}

AddrOfNode::AddrOfNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_) : 
    UnaryNode(NodeType::addrOfNode , span_, moduleId_, subject_)
{
}

Node* AddrOfNode::Clone(CloneContext& cloneContext) const
{
    AddrOfNode* clone = new AddrOfNode(GetSpan(), ModuleId(), Subject()->Clone(cloneContext));
    return clone;
}

void AddrOfNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string AddrOfNode::ToString() const
{
    return "&" + Subject()->ToString();
}

IsNode::IsNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::isNode, span_, moduleId_), expr(), targetTypeExpr()
{
}

IsNode::IsNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* expr_, Node* targetTypeExpr_) : 
    Node(NodeType::isNode, span_, moduleId_), expr(expr_), targetTypeExpr(targetTypeExpr_)
{
    expr->SetParent(this);
    targetTypeExpr->SetParent(this);
}

Node* IsNode::Clone(CloneContext& cloneContext) const
{
    IsNode* clone = new IsNode(GetSpan(), ModuleId(), expr->Clone(cloneContext), targetTypeExpr->Clone(cloneContext));
    return clone;
}

void IsNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void IsNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.Write(expr.get());
    writer.Write(targetTypeExpr.get());
}

void IsNode::Read(AstReader& reader)
{
    Node::Read(reader);
    expr.reset(reader.ReadNode());
    expr->SetParent(this);
    targetTypeExpr.reset(reader.ReadNode());
    targetTypeExpr->SetParent(this);
}

std::string IsNode::ToString() const
{
    return expr->ToString() + " is " + targetTypeExpr->ToString();
}

AsNode::AsNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::asNode, span_, moduleId_), expr(), targetTypeExpr()
{
}

AsNode::AsNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* expr_, Node* targetTypeExpr_) : Node(NodeType::asNode, span_, moduleId_), expr(expr_), targetTypeExpr(targetTypeExpr_)
{
    expr->SetParent(this);
    targetTypeExpr->SetParent(this);
}

Node* AsNode::Clone(CloneContext& cloneContext) const
{
    AsNode* clone = new AsNode(GetSpan(), ModuleId(), expr->Clone(cloneContext), targetTypeExpr->Clone(cloneContext));
    return clone;
}

void AsNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void AsNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.Write(expr.get());
    writer.Write(targetTypeExpr.get());
}

void AsNode::Read(AstReader& reader)
{
    Node::Read(reader);
    expr.reset(reader.ReadNode());
    expr->SetParent(this);
    targetTypeExpr.reset(reader.ReadNode());
    targetTypeExpr->SetParent(this);
}

std::string AsNode::ToString() const
{
    return expr->ToString() + " as " + targetTypeExpr->ToString();
}

IndexingNode::IndexingNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::indexingNode, span_, moduleId_), subject(), index()
{
}

IndexingNode::IndexingNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_, Node* index_) : 
    Node(NodeType::indexingNode, span_, moduleId_), subject(subject_), index(index_)
{
    subject->SetParent(this);
    index->SetParent(this);
}

Node* IndexingNode::Clone(CloneContext& cloneContext) const
{
    IndexingNode* clone = new IndexingNode(GetSpan(), ModuleId(), subject->Clone(cloneContext), index->Clone(cloneContext));
    return clone;
}

void IndexingNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void IndexingNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.Write(subject.get());
    writer.Write(index.get());
}

void IndexingNode::Read(AstReader& reader)
{
    Node::Read(reader);
    subject.reset(reader.ReadNode());
    subject->SetParent(this);
    index.reset(reader.ReadNode());
    index->SetParent(this);
}

std::string IndexingNode::ToString() const
{
    return subject->ToString() + "[" + index->ToString() + "]";
}

InvokeNode::InvokeNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::invokeNode, span_, moduleId_), subject(), arguments()
{
}

InvokeNode::InvokeNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_) : 
    Node(NodeType::invokeNode, span_, moduleId_), subject(subject_), arguments()
{
    subject->SetParent(this);
}

Node* InvokeNode::Clone(CloneContext& cloneContext) const
{
    InvokeNode* clone = new InvokeNode(GetSpan(), ModuleId(), subject->Clone(cloneContext));
    int n = arguments.Count();
    for (int i = 0; i < n; ++i)
    {
        Node* argument = arguments[i];
        clone->AddArgument(argument->Clone(cloneContext));
    }
    return clone;
}

void InvokeNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void InvokeNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.Write(subject.get());
    arguments.Write(writer);
}

void InvokeNode::Read(AstReader& reader)
{
    Node::Read(reader);
    subject.reset(reader.ReadNode());
    subject->SetParent(this);
    arguments.Read(reader);
    arguments.SetParent(this);
}

void InvokeNode::AddArgument(Node* argument)
{
    argument->SetParent(this);
    arguments.Add(argument);
}

std::string InvokeNode::ToString() const
{
    std::string s = subject->ToString();
    s.append("(");
    int n = arguments.Count();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            s.append(", ");
        }
        s.append(arguments[i]->ToString());
    }
    s.append(")");
    return s;
}

PostfixIncrementNode::PostfixIncrementNode(const Span& span_, const boost::uuids::uuid& moduleId_) : UnaryNode(NodeType::postfixIncrementNode, span_, moduleId_)
{
}

PostfixIncrementNode::PostfixIncrementNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_) : 
    UnaryNode(NodeType::postfixIncrementNode, span_, moduleId_, subject_)
{
}

Node* PostfixIncrementNode::Clone(CloneContext& cloneContext) const
{
    PostfixIncrementNode* clone = new PostfixIncrementNode(GetSpan(), ModuleId(), Subject()->Clone(cloneContext));
    return clone;
}

void PostfixIncrementNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string PostfixIncrementNode::ToString() const
{
    return Subject()->ToString() + "++";
}

PostfixDecrementNode::PostfixDecrementNode(const Span& span_, const boost::uuids::uuid& moduleId_) : UnaryNode(NodeType::postfixDecrementNode, span_, moduleId_)
{
}

PostfixDecrementNode::PostfixDecrementNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_) : UnaryNode(NodeType::postfixDecrementNode, span_, moduleId_, subject_)
{
}

Node* PostfixDecrementNode::Clone(CloneContext& cloneContext) const
{
    PostfixDecrementNode* clone = new PostfixDecrementNode(GetSpan(), ModuleId(), Subject()->Clone(cloneContext));
    return clone;
}

void PostfixDecrementNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string PostfixDecrementNode::ToString() const
{
    return Subject()->ToString() + "--";
}

SizeOfNode::SizeOfNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::sizeOfNode, span_, moduleId_), expression()
{
}

SizeOfNode::SizeOfNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* expression_) : 
    Node(NodeType::sizeOfNode, span_, moduleId_), expression(expression_)
{
    expression->SetParent(this);
}

Node* SizeOfNode::Clone(CloneContext& cloneContext) const
{
    SizeOfNode* clone = new SizeOfNode(GetSpan(), ModuleId(), expression->Clone(cloneContext));
    return clone;
}

void SizeOfNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void SizeOfNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.Write(expression.get());
}

void SizeOfNode::Read(AstReader& reader)
{
    Node::Read(reader);
    expression.reset(reader.ReadNode());
    expression->SetParent(this);
}

std::string SizeOfNode::ToString() const
{
    return "sizeof(" + expression->ToString() + ")";
}

TypeNameNode::TypeNameNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::typeNameNode, span_, moduleId_), expression(), static_(false)
{
}

TypeNameNode::TypeNameNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* expression_) : 
    Node(NodeType::typeNameNode, span_, moduleId_), expression(expression_), static_(false)
{
    expression->SetParent(this);
}

Node* TypeNameNode::Clone(CloneContext& cloneContext) const
{
    TypeNameNode* clone = new TypeNameNode(GetSpan(), ModuleId(), expression->Clone(cloneContext));
    if (static_)
    {
        clone->SetStatic();
    }
    return clone;
}

void TypeNameNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void TypeNameNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.Write(expression.get());
    writer.GetBinaryWriter().Write(static_);
}

void TypeNameNode::Read(AstReader& reader)
{
    Node::Read(reader);
    expression.reset(reader.ReadNode());
    expression->SetParent(this);
    static_ = reader.GetBinaryReader().ReadBool();
}

std::string TypeNameNode::ToString() const
{
    return "typename(" + expression->ToString() + ")";
}

TypeIdNode::TypeIdNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::typeIdNode, span_, moduleId_), expression()
{
}

TypeIdNode::TypeIdNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* expression_) : Node(NodeType::typeIdNode, span_, moduleId_), expression(expression_)
{
    expression->SetParent(this);
}

Node* TypeIdNode::Clone(CloneContext& cloneContext) const
{
    TypeIdNode* clone = new TypeIdNode(GetSpan(), ModuleId(), expression->Clone(cloneContext));
    return clone;
}

void TypeIdNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void TypeIdNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.Write(expression.get());
}

void TypeIdNode::Read(AstReader& reader)
{
    Node::Read(reader);
    expression.reset(reader.ReadNode());
    expression->SetParent(this);
}

std::string TypeIdNode::ToString() const
{
    return "typeid(" + expression->ToString() + ")";
}

CastNode::CastNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::castNode, span_, moduleId_), targetTypeExpr(), sourceExpr()
{
}

CastNode::CastNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* targetTypeExpr_, Node* sourceExpr_) : 
    Node(NodeType::castNode, span_, moduleId_), targetTypeExpr(targetTypeExpr_), sourceExpr(sourceExpr_)
{
    targetTypeExpr->SetParent(this);
    sourceExpr->SetParent(this);
}

Node* CastNode::Clone(CloneContext& cloneContext) const
{
    CastNode* clone = new CastNode(GetSpan(), ModuleId(), targetTypeExpr->Clone(cloneContext), sourceExpr->Clone(cloneContext));
    return clone;
}

void CastNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void CastNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.Write(targetTypeExpr.get());
    writer.Write(sourceExpr.get());
}

void CastNode::Read(AstReader& reader)
{
    Node::Read(reader);
    targetTypeExpr.reset(reader.ReadNode());
    targetTypeExpr->SetParent(this);
    sourceExpr.reset(reader.ReadNode());
    sourceExpr->SetParent(this);
}

std::string CastNode::ToString() const
{
    return "cast<" + targetTypeExpr->ToString() + ">(" + sourceExpr->ToString() + ")";
}

ConstructNode::ConstructNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::constructNode, span_, moduleId_), typeExpr(), arguments()
{
}

ConstructNode::ConstructNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* typeExpr_) : 
    Node(NodeType::constructNode, span_, moduleId_), typeExpr(typeExpr_), arguments()
{
    typeExpr->SetParent(this);
}

Node* ConstructNode::Clone(CloneContext& cloneContext) const
{
    ConstructNode* clone = new ConstructNode(GetSpan(), ModuleId(), typeExpr->Clone(cloneContext));
    int n = arguments.Count();
    for (int i = 0; i < n; ++i)
    {
        Node* argument = arguments[i];
        clone->AddArgument(argument->Clone(cloneContext));
    }
    return clone;
}

void ConstructNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void ConstructNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.Write(typeExpr.get());
    arguments.Write(writer);
}

void ConstructNode::Read(AstReader& reader)
{
    Node::Read(reader);
    typeExpr.reset(reader.ReadNode());
    typeExpr->SetParent(this);
    arguments.Read(reader);
    arguments.SetParent(this);
}

void ConstructNode::AddArgument(Node* argument)
{
    argument->SetParent(this);
    arguments.Add(argument);
}

std::string ConstructNode::ToString() const
{
    std::string s = "construct<" + typeExpr->ToString() + ">(";
    int n = arguments.Count();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            s.append(", ");
        }
        s.append(arguments[i]->ToString());
    }
    s.append(")");
    return s;
}

NewNode::NewNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::newNode, span_, moduleId_), typeExpr(), arguments()
{
}

NewNode::NewNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* typeExpr_) : 
    Node(NodeType::newNode, span_, moduleId_), typeExpr(typeExpr_), arguments()
{
    typeExpr->SetParent(this);
}

Node* NewNode::Clone(CloneContext& cloneContext) const
{
    NewNode* clone = new NewNode(GetSpan(), ModuleId(), typeExpr->Clone(cloneContext));
    int n = arguments.Count();
    for (int i = 0; i < n; ++i)
    {
        Node* argument = arguments[i];
        clone->AddArgument(argument->Clone(cloneContext));
    }
    return clone;
}

void NewNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void NewNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.Write(typeExpr.get());
    arguments.Write(writer);
}

void NewNode::Read(AstReader& reader)
{
    Node::Read(reader);
    typeExpr.reset(reader.ReadNode());
    typeExpr->SetParent(this);
    arguments.Read(reader);
    arguments.SetParent(this);
}

void NewNode::AddArgument(Node* argument)
{
    argument->SetParent(this);
    arguments.Add(argument);
}

std::string NewNode::ToString() const
{
    std::string s = "new ";
    s.append(typeExpr->ToString()).append("(");
    int n = arguments.Count();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            s.append(", ");
        }
        s.append(arguments[i]->ToString());
    }
    s.append(")");
    return s;
}

ThisNode::ThisNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::thisNode, span_, moduleId_)
{
}

Node* ThisNode::Clone(CloneContext& cloneContext) const
{
    ThisNode* clone = new ThisNode(GetSpan(), ModuleId());
    return clone;
}

void ThisNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string ThisNode::ToString() const
{
    return "this";
}

BaseNode::BaseNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::baseNode, span_, moduleId_)
{
}

Node* BaseNode::Clone(CloneContext& cloneContext) const
{
    BaseNode* clone = new BaseNode(GetSpan(), ModuleId());
    return clone;
}

void BaseNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string BaseNode::ToString() const
{
    return "base";
}

ParenthesizedExpressionNode::ParenthesizedExpressionNode(const Span& span_, const boost::uuids::uuid& moduleId_) : 
    UnaryNode(NodeType::parenthesizedExpressionNode, span_, moduleId_)
{
}

ParenthesizedExpressionNode::ParenthesizedExpressionNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* child_) : 
    UnaryNode(NodeType::parenthesizedExpressionNode, span_, moduleId_, child_)
{
}

Node* ParenthesizedExpressionNode::Clone(CloneContext& cloneContext) const
{
    ParenthesizedExpressionNode* clone = new ParenthesizedExpressionNode(GetSpan(), ModuleId(), Subject()->Clone(cloneContext));
    return clone;
}

void ParenthesizedExpressionNode::Write(AstWriter& writer)
{
    UnaryNode::Write(writer);
}

void ParenthesizedExpressionNode::Read(AstReader& reader)
{
    UnaryNode::Read(reader);
}

void ParenthesizedExpressionNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string ParenthesizedExpressionNode::ToString() const
{
    return "(" + Subject()->ToString() + ")";
}

} } // namespace sngcm::ast
