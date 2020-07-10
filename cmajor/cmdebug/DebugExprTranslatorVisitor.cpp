// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/DebugExprTranslatorVisitor.hpp>
#include <cmajor/cmdebug/DebugExpr.hpp>
#include <cmajor/cmdebug/DebugInfo.hpp>
#include <cmajor/cmdebug/DIExpr.hpp>

namespace cmajor { namespace debug {

DebugExprTranslatorVisitor::DebugExprTranslatorVisitor(DebugInfo* debugInfo_, Scope* scope_) : debugInfo(debugInfo_), scope(scope_)
{
}

void DebugExprTranslatorVisitor::Visit(VariableNameDebugExprNode& node)
{
    DIVariable* variable = scope->GetVariable(node.Identifier());
    if (variable)
    {
        currentNode.reset(new DIVariableReferenceNode(variable));
    }
    else
    {
        throw std::runtime_error("variable '" + node.Identifier() + "' not found from scope '" + scope->Name() + "'");
    }
}

void DebugExprTranslatorVisitor::Visit(IntegerDebugExprNode& node)
{
    currentNode.reset(new DIIntegerLiteralNode(debugInfo->GetMainProject(), node.Value()));
}

void DebugExprTranslatorVisitor::Visit(ParenthesizedDebugExprNode& node)
{
    node.ExprNode()->Accept(*this);
    currentNode.reset(new DIParenExprNode(currentNode.release()));
}

void DebugExprTranslatorVisitor::Visit(DerefDebugExprNode& node)
{
    node.Operand()->Accept(*this);
    std::unique_ptr<DINode> operandNode = std::move(currentNode);
    DIType* operandType = nullptr;
    switch (operandNode->Type()->GetKind())
    {
        case DIType::Kind::pointerType:
        {
            DIPointerType* pointerType = static_cast<DIPointerType*>(operandNode->Type());
            operandType = pointerType->PointedToType();
            break;
        }
        default:
        {
            throw std::runtime_error("cannot dereference an object of a non-pointer type (" + operandNode->Type()->Name() + ")");
        }
    }
    currentNode.reset(new DIDerefNode(operandType, operandNode.release()));
}

void DebugExprTranslatorVisitor::Visit(AddrOfDebugExprNode& node)
{
    node.Operand()->Accept(*this);
    std::unique_ptr<DINode> operandNode = std::move(currentNode);
    DIType* type = MakePointerType(operandNode->Type());
    currentNode.reset(new DIAddrOfNode(type, operandNode.release()));
}

void DebugExprTranslatorVisitor::Visit(UnaryMinusDebugExprNode& node)
{
    node.Operand()->Accept(*this);
    currentNode.reset(new DIUnaryMinusNode(currentNode.release()));
}

void DebugExprTranslatorVisitor::Visit(UnaryPlusDebugExprNode& node)
{
    node.Operand()->Accept(*this);
    currentNode.reset(new DIUnaryPlusNode(currentNode.release()));
}

void DebugExprTranslatorVisitor::Visit(DotDebugExprNode& node)
{
    node.Subject()->Accept(*this);
    DIType* type = currentNode->Type();
    currentNode.reset(new DIDotNode(type, currentNode.release(), node.Member(), true));
}

void DebugExprTranslatorVisitor::Visit(SubscriptDebugExprNode& node)
{
    node.Subject()->Accept(*this);
    std::unique_ptr<DINode> subject(currentNode.release());
    node.Index()->Accept(*this);
    std::unique_ptr<DINode> index(currentNode.release());
    currentNode.reset(new DISubscriptNode(subject.release(), index.release()));
}

void DebugExprTranslatorVisitor::Visit(TypeIdDebugExprNode& node)
{
    currentNode.reset(new DITypeIdNode(debugInfo, node.TypeId()));
}

void DebugExprTranslatorVisitor::Visit(CastDebugExprNode& node)
{
    node.TypeIdNode()->Accept(*this);
    std::unique_ptr<DINode> typeIdNode = std::move(currentNode);
    node.ExprNode()->Accept(*this);
    currentNode.reset(new DICastNode(typeIdNode.release(), currentNode.release()));
}

void DebugExprTranslatorVisitor::Visit(BaseDebugExprNode& node)
{
    DIType* type = currentNode->Type();
    currentNode.reset(new DIDotNode(type, currentNode.release(), "base", true));
}

} } // namespace cmajor::debug
