// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/codedom/Expression.hpp>
#include <cmajor/codedom/Visitor.hpp>
#include <cmajor/codedom/Type.hpp>
#include <cmajor/util/Unicode.hpp>

namespace cmajor { namespace codedom {

using namespace cmajor::util;
using namespace cmajor::unicode;

void PrintExprs(const std::vector<CppObject*>& expressions, CodeFormatter& formatter)
{
    bool first = true;
    for (CppObject* expr : expressions)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            formatter.Write(", ");
        }
        expr->Print(formatter);
    }
}

Expression::Expression(const std::u32string& name_): CppObject(name_) 
{
}

ThisAccess::ThisAccess(): Expression(U"this") 
{
}

void ThisAccess::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

IdExpr::IdExpr(const std::u32string& value_): Expression(value_) 
{
}

void IdExpr::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

UnaryExpression::UnaryExpression(const std::u32string& name_, CppObject* child_): Expression(name_), child(child_)
{
    Own(child);
}

PostfixExpression::PostfixExpression(const std::u32string& name_, CppObject* child_): UnaryExpression(name_, child_)
{
}

IndexExpr::IndexExpr(CppObject* child_, CppObject* index_): PostfixExpression(U"[]", child_), index(index_) 
{
    Own(index);
}

void IndexExpr::Print(CodeFormatter& formatter)
{
    Child()->Print(formatter);
    formatter.Write("[");
    index->Print(formatter);
    formatter.Write("]");
}

void IndexExpr::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
    Child()->Accept(visitor);
}

InvokeExpr::InvokeExpr(CppObject* child_, const std::vector<CppObject*>& arguments_): PostfixExpression(U"()", child_), arguments(arguments_) 
{
    for (CppObject* arg : arguments)
    {
        Own(arg);
    }
}

void InvokeExpr::Print(CodeFormatter& formatter)
{
    Child()->Print(formatter);
    formatter.Write("(");
    PrintExprs(arguments, formatter);
    formatter.Write(")");
}

void InvokeExpr::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
    Child()->Accept(visitor);
    for (CppObject* arg : arguments)
    {
        arg->Accept(visitor);
    }
}

MemberAccessExpr::MemberAccessExpr(CppObject* child_, CppObject* member_): PostfixExpression(U".", child_), member(member_) 
{
    Own(member);
}

void MemberAccessExpr::Print(CodeFormatter& formatter)
{
    Child()->Print(formatter);
    formatter.Write(".");
    member->Print(formatter);
}

void MemberAccessExpr::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
    Child()->Accept(visitor);
}

PtrMemberAccessExpr::PtrMemberAccessExpr(CppObject* child_, CppObject* member_): PostfixExpression(U"->", child_), member(member_) 
{
    Own(member);
}

void PtrMemberAccessExpr::Print(CodeFormatter& formatter)
{
    Child()->Print(formatter);
    formatter.Write("->");
    member->Print(formatter);
}

void PtrMemberAccessExpr::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
    Child()->Accept(visitor);
}

PostIncrementExpr::PostIncrementExpr(CppObject* child_): PostfixExpression(U"++", child_) 
{
}

void PostIncrementExpr::Print(CodeFormatter& formatter)
{
    Child()->Print(formatter);
    formatter.Write("++");
}

void PostIncrementExpr::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
    Child()->Accept(visitor);
}

PostDecrementExpr::PostDecrementExpr(CppObject* child_): PostfixExpression(U"--", child_) 
{
}

void PostDecrementExpr::Print(CodeFormatter& formatter)
{
    Child()->Print(formatter);
    formatter.Write("--");
}

void PostDecrementExpr::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
    Child()->Accept(visitor);
}

PostCastExpr::PostCastExpr(const std::u32string& name_, CppObject* type_, CppObject* child_): PostfixExpression(name_, child_), type(type_) 
{
    Own(type);
}

void PostCastExpr::Print(CodeFormatter& formatter)
{
    formatter.Write(ToUtf8(Name()) + "<");
    type->Print(formatter);
    formatter.Write(">(");
    Child()->Print(formatter);
    formatter.Write(")");
}

void PostCastExpr::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
    Child()->Accept(visitor);
    type->Accept(visitor);
}

TypeIdExpr::TypeIdExpr(CppObject* child_): PostfixExpression(U"typeid", child_) 
{
}

void TypeIdExpr::Print(CodeFormatter& formatter)
{
    formatter.Write("typeid(");
    Child()->Print(formatter);
    formatter.Write(")");
}

void TypeIdExpr::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
    Child()->Accept(visitor);
}

PreIncrementExpr::PreIncrementExpr(CppObject* child_): UnaryExpression(U"++", child_) 
{
}

void PreIncrementExpr::Print(CodeFormatter& formatter)
{
    formatter.Write("++");
    Child()->Print(formatter);
}

void PreIncrementExpr::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
    Child()->Accept(visitor);
}

PreDecrementExpr::PreDecrementExpr(CppObject* child_): UnaryExpression(U"--", child_) 
{
}

void PreDecrementExpr::Print(CodeFormatter& formatter)
{
    formatter.Write("--");
    Child()->Print(formatter);
}

void PreDecrementExpr::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
    Child()->Accept(visitor);
}

UnaryOpExpr::UnaryOpExpr(Operator op_, CppObject* child_): UnaryExpression(GetOperatorStr(op_), child_), op(op_) 
{
}

void UnaryOpExpr::Print(CodeFormatter& formatter)
{
    formatter.Write(ToUtf8(Name()));
    if (Child()->Rank() < Rank())
    {
        formatter.Write("(");
    }
    Child()->Print(formatter);
    if (Child()->Rank() < Rank())
    {
        formatter.Write(")");
    }
}

void UnaryOpExpr::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
    Child()->Accept(visitor);
}

SizeOfExpr::SizeOfExpr(CppObject* child_, bool parens_): UnaryExpression(U"sizeof", child_), parens(parens_) 
{
}

void SizeOfExpr::Print(CodeFormatter& formatter)
{
    formatter.Write("sizeof");
    if (parens)
    {
        formatter.Write("(");
    }
    else 
    {
        formatter.Write(" ");
    }
    Child()->Print(formatter);
    if (parens)
    {
        formatter.Write(")");
    }
}

void SizeOfExpr::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
    Child()->Accept(visitor);
}

CastExpr::CastExpr(CppObject* typeId_, CppObject* expr_): Expression(U"cast"), typeId(typeId_), expr(expr_) 
{
    Own(typeId);
    Own(expr);
}

void CastExpr::Print(CodeFormatter& formatter)
{
    formatter.Write("(");
    typeId->Print(formatter);
    formatter.Write(")");
    if (expr->Rank() < Rank())
    {
        formatter.Write("(");
    }
    expr->Print(formatter);
    if (expr->Rank() < Rank())
    {
        formatter.Write(")");
    }
}

void CastExpr::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
    typeId->Accept(visitor);
    expr->Accept(visitor);
}

BinaryExpression::BinaryExpression(const std::u32string& name_, CppObject* left_, CppObject* right_): Expression(name_), left(left_), right(right_) 
{
    Own(left);
    Own(right);
}

BinaryOpExpr::BinaryOpExpr(CppObject* left_, Operator op_, int rank_, CppObject* right_): BinaryExpression(GetOperatorStr(op_), left_, right_), op(op_), rank(rank_) 
{
}

void BinaryOpExpr::Print(CodeFormatter& formatter)
{
    if (Left()->Rank() < Rank())
    {
        formatter.Write("(");
    }
    Left()->Print(formatter);
    if (Left()->Rank() < Rank())
    {
        formatter.Write(")");
    }
    formatter.Write(" ");
    formatter.Write(ToUtf8(Name()));
    formatter.Write(" ");
    if (Right()->Rank() < Rank())
    {
        formatter.Write("(");
    }
    Right()->Print(formatter);
    if (Right()->Rank() < Rank())
    {
        formatter.Write(")");
    }
}

void BinaryOpExpr::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
    Left()->Accept(visitor);
    Right()->Accept(visitor);
}

ConditionalExpr::ConditionalExpr(CppObject* condition_, CppObject* thenExpr_, CppObject* elseExpr_): 
    Expression(U"?:"), condition(condition_), thenExpr(thenExpr_), elseExpr(elseExpr_) 
{
    Own(condition);
    Own(thenExpr);
    Own(elseExpr);
}

void ConditionalExpr::Print(CodeFormatter& formatter)
{
    condition->Print(formatter);
    formatter.Write(" ? ");
    thenExpr->Print(formatter);
    formatter.Write(" : ");
    elseExpr->Print(formatter);
}

void ConditionalExpr::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
    condition->Accept(visitor);
    thenExpr->Accept(visitor);
    elseExpr->Accept(visitor);
}

ThrowExpr::ThrowExpr(CppObject* exception_): Expression(U"throw"), exception(exception_) 
{
    Own(exception);
}

void ThrowExpr::Print(CodeFormatter& formatter)
{
    formatter.Write("throw");
    if (exception)
    {
        formatter.Write(" ");
        exception->Print(formatter);
    }
}

void ThrowExpr::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
    if (exception)
    {
        exception->Accept(visitor);
    }
}

NewExpr::NewExpr(bool global_, const std::vector<CppObject*>& placement_, TypeId* typeId_, bool parens_, const std::vector<CppObject*>& initializer_):
    Expression(U"new"), global(global_), placement(placement_), typeId(typeId_), parens(parens_), initializer(initializer_) 
{
    for (CppObject* p : placement)
    {
        Own(p);
    }
    Own(typeId);
    for (CppObject* i : initializer)
    {
        Own(i);
    }
}

void NewExpr::Print(CodeFormatter& formatter)
{
    if (global)
    {
        formatter.Write("::");
    }
    formatter.Write("new");
    if (!placement.empty())
    {
        formatter.Write("(");
        PrintExprs(placement, formatter);
        formatter.Write(")");
    }
    if (parens)
    {
        formatter.Write("(");
    }
    formatter.Write(" ");
    typeId->Print(formatter);
    if (parens)
    {
        formatter.Write(")");
    }
    if (!initializer.empty())
    {
        formatter.Write("(");
        PrintExprs(initializer, formatter);
        formatter.Write(")");
    }
}

void NewExpr::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
    for (CppObject* p : placement)
    {
        p->Accept(visitor);
    }
    if (typeId)
    {
        typeId->Accept(visitor);
    }
    for (CppObject* i : initializer)
    {
        i->Accept(visitor);
    }
}

DeleteExpr::DeleteExpr(bool global_, bool isArray_, CppObject* expr_): Expression(U"delete"), global(global_), isArray(isArray_), expr(expr_) 
{
    Own(expr);
}

void DeleteExpr::Print(CodeFormatter& formatter)
{
    if (global)
    {
        formatter.Write("::");
    }
    formatter.Write("delete");
    if (isArray)
    {
        formatter.Write("[]");
    }
    formatter.Write(" ");
    expr->Print(formatter);
}

void DeleteExpr::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
    expr->Accept(visitor);
}

} } // namespace cmajor::codedom
