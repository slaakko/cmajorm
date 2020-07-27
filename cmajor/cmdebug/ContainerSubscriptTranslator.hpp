// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_CONTAINER_SUBSCRIPT_TRANSLATOR_INCLUDED
#define CMAJOR_DEBUG_CONTAINER_SUBSCRIPT_TRANSLATOR_INCLUDED
#include <cmajor/cmdebug/BoundDebugNodeVisitor.hpp>
#include <cmajor/cmdebug/Container.hpp>

namespace cmajor { namespace debug {

class Debugger;
class DIType;

class ContainerSubscriptTranslator : public BoundDebugNodeVisitor
{
public:
    ContainerSubscriptTranslator(Debugger& debugger_);
    void Visit(BoundDebugExpression& expr) override;
    void Visit(BoundVariableReferenceNode& node) override;
    void Visit(BoundIntegerLiteralNode& node) override;
    void Visit(BoundAddNode& node) override;
    void Visit(BoundSubNode& node) override;
    void Visit(BoundDerefNode& node) override;
    void Visit(BoundAddrOfNode& node) override;
    void Visit(BoundDotNode& node) override;
    void Visit(BoundSubscriptNode& node) override;
    void Visit(BoundRangeNode& node) override;
    void Visit(BoundParenExprNode& node) override;
    void Visit(BoundTypeNode& node) override;
    void Visit(BoundCastNode& node) override;
    ContainerClassTemplateKind GetContainerKind(DIType* type);
    const std::string& Expression() const { return expression; }
private:
    Debugger& debugger;
    std::string expression;
    bool evaluateIndex;
    int64_t index;
};

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_CONTAINER_SUBSCRIPT_TRANSLATOR_INCLUDED
