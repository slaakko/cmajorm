// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/ContainerSubscriptTranslator.hpp>
#include <cmajor/cmdebug/BoundDebugExpr.hpp>
#include <cmajor/cmdebug/DebugExpr.hpp>
#include <cmajor/cmdebug/DIType.hpp>
#include <cmajor/cmdebug/Container.hpp>
#include <cmajor/cmdebug/Debugger.hpp>

namespace cmajor { namespace debug {

ContainerSubscriptTranslator::ContainerSubscriptTranslator(Debugger& debugger_) : debugger(debugger_), evaluateIndex(false), index(-1)
{
}

void ContainerSubscriptTranslator::Visit(BoundDebugExpression& expr)
{
    expr.Node()->Accept(*this);
}

void ContainerSubscriptTranslator::Visit(BoundVariableReferenceNode& node)
{
    expression = node.SourceNode()->ToString();
}

void ContainerSubscriptTranslator::Visit(BoundIntegerLiteralNode& node)
{
    if (evaluateIndex)
    {
        index = node.Value();
    }
    else
    {
        expression = node.SourceNode()->ToString();
    }
}

void ContainerSubscriptTranslator::Visit(BoundAddNode& node)
{
    node.Left()->Accept(*this);
    std::string left = expression;
    node.Right()->Accept(*this);
    std::string right = expression;
    expression = left + " + " + right;
}

void ContainerSubscriptTranslator::Visit(BoundSubNode& node)
{
    node.Left()->Accept(*this);
    std::string left = expression;
    node.Right()->Accept(*this);
    std::string right = expression;
    expression = left + " - " + right;
}

void ContainerSubscriptTranslator::Visit(BoundDerefNode& node)
{
    node.Subject()->Accept(*this);
    std::string subject = expression;
    expression = "(*" + subject + ")";
}

void ContainerSubscriptTranslator::Visit(BoundAddrOfNode& node)
{
    node.Subject()->Accept(*this);
    std::string subject = expression;
    expression = "(&" + subject + ")";
}

void ContainerSubscriptTranslator::Visit(BoundDotNode& node)
{
    node.Subject()->Accept(*this);
    std::string subject = expression;
    expression = subject + "." + node.Member();
}

void ContainerSubscriptTranslator::Visit(BoundSubscriptNode& node)
{
    BoundDebugNode* subject = node.Subject();
    DIType* subjectType = subject->Type();
    bool translated = false;
    if (subjectType->GetKind() == DIType::Kind::specializationType)
    {
        ContainerClassTemplateKind containerKind = GetContainerKind(subjectType);
        if (containerKind != ContainerClassTemplateKind::notContainerClassTemplate)
        {
            index = -1;
            bool prevEvaluateIndex = evaluateIndex;
            evaluateIndex = true;
            node.Index()->Accept(*this);
            evaluateIndex = prevEvaluateIndex;
            if (index == -1)
            {
                throw std::runtime_error(node.ToString() + ": invalid subscript");
            }
            subject->Accept(*this);
            std::string containerExpr = expression;
            Container* container = debugger.GetContainer(containerKind, containerExpr);
            expression = container->SubscriptExpressionString(containerExpr, index);
            translated = true;
        }
    }
    if (!translated)
    {
        node.Index()->Accept(*this);
        std::string indexStr = expression;
        node.Subject()->Accept(*this);
        std::string subject = expression;
        expression = subject + "[" + indexStr + "]";
    }
}

void ContainerSubscriptTranslator::Visit(BoundRangeNode& node)
{
    node.Subject()->Accept(*this);
    std::string subject = expression;
    node.RangeStart()->Accept(*this);
    std::string rangeStart = expression;
    node.RangeEnd()->Accept(*this);
    std::string rangeEnd = expression;
    expression = subject + "[" + rangeStart + ", " + rangeEnd + "]";
}

void ContainerSubscriptTranslator::Visit(BoundParenExprNode& node)
{
    node.Subject()->Accept(*this);
    std::string subject = expression;
    expression = "(" + subject + ")";
}

void ContainerSubscriptTranslator::Visit(BoundTypeNode& node)
{
    expression = node.SourceNode()->ToString();
}

void ContainerSubscriptTranslator::Visit(BoundCastNode& node)
{
    node.TypeIdNode()->Accept(*this);
    std::string typeId = expression;
    node.ExprNode()->Accept(*this);
    std::string expr = expression;
    expression = "cast<" + typeId + ">(" + expr + ")";
}

ContainerClassTemplateKind ContainerSubscriptTranslator::GetContainerKind(DIType* type)
{
    if (type->GetKind() == DIType::Kind::specializationType)
    {
        DIClassTemplateSpecializationType* specialization = static_cast<DIClassTemplateSpecializationType*>(type);
        return specialization->GetContainerClassTemplateKind();
    }
    return ContainerClassTemplateKind::notContainerClassTemplate;
}

} } // namespace cmajor::debug
