// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/DebugExpressionEvaluator.hpp>
#include <cmajor/cmdebug/Debugger.hpp>
#include <cmajor/cmdebug/BoundDebugExpr.hpp>
#include <cmajor/cmdebug/DebugExpr.hpp>
#include <cmajor/cmdebug/DIType.hpp>
#include <cmajor/cmdebug/Gdb.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/lexical_cast.hpp>

namespace cmajor { namespace debug {

using namespace soulng::unicode;

DebugExpressionEvaluator::DebugExpressionEvaluator(Debugger& debugger_, const std::string& expression_) : debugger(debugger_), expression(expression_),
    integer(-1), rangeStart(-1), rangeEnd(-1)
{
}

void DebugExpressionEvaluator::Visit(BoundDebugExpression& expr)
{
    bool addTypes = false;
    BoundDebugNode* node = expr.Node();
    switch (node->GetKind())
    {
        case BoundDebugNode::Kind::typeNode:
        {
            DIType* type = node->Type();
            JsonObject* object = new JsonObject();
            object->AddField(U"type", type->ToJson());
            result.reset(object);
            break;
        }
        case BoundDebugNode::Kind::debuggerVarNode:
        {
            GdbVarEvaluateExpressionCommand command(node->GdbExprString(), Format::default_);
            debugger.ExecuteGDBCommand(command);
            result.reset(debugger.ReleaseResult());
            AddTypes(node);
            ContainerClassTemplateKind containerKind = GetContainerKind(node->Type());
            if (containerKind != ContainerClassTemplateKind::notContainerClassTemplate)
            {
                Container* container = debugger.GetContainer(containerKind, node->SourceNode()->ToString());
                if (result && result->Type() == JsonValueType::object)
                {
                    JsonObject* resultObject = static_cast<JsonObject*>(result.get());
                    resultObject->AddField(U"container", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(ContainerName(container->GetKind())))));
                    int64_t count = container->Count(node->SourceNode()->ToString());
                    if (count != -1)
                    {
                        resultObject->AddField(U"count", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(std::to_string(count)))));
                    }
                }
            }
            break;
        }
        case BoundDebugNode::Kind::subscriptNode:
        {
            node->Accept(*this);
            break;
        }
        case BoundDebugNode::Kind::rangeNode:
        {
            node->Accept(*this);
            break;
        }
        default:
        {
            Evaluate(node);
            break;
        }
    }
}

void DebugExpressionEvaluator::Visit(BoundIntegerLiteralNode& node)
{
    integer = node.Value();
}

void DebugExpressionEvaluator::Visit(BoundSubscriptNode& node)
{
    integer = -1;
    node.Index()->Accept(*this);
    int64_t index = integer;
    if (index == -1)
    {
        throw std::runtime_error(node.ToString() + ": invalid subscript");
    }
    BoundDebugNode* subject = node.Subject();
    DIType* subjectType = subject->Type();
    switch (subjectType->GetKind())
    {
        case DIType::Kind::pointerType:
        case DIType::Kind::arrayType:
        {
            Evaluate(&node);
            break;
        }
        case DIType::Kind::specializationType:
        {
            ContainerClassTemplateKind containerKind = GetContainerKind(subjectType);
            if (containerKind != ContainerClassTemplateKind::notContainerClassTemplate)
            {
                Container* container = debugger.GetContainer(containerKind, subject->SourceNode()->ToString());
                result = container->Subscript(subject->SourceNode()->ToString(), index);
            }
            else
            {
                throw std::runtime_error(node.ToString() + ": unknown container type");
            }
            break;
        }
    }
}

void DebugExpressionEvaluator::Visit(BoundRangeNode& node)
{
    integer = -1;
    node.RangeStart()->Accept(*this);
    rangeStart = integer;
    integer = -1;
    node.RangeEnd()->Accept(*this);
    rangeEnd = integer;
    BoundDebugNode* subject = node.Subject();
    DIType* subjectType = subject->Type();
    switch (subjectType->GetKind())
    {
        case DIType::Kind::pointerType:
        {

            break;
        }
        case DIType::Kind::arrayType:
        {

            break;
        }
        case DIType::Kind::specializationType:
        {
            ContainerClassTemplateKind containerKind = GetContainerKind(subjectType);
            if (containerKind != ContainerClassTemplateKind::notContainerClassTemplate)
            {
                Container* container = debugger.GetContainer(containerKind, subject->SourceNode()->ToString());
                result = container->Range(subject->SourceNode()->ToString(), rangeStart, rangeEnd);
            }
            else
            {
                throw std::runtime_error(node.ToString() + ": unknown container type");
            }
            break;
        }
    }
}

void DebugExpressionEvaluator::Evaluate(BoundDebugNode* node)
{
    DebuggerVariable variable = debugger.GetNextDebuggerVariable();
    std::string gdbExprString = node->GdbExprString();
    GdbVarCreateCommand command(variable.GdbVarName(), "*", gdbExprString);
    bool succeeded = debugger.ExecuteGDBCommand(command);
    if (succeeded)
    {
        debugger.AddDebuggerVariable(variable);
    }
    result.reset(debugger.ReleaseResult());
    bool uninitialized = false;
    if (succeeded)
    {
        if (result->Type() == JsonValueType::object)
        {
            JsonObject* jsonObject = static_cast<JsonObject*>(result.get());
            JsonValue* variableValue = jsonObject->GetField(U"variable");
            if (variableValue && variableValue->Type() == JsonValueType::object)
            {
                JsonObject* variableObject = static_cast<JsonObject*>(variableValue);
                variableObject->AddField(U"varIndex", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(std::to_string(variable.Index())))));
                if (node->GetKind() == BoundDebugNode::Kind::variableReferenceNode)
                {
                    BoundVariableReferenceNode* variableReferenceNode = static_cast<BoundVariableReferenceNode*>(node);
                    DIVariable* variable = variableReferenceNode->Variable();
                    variableObject->AddField(U"kind", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(DIVariable::KindStr(variable->GetKind())))));
                    if (variable->GetKind() == DIVariable::Kind::localVariable && variable->GetInitLineNumber() != -1)
                    {
                        if (debugger.StoppedInstruction()->SourceLineNumber() > variable->GetInitLineNumber())
                        {
                            variableObject->AddField(U"status", std::unique_ptr<JsonValue>(new JsonString(U"initialized")));
                        }
                        else
                        {
                            variableObject->AddField(U"status", std::unique_ptr<JsonValue>(new JsonString(U"uninitialized")));
                            uninitialized = true;
                        }
                    }
                }
            }
        }
        AddTypes(node);
        ContainerClassTemplateKind containerKind = GetContainerKind(node->Type());
        if (containerKind != ContainerClassTemplateKind::notContainerClassTemplate)
        {
            Container* container = debugger.GetContainer(containerKind, node->SourceNode()->ToString());
            if (result && result->Type() == JsonValueType::object)
            {
                JsonObject* resultObject = static_cast<JsonObject*>(result.get());
                resultObject->AddField(U"container", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(ContainerName(container->GetKind())))));
                if (!uninitialized)
                {
                    int64_t count = container->Count(node->SourceNode()->ToString());
                    if (count != -1)
                    {
                        resultObject->AddField(U"count", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(std::to_string(count)))));
                    }
                }
            }
        }
    }
}

void DebugExpressionEvaluator::AddTypes(BoundDebugNode* node)
{
    if (result && result->Type() == JsonValueType::object)
    {
        JsonObject* resultObject = static_cast<JsonObject*>(result.get());
        DITypeRef staticTypeRef(node->Type());
        resultObject->AddField(U"static_type", staticTypeRef.ToJson());
        DIType* dynamicType = debugger.GetDynamicType(node->Type(), node);
        if (dynamicType)
        {
            DITypeRef dynamicTypeRef(dynamicType);
            resultObject->AddField(U"dynamic_type", dynamicTypeRef.ToJson());
        }
    }
}

ContainerClassTemplateKind DebugExpressionEvaluator::GetContainerKind(DIType* type)
{
    if (type->GetKind() == DIType::Kind::specializationType)
    {
        DIClassTemplateSpecializationType* specialization = static_cast<DIClassTemplateSpecializationType*>(type);
        return specialization->GetContainerClassTemplateKind();
    }
    return ContainerClassTemplateKind::notContainerClassTemplate;
}

} } // namespace cmajor::debug
