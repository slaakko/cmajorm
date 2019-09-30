// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_STATEMENT_BINDER_INCLUDED
#define CMAJOR_BINDER_STATEMENT_BINDER_INCLUDED
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/binder/BoundStatement.hpp>
#include <cmajor/ast/Visitor.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::ast;

class BoundGotoCaseStatement;
class BoundGotoDefaultStatement;
class BoundClass;
class BoundFunction;

void CheckFunctionReturnPaths(FunctionSymbol* functionSymbol, FunctionNode& functionNode, ContainerScope* containerScope, BoundCompileUnit& boundCompileUnit);

class BINDER_API StatementBinder : public Visitor
{
public:
    StatementBinder(BoundCompileUnit& boundCompileUnit_);
    void Visit(CompileUnitNode& compileUnitNode) override;
    void Visit(NamespaceNode& namespaceNode) override;
    void Visit(EnumTypeNode& enumTypeNode) override;
    void Visit(ClassNode& classNode) override;
    void Visit(FunctionNode& functionNode) override;
    void Visit(StaticConstructorNode& staticConstructorNode) override;
    void Visit(ConstructorNode& constructorNode) override;
    void Visit(DestructorNode& destructorNode) override;
    void Visit(MemberFunctionNode& memberFunctionNode) override;
    void Visit(ConversionFunctionNode& conversionFunctionNode) override;
    void Visit(CompoundStatementNode& compoundStatementNode) override;
    void Visit(ReturnStatementNode& returnStatementNode) override;
    void Visit(IfStatementNode& ifStatementNode) override;
    void Visit(WhileStatementNode& whileStatementNode) override;
    void Visit(DoStatementNode& doStatementNode) override;
    void Visit(ForStatementNode& forStatementNode) override;
    void Visit(BreakStatementNode& breakStatementNode) override;
    void Visit(ContinueStatementNode& continueStatementNode) override;
    void Visit(GotoStatementNode& gotoStatementNode) override;
    void Visit(ConstructionStatementNode& constructionStatementNode) override;
    void Visit(DeleteStatementNode& deleteStatementNode) override;
    void Visit(DestroyStatementNode& destroyStatementNode) override;
    void Visit(AssignmentStatementNode& assignmentStatementNode) override;
    void Visit(ExpressionStatementNode& expressionStatementNode) override;
    void Visit(EmptyStatementNode& emptyStatementNode) override;
    void Visit(RangeForStatementNode& rangeForStatementNode) override;
    void Visit(SwitchStatementNode& switchStatementNode) override;
    void Visit(CaseStatementNode& caseStatementNode) override;
    void Visit(DefaultStatementNode& defaultStatementNode) override;
    void Visit(GotoCaseStatementNode& gotoCaseStatementNode) override;
    void Visit(GotoDefaultStatementNode& gotoDefaultStatementNode) override;
    void Visit(ThrowStatementNode& throwStatementNode) override;
    void Visit(TryStatementNode& tryStatementNode) override;
    void Visit(CatchNode& catchNode) override;
    void Visit(AssertStatementNode& assertStatementNode) override;
    void Visit(ConditionalCompilationPartNode& conditionalCompilationPartNode) override;
    void Visit(ConditionalCompilationDisjunctionNode& conditionalCompilationDisjunctionNode) override;
    void Visit(ConditionalCompilationConjunctionNode& conditionalCompilationConjunctionNode) override;
    void Visit(ConditionalCompilationNotNode& conditionalCompilationNotNode) override;
    void Visit(ConditionalCompilationPrimaryNode& conditionalCompilationPrimaryNode) override;
    void Visit(ConditionalCompilationStatementNode& conditionalCompilationStatementNode) override;
    void CompileStatement(Node* statementNode, bool setPostfix);
    BoundCompileUnit& GetBoundCompileUnit() { return boundCompileUnit; }
    void SetCurrentClass(BoundClass* currentClass_) { currentClass = currentClass_; }
    BoundClass* CurrentClass() const { return currentClass;  }
    ContainerScope* GetContainerScope() { return containerScope; }
    void SetContainerScope(ContainerScope* containerScope_) { containerScope = containerScope_; }
    BoundFunction* CurrentFunction() { return currentFunction; }
    void SetCurrentFunction(BoundFunction* currentFunction_) { currentFunction = currentFunction_; }
    void SetCurrentConstructor(ConstructorSymbol* currentConstructorSymbol_, ConstructorNode* currentConstructorNode_);
    ConstructorSymbol* CurrentConstructorSymbol() { return currentConstructorSymbol; }
    ConstructorNode* CurrentConstructorNode() { return currentConstructorNode; }
    void SetCurrentDestructor(DestructorSymbol* currentDestructorSymbol_, DestructorNode* currentDestructorNode_);
    void SetCurrentMemberFunction(MemberFunctionSymbol* currentMemberFunctionSymbol_, MemberFunctionNode* currentMemberFunctionNode_);
    BoundStatement* ReleaseStatement() { return statement.release(); }
    bool CompilingThrow() const { return compilingThrow; }
    bool InsideCatch() const { return insideCatch; }
private:
    BoundCompileUnit& boundCompileUnit;
    SymbolTable& symbolTable;
    Module* module;
    ContainerScope* containerScope;
    std::unique_ptr<BoundStatement> statement;
    int compoundLevel;
    bool insideCatch;
    BoundClass* currentClass;
    BoundFunction* currentFunction;
    StaticConstructorSymbol* currentStaticConstructorSymbol;
    StaticConstructorNode* currentStaticConstructorNode;
    ConstructorSymbol* currentConstructorSymbol;
    ConstructorNode* currentConstructorNode;
    DestructorSymbol* currentDestructorSymbol;
    DestructorNode* currentDestructorNode;
    MemberFunctionSymbol* currentMemberFunctionSymbol;
    MemberFunctionNode* currentMemberFunctionNode;
    TypeSymbol* switchConditionType;
    std::unordered_map<IntegralValue, CaseStatementNode*, IntegralValueHash>* currentCaseValueMap;
    std::vector<std::pair<BoundGotoCaseStatement*, IntegralValue>>* currentGotoCaseStatements;
    std::vector<BoundGotoDefaultStatement*>* currentGotoDefaultStatements;
    bool postfix;
    bool compilingThrow;
    bool compilingReleaseExceptionStatement;
    bool dontCheckDuplicateFunctionSymbols;
    std::stack<bool> conditionalCompilationStack;
    void AddStatement(BoundStatement* boundStatement);
    void AddReleaseExceptionStatement(const Span& span);
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_STATEMENT_BINDER_INCLUDED
