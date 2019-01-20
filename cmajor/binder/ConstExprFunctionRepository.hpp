// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_CONST_EXPR_FUNCTION_REPOSITORY_INCLUDED
#define CMAJOR_BINDER_CONST_EXPR_FUNCTION_REPOSITORY_INCLUDED
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <cmajor/ast/Function.hpp>

namespace cmajor { namespace binder {

class BoundCompileUnit;
using namespace cmajor::ast;
using namespace cmajor::symbols;

class ConstExprFunctionRepository
{
public:
    ConstExprFunctionRepository(BoundCompileUnit& boundCompileUnit_);
    FunctionNode* GetFunctionNodeFor(FunctionSymbol* constExprFunctionSymbol);
private:
    BoundCompileUnit& boundCompileUnit;
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_CONST_EXPR_FUNCTION_REPOSITORY_INCLUDED
