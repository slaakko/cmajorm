// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_EVALUATOR_INCLUDED
#define CMAJOR_BINDER_EVALUATOR_INCLUDED
#include <cmajor/symbols/Value.hpp>
#include <cmajor/symbols/Scope.hpp>
#include <cmajor/ast/Node.hpp>

namespace cmajor { namespace binder {

class BoundCompileUnit;
class BoundFunction;

using namespace cmajor::symbols;
using namespace cmajor::ast;

std::unique_ptr<Value> Evaluate(Node* node, TypeSymbol* targetType, ContainerScope* containerScope, BoundCompileUnit& boundCompileUnit, bool dontThrow, BoundFunction* currentFunction, const Span& span);

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_EVALUATOR_INCLUDED
