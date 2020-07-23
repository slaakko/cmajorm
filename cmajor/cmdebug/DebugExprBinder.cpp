// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/DebugExprBinder.hpp>
#include <cmajor/cmdebug/DebugExpr.hpp>
#include <cmajor/cmdebug/DebugInfo.hpp>
#include <cmajor/cmdebug/BoundDebugExpr.hpp>
#include <cmajor/cmdebug/Debugger.hpp>

namespace cmajor { namespace debug {

DebugExprBinder::DebugExprBinder(Debugger& debugger_, DebugInfo* debugInfo_, Scope* scope_) : debugger(debugger_), debugInfo(debugInfo_), scope(scope_)
{
}

void DebugExprBinder::Visit(AddressDebugExprNode& node)
{
    currentNode.reset(new BoundAddressNode(debugInfo->GetMainProject()->GetLongType(), node.Value(), &node));
}

void DebugExprBinder::Visit(IdentifierDebugExprNode& node)
{
    DIVariable* variable = scope->GetVariable(node.Identifier());
    if (variable)
    {
        currentNode.reset(new BoundVariableReferenceNode(variable->GetType(), variable, &node));
    }
    else
    {
        throw std::runtime_error("variable '" + node.Identifier() + "' not found from scope '" + scope->Name() + "'");
    }
}

void DebugExprBinder::Visit(IntegerDebugExprNode& node)
{
    currentNode.reset(new BoundIntegerLiteralNode(debugInfo->GetMainProject()->GetLongType(), node.Value(), &node));
}

void DebugExprBinder::Visit(AddDebugExprNode& node)
{
    node.Left()->Accept(*this);
    std::unique_ptr<BoundDebugNode> left(std::move(currentNode));
    node.Right()->Accept(*this);
    std::unique_ptr<BoundDebugNode> right(std::move(currentNode));
    bool valid = false;
    if (left->Type()->GetKind() == DIType::Kind::primitiveType)
    {
        DIPrimitiveType* primitiveType = static_cast<DIPrimitiveType*>(left->Type());
        if (primitiveType->IsIntegerType())
        {
            if (right->Type()->GetKind() == DIType::Kind::pointerType)
            {
                currentNode.reset(new BoundAddNode(right->Type(), left.release(), right.release(), &node));
                valid = true;
            }
        }
    }
    else if (left->Type()->GetKind() == DIType::Kind::pointerType)
    {
        if (right->Type()->GetKind() == DIType::Kind::primitiveType)
        {
            DIPrimitiveType* primitiveType = static_cast<DIPrimitiveType*>(right->Type());
            if (primitiveType->IsIntegerType())
            {
                currentNode.reset(new BoundAddNode(left->Type(), left.release(), right.release(), &node));
                valid = true;
            }
        }
    }
    if (!valid)
    {
        throw std::runtime_error("add expression not valid: (pointer + integer) | (integer + pointer) expected: " + node.ToString());
    }
}

void DebugExprBinder::Visit(SubDebugExprNode& node)
{
    node.Left()->Accept(*this);
    std::unique_ptr<BoundDebugNode> left(std::move(currentNode));
    node.Right()->Accept(*this);
    std::unique_ptr<BoundDebugNode> right(std::move(currentNode));
    bool valid = false;
    if (left->Type()->GetKind() == DIType::Kind::pointerType)
    {
        DIPointerType* leftPtrType = static_cast<DIPointerType*>(left->Type());
        if (right->Type()->GetKind() == DIType::Kind::pointerType)
        {
            DIPointerType* rightPtrType = static_cast<DIPointerType*>(right->Type());
            if (leftPtrType->Id() == rightPtrType->Id())
            {
                currentNode.reset(new BoundSubNode(debugInfo->GetMainProject()->GetLongType(), left.release(), right.release(), &node));
                valid = true;
            }
        }
        else if (right->Type()->GetKind() == DIType::Kind::primitiveType)
        {
            DIPrimitiveType* primitiveType = static_cast<DIPrimitiveType*>(right->Type());
            if (primitiveType->IsIntegerType())
            {
                currentNode.reset(new BoundSubNode(left->Type(), left.release(), right.release(), &node));
                valid = true;
            }
        }
    }
    if (!valid)
    {
        throw std::runtime_error("subtract expression not valid: (pointer - integer) | (pointer - pointer) expected: " + node.ToString());
    }
}

void DebugExprBinder::Visit(ParenthesizedDebugExprNode& node)
{
    node.ExprNode()->Accept(*this);
    currentNode.reset(new BoundParenExprNode(currentNode.release(), &node));
}

void DebugExprBinder::Visit(DerefDebugExprNode& node)
{
    node.Operand()->Accept(*this);
    std::unique_ptr<BoundDebugNode> operandNode = std::move(currentNode);
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
    currentNode.reset(new BoundDerefNode(operandType, operandNode.release(), &node));
}

void DebugExprBinder::Visit(AddrOfDebugExprNode& node)
{
    node.Operand()->Accept(*this);
    std::unique_ptr<BoundDebugNode> operandNode = std::move(currentNode);
    DIType* type = MakePointerType(operandNode->Type());
    currentNode.reset(new BoundAddrOfNode(type, operandNode.release(), &node));
}

void DebugExprBinder::Visit(DotDebugExprNode& node)
{
    node.Subject()->Accept(*this);
    DIType* type = currentNode->Type();
    Scope* scope = type->GetScope();
    if (scope)
    {
        if (node.Member() == "base")
        {
            if (type->GetKind() == DIType::Kind::classType || type->GetKind() == DIType::Kind::specializationType)
            {
                DIClassType* classType = static_cast<DIClassType*>(type);
                if (!classType->BaseClassId().is_nil())
                {
                    DIType* dotMemberType = classType->BaseClassType();
                    std::string gdbExprString = currentNode->GdbExprString() + ".m0";
                    BoundDotNode* boundDotNode = new BoundDotNode(dotMemberType, currentNode.release(), node.Member(), gdbExprString, &node);
                    currentNode.reset(boundDotNode);
                }
                else
                {
                    throw std::runtime_error("class '" + classType->Name() + "' has no base class");
                }
            }
            else
            {
                throw std::runtime_error("expression '" + currentNode->ToString() + "' has no class scope");
            }
        }
        else
        {
            DIVariable* memberVariable = scope->GetVariable(node.Member());
            if (memberVariable)
            {
                DIType* dotMemberType = memberVariable->GetType();
                std::string gdbExprString = currentNode->GdbExprString() + "." + memberVariable->IrName();
                BoundDotNode* boundDotNode = new BoundDotNode(dotMemberType, currentNode.release(), node.Member(), gdbExprString, &node);
                currentNode.reset(boundDotNode);
            }
            else
            {
                throw std::runtime_error("member variable '" + node.Member() + "' not found from scope '" + scope->Name() + "'");
            }
        }
    }
    else
    {
        throw std::runtime_error("expression '" + currentNode->ToString() + "' has no class scope");
    }
}

void DebugExprBinder::Visit(SubscriptDebugExprNode& node)
{
    DIType* type = nullptr;
    node.Subject()->Accept(*this);
    std::unique_ptr<BoundDebugNode> subject(currentNode.release());
    DIType* subjectType = subject->Type();
    node.Index()->Accept(*this);
    std::unique_ptr<BoundDebugNode> index(currentNode.release());
    bool indexTypeValid = false;
    if (index->Type()->GetKind() == DIType::Kind::primitiveType)
    {
        DIPrimitiveType* primitiveType = static_cast<DIPrimitiveType*>(index->Type());
        if (primitiveType->GetPrimitiveTypeKind() == DIPrimitiveType::DIPrimitiveType::Kind::longType)
        {
            indexTypeValid = true;
        }
    }
    if (!indexTypeValid)
    {
        throw std::runtime_error("index type of subscript expression expected to be 'long'");
    }
    switch (subjectType->GetKind())
    {
        case DIType::Kind::pointerType:
        {
            DIPointerType* pointerType = static_cast<DIPointerType*>(subjectType);
            type = pointerType->PointedToType();
            break;
        }
        case DIType::Kind::arrayType:
        {
            DIArrayType* arrayType = static_cast<DIArrayType*>(subjectType);
            type = arrayType->ElementType();
            break;
        }
        case DIType::Kind::specializationType:
        {
            DIClassTemplateSpecializationType* specializationType = static_cast<DIClassTemplateSpecializationType*>(subjectType);
            type = GetValueType(specializationType);
            break;
        }
        default:
        {
            throw std::runtime_error("the subject type of a subscript expression must be a pointer, array or container type");
        }
    }
    currentNode.reset(new BoundSubscriptNode(type, subject.release(), index.release(), &node));
}

void DebugExprBinder::Visit(RangeDebugExprNode& node)
{
    DIType* type = nullptr;
    node.Subject()->Accept(*this);
    std::unique_ptr<BoundDebugNode> subject(currentNode.release());
    DIType* subjectType = subject->Type();
    node.RangeStart()->Accept(*this);
    std::unique_ptr<BoundDebugNode> rangeStart(currentNode.release());
    bool startTypeValid = false;
    if (rangeStart->Type()->GetKind() == DIType::Kind::primitiveType)
    {
        DIPrimitiveType* primitiveType = static_cast<DIPrimitiveType*>(rangeStart->Type());
        if (primitiveType->GetPrimitiveTypeKind() == DIPrimitiveType::DIPrimitiveType::Kind::longType)
        {
            startTypeValid = true;
        }
    }
    if (!startTypeValid)
    {
        throw std::runtime_error("start index type of subscript expression expected to be 'long'");
    }
    bool rangeEndSet = false;
    std::unique_ptr<BoundDebugNode> rangeEnd;
    if (node.RangeEnd()->GetKind() == DebugExprNode::Kind::identifier)
    {
        IdentifierDebugExprNode* idNode = static_cast<IdentifierDebugExprNode*>(node.RangeEnd());
        if (idNode->Identifier() == "count")
        {
            rangeEnd.reset(new BoundIntegerLiteralNode(debugInfo->GetMainProject()->GetLongType(), -1, &node));
            rangeEndSet = true;
        }
    }
    if (!rangeEndSet)
    {
        node.RangeEnd()->Accept(*this);
        rangeEnd.reset(currentNode.release());
    }
    bool endTypeValid = false;
    if (rangeStart->Type()->GetKind() == DIType::Kind::primitiveType)
    {
        DIPrimitiveType* primitiveType = static_cast<DIPrimitiveType*>(rangeEnd->Type());
        if (primitiveType->GetPrimitiveTypeKind() == DIPrimitiveType::DIPrimitiveType::Kind::longType)
        {
            endTypeValid = true;
        }
    }
    if (!endTypeValid)
    {
        throw std::runtime_error("end index type of subscript expression expected to be 'long'");
    }
    switch (subjectType->GetKind())
    {
        case DIType::Kind::pointerType:
        {
            DIPointerType* pointerType = static_cast<DIPointerType*>(subjectType);
            type = pointerType->PointedToType();
            break;
        }
        case DIType::Kind::arrayType:
        {
            DIArrayType* arrayType = static_cast<DIArrayType*>(subjectType);
            type = arrayType->ElementType();
            break;
        }
        case DIType::Kind::specializationType:
        {
            DIClassTemplateSpecializationType* specializationType = static_cast<DIClassTemplateSpecializationType*>(subjectType);
            type = GetValueType(specializationType);
            break;
        }
        default:
        {
            throw std::runtime_error("the subject type of a subscript expression must be a pointer, array or container type");
        }
    }
    currentNode.reset(new BoundRangeNode(type, subject.release(), rangeStart.release(), rangeEnd.release(), &node));
}

void DebugExprBinder::Visit(TypeIdDebugExprNode& node)
{
    DIType* type = debugInfo->GetType(node.TypeId());
    if (!type)
    {
        throw std::runtime_error("type with id '" + node.TypeId() + "' not found");
    }
    currentNode.reset(new BoundTypeNode(type, &node));
}

void DebugExprBinder::Visit(CastDebugExprNode& node)
{
    node.TypeIdNode()->Accept(*this);
    std::unique_ptr<BoundDebugNode> typeNode = std::move(currentNode);
    if (!typeNode->IsTypeNode())
    {
        throw std::runtime_error("typeid expresssion expected");
    }
    node.ExprNode()->Accept(*this);
    std::unique_ptr<BoundDebugNode> exprNode = std::move(currentNode);
    std::string gdbExprString;
    DIType* type = typeNode->Type();
    if (exprNode->GetKind() == BoundDebugNode::Kind::addressNode)
    {
        gdbExprString = "{" + type->IrName() + "}" + exprNode->GdbExprString();
    }
    else
    {
        gdbExprString = "(" + type->IrName() + ")" + exprNode->GdbExprString();
    }
    currentNode.reset(new BoundCastNode(typeNode.release(), exprNode.release(), gdbExprString, &node));
}

void DebugExprBinder::Visit(DebuggerVarExprNode& node)
{
    const DebuggerVariable* variable = debugger.GetDebuggerVariable(node.VariableIndex());
    if (variable)
    {
        currentNode.reset(new BoundDebuggerVarNode(debugInfo->GetMainProject()->GetLongType(), variable, &node));
    }
    else
    {
        throw std::runtime_error("debugger variable index " + std::to_string(node.VariableIndex()) + " not found");
    }
}

BoundDebugExpression* DebugExprBinder::BoundExpression(DebugExprNode* sourceNode)
{
    expression.reset(new BoundDebugExpression(currentNode.release(), sourceNode));
    return expression.get();
}

DIType* DebugExprBinder::GetValueType(DIClassTemplateSpecializationType* specializationType) const
{
    if (specializationType->GetContainerClassTemplateKind() != ContainerClassTemplateKind::notContainerClassTemplate)
    {
        boost::uuids::uuid valueTypeId = specializationType->ValueTypeId();
        DIType* valueType = specializationType->GetProject()->GetType(valueTypeId);
        if (valueType)
        {
            return valueType;
        }
        else
        {
            throw std::runtime_error("value type for container class template specialization '" + specializationType->Name() + "' not found");
        }
    }
    else
    {
        throw std::runtime_error("class template specialization '" + specializationType->Name() + "' is not a container class template specialization");
    }
}

} } // namespace cmajor::debug
