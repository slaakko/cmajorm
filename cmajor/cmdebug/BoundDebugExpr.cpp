// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/BoundDebugExpr.hpp>
#include <cmajor/cmdebug/BoundDebugNodeVisitor.hpp>
#include <cmajor/cmdebug/DIVariable.hpp>
#include <cmajor/cmdebug/DebugInfo.hpp>
#include <cmajor/cmdebug/Debugger.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace cmajor { namespace debug {

std::string InitializationStatusStr(InitializationStatus status)
{
    switch (status)
    {
        case InitializationStatus::unknown: return "unknown";
        case InitializationStatus::initialized: return "initialized";
        case InitializationStatus::uninitialized: return "uninitialized";
    }
    return std::string();
}

BoundDebugNode::BoundDebugNode(Kind kind_, DIType* type_, DebugExprNode* sourceNode_) : kind(kind_), type(type_), sourceNode(sourceNode_)
{
}

BoundDebugNode::~BoundDebugNode()
{
}

BoundVariableReferenceNode::BoundVariableReferenceNode(DIType* type, DIVariable* variable_, InitializationStatus status_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::variableReferenceNode, type, sourceNode), variable(variable_), status(status_)
{
}

void BoundVariableReferenceNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundVariableReferenceNode::ToString() const
{
    return variable->Name();
}

BoundDebugNode* BoundVariableReferenceNode::Clone() const
{
    return new BoundVariableReferenceNode(Type(), variable, status, SourceNode());
}

std::string BoundVariableReferenceNode::GdbExprString() const
{
    return variable->IrName();
}

BoundIntegerLiteralNode::BoundIntegerLiteralNode(DIType* type, int64_t value_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::integerLiteralNode, type, sourceNode), value(value_)
{
}

void BoundIntegerLiteralNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundIntegerLiteralNode::ToString() const
{
    return std::to_string(value);
}

BoundDebugNode* BoundIntegerLiteralNode::Clone() const
{
    return new BoundIntegerLiteralNode(Type(), value, SourceNode());
}

std::string BoundIntegerLiteralNode::GdbExprString() const
{
    return std::to_string(value);
}

BoundDisjunctionNode::BoundDisjunctionNode(DIType* type, BoundDebugNode* left_, BoundDebugNode* right_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::disjunctionNode, type, sourceNode), left(left_), right(right_)
{
}

void BoundDisjunctionNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundDisjunctionNode::ToString() const
{
    return left->ToString() + "||" + right->ToString();
}

BoundDebugNode* BoundDisjunctionNode::Clone() const
{
    return new BoundDisjunctionNode(Type(), left->Clone(), right->Clone(), SourceNode());
}

std::string BoundDisjunctionNode::GdbExprString() const
{
    return left->GdbExprString() + "||" + right->GdbExprString();
}

BoundConjunctionNode::BoundConjunctionNode(DIType* type, BoundDebugNode* left_, BoundDebugNode* right_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::conjunctionNode, type, sourceNode), left(left_), right(right_)
{
}

void BoundConjunctionNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundConjunctionNode::ToString() const
{
    return left->ToString() + "&&" + right->ToString();
}

BoundDebugNode* BoundConjunctionNode::Clone() const
{
    return new BoundConjunctionNode(Type(), left->Clone(), right->Clone(), SourceNode());
}

std::string BoundConjunctionNode::GdbExprString() const
{
    return left->GdbExprString() + "&&" + right->GdbExprString();
}

BoundBitOrNode::BoundBitOrNode(DIType* type, BoundDebugNode* left_, BoundDebugNode* right_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::bitOrNode, type, sourceNode), left(left_), right(right_)
{
}

void BoundBitOrNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundBitOrNode::ToString() const
{
    return left->ToString() + "|" + right->ToString();
}

BoundDebugNode* BoundBitOrNode::Clone() const
{
    return new BoundBitOrNode(Type(), left->Clone(), right->Clone(), SourceNode());
}

std::string BoundBitOrNode::GdbExprString() const
{
    return left->GdbExprString() + "|" + right->GdbExprString();
}

BoundBitXorNode::BoundBitXorNode(DIType* type, BoundDebugNode* left_, BoundDebugNode* right_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::bitXorNode, type, sourceNode), left(left_), right(right_)
{
}

void BoundBitXorNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundBitXorNode::ToString() const
{
    return left->ToString() + "^" + right->ToString();
}

BoundDebugNode* BoundBitXorNode::Clone() const
{
    return new BoundBitXorNode(Type(), left->Clone(), right->Clone(), SourceNode());
}

std::string BoundBitXorNode::GdbExprString() const
{
    return left->GdbExprString() + "^" + right->GdbExprString();
}

BoundBitAndNode::BoundBitAndNode(DIType* type, BoundDebugNode* left_, BoundDebugNode* right_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::bitAndNode , type, sourceNode), left(left_), right(right_)
{
}

void BoundBitAndNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundBitAndNode::ToString() const
{
    return left->ToString() + "&" + right->ToString();
}

BoundDebugNode* BoundBitAndNode::Clone() const
{
    return new BoundBitAndNode(Type(), left->Clone(), right->Clone(), SourceNode());
}

std::string BoundBitAndNode::GdbExprString() const
{
    return left->GdbExprString() + "&" + right->GdbExprString();
}

BoundEqualNode::BoundEqualNode(DIType* type, BoundDebugNode* left_, BoundDebugNode* right_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::equalNode, type, sourceNode), left(left_), right(right_)
{
}

void BoundEqualNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundEqualNode::ToString() const
{
    return left->ToString() + "==" + right->ToString();
}

BoundDebugNode* BoundEqualNode::Clone() const
{
    return new BoundEqualNode(Type(), left->Clone(), right->Clone(), SourceNode());
}

std::string BoundEqualNode::GdbExprString() const
{
    return left->GdbExprString() + "==" + right->GdbExprString();
}

BoundNotEqualNode::BoundNotEqualNode(DIType* type, BoundDebugNode* left_, BoundDebugNode* right_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::notEqualNode, type, sourceNode), left(left_), right(right_)
{
}

void BoundNotEqualNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundNotEqualNode::ToString() const
{
    return left->ToString() + "!=" + right->ToString();
}

BoundDebugNode* BoundNotEqualNode::Clone() const
{
    return new BoundNotEqualNode(Type(), left->Clone(), right->Clone(), SourceNode());
}

std::string BoundNotEqualNode::GdbExprString() const
{
    return left->GdbExprString() + "!=" + right->GdbExprString();
}

BoundLessEqualNode::BoundLessEqualNode(DIType* type, BoundDebugNode* left_, BoundDebugNode* right_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::lessEqualNode, type, sourceNode), left(left_), right(right_)
{
}

void BoundLessEqualNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundLessEqualNode::ToString() const
{
    return left->ToString() + "<=" + right->ToString();
}

BoundDebugNode* BoundLessEqualNode::Clone() const
{
    return new BoundLessEqualNode(Type(), left->Clone(), right->Clone(), SourceNode());
}

std::string BoundLessEqualNode::GdbExprString() const
{
    return left->GdbExprString() + "<=" + right->GdbExprString();
}

BoundGreaterEqualNode::BoundGreaterEqualNode(DIType* type, BoundDebugNode* left_, BoundDebugNode* right_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::greaterEqualNode, type, sourceNode), left(left_), right(right_)
{
}

void BoundGreaterEqualNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundGreaterEqualNode::ToString() const
{
    return left->ToString() + ">=" + right->ToString();
}

BoundDebugNode* BoundGreaterEqualNode::Clone() const
{
    return new BoundGreaterEqualNode(Type(), left->Clone(), right->Clone(), SourceNode());
}

std::string BoundGreaterEqualNode::GdbExprString() const
{
    return left->GdbExprString() + ">=" + right->GdbExprString();
}

BoundLessNode::BoundLessNode(DIType* type, BoundDebugNode* left_, BoundDebugNode* right_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::lessNode, type, sourceNode), left(left_), right(right_)
{
}

void BoundLessNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundLessNode::ToString() const
{
    return left->ToString() + "<" + right->ToString();
}

BoundDebugNode* BoundLessNode::Clone() const
{
    return new BoundLessNode(Type(), left->Clone(), right->Clone(), SourceNode());
}

std::string BoundLessNode::GdbExprString() const
{
    return left->GdbExprString() + "<" + right->GdbExprString();
}

BoundGreaterNode::BoundGreaterNode(DIType* type, BoundDebugNode* left_, BoundDebugNode* right_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::greaterNode, type, sourceNode), left(left_), right(right_)
{
}

void BoundGreaterNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundGreaterNode::ToString() const
{
    return left->ToString() + ">" + right->ToString();
}

BoundDebugNode* BoundGreaterNode::Clone() const
{
    return new BoundGreaterNode(Type(), left->Clone(), right->Clone(), SourceNode());
}

std::string BoundGreaterNode::GdbExprString() const
{
    return left->GdbExprString() + ">" + right->GdbExprString();
}

BoundShiftLeftNode::BoundShiftLeftNode(DIType* type, BoundDebugNode* left_, BoundDebugNode* right_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::shiftLeftNode, type, sourceNode), left(left_), right(right_)
{
}

void BoundShiftLeftNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundShiftLeftNode::ToString() const
{
    return left->ToString() + "<<" + right->ToString();
}

BoundDebugNode* BoundShiftLeftNode::Clone() const
{
    return new BoundShiftLeftNode(Type(), left->Clone(), right->Clone(), SourceNode());
}

std::string BoundShiftLeftNode::GdbExprString() const
{
    return left->GdbExprString() + "<<" + right->GdbExprString();
}

BoundShiftRightNode::BoundShiftRightNode(DIType* type, BoundDebugNode* left_, BoundDebugNode* right_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::shiftRightNode, type, sourceNode), left(left_), right(right_)
{
}

void BoundShiftRightNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundShiftRightNode::ToString() const
{
    return left->ToString() + ">>" + right->ToString();
}

BoundDebugNode* BoundShiftRightNode::Clone() const
{
    return new BoundShiftRightNode(Type(), left->Clone(), right->Clone(), SourceNode());
}

std::string BoundShiftRightNode::GdbExprString() const
{
    return left->GdbExprString() + ">>" + right->GdbExprString();
}

BoundAddNode::BoundAddNode(DIType* type, BoundDebugNode* left_, BoundDebugNode* right_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::addNode, type, sourceNode), left(left_), right(right_)
{
}

void BoundAddNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundAddNode::ToString() const
{
    return left->ToString() + "+" + right->ToString();
}

BoundDebugNode* BoundAddNode::Clone() const
{
    return new BoundAddNode(Type(), left->Clone(), right->Clone(), SourceNode());
}

std::string BoundAddNode::GdbExprString() const
{
    return left->GdbExprString() + "+" + right->GdbExprString();
}

BoundSubNode::BoundSubNode(DIType* type, BoundDebugNode* left_, BoundDebugNode* right_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::subNode, type, sourceNode), left(left_), right(right_)
{
}

void BoundSubNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundSubNode::ToString() const
{
    return left->ToString() + "-" + right->ToString();
}

BoundDebugNode* BoundSubNode::Clone() const
{
    return new BoundSubNode(Type(), left->Clone(), right->Clone(), SourceNode());
}

std::string BoundSubNode::GdbExprString() const
{
    return left->GdbExprString() + "-" + right->GdbExprString();
}

BoundMulNode::BoundMulNode(DIType* type, BoundDebugNode* left_, BoundDebugNode* right_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::mulNode, type, sourceNode), left(left_), right(right_)
{
}

void BoundMulNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundMulNode::ToString() const
{
    return left->ToString() + "*" + right->ToString();
}

BoundDebugNode* BoundMulNode::Clone() const
{
    return new BoundMulNode(Type(), left->Clone(), right->Clone(), SourceNode());
}

std::string BoundMulNode::GdbExprString() const
{
    return left->GdbExprString() + "*" + right->GdbExprString();
}

BoundDivNode::BoundDivNode(DIType* type, BoundDebugNode* left_, BoundDebugNode* right_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::divNode, type, sourceNode), left(left_), right(right_)
{
}

void BoundDivNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundDivNode::ToString() const
{
    return left->ToString() + "/" + right->ToString();
}

BoundDebugNode* BoundDivNode::Clone() const
{
    return new BoundDivNode(Type(), left->Clone(), right->Clone(), SourceNode());
}

std::string BoundDivNode::GdbExprString() const
{
    return left->GdbExprString() + "/" + right->GdbExprString();
}

BoundModNode::BoundModNode(DIType* type, BoundDebugNode* left_, BoundDebugNode* right_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::modNode, type, sourceNode), left(left_), right(right_)
{
}

void BoundModNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundModNode::ToString() const
{
    return left->ToString() + "%" + right->ToString();
}

BoundDebugNode* BoundModNode::Clone() const
{
    return new BoundModNode(Type(), left->Clone(), right->Clone(), SourceNode());
}

std::string BoundModNode::GdbExprString() const
{
    return left->GdbExprString() + "%" + right->GdbExprString();
}

BoundDerefNode::BoundDerefNode(DIType* type, BoundDebugNode* subject_, DebugExprNode* sourceNode) : BoundDebugNode(Kind::derefNode, type, sourceNode), subject(subject_)
{
}

void BoundDerefNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundDerefNode::ToString() const
{
    return "(*" + subject->ToString() + ")";
}

BoundDebugNode* BoundDerefNode::Clone() const
{
    return new BoundDerefNode(Type(), subject->Clone(), SourceNode());
}

std::string BoundDerefNode::GdbExprString() const
{
    return "(*" + subject->GdbExprString() + ")";
}

BoundAddrOfNode::BoundAddrOfNode(DIType* type, BoundDebugNode* subject_, DebugExprNode* sourceNode) : BoundDebugNode(Kind::addrOfNode, type, sourceNode), subject(subject_)
{
}

void BoundAddrOfNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundAddrOfNode::ToString() const
{
    return "&" + subject->ToString();
}

BoundDebugNode* BoundAddrOfNode::Clone() const
{
    return new BoundAddrOfNode(Type(), subject->Clone(), SourceNode());
}

std::string BoundAddrOfNode::GdbExprString() const
{
    return "&" + subject->GdbExprString();
}

BoundDotNode::BoundDotNode(DIType* type_, BoundDebugNode* subject_, const std::string& member_, const std::string& gdbExprString_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::dotNode, type_, sourceNode), type(type_), subject(subject_), member(member_), gdbExprString(gdbExprString_)
{
}

void BoundDotNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundDotNode::ToString() const
{
    return subject->ToString() + "." + member;
}

BoundDebugNode* BoundDotNode::Clone() const
{
    return new BoundDotNode(type, subject->Clone(), member, gdbExprString, SourceNode());
}

BoundSubscriptNode::BoundSubscriptNode(DIType* type, BoundDebugNode* subject_, BoundDebugNode* index_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::subscriptNode, type, sourceNode), subject(subject_), index(index_)
{
}

void BoundSubscriptNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundSubscriptNode::ToString() const
{
    return subject->ToString() + "[" + index->ToString() + "]";
}

BoundDebugNode* BoundSubscriptNode::Clone() const
{
    return new BoundSubscriptNode(Type(), subject->Clone(), index->Clone(), SourceNode());
}

std::string BoundSubscriptNode::GdbExprString() const
{
    return subject->GdbExprString() + "[" + index->GdbExprString() + "]";
}

BoundRangeNode::BoundRangeNode(DIType* elementType, BoundDebugNode* subject_, BoundDebugNode* rangeStart_, BoundDebugNode* rangeEnd_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::rangeNode, elementType, sourceNode), subject(subject_), rangeStart(rangeStart_), rangeEnd(rangeEnd_)
{
}

void BoundRangeNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundRangeNode::ToString() const
{
    return subject->ToString() + "[" + rangeStart->ToString() + ", " + rangeEnd->ToString() + "]";
}

BoundDebugNode* BoundRangeNode::Clone() const
{
    return new BoundRangeNode(Type(), subject->Clone(), rangeStart->Clone(), rangeEnd->Clone(), SourceNode());
}

std::string BoundRangeNode::GdbExprString() const
{
    return std::string();
}

BoundParenExprNode::BoundParenExprNode(BoundDebugNode* subject_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::parenExprNode, subject_->Type(), sourceNode), subject(subject_)
{
}

void BoundParenExprNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundParenExprNode::ToString() const
{
    return "(" + subject->ToString() + ")";
}

BoundDebugNode* BoundParenExprNode::Clone() const
{
    return new BoundParenExprNode(subject->Clone(), SourceNode());
}

std::string BoundParenExprNode::GdbExprString() const
{
    return "(" + subject->GdbExprString() + ")";
}

BoundTypeNode::BoundTypeNode(DIType* type, DebugExprNode* sourceNode) : BoundDebugNode(Kind::typeNode, type, sourceNode)
{
}

void BoundTypeNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundTypeNode::ToString() const
{
    return "typeid(\"" + boost::uuids::to_string(Type()->Id()) + "\")";
}

BoundDebugNode* BoundTypeNode::Clone() const
{
    return new BoundTypeNode(Type(), SourceNode());
}

std::string BoundTypeNode::GdbExprString() const
{
    return std::string();
}

BoundCastNode::BoundCastNode(BoundDebugNode* typeIdNode_, BoundDebugNode* exprNode_, const std::string& gdbExprString_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::castNode, typeIdNode_->Type(), sourceNode), typeIdNode(typeIdNode_), exprNode(exprNode_), gdbExprString(gdbExprString_)
{
}

void BoundCastNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundCastNode::ToString() const
{
    return "cast<" + typeIdNode->ToString() + ">(" + exprNode->ToString() + ")";
}

BoundDebugNode* BoundCastNode::Clone() const
{
    return new BoundCastNode(typeIdNode->Clone(), exprNode->Clone(), gdbExprString, SourceNode());
}

std::string BoundCastNode::GdbExprString() const
{
    return gdbExprString;
}

BoundDebugExpression::BoundDebugExpression(BoundDebugNode* node_, DebugExprNode* sourceNode, bool hasContainerSubscript_, InitializationStatus status_) :
    BoundDebugNode(Kind::debugExpression, node_->Type(), sourceNode), node(node_), hasContainerSubscript(hasContainerSubscript_), status(status_)
{
}

void BoundDebugExpression::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundDebugExpression::ToString() const
{
    return node->ToString();
}

BoundDebugNode* BoundDebugExpression::Clone() const
{
    return new BoundDebugExpression(node->Clone(), SourceNode(), hasContainerSubscript, status);
}

std::string BoundDebugExpression::GdbExprString() const
{
    return node->GdbExprString();
}

} } // namespace cmajor::debug
