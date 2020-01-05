// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/BoundFunction.hpp>
#include <cmajor/binder/BoundStatement.hpp>
#include <cmajor/binder/BoundNodeVisitor.hpp>
#include <cmajor/symbols/Exception.hpp>

namespace cmajor { namespace binder {

BoundFunction::BoundFunction(Module* module_, FunctionSymbol* functionSymbol_) : 
    BoundNode(module_, functionSymbol_->GetSpan(), BoundNodeType::boundFunction), functionSymbol(functionSymbol_), hasGotos(false)
{
}

void BoundFunction::Load(Emitter& emitter, OperationFlags flags)
{
    throw Exception(GetModule(), "cannot load from function", GetSpan());
}

void BoundFunction::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception(GetModule(), "cannot store to function", GetSpan());
}

void BoundFunction::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

void BoundFunction::SetBody(std::unique_ptr<BoundCompoundStatement>&& body_)
{
    body = std::move(body_);
}

void BoundFunction::AddTemporaryDestructorCall(std::unique_ptr<BoundFunctionCall>&& destructorCall)
{
    temporaryDestructorCalls.push_back(std::move(destructorCall));
}

void BoundFunction::MoveTemporaryDestructorCallsTo(BoundExpression& expression)
{
    for (std::unique_ptr<BoundFunctionCall>& destructorCall : temporaryDestructorCalls)
    {
        expression.AddTemporaryDestructorCall(std::move(destructorCall));
    }
    temporaryDestructorCalls.clear();
}

void BoundFunction::AddLabeledStatement(BoundStatement* labeledStatement)
{
    labeledStatements.push_back(labeledStatement);
}

} } // namespace cmajor::binder
