// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/BoundFunction.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/binder/BoundStatement.hpp>
#include <cmajor/binder/BoundNodeVisitor.hpp>
#include <cmajor/binder/BoundClass.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>

namespace cmajor { namespace binder {

BoundFunction::BoundFunction(BoundCompileUnit* boundCompileUnit_, FunctionSymbol* functionSymbol_) : 
    BoundNode(&boundCompileUnit_->GetModule(), functionSymbol_->GetSpan(), BoundNodeType::boundFunction), boundCompileUnit(boundCompileUnit_), functionSymbol(functionSymbol_), hasGotos(false)
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

void BoundFunction::AddTemporaryDestructorCall(std::unique_ptr<BoundFunctionCall>&& destructorCall,
    BoundFunction* currentFunction, ContainerScope* currentContainerScope, const Span& span)
{
    FunctionSymbol* functionSymbol = destructorCall->GetFunctionSymbol();
    if (functionSymbol->GetSymbolType() == SymbolType::destructorSymbol)
    {
        DestructorSymbol* destructorSymbol = static_cast<DestructorSymbol*>(functionSymbol);
        if (destructorSymbol->IsGeneratedFunction() && !GetGlobalFlag(GlobalFlags::info))
        {
            if (destructorSymbol->Parent()->IsClassTypeSymbol())
            {
                ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(destructorSymbol->Parent());
                if (!boundCompileUnit->IsGeneratedDestructorInstantiated(destructorSymbol))
                {
                    boundCompileUnit->SetGeneratedDestructorInstantiated(destructorSymbol);
                    std::unique_ptr<BoundClass> boundClass(new BoundClass(&boundCompileUnit->GetModule(), classType));
                    GenerateDestructorImplementation(boundClass.get(), destructorSymbol, *boundCompileUnit, currentContainerScope, currentFunction, span);
                    boundCompileUnit->AddBoundNode(std::move(boundClass));
                }
            }
        }
    }
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

void BoundFunction::SetEnterCode(std::vector<std::unique_ptr<BoundStatement>>&& enterCode_)
{
    enterCode = std::move(enterCode_);
}

void BoundFunction::SetExitCode(std::vector<std::unique_ptr<BoundStatement>>&& exitCode_)
{
    exitCode = std::move(exitCode_);
}

void BoundFunction::SetLineCode(std::unique_ptr<BoundStatement>&& lineCode_)
{
    lineCode.reset(lineCode_.release());
}

} } // namespace cmajor::binder
