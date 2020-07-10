// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_DEBUG_EXPR_VISITOR_INCLUDED
#define CMAJOR_DEBUG_DEBUG_EXPR_VISITOR_INCLUDED
#include <cmajor/cmdebug/DebugApi.hpp>

namespace cmajor { namespace debug {

class VariableNameDebugExprNode;
class IntegerDebugExprNode;
class ParenthesizedDebugExprNode;
class DerefDebugExprNode;
class AddrOfDebugExprNode;
class UnaryMinusDebugExprNode;
class UnaryPlusDebugExprNode;
class DotDebugExprNode;
class SubscriptDebugExprNode;
class TypeIdDebugExprNode;
class CastDebugExprNode;
class BaseDebugExprNode;

class DEBUG_API DebugExprVisitor
{
public:
    virtual void Visit(VariableNameDebugExprNode& node) {}
    virtual void Visit(IntegerDebugExprNode& node) {}
    virtual void Visit(ParenthesizedDebugExprNode& node) {}
    virtual void Visit(DerefDebugExprNode& node) {}
    virtual void Visit(AddrOfDebugExprNode& node) {}
    virtual void Visit(UnaryMinusDebugExprNode& node) {}
    virtual void Visit(UnaryPlusDebugExprNode& node) {}
    virtual void Visit(DotDebugExprNode& node) {}
    virtual void Visit(SubscriptDebugExprNode& node) {}
    virtual void Visit(TypeIdDebugExprNode& node) {}
    virtual void Visit(CastDebugExprNode& node) {}
    virtual void Visit(BaseDebugExprNode& node) {}
};

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_DEBUG_EXPR_VISITOR_INCLUDED
