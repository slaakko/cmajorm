// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_AST_CLONE_INCLUDED
#define CMAJOR_AST_CLONE_INCLUDED
#include <memory>
#include <vector>

namespace cmajor { namespace ast {

class FunctionNode;

class CloneContext
{
public:     
    CloneContext();
    void SetInstantiateFunctionNode() { instantiateFunctionNode = true; }
    bool InstantiateFunctionNode() const { return instantiateFunctionNode; }
    void SetInstantiateClassNode() { instantiateClassNode = true; }
    bool InstantiateClassNode() const { return instantiateClassNode; }
    void SetMakeTestUnits() { makeTestUnits = true; }
    bool MakeTestUnits() const { return makeTestUnits; }
    void AddUnitTestFunction(std::unique_ptr<FunctionNode>&& unitTestFunction);
    std::vector<std::unique_ptr<FunctionNode>>& UnitTestFunctions();
private:
    bool instantiateFunctionNode;
    bool instantiateClassNode;
    bool makeTestUnits;
    std::vector<std::unique_ptr<FunctionNode>> unitTestFunctions;
};

} } // namespace cmajor::ast

#endif // CMAJOR_AST_CLONE_INCLUDED
