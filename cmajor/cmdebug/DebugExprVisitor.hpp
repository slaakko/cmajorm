// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_DEBUG_EXPR_VISITOR_INCLUDED
#define CMAJOR_DEBUG_DEBUG_EXPR_VISITOR_INCLUDED
#include <cmajor/cmdebug/DebugApi.hpp>

namespace cmajor { namespace debug {

class DebugExpression;
class IdentifierDebugExprNode;
class IntegerDebugExprNode;
class AddDebugExprNode;
class SubDebugExprNode;
class ParenthesizedDebugExprNode;
class DerefDebugExprNode;
class AddrOfDebugExprNode;
class DotDebugExprNode;
class SubscriptDebugExprNode;
class RangeDebugExprNode;
class TypeIdDebugExprNode;
class CastDebugExprNode;
class BaseDebugExprNode;

class DEBUG_API DebugExprVisitor
{
public:
    virtual ~DebugExprVisitor();
    virtual void Visit(DebugExpression& expr) {}
    virtual void Visit(IdentifierDebugExprNode& node) {}
    virtual void Visit(IntegerDebugExprNode& node) {}
    virtual void Visit(AddDebugExprNode& node) {}
    virtual void Visit(SubDebugExprNode& node) {}
    virtual void Visit(ParenthesizedDebugExprNode& node) {}
    virtual void Visit(DerefDebugExprNode& node) {}
    virtual void Visit(AddrOfDebugExprNode& node) {}
    virtual void Visit(DotDebugExprNode& node) {}
    virtual void Visit(SubscriptDebugExprNode& node) {}
    virtual void Visit(RangeDebugExprNode& node) {}
    virtual void Visit(TypeIdDebugExprNode& node) {}
    virtual void Visit(CastDebugExprNode& node) {}
    virtual void Visit(BaseDebugExprNode& node) {}
};

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_DEBUG_EXPR_VISITOR_INCLUDED
