// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/Clone.hpp>
#include <sngcm/ast/Function.hpp>

namespace sngcm { namespace ast {

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

} } // namespace sngcm::ast
