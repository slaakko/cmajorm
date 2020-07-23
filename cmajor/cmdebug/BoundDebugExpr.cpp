// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/BoundDebugExpr.hpp>
#include <cmajor/cmdebug/BoundDebugNodeVisitor.hpp>
#include <cmajor/cmdebug/DIVariable.hpp>
#include <cmajor/cmdebug/DebugInfo.hpp>
#include <cmajor/cmdebug/Debugger.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace cmajor { namespace debug {

BoundDebugNode::BoundDebugNode(Kind kind_, DIType* type_, DebugExprNode* sourceNode_) : kind(kind_), type(type_), sourceNode(sourceNode_)
{
}

BoundDebugNode::~BoundDebugNode()
{
}

BoundAddressNode::BoundAddressNode(DIType* type, const std::string& value_, DebugExprNode* sourceNode) : BoundDebugNode(Kind::addressNode, type, sourceNode), value(value_)
{
}

void BoundAddressNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundAddressNode::ToString() const
{
    return value;
}

BoundDebugNode* BoundAddressNode::Clone() const
{
    return new BoundAddressNode(Type(), value, SourceNode());
}

std::string BoundAddressNode::GdbExprString() const
{
    return value;
}

BoundVariableReferenceNode::BoundVariableReferenceNode(DIType* type, DIVariable* variable_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::variableReferenceNode, type, sourceNode), variable(variable_)
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
    return new BoundVariableReferenceNode(Type(), variable, SourceNode());
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
    return left->ToString() + " + " + right->ToString();
}

BoundDebugNode* BoundAddNode::Clone() const
{
    return new BoundAddNode(Type(), left->Clone(), right->Clone(), SourceNode());
}

std::string BoundAddNode::GdbExprString() const
{
    return left->GdbExprString() + " + " + right->GdbExprString();
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
    return left->ToString() + " - " + right->ToString();
}

BoundDebugNode* BoundSubNode::Clone() const
{
    return new BoundSubNode(Type(), left->Clone(), right->Clone(), SourceNode());
}

std::string BoundSubNode::GdbExprString() const
{
    return left->GdbExprString() + " - " + right->GdbExprString();
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
    return "*" + subject->ToString();
}

BoundDebugNode* BoundDerefNode::Clone() const
{
    return new BoundDerefNode(Type(), subject->Clone(), SourceNode());
}

std::string BoundDerefNode::GdbExprString() const
{
    return "*" + subject->GdbExprString();
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

BoundParenExprNode::BoundParenExprNode(BoundDebugNode* subject_, DebugExprNode* sourceNode) : BoundDebugNode(Kind::parenExprNode, subject_->Type(), sourceNode), subject(subject_)
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

BoundDebuggerVarNode::BoundDebuggerVarNode(DIType* type, const DebuggerVariable* variable_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::debuggerVarNode, type, sourceNode), variable(variable_)
{
}

void BoundDebuggerVarNode::Accept(BoundDebugNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::string BoundDebuggerVarNode::ToString() const
{
    return "$" + std::to_string(variable->Index());
}

BoundDebugNode* BoundDebuggerVarNode::Clone() const
{
    return new BoundDebuggerVarNode(Type(), variable, SourceNode());
}

std::string BoundDebuggerVarNode::GdbExprString() const
{
    return variable->GdbVarName();
}

BoundDebugExpression::BoundDebugExpression(BoundDebugNode* node_, DebugExprNode* sourceNode) :
    BoundDebugNode(Kind::debugExpression, node_->Type(), sourceNode), node(node_)
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
    return new BoundDebugExpression(node->Clone(), SourceNode());
}

std::string BoundDebugExpression::GdbExprString() const
{
    return node->GdbExprString();
}

} } // namespace cmajor::debug
