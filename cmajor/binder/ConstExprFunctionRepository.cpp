// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/ConstExprFunctionRepository.hpp>
#include <cmajor/binder/TypeBinder.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>

namespace cmajor { namespace binder {

ConstExprFunctionRepository::ConstExprFunctionRepository(BoundCompileUnit& boundCompileUnit_) : boundCompileUnit(boundCompileUnit_)
{
}

FunctionNode* ConstExprFunctionRepository::GetFunctionNodeFor(FunctionSymbol* constExprFunctionSymbol)
{
    Node* node = boundCompileUnit.GetSymbolTable().GetNodeNoThrow(constExprFunctionSymbol);
    if (!node)
    {
        node = constExprFunctionSymbol->GetFunctionNode();
    }
    FunctionNode* functionNode = static_cast<FunctionNode*>(node);
    boundCompileUnit.GetSymbolTable().MapNode(node, constExprFunctionSymbol);
    if (constExprFunctionSymbol->IsProject() && !constExprFunctionSymbol->IsBound())
    {
        TypeBinder typeBinder(boundCompileUnit);
        functionNode->Accept(typeBinder);
    }
    return functionNode;
}

} } // namespace cmajor::binder
