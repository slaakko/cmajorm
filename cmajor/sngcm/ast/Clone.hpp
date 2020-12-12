// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_CLONE_INCLUDED
#define SNGCM_AST_CLONE_INCLUDED
#include <sngcm/ast/AstApi.hpp>
#include <soulng/lexer/Span.hpp>
#include <boost/uuid/uuid.hpp>
#include <memory>
#include <vector>

namespace sngcm { namespace ast {

class FunctionNode;

using namespace soulng::lexer;

class SNGCM_AST_API CloneContext
{
public:     
    CloneContext();
    ~CloneContext();
    void SetInstantiateFunctionNode() { instantiateFunctionNode = true; }
    bool InstantiateFunctionNode() const { return instantiateFunctionNode; }
    void SetInstantiateClassNode() { instantiateClassNode = true; }
    bool InstantiateClassNode() const { return instantiateClassNode; }
    void SetMakeTestUnits() { makeTestUnits = true; }
    bool MakeTestUnits() const { return makeTestUnits; }
    void AddUnitTestFunction(FunctionNode* unitTestFunction);
    std::vector<FunctionNode*>& UnitTestFunctions();
private:
    bool instantiateFunctionNode;
    bool instantiateClassNode;
    bool makeTestUnits;
    std::vector<FunctionNode*> unitTestFunctions;
};

} } // namespace sngcm::ast

#endif // SNGCM_AST_CLONE_INCLUDED
