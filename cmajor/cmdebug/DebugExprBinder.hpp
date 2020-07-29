// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_DEBUG_EXPR_BINDER_INCLUDED
#define CMAJOR_DEBUG_DEBUG_EXPR_BINDER_INCLUDED
#include <cmajor/cmdebug/DebugExprVisitor.hpp>
#include <cmajor/cmdebug/BoundDebugExpr.hpp>
#include <memory>
#include <string>

namespace cmajor { namespace debug {

class Debugger;
class DebugInfo;
class Scope;
class BoundDebugNode;
class BoundDebugExpression;
class DebugExprNode;
class DIType;
class DIClassTemplateSpecializationType;

class DEBUG_API DebugExprBinder : public DebugExprVisitor
{
public:
    DebugExprBinder(Debugger& debugger_, DebugInfo* debugInfo_, Scope* scope_);
    void Visit(IdentifierDebugExprNode& node) override;
    void Visit(IntegerDebugExprNode& node) override;
    void Visit(AddDebugExprNode& node) override;
    void Visit(SubDebugExprNode& node) override;
    void Visit(ParenthesizedDebugExprNode& node) override;
    void Visit(DerefDebugExprNode& node) override;
    void Visit(AddrOfDebugExprNode& node) override;
    void Visit(DotDebugExprNode& node) override;
    void Visit(SubscriptDebugExprNode& node) override;
    void Visit(RangeDebugExprNode& node) override;
    void Visit(TypeIdDebugExprNode& node) override;
    void Visit(CastDebugExprNode& node) override;
    BoundDebugExpression* BoundExpression(DebugExprNode* sourceNode);
    DIType* GetValueType(DIClassTemplateSpecializationType* specializationType) const;
private:
    Debugger& debugger;
    DebugInfo* debugInfo;
    Scope* scope;
    std::unique_ptr<BoundDebugNode> currentNode;
    std::unique_ptr<BoundDebugExpression> expression;
    bool hasContainerSubscript;
    InitializationStatus status;
};

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_DEBUG_EXPR_BINDER_INCLUDED
