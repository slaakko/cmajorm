// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEDOM_STATEMENT_INCLUDED
#define CMAJOR_CODEDOM_STATEMENT_INCLUDED
#include <cmajor/codedom/Object.hpp>

namespace cmajor { namespace codedom {

class CODEDOM_API Statement: public CppObject
{
public:
    Statement(const std::u32string& name_);
    virtual bool IsCompoundStatement() const { return false; }
};

class CODEDOM_API LabeledStatement: public Statement
{
public:
    LabeledStatement(const std::u32string& label_, Statement* statement_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    std::u32string label;
    Statement* statement;
};

class CODEDOM_API CaseStatement: public Statement
{
public:
    CaseStatement(CppObject* expression_, Statement* statement_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    CppObject* expression;
    Statement* statement;
};

class CODEDOM_API DefaultStatement : public Statement
{
public:
    DefaultStatement(Statement* statement_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    Statement* statement;
};

class CODEDOM_API EmptyStatement : public Statement
{
public:
    EmptyStatement();
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
};

class CODEDOM_API ExpressionStatement : public Statement
{
public:
    ExpressionStatement(CppObject* expression_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    CppObject* expression;
};

class CODEDOM_API CompoundStatement : public Statement
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

class CODEDOM_API SelectionStatement : public Statement
{
public:
    SelectionStatement(const std::u32string& name_);
};

class CODEDOM_API IfStatement : public SelectionStatement
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

class CODEDOM_API SwitchStatement : public SelectionStatement
{
public:
    SwitchStatement(CppObject* condition_, Statement* statement_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    CppObject* condition;
    Statement* statement;
};

class CODEDOM_API IterationStatement : public Statement
{
public:
    IterationStatement(const std::u32string& name_);
};

class CODEDOM_API WhileStatement : public IterationStatement
{
public:
    WhileStatement(CppObject* condition_, Statement* statement_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    CppObject* condition;
    Statement* statement;
};

class CODEDOM_API DoStatement : public IterationStatement
{
public:
    DoStatement(Statement* statement_, CppObject* condition_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    Statement* statement;
    CppObject* condition;
};

class CODEDOM_API ForStatement : public IterationStatement
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

class CODEDOM_API JumpStatement : public Statement
{
public:
    JumpStatement(const std::u32string& name_);
};

class CODEDOM_API BreakStatement : public JumpStatement
{
public:
    BreakStatement();
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
};

class CODEDOM_API ContinueStatement : public JumpStatement
{
public:
    ContinueStatement();
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
};

class CODEDOM_API GotoStatement : public JumpStatement
{
public:
    GotoStatement(const std::u32string& target_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    std::u32string target;
};

class CODEDOM_API ReturnStatement : public JumpStatement
{
public:
    ReturnStatement(CppObject* expression_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    CppObject* expression;
};

class TypeId;

class CODEDOM_API ConditionWithDeclarator : public CppObject
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

class CODEDOM_API DeclarationStatement : public Statement
{
public:
    DeclarationStatement(CppObject* declaration_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    CppObject* declaration;
};

class CODEDOM_API ExceptionDeclaration : public CppObject
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

class CODEDOM_API Handler : public CppObject
{
public:
    Handler(ExceptionDeclaration* exceptionDeclaration_, CompoundStatement* statement_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    ExceptionDeclaration* exceptionDeclaration;
    CompoundStatement* statement;
};

class CODEDOM_API TryStatement : public Statement
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
