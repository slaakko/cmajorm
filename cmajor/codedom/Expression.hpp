// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEDOM_EXPRESSION_INCLUDED
#define CMAJOR_CODEDOM_EXPRESSION_INCLUDED
#include <cmajor/codedom/Object.hpp>
#include <cmajor/codedom/Operator.hpp>

namespace cmajor { namespace codedom {

void PrintExprs(const std::vector<CppObject*>& expressions, CodeFormatter& formatter);

class Expression : public CppObject
{
public:
    Expression(const std::u32string& name_);
};

class ThisAccess : public Expression
{
public:
    ThisAccess();
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 25; }
};

class IdExpr : public Expression
{
public:
    IdExpr(const std::u32string& value_);
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 26; }
};

class UnaryExpression : public Expression
{
public:
    UnaryExpression(const std::u32string& name_, CppObject* child_);
    CppObject* Child() const { return child; }
private:
    CppObject* child;
};

class PostfixExpression : public UnaryExpression
{
public:
    PostfixExpression(const std::u32string& name_, CppObject* child_);
};

class IndexExpr : public PostfixExpression
{
public:
    IndexExpr(CppObject* child_, CppObject* index_);
    CppObject* Index() const { return index; }
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 18; }
private:
    CppObject* index;
};

class InvokeExpr : public PostfixExpression
{
public:
    InvokeExpr(CppObject* child_, const std::vector<CppObject*>& arguments_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 19; }
private:
    std::vector<CppObject*> arguments;
};

class MemberAccessExpr : public PostfixExpression
{
public:
    MemberAccessExpr(CppObject* child_, CppObject* member_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 20; }
private:
    CppObject* member;
};

class PtrMemberAccessExpr : public PostfixExpression
{
public:
    PtrMemberAccessExpr(CppObject* child_, CppObject* member_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 21; }
private:
    CppObject* member;
};

class PostIncrementExpr : public PostfixExpression
{
public:
    PostIncrementExpr(CppObject* child_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 22; }
};

class PostDecrementExpr : public PostfixExpression
{
public:
    PostDecrementExpr(CppObject* child_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 22; }
};

class PostCastExpr: public PostfixExpression
{
public:
    PostCastExpr(const std::u32string& name_, CppObject* type_, CppObject* child_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 23; }
private:
    CppObject* type;
};

class TypeIdExpr : public PostfixExpression
{
public:
    TypeIdExpr(CppObject* child_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 24; }
};

class PreIncrementExpr : public UnaryExpression
{
public:
    PreIncrementExpr(CppObject* child_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 14; }
};

class PreDecrementExpr : public UnaryExpression
{
public:
    PreDecrementExpr(CppObject* child_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 14; }
};

class UnaryOpExpr : public UnaryExpression
{
public:
    UnaryOpExpr(Operator op_, CppObject* child_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    Operator Op() const { return op; }
    int Rank() const override { return 15; }
private:
    Operator op;
};

class SizeOfExpr : public UnaryExpression
{
public:
    SizeOfExpr(CppObject* child_, bool parens_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 16; }
private:
    bool parens;
};

class CastExpr : public Expression
{
public:
    CastExpr(CppObject* typeId_, CppObject* expr_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 13; }
private:
    CppObject* typeId;
    CppObject* expr;
};

class BinaryExpression : public Expression
{
public:
    BinaryExpression(const std::u32string& name_, CppObject* left_, CppObject* right_);
    CppObject* Left() const { return left; }
    CppObject* Right() const { return right; }
private:
    CppObject* left;
    CppObject* right;
};

class BinaryOpExpr : public BinaryExpression
{
public:
    BinaryOpExpr(CppObject* left_, Operator op_, int rank_, CppObject* right_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    Operator Op() const { return op; }
    int Rank() const override { return rank; }
private:
    Operator op;
    int rank;
};

class ConditionalExpr : public Expression
{
public:
    ConditionalExpr(CppObject* condition_, CppObject* thenExpr_, CppObject* elseExpr_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    CppObject* condition;
    CppObject* thenExpr;
    CppObject* elseExpr;
};

class ThrowExpr : public Expression
{
public:
    ThrowExpr(CppObject* exception_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    CppObject* exception;
};

class TypeId;

class NewExpr : public Expression
{
public:
    NewExpr(bool global_, const std::vector<CppObject*>& placement_, TypeId* typeId_, bool parens_, const std::vector<CppObject*>& initializer_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 17; }
private:
    bool global;
    std::vector<CppObject*> placement;
    TypeId* typeId;
    bool parens;
    std::vector<CppObject*> initializer;
};

class DeleteExpr : public Expression
{
public:
    DeleteExpr(bool global_, bool isArray_, CppObject* expr_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 17; }
private:
    bool global;
    bool isArray;
    CppObject* expr;
};

} } // namespace cmajor::codedom

#endif // CMAJOR_CODEDOM_EXPRESSION_INCLUDED

