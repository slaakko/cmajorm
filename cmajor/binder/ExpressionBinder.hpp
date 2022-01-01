// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_EXPRESSION_BINDER_INCLUDED
#define CMAJOR_BINDER_EXPRESSION_BINDER_INCLUDED
#include <sngcm/ast/Node.hpp>
#include <cmajor/symbols/Scope.hpp>

namespace cmajor { namespace binder {

using sngcm::ast::Node;
using cmajor::symbols::ContainerScope;

class BoundCompileUnit;
class BoundFunction;
class BoundExpression;
class StatementBinder;

std::unique_ptr<BoundExpression> BindExpression(Node* node, BoundCompileUnit& boundCompileUnit, BoundFunction* boundFunction, ContainerScope* containerScope, StatementBinder* statementBinder);
std::unique_ptr<BoundExpression> BindExpression(Node* node, BoundCompileUnit& boundCompileUnit, BoundFunction* boundFunction, ContainerScope* containerScope, StatementBinder* statementBinder, 
    bool lvalue);
std::unique_ptr<BoundExpression> BindExpression(Node* node, BoundCompileUnit& boundCompileUnit, BoundFunction* boundFunction, ContainerScope* containerScope, StatementBinder* statementBinder, 
    bool lvalue, bool acceptFunctionGroupOrMemberExpression);
std::unique_ptr<BoundExpression> BindExpression(Node* node, BoundCompileUnit& boundCompileUnit, BoundFunction* boundFunction, ContainerScope* containerScope, StatementBinder* statementBinder, 
    bool lvalue, bool acceptFunctionGroupOrMemberExpression, bool acceptIncomplete);
std::unique_ptr<BoundExpression> BindExpression(Node* node, BoundCompileUnit& boundCompileUnit, BoundFunction* boundFunction, ContainerScope* containerScope, StatementBinder* statementBinder,
    bool lvalue, bool acceptFunctionGroupOrMemberExpression, bool acceptIncomplete, bool moveTemporaryDestructorCalls);

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_EXPRESSION_BINDER_INCLUDED
