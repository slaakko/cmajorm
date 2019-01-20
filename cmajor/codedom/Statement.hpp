// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEDOM_STATEMENT_INCLUDED
#define CMAJOR_CODEDOM_STATEMENT_INCLUDED
#include <cmajor/codedom/Object.hpp>

namespace cmajor { namespace codedom {

class Statement: public CppObject
{
public:
    Statement(const std::u32string& name_);
    virtual bool IsCompoundStatement() const { return false; }
};

class LabeledStatement: public Statement
{
public:
    LabeledStatement(const std::u32string& label_, Statement* statement_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    std::u32string label;
    Statement* statement;
};

class CaseStatement: public Statement
{
public:
    CaseStatement(CppObject* expression_, Statement* statement_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    CppObject* expression;
    Statement* statement;
};

class DefaultStatement : public Statement
{
public:
    DefaultStatement(Statement* statement_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    Statement* statement;
};

class EmptyStatement : public Statement
{
public:
    EmptyStatement();
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
};

class ExpressionStatement : public Statement
{
public:
    ExpressionStatement(CppObject* expression_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    CppObject* expression;
};

class CompoundStatement : public Statement
{
public:
    CompoundStatement();
    void Add(Statement* statement);
    bool IsCompoundStatement() const override { return true; }
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    std::vector<Statement*> statements;
};

typedef std::shared_ptr<CompoundStatement> CompoundStatementPtr;

class SelectionStatement : public Statement
{
public:
    SelectionStatement(const std::u32string& name_);
};

class IfStatement : public SelectionStatement
{
public:
    IfStatement(CppObject* condition_, Statement* thenStatement_, Statement* elseStatement_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    CppObject* condition;
    Statement* thenStatement;
    Statement* elseStatement;
};

class SwitchStatement : public SelectionStatement
{
public:
    SwitchStatement(CppObject* condition_, Statement* statement_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    CppObject* condition;
    Statement* statement;
};

class IterationStatement : public Statement
{
public:
    IterationStatement(const std::u32string& name_);
};

class WhileStatement : public IterationStatement
{
public:
    WhileStatement(CppObject* condition_, Statement* statement_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    CppObject* condition;
    Statement* statement;
};

class DoStatement : public IterationStatement
{
public:
    DoStatement(Statement* statement_, CppObject* condition_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    Statement* statement;
    CppObject* condition;
};

class ForStatement : public IterationStatement
{
public:
    ForStatement(CppObject* initialization_, CppObject* condition_, CppObject* iteration_, Statement* statement_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    CppObject* initialization;
    CppObject* condition;
    CppObject* iteration;
    Statement* statement;
};

class JumpStatement : public Statement
{
public:
    JumpStatement(const std::u32string& name_);
};

class BreakStatement : public JumpStatement
{
public:
    BreakStatement();
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
};

class ContinueStatement : public JumpStatement
{
public:
    ContinueStatement();
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
};

class GotoStatement : public JumpStatement
{
public:
    GotoStatement(const std::u32string& target_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    std::u32string target;
};

class ReturnStatement : public JumpStatement
{
public:
    ReturnStatement(CppObject* expression_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    CppObject* expression;
};

class TypeId;

class ConditionWithDeclarator : public CppObject
{
public:
    ConditionWithDeclarator(TypeId* type_, const std::u32string& declarator_, CppObject* expression_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    TypeId* Type() const { return type; }
    const std::u32string& Declarator() const { return declarator; }
    CppObject* Expression() const { return expression; }
private:
    TypeId* type;
    std::u32string declarator;
    CppObject* expression;
};

class DeclarationStatement : public Statement
{
public:
    DeclarationStatement(CppObject* declaration_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    CppObject* declaration;
};

class ExceptionDeclaration : public CppObject
{
public:
    ExceptionDeclaration();
    TypeId* GetTypeId() const { return typeId; }
    bool CatchAll() const { return catchAll; }
    bool& CatchAll() { return catchAll; }
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    TypeId* typeId;
    bool catchAll;
};

typedef std::shared_ptr<ExceptionDeclaration> ExceptionDeclarationPtr;

class Handler : public CppObject
{
public:
    Handler(ExceptionDeclaration* exceptionDeclaration_, CompoundStatement* statement_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    ExceptionDeclaration* exceptionDeclaration;
    CompoundStatement* statement;
};

class TryStatement : public Statement
{
public:
    TryStatement(CompoundStatement* statement_);
    void Add(Handler* handler);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    CompoundStatement* GetStatement() const { return statement; }
private:
    CompoundStatement* statement;
    std::vector<Handler*> handlers;
};

} } // namespace cmajor::codedom

#endif // CMAJOR_CODEDOM_STATEMENT_INCLUDED
