// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_DEBUG_EXPR_TRANSLATOR_VISITOR_INCLUDED
#define CMAJOR_DEBUG_DEBUG_EXPR_TRANSLATOR_VISITOR_INCLUDED
#include <cmajor/cmdebug/DebugExprVisitor.hpp>
#include <memory>
#include <string>

namespace cmajor { namespace debug {

class DebugInfo;
class Scope;
class DINode;

class DEBUG_API DebugExprTranslatorVisitor : public DebugExprVisitor
{
public:
    DebugExprTranslatorVisitor(DebugInfo* debugInfo_, Scope* scope_);
    void Visit(VariableNameDebugExprNode& node) override;
    void Visit(IntegerDebugExprNode& node) override;
    void Visit(ParenthesizedDebugExprNode& node) override;
    void Visit(DerefDebugExprNode& node) override;
    void Visit(AddrOfDebugExprNode& node) override;
    void Visit(UnaryMinusDebugExprNode& node) override;
    void Visit(UnaryPlusDebugExprNode& node) override;
    void Visit(DotDebugExprNode& node) override;
    void Visit(SubscriptDebugExprNode& node) override;
    void Visit(TypeIdDebugExprNode& node) override;
    void Visit(CastDebugExprNode& node) override;
    void Visit(BaseDebugExprNode& node) override;
    DINode* TranslatedNode() const { return currentNode.get(); }
private:
    DebugInfo* debugInfo;
    Scope* scope;
    std::unique_ptr<DINode> currentNode;
};

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_DEBUG_EXPR_TRANSLATOR_VISITOR_INCLUDED
