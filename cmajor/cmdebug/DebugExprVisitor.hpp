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
class DisjunctionDebugExprNode;
class ConjunctionDebugExprNode;
class BitOrDebugExprNode;
class BitXorDebugExprNode;
class BitAndDebugExprNode;
class EqualDebugExprNode;
class NotEqualDebugExprNode;
class LessEqualDebugExprNode;
class GreaterEqualDebugExprNode;
class LessDebugExprNode;
class GreaterDebugExprNode;
class ShiftLeftDebugExprNode;
class ShiftRightDebugExprNode;
class AddDebugExprNode;
class SubDebugExprNode;
class MulDebugExprNode;
class DivDebugExprNode;
class ModDebugExprNode;
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
    virtual void Visit(DisjunctionDebugExprNode& node) {}
    virtual void Visit(ConjunctionDebugExprNode& node) {}
    virtual void Visit(BitOrDebugExprNode& node) {}
    virtual void Visit(BitXorDebugExprNode& node) {}
    virtual void Visit(BitAndDebugExprNode& node) {}
    virtual void Visit(EqualDebugExprNode& node) {}
    virtual void Visit(NotEqualDebugExprNode& node) {}
    virtual void Visit(LessEqualDebugExprNode& node) {}
    virtual void Visit(GreaterEqualDebugExprNode& node) {}
    virtual void Visit(LessDebugExprNode& node) {}
    virtual void Visit(GreaterDebugExprNode& node) {}
    virtual void Visit(ShiftLeftDebugExprNode& node) {}
    virtual void Visit(ShiftRightDebugExprNode& node) {}
    virtual void Visit(AddDebugExprNode& node) {}
    virtual void Visit(SubDebugExprNode& node) {}
    virtual void Visit(MulDebugExprNode& node) {}
    virtual void Visit(DivDebugExprNode& node) {}
    virtual void Visit(ModDebugExprNode& node) {}
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
