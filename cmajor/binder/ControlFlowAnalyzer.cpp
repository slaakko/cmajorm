// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/ControlFlowAnalyzer.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/binder/BoundClass.hpp>
#include <cmajor/binder/BoundFunction.hpp>
#include <cmajor/binder/BoundStatement.hpp>
#include <cmajor/binder/BoundNodeVisitor.hpp>
#include <cmajor/symbols/Module.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace binder {

using namespace soulng::unicode;

class ControlFlowAnalyzer : public BoundNodeVisitor
{
public:
    ControlFlowAnalyzer();
    void Visit(BoundCompileUnit& boundCompileUnit) override;
    void Visit(BoundClass& boundClass) override;
    void Visit(BoundFunction& boundFunction) override;
    void Visit(BoundSequenceStatement& boundSequenceStatement) override;
    void Visit(BoundCompoundStatement& boundCompoundStatement) override;
    void Visit(BoundReturnStatement& boundReturnStatement) override;
    void Visit(BoundIfStatement& boundIfStatement) override;
    void Visit(BoundWhileStatement& boundWhileStatement) override;
    void Visit(BoundDoStatement& boundDoStatement) override;
    void Visit(BoundForStatement& boundForStatement) override;
    void Visit(BoundSwitchStatement& boundSwitchStatement) override;
    void Visit(BoundCaseStatement& boundCaseStatement) override;
    void Visit(BoundDefaultStatement& boundDefaultStatement) override;
    void Visit(BoundGotoCaseStatement& boundGotoCaseStatement) override;
    void Visit(BoundGotoDefaultStatement& boundGotoDefaultStatement) override;
    void Visit(BoundBreakStatement& boundBreakStatement) override;
    void Visit(BoundContinueStatement& boundContinueStatement) override;
    void Visit(BoundGotoStatement& boundGotoStatement) override;
    void Visit(BoundConstructionStatement& boundConstructionStatement) override;
    void Visit(BoundAssignmentStatement& boundAssignmentStatement) override;
    void Visit(BoundExpressionStatement& boundExpressionStatement) override;
    void Visit(BoundEmptyStatement& boundEmptyStatement) override;
    void Visit(BoundSetVmtPtrStatement& boundSetVmtPtrStatement) override;
    void Visit(BoundThrowStatement& boundThrowStatement) override;
    void Visit(BoundRethrowStatement& boundRethrowStatement) override;
    void Visit(BoundTryStatement& boundTryStatement) override;
    void Visit(BoundCatchStatement& boundCatchStatement) override;
private:
    Module* module;
    BoundFunction* currentFunction;
    bool collectLabels;
    bool resolveGotos;
    std::unordered_map<std::u32string, BoundStatement*> labelStatementMap;
    void CollectLabel(BoundStatement& statement);
    void ResolveGoto(BoundGotoStatement& boundGotoStatement);
};

ControlFlowAnalyzer::ControlFlowAnalyzer() : module(nullptr), currentFunction(nullptr), collectLabels(false), resolveGotos(false)
{
}

void ControlFlowAnalyzer::CollectLabel(BoundStatement& statement)
{
    if (!statement.Label().empty())
    {
        currentFunction->AddLabeledStatement(&statement);
        auto it = labelStatementMap.find(statement.Label());
        if (it == labelStatementMap.cend())
        {
            labelStatementMap[statement.Label()] = &statement;
        }
        else
        {
            throw Exception(module, "duplicate label '" + ToUtf8(statement.Label()) + "'", statement.GetSpan(), it->second->GetSpan());
        }
    }
}

void ControlFlowAnalyzer::ResolveGoto(BoundGotoStatement& boundGotoStatement)
{
    const std::u32string& target = boundGotoStatement.Target();
    auto it = labelStatementMap.find(target);
    if (it != labelStatementMap.cend())
    {
        BoundStatement* targetStatement = it->second;
        BoundCompoundStatement* targetBlock = targetStatement->Block();
        Assert(targetBlock, "target block not found");
        boundGotoStatement.SetTargetStatement(targetStatement);
        boundGotoStatement.SetTargetBlock(targetBlock);
        BoundCompoundStatement* gotoBlock = boundGotoStatement.Block();
        Assert(gotoBlock, "goto block not found");
        while (gotoBlock && gotoBlock != targetBlock)
        {
            if (gotoBlock->Parent())
            {
                gotoBlock = gotoBlock->Parent()->Block();
            }
            else
            {
                gotoBlock = nullptr;
            }
        }
        if (!gotoBlock)
        {
            throw Exception(module, "goto target '" + ToUtf8(target) + "' not in enclosing block", boundGotoStatement.GetSpan(), targetStatement->GetSpan());
        }
    }
    else
    {
        throw Exception(module, "goto target '" + ToUtf8(target) + "' not found", boundGotoStatement.GetSpan());
    }
}

void ControlFlowAnalyzer::Visit(BoundCompileUnit& boundCompileUnit)
{
    module = &boundCompileUnit.GetModule();
    int n = boundCompileUnit.BoundNodes().size();
    for (int i = 0; i < n; ++i)
    {
        BoundNode* boundNode = boundCompileUnit.BoundNodes()[i].get();
        boundNode->Accept(*this);
    }
}

void ControlFlowAnalyzer::Visit(BoundClass& boundClass)
{
    int n = boundClass.Members().size();
    for (int i = 0; i < n; ++i)
    {
        BoundNode* boundNode = boundClass.Members()[i].get();
        boundNode->Accept(*this);
    }
}

void ControlFlowAnalyzer::Visit(BoundFunction& boundFunction)
{
    if (!boundFunction.HasGotos()) return;
    BoundFunction* prevFunction = currentFunction;
    currentFunction = &boundFunction;
    bool prevCollectLabels = collectLabels;
    collectLabels = true;
    boundFunction.Body()->Accept(*this);
    collectLabels = prevCollectLabels;
    bool prevResolveGotos = resolveGotos;
    resolveGotos = true;
    boundFunction.Body()->Accept(*this);
    resolveGotos = prevResolveGotos;
    currentFunction = prevFunction;
}

void ControlFlowAnalyzer::Visit(BoundSequenceStatement& boundSequenceStatement)
{
    if (collectLabels)
    {
        CollectLabel(boundSequenceStatement);
    }
    boundSequenceStatement.First()->Accept(*this);
    boundSequenceStatement.Second()->Accept(*this);
}

void ControlFlowAnalyzer::Visit(BoundCompoundStatement& boundCompoundStatement)
{
    if (collectLabels)
    {
        CollectLabel(boundCompoundStatement);
    }
    int n = boundCompoundStatement.Statements().size();
    for (int i = 0; i < n; ++i)
    {
        BoundStatement* statement = boundCompoundStatement.Statements()[i].get();
        statement->Accept(*this);
    }
}

void ControlFlowAnalyzer::Visit(BoundReturnStatement& boundReturnStatement)
{
    if (collectLabels)
    {
        CollectLabel(boundReturnStatement);
    }
}

void ControlFlowAnalyzer::Visit(BoundIfStatement& boundIfStatement)
{
    if (collectLabels)
    {
        CollectLabel(boundIfStatement);
    }
    boundIfStatement.ThenS()->Accept(*this);
    if (boundIfStatement.ElseS())
    {
        boundIfStatement.ElseS()->Accept(*this);
    }
}

void ControlFlowAnalyzer::Visit(BoundWhileStatement& boundWhileStatement)
{
    if (collectLabels)
    {
        CollectLabel(boundWhileStatement);
    }
    boundWhileStatement.Statement()->Accept(*this);
}

void ControlFlowAnalyzer::Visit(BoundDoStatement& boundDoStatement)
{
    if (collectLabels)
    {
        CollectLabel(boundDoStatement);
    }
    boundDoStatement.Statement()->Accept(*this);

}

void ControlFlowAnalyzer::Visit(BoundForStatement& boundForStatement)
{
    if (collectLabels)
    {
        CollectLabel(boundForStatement);
    }
    boundForStatement.InitS()->Accept(*this);
    boundForStatement.LoopS()->Accept(*this);
    boundForStatement.ActionS()->Accept(*this);
}

void ControlFlowAnalyzer::Visit(BoundSwitchStatement& boundSwitchStatement)
{
    if (collectLabels)
    {
        CollectLabel(boundSwitchStatement);
    }
    int n = boundSwitchStatement.CaseStatements().size();
    for (int i = 0; i < n; ++i)
    {
        BoundCaseStatement* caseStatement = boundSwitchStatement.CaseStatements()[i].get();
        caseStatement->Accept(*this);
    }
    if (boundSwitchStatement.DefaultStatement())
    {
        boundSwitchStatement.DefaultStatement()->Accept(*this);
    }
}

void ControlFlowAnalyzer::Visit(BoundCaseStatement& boundCaseStatement)
{
    if (collectLabels)
    {
        CollectLabel(boundCaseStatement);
    }
    if (boundCaseStatement.CompoundStatement())
    {
        boundCaseStatement.CompoundStatement()->Accept(*this);
    }
}

void ControlFlowAnalyzer::Visit(BoundDefaultStatement& boundDefaultStatement)
{
    if (collectLabels)
    {
        CollectLabel(boundDefaultStatement);
    }
    if (boundDefaultStatement.CompoundStatement())
    {
        boundDefaultStatement.CompoundStatement()->Accept(*this);
    }
}

void ControlFlowAnalyzer::Visit(BoundGotoCaseStatement& boundGotoCaseStatement)
{
    if (collectLabels)
    {
        CollectLabel(boundGotoCaseStatement);
    }
}

void ControlFlowAnalyzer::Visit(BoundGotoDefaultStatement& boundGotoDefaultStatement)
{
    if (collectLabels)
    {
        CollectLabel(boundGotoDefaultStatement);
    }
}

void ControlFlowAnalyzer::Visit(BoundBreakStatement& boundBreakStatement)
{
    if (collectLabels)
    {
        CollectLabel(boundBreakStatement);
    }
}

void ControlFlowAnalyzer::Visit(BoundContinueStatement& boundContinueStatement)
{
    if (collectLabels)
    {
        CollectLabel(boundContinueStatement);
    }
}

void ControlFlowAnalyzer::Visit(BoundGotoStatement& boundGotoStatement)
{
    if (collectLabels)
    {
        CollectLabel(boundGotoStatement);
    }
    if (resolveGotos)
    {
        ResolveGoto(boundGotoStatement);
    }
}

void ControlFlowAnalyzer::Visit(BoundConstructionStatement& boundConstructionStatement)
{
    if (collectLabels)
    {
        CollectLabel(boundConstructionStatement);
    }
}

void ControlFlowAnalyzer::Visit(BoundAssignmentStatement& boundAssignmentStatement)
{
    if (collectLabels)
    {
        CollectLabel(boundAssignmentStatement);
    }
}

void ControlFlowAnalyzer::Visit(BoundExpressionStatement& boundExpressionStatement)
{
    if (collectLabels)
    {
        CollectLabel(boundExpressionStatement);
    }
}

void ControlFlowAnalyzer::Visit(BoundEmptyStatement& boundEmptyStatement)
{
    if (collectLabels)
    {
        CollectLabel(boundEmptyStatement);
    }
}

void ControlFlowAnalyzer::Visit(BoundSetVmtPtrStatement& boundSetVmtPtrStatement)
{
    if (collectLabels)
    {
        CollectLabel(boundSetVmtPtrStatement);
    }
}

void ControlFlowAnalyzer::Visit(BoundThrowStatement& boundThrowStatement)
{
    if (collectLabels)
    {
        CollectLabel(boundThrowStatement);
    }
}

void ControlFlowAnalyzer::Visit(BoundRethrowStatement& boundRethrowStatement)
{
    if (collectLabels)
    {
        CollectLabel(boundRethrowStatement);
    }
}

void ControlFlowAnalyzer::Visit(BoundTryStatement& boundTryStatement)
{
    if (collectLabels)
    {
        CollectLabel(boundTryStatement);
    }
    boundTryStatement.TryBlock()->Accept(*this);
}

void ControlFlowAnalyzer::Visit(BoundCatchStatement& boundCatchStatement) 
{
    if (collectLabels)
    {
        CollectLabel(boundCatchStatement);
    }
    boundCatchStatement.CatchBlock()->Accept(*this);
}

void AnalyzeControlFlow(BoundCompileUnit& boundCompileUUnit)
{
    ControlFlowAnalyzer controlFlowAnalyzer;
    boundCompileUUnit.Accept(controlFlowAnalyzer);
}

} } // namespace cmajor::binder
