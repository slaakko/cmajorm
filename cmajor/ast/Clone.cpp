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

void CloneContext::AddUnitTestFunction(std::unique_ptr<FunctionNode>&& unitTestFunction)
{
    unitTestFunctions.push_back(std::move(unitTestFunction));
}

std::vector<std::unique_ptr<FunctionNode>>& CloneContext::UnitTestFunctions()
{
    return unitTestFunctions;
}

} } // namespace cmajor::ast
