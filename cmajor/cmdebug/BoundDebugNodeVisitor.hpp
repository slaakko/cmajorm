// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_BOUND_DEBUG_NODE_VISITOR_INCLUDED
#define CMAJOR_DEBUG_BOUND_DEBUG_NODE_VISITOR_INCLUDED
#include <cmajor/cmdebug/DebugApi.hpp>

namespace cmajor { namespace debug {

class BoundDebugExpression;
class BoundVariableReferenceNode;
class BoundIntegerLiteralNode;
class BoundAddNode;
class BoundSubNode;
class BoundDerefNode;
class BoundAddrOfNode;
class BoundDotNode;
class BoundSubscriptNode;
class BoundRangeNode;
class BoundParenExprNode;
class BoundTypeNode;
class BoundCastNode;

class DEBUG_API BoundDebugNodeVisitor
{
public:
    virtual ~BoundDebugNodeVisitor();
    virtual void Visit(BoundDebugExpression& expr) {}
    virtual void Visit(BoundVariableReferenceNode& node) {}
    virtual void Visit(BoundIntegerLiteralNode& node) {}
    virtual void Visit(BoundAddNode& node) {}
    virtual void Visit(BoundSubNode& node) {}
    virtual void Visit(BoundDerefNode& node) {}
    virtual void Visit(BoundAddrOfNode& node) {}
    virtual void Visit(BoundDotNode& node) {}
    virtual void Visit(BoundSubscriptNode& node) {}
    virtual void Visit(BoundRangeNode& node) {}
    virtual void Visit(BoundParenExprNode& node) {}
    virtual void Visit(BoundTypeNode& node) {}
    virtual void Visit(BoundCastNode& node) {}
};

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_BOUND_DEBUG_NODE_VISITOR_INCLUDED
