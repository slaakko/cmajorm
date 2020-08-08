// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_BOUND_STATEMENT_INCLUDED
#define CMAJOR_BINDER_BOUND_STATEMENT_INCLUDED
#include <cmajor/binder/BoundExpression.hpp>
#include <cmajor/symbols/VariableSymbol.hpp>
#include <cmajor/symbols/Value.hpp>
#include <sngcm/ast/Statement.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::symbols;
using namespace sngcm::ast;

class BoundFunctionCall;
class BoundExpression;
class BoundCompoundStatement;

enum class BoundStatementFlags : uint8_t
{
    none = 0,
    postfix = 1 << 0,
    generated = 1 << 1
};

inline BoundStatementFlags operator|(BoundStatementFlags left, BoundStatementFlags right)
{
    return BoundStatementFlags(uint8_t(left) | uint8_t(right));
}

inline BoundStatementFlags operator&(BoundStatementFlags left, BoundStatementFlags right)
{
    return BoundStatementFlags(uint8_t(left) & uint8_t(right));
}

class BINDER_API BoundStatement : public BoundNode
{
public:
    BoundStatement(Module* module_, const Span& span_, BoundNodeType boundNodeType_);
    BoundStatement(const BoundStatement&) = delete;
    BoundStatement& operator=(const BoundStatement&) = delete;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    BoundStatement* Parent() { return parent; }
    void SetParent(BoundStatement* parent_) { parent = parent_; }
    BoundCompoundStatement* Block();
    void SetLabel(const std::u32string& label_);
    const std::u32string& Label() const { return label; }
    void SetPostfix() { SetFlag(BoundStatementFlags::postfix); }
    bool Postfix() const { return GetFlag(BoundStatementFlags::postfix); }
    void SetGenerated() { SetFlag(BoundStatementFlags::generated); }
    bool Generated() const { return GetFlag(BoundStatementFlags::generated); }
    virtual bool IsOrContainsBoundReturnStatement() const { return false; }
private:
    std::u32string label;
    BoundStatement* parent;
    BoundStatementFlags flags;
    bool GetFlag(BoundStatementFlags flag) const { return (flags & flag) != BoundStatementFlags::none;  }
    void SetFlag(BoundStatementFlags flag) { flags = flags | flag; }
};

class BINDER_API BoundSequenceStatement : public BoundStatement
{
public:
    BoundSequenceStatement(Module* module_, const Span& span_, std::unique_ptr<BoundStatement>&& first_, std::unique_ptr<BoundStatement>&& second_);
    void Accept(BoundNodeVisitor& visitor) override;
    BoundStatement* First() { return first.get(); }
    BoundStatement* Second() { return second.get(); }
    bool IsOrContainsBoundReturnStatement() const override { return second->IsOrContainsBoundReturnStatement(); }
private:
    std::unique_ptr<BoundStatement> first;
    std::unique_ptr<BoundStatement> second;
};

class BINDER_API BoundCompoundStatement : public BoundStatement
{
public:
    BoundCompoundStatement(Module* module_, const Span& span_);
    BoundCompoundStatement(Module* module_, const Span& span_, const Span& endSpan_);
    BoundCompoundStatement(const BoundCompoundStatement&) = delete;
    BoundCompoundStatement& operator=(const BoundCompoundStatement&) = delete;
    void Accept(BoundNodeVisitor& visitor) override;
    void InsertStatementToFront(std::unique_ptr<BoundStatement>&& statement);
    void AddStatement(std::unique_ptr<BoundStatement>&& statement);
    const std::vector<std::unique_ptr<BoundStatement>>& Statements() const { return statements; }
    const Span& EndSpan() const { return endSpan; }
private:
    std::vector<std::unique_ptr<BoundStatement>> statements;
    Span endSpan;
};

class BINDER_API BoundReturnStatement : public BoundStatement
{
public:
    BoundReturnStatement(Module* module_, std::unique_ptr<BoundFunctionCall>&& returnFunctionCall_, const Span& span_);
    BoundReturnStatement(const BoundReturnStatement&) = delete;
    BoundReturnStatement& operator=(const BoundReturnStatement&) = delete;
    void Accept(BoundNodeVisitor& visitor) override;
    BoundFunctionCall* ReturnFunctionCall() { return returnFunctionCall.get(); }
    bool IsOrContainsBoundReturnStatement() const override { return true; }
private:
    std::unique_ptr<BoundFunctionCall> returnFunctionCall;
};

class BINDER_API BoundIfStatement : public BoundStatement
{
public:
    BoundIfStatement(Module* module_, const Span& span_, std::unique_ptr<BoundExpression>&& condition_, std::unique_ptr<BoundStatement>&& thenS_, std::unique_ptr<BoundStatement>&& elseS_);
    void Accept(BoundNodeVisitor& visitor) override;
    BoundExpression* Condition() { return condition.get(); }
    BoundStatement* ThenS() { return thenS.get(); }
    BoundStatement* ElseS() { return elseS.get(); }
private:
    std::unique_ptr<BoundExpression> condition;
    std::unique_ptr<BoundStatement> thenS;
    std::unique_ptr<BoundStatement> elseS;
};

class BINDER_API BoundWhileStatement : public BoundStatement
{
public:
    BoundWhileStatement(Module* module_, const Span& span_, std::unique_ptr<BoundExpression>&& condition_, std::unique_ptr<BoundStatement>&& statement_);
    void Accept(BoundNodeVisitor& visitor) override;
    BoundExpression* Condition() { return condition.get(); }
    BoundStatement* Statement() { return statement.get(); }
private:
    std::unique_ptr<BoundExpression> condition;
    std::unique_ptr<BoundStatement> statement;
};

class BINDER_API BoundDoStatement : public BoundStatement
{
public:
    BoundDoStatement(Module* module_, const Span& span_, std::unique_ptr<BoundStatement>&& statement_, std::unique_ptr<BoundExpression>&& condition_);
    void Accept(BoundNodeVisitor& visitor) override;
    BoundStatement* Statement() { return statement.get(); }
    BoundExpression* Condition() { return condition.get(); }
private:
    std::unique_ptr<BoundStatement> statement;
    std::unique_ptr<BoundExpression> condition;
};

class BINDER_API BoundForStatement : public BoundStatement
{
public:
    BoundForStatement(Module* module_, const Span& span_, std::unique_ptr<BoundStatement>&& initS_, std::unique_ptr<BoundExpression>&& condition_, std::unique_ptr<BoundStatement>&& loopS_, std::unique_ptr<BoundStatement>&& actionS_);
    void Accept(BoundNodeVisitor& visitor) override;
    BoundStatement* InitS() { return initS.get(); }
    BoundExpression* Condition() { return condition.get(); }
    BoundStatement* LoopS() { return loopS.get(); }
    BoundStatement* ActionS() { return actionS.get(); }
private:
    std::unique_ptr<BoundStatement> initS;
    std::unique_ptr<BoundExpression> condition;
    std::unique_ptr<BoundStatement> loopS;
    std::unique_ptr<BoundStatement> actionS;
};

class BoundCaseStatement;
class BoundDefaultStatement;

class BINDER_API BoundSwitchStatement : public BoundStatement
{
public:
    BoundSwitchStatement(Module* module_, const Span& span_, std::unique_ptr<BoundExpression>&& condition_);
    BoundExpression* Condition() { return condition.get(); }
    const std::vector<std::unique_ptr<BoundCaseStatement>>& CaseStatements() { return caseStatements; }
    void AddCaseStatement(std::unique_ptr<BoundCaseStatement>&& caseStatement);
    BoundDefaultStatement* DefaultStatement() { return defaultStatement.get(); }
    void SetDefaultStatement(std::unique_ptr<BoundDefaultStatement>&& defaultStatement_);
    void Accept(BoundNodeVisitor& visitor) override;
private:
    std::unique_ptr<BoundExpression> condition;
    std::vector<std::unique_ptr<BoundCaseStatement>> caseStatements;
    std::unique_ptr<BoundDefaultStatement> defaultStatement;
};

class BINDER_API BoundCaseStatement : public BoundStatement
{
public:
    BoundCaseStatement(Module* module_, const Span& span_);
    void AddCaseValue(std::unique_ptr<Value>&& caseValue_);
    const std::vector<std::unique_ptr<Value>>& CaseValues() const { return caseValues; }
    void AddStatement(std::unique_ptr<BoundStatement>&& statement);
    BoundCompoundStatement* CompoundStatement() { return compoundStatement.get(); }
    void Accept(BoundNodeVisitor& visitor) override;
private:
    std::vector<std::unique_ptr<Value>> caseValues;
    std::unique_ptr<BoundCompoundStatement> compoundStatement;
};

class BINDER_API BoundDefaultStatement : public BoundStatement
{
public:
    BoundDefaultStatement(Module* module_, const Span& span_);
    void AddStatement(std::unique_ptr<BoundStatement>&& statement);
    BoundCompoundStatement* CompoundStatement() { return compoundStatement.get(); }
    void Accept(BoundNodeVisitor& visitor) override;
private:
    std::unique_ptr<BoundCompoundStatement> compoundStatement;
};

class BINDER_API BoundGotoCaseStatement : public BoundStatement
{
public:
    BoundGotoCaseStatement(Module* module_, const Span& span_, std::unique_ptr<Value>&& caseValue_);
    void Accept(BoundNodeVisitor& visitor) override;
    Value* CaseValue() { return caseValue.get(); }
private:
    std::unique_ptr<Value> caseValue;
};

class BINDER_API BoundGotoDefaultStatement : public BoundStatement
{
public:
    BoundGotoDefaultStatement(Module* module_, const Span& span_);
    void Accept(BoundNodeVisitor& visitor) override;
};

class BINDER_API BoundBreakStatement : public BoundStatement
{
public:
    BoundBreakStatement(Module* module_, const Span& span_);
    void Accept(BoundNodeVisitor& visitor) override;
};

class BINDER_API BoundContinueStatement : public BoundStatement
{
public:
    BoundContinueStatement(Module* module_, const Span& span_);
    void Accept(BoundNodeVisitor& visitor) override;
};

class BINDER_API BoundGotoStatement : public BoundStatement
{
public:
    BoundGotoStatement(Module* module_, const Span& span_, const std::u32string& target_);
    void Accept(BoundNodeVisitor& visitor) override;
    const std::u32string& Target() const { return target; }
    void SetTargetStatement(BoundStatement* targetStatement_) { targetStatement = targetStatement_; }
    BoundStatement* TargetStatement() { return targetStatement; }
    void SetTargetBlock(BoundCompoundStatement* targetBlock_) { targetBlock = targetBlock_; }
    BoundCompoundStatement* TargetBlock() { return targetBlock; }
private:
    std::u32string target;
    BoundStatement* targetStatement;
    BoundCompoundStatement* targetBlock;
};

class BINDER_API BoundConstructionStatement : public BoundStatement
{
public:
    BoundConstructionStatement(Module* module_, std::unique_ptr<BoundFunctionCall>&& constructorCall_, const Span& span);
    void Accept(BoundNodeVisitor& visitor) override;
    BoundFunctionCall* ConstructorCall() { return constructorCall.get(); }
    void SetLocalVariable(LocalVariableSymbol* localVariable_);
    LocalVariableSymbol* GetLocalVariable() const { return localVariable; }
private:
    std::unique_ptr<BoundFunctionCall> constructorCall;
    LocalVariableSymbol* localVariable;
};

class BINDER_API BoundAssignmentStatement : public BoundStatement
{
public:
    BoundAssignmentStatement(Module* module_, std::unique_ptr<BoundFunctionCall>&& assignmentCall_, const Span& span);
    void Accept(BoundNodeVisitor& visitor) override;
    BoundFunctionCall* AssignmentCall() { return assignmentCall.get(); }
private:
    std::unique_ptr<BoundFunctionCall> assignmentCall;
};

class BINDER_API BoundExpressionStatement : public BoundStatement
{
public:
    BoundExpressionStatement(Module* module_, std::unique_ptr<BoundExpression>&& expression_, const Span& span);
    void Accept(BoundNodeVisitor& visitor) override;
    BoundExpression* Expression() { return expression.get(); }
private:
    std::unique_ptr<BoundExpression> expression;
};

class BINDER_API BoundInitializationStatement : public BoundStatement
{
public:
    BoundInitializationStatement(Module* module_, std::unique_ptr<BoundExpression>&& initializationExpression_);
    void Accept(BoundNodeVisitor& visitor) override;
    BoundExpression* InitializationExpression() { return initializationExpression.get(); }
private:
    std::unique_ptr<BoundExpression> initializationExpression;
};

class BINDER_API BoundEmptyStatement : public BoundStatement
{
public:
    BoundEmptyStatement(Module* module_, const Span& span_);
    void Accept(BoundNodeVisitor& visitor) override;
};

class BINDER_API BoundSetVmtPtrStatement : public BoundStatement
{
public:
    BoundSetVmtPtrStatement(Module * module_, std::unique_ptr<BoundExpression>&& classPtr_, ClassTypeSymbol* classType_);
    void Accept(BoundNodeVisitor& visitor) override;
    BoundExpression* ClassPtr() { return classPtr.get(); }
    ClassTypeSymbol* ClassType() { return classType; }
private:
    std::unique_ptr<BoundExpression> classPtr;
    ClassTypeSymbol* classType;
};

class BINDER_API BoundThrowStatement : public BoundStatement
{
public:
    BoundThrowStatement(Module* module_, const Span& span_, std::unique_ptr<BoundExpression>&& throwCallExpr_);
    void Accept(BoundNodeVisitor& visitor) override;
    BoundExpression* ThrowCallExpr() { return throwCallExpr.get(); }
private:
    std::unique_ptr<BoundExpression> throwCallExpr;
};

class BINDER_API BoundRethrowStatement : public BoundStatement
{
public:
    BoundRethrowStatement(Module* module_, const Span& span_, std::unique_ptr<BoundExpression>&& releaseCall_);
    void Accept(BoundNodeVisitor& visitor) override;
    BoundExpression* ReleaseCall() { return releaseCall.get(); }
private:
    std::unique_ptr<BoundExpression> releaseCall;
};

class BoundCatchStatement;

class BINDER_API BoundTryStatement : public BoundStatement
{
public:
    BoundTryStatement(Module* module_, const Span& span_);
    void SetTryBlock(std::unique_ptr<BoundStatement>&& tryBlock_);
    BoundStatement* TryBlock() { return tryBlock.get(); }
    void AddCatch(std::unique_ptr<BoundCatchStatement>&& catchStatement);
    const std::vector<std::unique_ptr<BoundCatchStatement>>& Catches() const { return catches; }
    void Accept(BoundNodeVisitor& visitor) override;
private:
    std::unique_ptr<BoundStatement> tryBlock;
    std::vector<std::unique_ptr<BoundCatchStatement>> catches;
};

class BINDER_API BoundCatchStatement : public BoundStatement
{
public:
    BoundCatchStatement(Module* module_, const Span& span__);
    void SetCatchedType(TypeSymbol* catchedType_) { catchedType = catchedType_; }
    TypeSymbol* CatchedType() { return catchedType; }
    void SetCatchVar(LocalVariableSymbol* catchVar_) { catchVar = catchVar_; }
    LocalVariableSymbol* CatchVar() { return catchVar; }
    void SetCatchBlock(std::unique_ptr<BoundStatement>&& catchBlock_);
    BoundStatement* CatchBlock() { return catchBlock.get(); }
    void Accept(BoundNodeVisitor& visitor) override;
    int CatchedTypeUuidId() const { return catchTypeUuidId; }
    void SetCatchedTypeUuidId(int catchTypeUuidId_) { catchTypeUuidId = catchTypeUuidId_; }
private:
    TypeSymbol* catchedType;
    LocalVariableSymbol* catchVar;
    std::unique_ptr<BoundStatement> catchBlock;
    int catchTypeUuidId;
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_BOUND_STATEMENT_INCLUDED
