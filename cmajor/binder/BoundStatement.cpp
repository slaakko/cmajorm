// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/BoundStatement.hpp>
#include <cmajor/binder/BoundNodeVisitor.hpp>
#include <cmajor/binder/BoundExpression.hpp>
#include <cmajor/symbols/Exception.hpp>

namespace cmajor { namespace binder {

BoundStatement::BoundStatement(Module* module_, const Span& span_, BoundNodeType boundNodeType_) : 
    BoundNode(module_, span_, boundNodeType_), parent(nullptr), flags(BoundStatementFlags::none)
{
}

BoundCompoundStatement* BoundStatement::Block() 
{
    if (GetBoundNodeType() == BoundNodeType::boundCompoundStatement)
    {
        return static_cast<BoundCompoundStatement*>(this);
    }
    return parent->Block();
}

void BoundStatement::Load(Emitter& emitter, OperationFlags flags)
{
    throw Exception(GetModule(), "cannot load from statement", GetSpan());
}

void BoundStatement::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception(GetModule(), "cannot store to statement", GetSpan());
}

void BoundStatement::SetLabel(const std::u32string& label_)
{
    label = label_;
}

BoundSequenceStatement::BoundSequenceStatement(Module* module_, const Span& span_, std::unique_ptr<BoundStatement>&& first_, std::unique_ptr<BoundStatement>&& second_) :
    BoundStatement(module_, span_, BoundNodeType::boundSequenceStatement), first(std::move(first_)), second(std::move(second_))
{
    first->SetParent(this);
    second->SetParent(this);
}

void BoundSequenceStatement::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundCompoundStatement::BoundCompoundStatement(Module* module_, const Span& span_) : BoundStatement(module_, span_, BoundNodeType::boundCompoundStatement), endSpan()
{
}

BoundCompoundStatement::BoundCompoundStatement(Module* module_, const Span& span_, const Span& endSpan_) : BoundStatement(module_, span_, BoundNodeType::boundCompoundStatement), endSpan(endSpan_)
{
}

void BoundCompoundStatement::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

void BoundCompoundStatement::InsertStatementToFront(std::unique_ptr<BoundStatement>&& statement)
{
    statement->SetParent(this);
    statements.insert(statements.begin(), std::move(statement));
}

void BoundCompoundStatement::AddStatement(std::unique_ptr<BoundStatement>&& statement)
{
    statement->SetParent(this);
    statements.push_back(std::move(statement));
}

BoundReturnStatement::BoundReturnStatement(Module* module_, std::unique_ptr<BoundFunctionCall>&& returnFunctionCall_, const Span& span_) :
    BoundStatement(module_, span_, BoundNodeType::boundReturnStatement), returnFunctionCall(std::move(returnFunctionCall_))
{
}

void BoundReturnStatement::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundIfStatement::BoundIfStatement(Module* module_, const Span& span_, std::unique_ptr<BoundExpression>&& condition_, std::unique_ptr<BoundStatement>&& thenS_, std::unique_ptr<BoundStatement>&& elseS_) :
    BoundStatement(module_, span_, BoundNodeType::boundIfStatement), condition(std::move(condition_)), thenS(std::move(thenS_)), elseS(std::move(elseS_))
{
    thenS->SetParent(this);
    if (elseS)
    {
        elseS->SetParent(this);
    }
}

void BoundIfStatement::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundWhileStatement::BoundWhileStatement(Module* module_, const Span& span_, std::unique_ptr<BoundExpression>&& condition_, std::unique_ptr<BoundStatement>&& statement_) :
    BoundStatement(module_, span_, BoundNodeType::boundWhileStatement), condition(std::move(condition_)), statement(std::move(statement_))
{
    statement->SetParent(this);
}

void BoundWhileStatement::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundDoStatement::BoundDoStatement(Module* module_, const Span& span_, std::unique_ptr<BoundStatement>&& statement_, std::unique_ptr<BoundExpression>&& condition_) :
    BoundStatement(module_, span_, BoundNodeType::boundDoStatement), statement(std::move(statement_)), condition(std::move(condition_))
{
    statement->SetParent(this);
}

void BoundDoStatement::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundForStatement::BoundForStatement(Module* module_, const Span& span_, std::unique_ptr<BoundStatement>&& initS_, std::unique_ptr<BoundExpression>&& condition_, std::unique_ptr<BoundStatement>&& loopS_,
    std::unique_ptr<BoundStatement>&& actionS_) : BoundStatement(module_, span_, BoundNodeType::boundForStatement), initS(std::move(initS_)), condition(std::move(condition_)), loopS(std::move(loopS_)), 
    actionS(std::move(actionS_))
{
    initS->SetParent(this);
    loopS->SetParent(this);
    actionS->SetParent(this);
}

void BoundForStatement::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundSwitchStatement::BoundSwitchStatement(Module* module_, const Span& span_, std::unique_ptr<BoundExpression>&& condition_) :
    BoundStatement(module_, span_, BoundNodeType::boundSwitchStatement), condition(std::move(condition_))
{
}

void BoundSwitchStatement::AddCaseStatement(std::unique_ptr<BoundCaseStatement>&& caseStatement)
{
    caseStatement->SetParent(this);
    caseStatements.push_back(std::move(caseStatement));
}

void BoundSwitchStatement::SetDefaultStatement(std::unique_ptr<BoundDefaultStatement>&& defaultStatement_)
{
    defaultStatement = std::move(defaultStatement_);
    defaultStatement->SetParent(this);
}

void BoundSwitchStatement::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundCaseStatement::BoundCaseStatement(Module* module_, const Span& span_) : BoundStatement(module_, span_, BoundNodeType::boundCaseStatement)
{
}

void BoundCaseStatement::AddCaseValue(std::unique_ptr<Value>&& caseValue)
{
    caseValues.push_back(std::move(caseValue));
}

void BoundCaseStatement::AddStatement(std::unique_ptr<BoundStatement>&& statement)
{
    if (compoundStatement)
    {
        compoundStatement->AddStatement(std::move(statement));
    }
    else if (statement->GetBoundNodeType() == BoundNodeType::boundCompoundStatement)
    {
        compoundStatement.reset(static_cast<BoundCompoundStatement*>(statement.release()));
        compoundStatement->SetParent(this);
    }
    else
    {
        compoundStatement.reset(new BoundCompoundStatement(GetModule(), GetSpan()));
        compoundStatement->SetParent(this);
        compoundStatement->AddStatement(std::move(statement));
    }
}

void BoundCaseStatement::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundDefaultStatement::BoundDefaultStatement(Module* module_, const Span& span_) : 
    BoundStatement(module_, span_, BoundNodeType::boundDefaultStatement)
{
}

void BoundDefaultStatement::AddStatement(std::unique_ptr<BoundStatement>&& statement)
{
    if (compoundStatement)
    {
        compoundStatement->AddStatement(std::move(statement));
    }
    else if (statement->GetBoundNodeType() == BoundNodeType::boundCompoundStatement)
    {
        compoundStatement.reset(static_cast<BoundCompoundStatement*>(statement.release()));
        compoundStatement->SetParent(this);
    }
    else
    {
        compoundStatement.reset(new BoundCompoundStatement(GetModule(), GetSpan()));
        compoundStatement->SetParent(this);
        compoundStatement->AddStatement(std::move(statement));
    }
}

void BoundDefaultStatement::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundGotoCaseStatement::BoundGotoCaseStatement(Module* module_, const Span& span_, std::unique_ptr<Value>&& caseValue_) :
    BoundStatement(module_, span_, BoundNodeType::boundGotoCaseStatement), caseValue(std::move(caseValue_))
{
}

void BoundGotoCaseStatement::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundGotoDefaultStatement::BoundGotoDefaultStatement(Module* module_, const Span& span_) : BoundStatement(module_, span_, BoundNodeType::boundGotoDefaultStatement)
{
}

void BoundGotoDefaultStatement::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundBreakStatement::BoundBreakStatement(Module* module_, const Span& span_) : BoundStatement(module_, span_, BoundNodeType::boundBreakStatement)
{
}

void BoundBreakStatement::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundContinueStatement::BoundContinueStatement(Module* module_, const Span& span_) : BoundStatement(module_, span_, BoundNodeType::boundContinueStatement)
{
}

void BoundContinueStatement::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundGotoStatement::BoundGotoStatement(Module* module_, const Span& span_, const std::u32string& target_) :
    BoundStatement(module_, span_, BoundNodeType::boundGotoStatement), target(target_), targetStatement(nullptr), targetBlock(nullptr)
{
}

void BoundGotoStatement::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundConstructionStatement::BoundConstructionStatement(Module* module_, std::unique_ptr<BoundFunctionCall>&& constructorCall_) :
    BoundStatement(module_, constructorCall_->GetSpan(), BoundNodeType::boundConstructionStatement), constructorCall(std::move(constructorCall_))
{
}

void BoundConstructionStatement::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundAssignmentStatement::BoundAssignmentStatement(Module* module_, std::unique_ptr<BoundFunctionCall>&& assignmentCall_) :
    BoundStatement(module_, assignmentCall_->GetSpan(), BoundNodeType::boundAssignmentStatement), assignmentCall(std::move(assignmentCall_))
{
}

void BoundAssignmentStatement::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundExpressionStatement::BoundExpressionStatement(Module* module_, std::unique_ptr<BoundExpression>&& expression_) :
    BoundStatement(module_, expression_->GetSpan(), BoundNodeType::boundExpressionStatement), expression(std::move(expression_))
{ 
}

void BoundExpressionStatement::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundEmptyStatement::BoundEmptyStatement(Module* module_, const Span& span_) : BoundStatement(module_, span_, BoundNodeType::boundEmptyStatement)
{
}

void BoundEmptyStatement::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundSetVmtPtrStatement::BoundSetVmtPtrStatement(Module* module_, std::unique_ptr<BoundExpression>&& classPtr_, ClassTypeSymbol* classType_) :
    BoundStatement(module_, classPtr_->GetSpan(), BoundNodeType::boundSetVmtPtrStatement), classPtr(std::move(classPtr_)), classType(classType_)
{
}

void BoundSetVmtPtrStatement::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundThrowStatement::BoundThrowStatement(Module* module_, const Span& span_, std::unique_ptr<BoundExpression>&& throwCallExpr_) : 
    BoundStatement(module_, span_, BoundNodeType::boundThrowStatement), throwCallExpr(std::move(throwCallExpr_))
{
}

void BoundThrowStatement::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundRethrowStatement::BoundRethrowStatement(Module* module_, const Span& span_, std::unique_ptr<BoundExpression>&& releaseCall_) :
    BoundStatement(module_, span_, BoundNodeType::boundRethrowStatement), releaseCall(std::move(releaseCall_))
{
}

void BoundRethrowStatement::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundTryStatement::BoundTryStatement(Module* module_, const Span& span_) : BoundStatement(module_, span_, BoundNodeType::boundTryStatement)
{
}

void BoundTryStatement::SetTryBlock(std::unique_ptr<BoundStatement>&& tryBlock_)
{
    tryBlock = std::move(tryBlock_);
    tryBlock->SetParent(this);
}

void BoundTryStatement::AddCatch(std::unique_ptr<BoundCatchStatement>&& catchStatement)
{
    catchStatement->SetParent(this);
    catches.push_back(std::move(catchStatement));
}

void BoundTryStatement::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundCatchStatement::BoundCatchStatement(Module* module_, const Span& span_) : 
    BoundStatement(module_, span_, BoundNodeType::boundCatchStatement), catchedType(nullptr), catchTypeUuidId(-1)
{
}

void BoundCatchStatement::SetCatchBlock(std::unique_ptr<BoundStatement>&& catchBlock_)
{
    catchBlock = std::move(catchBlock_);
    catchBlock->SetParent(this);
}

void BoundCatchStatement::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

} } // namespace cmajor::binder
