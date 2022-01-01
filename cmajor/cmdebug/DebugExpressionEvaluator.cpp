// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/DebugExpressionEvaluator.hpp>
#include <cmajor/cmdebug/Debugger.hpp>
#include <cmajor/cmdebug/DebugExpr.hpp>
#include <cmajor/cmdebug/DIType.hpp>
#include <cmajor/cmdebug/Gdb.hpp>
#include <cmajor/cmdebug/EvaluationGuard.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/lexical_cast.hpp>

namespace cmajor { namespace debug {

using namespace soulng::unicode;

DebugExpressionEvaluator::DebugExpressionEvaluator(Debugger& debugger_, DebugInfo* debugInfo_) :
    debugger(debugger_), debugInfo(debugInfo_), integer(-1), rangeStart(-1), rangeEnd(-1), status(InitializationStatus::unknown)
{
}

void DebugExpressionEvaluator::Visit(BoundDebugExpression& expr)
{
    bool addTypes = false;
    BoundDebugNode* node = expr.Node();
    status = expr.Status();
    switch (node->GetKind())
    {
        case BoundDebugNode::Kind::typeNode:
        {
            DIType* type = node->Type();
            JsonObject* object = new JsonObject();
            object->AddField(U"type", type->ToJson());
            object->AddField(U"success", std::unique_ptr<JsonValue>(new JsonBool(true)));
            result.reset(object);
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
            EvaluatePointerRange(&node, subject, rangeStart, rangeEnd);
            break;
        }
        case DIType::Kind::arrayType:
        {
            EvaluateArrayRange(&node, subject, rangeStart, rangeEnd);
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

void DebugExpressionEvaluator::EvaluatePointerRange(BoundDebugNode* node, BoundDebugNode* subject, int64_t rangeStart, int64_t rangeEnd)
{
    if (rangeEnd == -1)
    {
        throw std::runtime_error("must specify range end explicitly for pointer type range expression: " + node->ToString());
    }
    std::unique_ptr<JsonObject> rangeResult(new JsonObject());
    std::unique_ptr<JsonArray> rangeArray(new JsonArray());
    for (int64_t index = rangeStart; index < rangeEnd; ++index)
    {
        IntegerDebugExprNode integerLiteralNode(index);
        BoundIntegerLiteralNode boundIntegerLiteralNode(debugInfo->GetMainProject()->GetLongType(), index, &integerLiteralNode);
        SubscriptDebugExprNode subscriptNode(subject->SourceNode()->Clone(), integerLiteralNode.Clone());
        BoundSubscriptNode boundSubscriptNode(node->Type(), subject->Clone(), boundIntegerLiteralNode.Clone(), &subscriptNode);
        boundSubscriptNode.Accept(*this);
        std::unique_ptr<JsonValue> subscriptResult(result.release());
        rangeArray->AddItem(std::unique_ptr<JsonValue>(subscriptResult.release()));
    }
    rangeResult->AddField(U"range", std::unique_ptr<JsonValue>(rangeArray.release()));
    result.reset(rangeResult.release());
}

void DebugExpressionEvaluator::EvaluateArrayRange(BoundDebugNode* node, BoundDebugNode* subject, int64_t rangeStart, int64_t rangeEnd)
{
    DIType* subjectType = subject->Type();
    if (rangeEnd == -1)
    {
        if (subjectType->GetKind() == DIType::Kind::arrayType)
        {
            DIArrayType* arrayType = static_cast<DIArrayType*>(subjectType);
            if (arrayType->Size() != -1)
            {
                rangeEnd = arrayType->Size();
            }
        }
    }
    if (rangeEnd == -1)
    {
        throw std::runtime_error("must specify range end explicitly for array type without size range expression: " + node->ToString());
    }
    std::unique_ptr<JsonObject> rangeResult(new JsonObject());
    std::unique_ptr<JsonArray> rangeArray(new JsonArray());
    for (int64_t index = rangeStart; index < rangeEnd; ++index)
    {
        IntegerDebugExprNode integerLiteralNode(index);
        BoundIntegerLiteralNode boundIntegerLiteralNode(debugInfo->GetMainProject()->GetLongType(), index, &integerLiteralNode);
        SubscriptDebugExprNode subscriptNode(subject->SourceNode()->Clone(), integerLiteralNode.Clone());
        BoundSubscriptNode boundSubscriptNode(node->Type(), subject->Clone(), boundIntegerLiteralNode.Clone(), &subscriptNode);
        boundSubscriptNode.Accept(*this);
        std::unique_ptr<JsonValue> subscriptResult(result.release());
        rangeArray->AddItem(std::unique_ptr<JsonValue>(subscriptResult.release()));
    }
    rangeResult->AddField(U"range", std::unique_ptr<JsonValue>(rangeArray.release()));
    result.reset(rangeResult.release());
}

void DebugExpressionEvaluator::Evaluate(BoundDebugNode* node)
{
    DebuggerVariable variable = debugger.GetNextDebuggerVariable();
    std::string gdbExprString = node->GdbExprString();
    GdbVarCreateCommand command(variable.GdbVarName(), "*", gdbExprString);
    EvaluationGuard evaluationGuard(debugger);
    bool succeeded = debugger.ExecuteGDBCommand(command);
    result.reset(debugger.ReleaseResult());
    bool uninitialized = false;
    if (succeeded)
    {
        if (result->Type() == JsonValueType::object)
        {
            JsonObject* jsonObject = static_cast<JsonObject*>(result.get());
            jsonObject->AddField(U"status", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(InitializationStatusStr(status)))));
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
        std::unique_ptr<JsonValue> origResult(result.release());
        GdbVarDeleteCommand varDeleteComand(variable.GdbVarName(), false);
        bool succeeded = debugger.ExecuteGDBCommand(varDeleteComand);
        result.reset(origResult.release());
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
