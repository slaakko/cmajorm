// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/ast/Clone.hpp>
#include <cmajor/ast/Function.hpp>

namespace cmajor { namespace ast {

CloneContext::CloneContext() : instantiateFunctionNode(false), instantiateClassNode(false), makeTestUnits(false)
{
}

CloneContext::~CloneContext()
{
    for (FunctionNode* unitTestFunction : unitTestFunctions)
    {
        delete unitTestFunction;
    }
}

void CloneContext::AddUnitTestFunction(FunctionNode* unitTestFunction)
{
    unitTestFunctions.push_back(unitTestFunction);
}

std::vector<FunctionNode*>& CloneContext::UnitTestFunctions()
{
    return unitTestFunctions;
}

} } // namespace cmajor::ast
