// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEDOM_VISITOR_INCLUDED
#define CMAJOR_CODEDOM_VISITOR_INCLUDED

namespace cmajor { namespace codedom {

class Literal;
class Typedef;
class TypeSpecifier;
class TypeName;
class Type;
class TypeId;
class StorageClassSpecifier;
class AssignInit;
class Initializer;
class InitDeclarator;
class InitDeclaratorList;
class SimpleDeclaration;
class NamespaceAlias;
class UsingDeclaration;
class UsingDirective;
class ExpressionList;
class ThisAccess;
class IdExpr;
class IndexExpr;
class InvokeExpr;
class MemberAccessExpr;
class PtrMemberAccessExpr;
class PostIncrementExpr;
class PostDecrementExpr;
class PostCastExpr;
class TypeIdExpr;
class PreIncrementExpr;
class PreDecrementExpr;
class UnaryOpExpr;
class SizeOfExpr;
class CastExpr;
class BinaryOpExpr;
class ConditionalExpr;
class ThrowExpr;
class NewExpr;
class DeleteExpr;
class LabeledStatement;
class CaseStatement;
class DefaultStatement;
class EmptyStatement;
class ExpressionStatement;
class CompoundStatement;
class IfStatement;
class SwitchStatement;
class WhileStatement;
class DoStatement;
class ForStatement;
class BreakStatement;
class ContinueStatement;
class GotoStatement;
class ReturnStatement;
class ConditionWithDeclarator;
class DeclarationStatement;
class TryStatement;
class Handler;
class ExceptionDeclaration;

class Visitor
{
public:
    virtual void Visit(Literal& object) {}
    virtual void Visit(Typedef& object) {}
    virtual void Visit(TypeSpecifier& object) {}
    virtual void Visit(TypeName& object) {}
    virtual void Visit(Type& object) {}
    virtual void Visit(TypeId& object) {}
    virtual void Visit(StorageClassSpecifier& object) {}
    virtual void Visit(AssignInit& object) {}
    virtual void Visit(Initializer& object) {}
    virtual void Visit(InitDeclarator& object) {}
    virtual void Visit(InitDeclaratorList& object) {}
    virtual void Visit(SimpleDeclaration& object) {}
    virtual void Visit(NamespaceAlias& object) {}
    virtual void Visit(UsingDeclaration& object) {}
    virtual void Visit(UsingDirective& object) {}
    virtual void Visit(ThisAccess& object) {}
    virtual void Visit(IdExpr& object) {}
    virtual void Visit(IndexExpr& object) {}
    virtual void Visit(InvokeExpr& object) {}
    virtual void Visit(MemberAccessExpr& object) {}
    virtual void Visit(PtrMemberAccessExpr& object) {}
    virtual void Visit(PostIncrementExpr& object) {}
    virtual void Visit(PostDecrementExpr& object) {}
    virtual void Visit(PostCastExpr& object) {}
    virtual void Visit(TypeIdExpr& object) {}
    virtual void Visit(PreIncrementExpr& object) {}
    virtual void Visit(PreDecrementExpr& object) {}
    virtual void Visit(UnaryOpExpr& object) {}
    virtual void Visit(SizeOfExpr& object) {}
    virtual void Visit(CastExpr& object) {}
    virtual void Visit(BinaryOpExpr& object) {}
    virtual void Visit(ConditionalExpr& object) {}
    virtual void Visit(ThrowExpr& object) {}
    virtual void Visit(NewExpr& object) {}
    virtual void Visit(DeleteExpr& object) {}
    virtual void Visit(LabeledStatement& object) {}
    virtual void Visit(CaseStatement& object) {}
    virtual void Visit(DefaultStatement& object) {}
    virtual void Visit(EmptyStatement& object) {}
    virtual void Visit(ExpressionStatement& object) {}
    virtual void Visit(CompoundStatement& object) {}
    virtual void Visit(IfStatement& object) {}
    virtual void Visit(SwitchStatement& object) {}
    virtual void Visit(WhileStatement& object) {}
    virtual void Visit(DoStatement& object) {}
    virtual void Visit(ForStatement& object) {}
    virtual void Visit(BreakStatement& object) {}
    virtual void Visit(ContinueStatement& object) {}
    virtual void Visit(GotoStatement& object) {}
    virtual void Visit(ReturnStatement& object) {}
    virtual void Visit(ConditionWithDeclarator& object) {}
    virtual void Visit(DeclarationStatement& object) {}
    virtual void Visit(TryStatement& object) {}
    virtual void Visit(Handler& object) {}
    virtual void Visit(ExceptionDeclaration& object) {}
};

} } // namespace cmajor::codedom

#endif // CMAJOR_CODEDOM_VISITOR_INCLUDED
