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

class CODEDOM_API Expression : public CppObject
{
public:
    Expression(const std::u32string& name_);
};

class CODEDOM_API ThisAccess : public Expression
{
public:
    ThisAccess();
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 25; }
};

class CODEDOM_API IdExpr : public Expression
{
public:
    IdExpr(const std::u32string& value_);
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 26; }
};

class CODEDOM_API UnaryExpression : public Expression
{
public:
    UnaryExpression(const std::u32string& name_, CppObject* child_);
    CppObject* Child() const { return child; }
private:
    CppObject* child;
};

class CODEDOM_API PostfixExpression : public UnaryExpression
{
public:
    PostfixExpression(const std::u32string& name_, CppObject* child_);
};

class CODEDOM_API IndexExpr : public PostfixExpression
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

class CODEDOM_API InvokeExpr : public PostfixExpression
{
public:
    InvokeExpr(CppObject* child_, const std::vector<CppObject*>& arguments_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 19; }
private:
    std::vector<CppObject*> arguments;
};

class CODEDOM_API MemberAccessExpr : public PostfixExpression
{
public:
    MemberAccessExpr(CppObject* child_, CppObject* member_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 20; }
private:
    CppObject* member;
};

class CODEDOM_API PtrMemberAccessExpr : public PostfixExpression
{
public:
    PtrMemberAccessExpr(CppObject* child_, CppObject* member_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 21; }
private:
    CppObject* member;
};

class CODEDOM_API PostIncrementExpr : public PostfixExpression
{
public:
    PostIncrementExpr(CppObject* child_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 22; }
};

class CODEDOM_API PostDecrementExpr : public PostfixExpression
{
public:
    PostDecrementExpr(CppObject* child_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 22; }
};

class CODEDOM_API PostCastExpr: public PostfixExpression
{
public:
    PostCastExpr(const std::u32string& name_, CppObject* type_, CppObject* child_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 23; }
private:
    CppObject* type;
};

class CODEDOM_API TypeIdExpr : public PostfixExpression
{
public:
    TypeIdExpr(CppObject* child_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 24; }
};

class CODEDOM_API PreIncrementExpr : public UnaryExpression
{
public:
    PreIncrementExpr(CppObject* child_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 14; }
};

class CODEDOM_API PreDecrementExpr : public UnaryExpression
{
public:
    PreDecrementExpr(CppObject* child_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 14; }
};

class CODEDOM_API UnaryOpExpr : public UnaryExpression
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

class CODEDOM_API SizeOfExpr : public UnaryExpression
{
public:
    SizeOfExpr(CppObject* child_, bool parens_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    int Rank() const override { return 16; }
private:
    bool parens;
};

class CODEDOM_API CastExpr : public Expression
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

class CODEDOM_API BinaryExpression : public Expression
{
public:
    BinaryExpression(const std::u32string& name_, CppObject* left_, CppObject* right_);
    CppObject* Left() const { return left; }
    CppObject* Right() const { return right; }
private:
    CppObject* left;
    CppObject* right;
};

class CODEDOM_API BinaryOpExpr : public BinaryExpression
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

class CODEDOM_API ConditionalExpr : public Expression
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

class CODEDOM_API ThrowExpr : public Expression
{
public:
    ThrowExpr(CppObject* exception_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    CppObject* exception;
};

class TypeId;

class CODEDOM_API NewExpr : public Expression
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

class CODEDOM_API DeleteExpr : public Expression
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

