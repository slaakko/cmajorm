// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_BOUND_FUNCTION_INCLUDED
#define CMAJOR_BINDER_BOUND_FUNCTION_INCLUDED
#include <cmajor/binder/BoundExpression.hpp>
#include <cmajor/binder/BoundStatement.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::symbols;

class BINDER_API BoundFunction : public BoundNode
{
public:
    BoundFunction(Module* module_, FunctionSymbol* functionSymbol_);
    BoundFunction(const BoundFunction&) = delete;
    BoundFunction& operator=(const BoundFunction&) = delete;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    const FunctionSymbol* GetFunctionSymbol() const { return functionSymbol; }
    FunctionSymbol* GetFunctionSymbol() { return functionSymbol; }
    void SetBody(std::unique_ptr<BoundCompoundStatement>&& body_);
    BoundCompoundStatement* Body() const { return body.get(); }
    void SetHasGotos() { hasGotos = true; }
    bool HasGotos() const { return hasGotos; }
    void AddTemporaryDestructorCall(std::unique_ptr<BoundFunctionCall>&& destructorCall);
    void MoveTemporaryDestructorCallsTo(BoundExpression& expression);
    void AddLabeledStatement(BoundStatement* labeledStatement);
    const std::vector<BoundStatement*>& LabeledStatements() const { return labeledStatements; }
private:
    FunctionSymbol* functionSymbol;
    std::unique_ptr<BoundCompoundStatement> body;
    bool hasGotos;
    std::vector<std::unique_ptr<BoundFunctionCall>> temporaryDestructorCalls;
    std::vector<BoundStatement*> labeledStatements;
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_BOUND_FUNCTION_INCLUDED
