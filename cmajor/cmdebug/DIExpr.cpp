// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/DIExpr.hpp>
#include <cmajor/cmdebug/DIVariable.hpp>
#include <cmajor/cmdebug/DebugInfo.hpp>

namespace cmajor { namespace debug {

DINode::DINode(DIType* type_) : type(type_)
{
}

DINode::~DINode()
{
}

DIVariableReferenceNode::DIVariableReferenceNode(DIVariable* variable_) : DINode(variable_->GetType()), variable(variable_)
{
}

std::string DIVariableReferenceNode::ToString() const
{
    return variable->Name();
}

DINode* DIVariableReferenceNode::Clone() const
{
    return new DIVariableReferenceNode(variable);
}

std::string DIVariableReferenceNode::GdbExprString() const
{
    return variable->IrName();
}

DIIntegerLiteralNode::DIIntegerLiteralNode(Project* project_, int64_t value_) : DINode(project_->GetLongType()), project(project_), value(value_)
{
}

std::string DIIntegerLiteralNode::ToString() const
{
    return std::to_string(value);
}

DINode* DIIntegerLiteralNode::Clone() const
{
    return new DIIntegerLiteralNode(project, value);
}

std::string DIIntegerLiteralNode::GdbExprString() const
{
    return std::to_string(value);
}

DIDerefNode::DIDerefNode(DIType* type, DINode* subject_) : DINode(type), subject(subject_)
{
}

std::string DIDerefNode::ToString() const
{
    return "*" + subject->ToString();
}

DINode* DIDerefNode::Clone() const
{
    return new DIDerefNode(Type(), subject->Clone());
}

std::string DIDerefNode::GdbExprString() const
{
    return "*" + subject->GdbExprString();
}

DIAddrOfNode::DIAddrOfNode(DIType* type, DINode* subject_) : DINode(type), subject(subject_)
{
}

std::string DIAddrOfNode::ToString() const
{
    return "&" + subject->ToString();
}

DINode* DIAddrOfNode::Clone() const
{
    return new DIAddrOfNode(Type(), subject->Clone());
}

std::string DIAddrOfNode::GdbExprString() const
{
    return "&" + subject->GdbExprString();
}

DIUnaryMinusNode::DIUnaryMinusNode(DINode* subject_) : DINode(subject_->Type()), subject(subject_)
{
}

std::string DIUnaryMinusNode::ToString() const
{
    return "-" + subject->ToString();
}

DINode* DIUnaryMinusNode::Clone() const
{
    return new DIUnaryMinusNode(subject->Clone());
}

std::string DIUnaryMinusNode::GdbExprString() const
{
    return "-" + subject->GdbExprString();
}

DIUnaryPlusNode::DIUnaryPlusNode(DINode* subject_) : DINode(subject_->Type()), subject(subject_)
{
}

std::string DIUnaryPlusNode::ToString() const
{
    return "+" + subject->ToString();
}

DINode* DIUnaryPlusNode::Clone() const
{
    return new DIUnaryPlusNode(subject->Clone());
}

std::string DIUnaryPlusNode::GdbExprString() const
{
    return "+" + subject->GdbExprString();
}

DIDotNode::DIDotNode(DIType* type_, DINode* subject_, const std::string& member_, bool translateMember_) :
    DINode(type_), type(type_), subject(subject_), member(member_), translateMember(translateMember_), gdbExprString()
{
    if (!translateMember)
    {
        gdbExprString = subject->GdbExprString() + "." + member;
    }
    else
    {
        Scope* scope = type->GetScope();
        if (scope)
        {
            if (member == "base")
            {
                if (type->GetKind() == DIType::Kind::classType || type->GetKind() == DIType::Kind::specializationType)
                {
                    DIClassType* classType = static_cast<DIClassType*>(type);
                    if (!classType->BaseClassId().is_nil())
                    {
                        type = classType->BaseClassType();
                        SetType(type);
                        gdbExprString = subject->GdbExprString() + ".m0";
                    }
                    else
                    {
                        throw std::runtime_error("class '" + classType->Name() + "' has no base class");
                    }
                }
            }
            else
            {
                DIVariable* memberVariable = scope->GetVariable(member);
                if (memberVariable)
                {
                    SetType(memberVariable->GetType());
                    gdbExprString = subject->GdbExprString() + "." + memberVariable->IrName();
                }
                else
                {
                    throw std::runtime_error("member variable '" + member + "' not found from scope '" + scope->Name());
                }
            }
        }
        else
        {
            throw std::runtime_error("expression '" + ToString() + "' has no class scope");
        }
    }
}

std::string DIDotNode::ToString() const
{
    return subject->ToString() + "." + member;
}

DINode* DIDotNode::Clone() const
{
    return new DIDotNode(type, subject->Clone(), member, translateMember);
}

DISubscriptNode::DISubscriptNode(DINode* subject_, DINode* index_) : DINode(subject_->Type()), subject(subject_), index(index_)
{
}

std::string DISubscriptNode::ToString() const
{
    return subject->ToString() + "[" + index->ToString() + "]";
}

DINode* DISubscriptNode::Clone() const
{
    return new DISubscriptNode(subject->Clone(), index->Clone());
}

std::string DISubscriptNode::GdbExprString() const
{
    return subject->GdbExprString() + "[" + index->GdbExprString() + "]";
}

DIParenExprNode::DIParenExprNode(DINode* subject_) : DINode(subject_->Type()), subject(subject_)
{
}

std::string DIParenExprNode::ToString() const
{
    return "(" + subject->ToString() + ")";
}

DINode* DIParenExprNode::Clone() const
{
    return new DIParenExprNode(subject->Clone());
}

std::string DIParenExprNode::GdbExprString() const
{
    return "(" + subject->GdbExprString() + ")";
}

DITypeIdNode::DITypeIdNode(DebugInfo* debugInfo_, const std::string& typeId_) : DINode(debugInfo_->GetType(typeId_)), debugInfo(debugInfo_), typeId(typeId_)
{
    if (!Type())
    {
        throw std::runtime_error("type '" + typeId + "' not found");
    }
}

std::string DITypeIdNode::ToString() const
{
    return "typeid(\"" + typeId + "\")";
}

DINode* DITypeIdNode::Clone() const
{
    return new DITypeIdNode(debugInfo, typeId);
}

std::string DITypeIdNode::GdbExprString() const
{
    return std::string();
}

DICastNode::DICastNode(DINode* typeIdNode_, DINode* exprNode_) : DINode(typeIdNode_->Type()), typeIdNode(typeIdNode_), exprNode(exprNode_)
{
}

std::string DICastNode::ToString() const
{
    return "cast<" + typeIdNode->ToString() + ">(" + exprNode->ToString() + ")";
}

DINode* DICastNode::Clone() const
{
    return new DICastNode(typeIdNode->Clone(), exprNode->Clone());
}

std::string DICastNode::GdbExprString() const
{
    DIType* type = Type();
    if (type->GetKind() == DIType::Kind::pointerType)
    {
        DIPointerType* pointerType = static_cast<DIPointerType*>(type);
        type = pointerType->PointedToType();
        if (type->GetKind() == DIType::Kind::classType || type->GetKind() == DIType::Kind::specializationType)
        {
            DIClassType* classType = static_cast<DIClassType*>(type);
            if (classType->IsPolymorphic())
            {
                return "(" + classType->IrName() + "*)" + exprNode->GdbExprString();
            }
            else
            {
                throw std::runtime_error("class type '" + classType->Name() + "' is not polymorphic");
            }
        }
        else
        {
            throw std::runtime_error("type '" + pointerType->Name() + " is not a pointer-to-polymorphic-class-type");
        }
    }
    else
    {
        throw std::runtime_error("type '" + type->Name() + " is not a pointer-to-polymorphic-class-type");
    }
}

} } // namespace cmajor::debug
