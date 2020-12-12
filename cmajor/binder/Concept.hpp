// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_CONCEPT_INCLUDED
#define CMAJOR_BINDER_CONCEPT_INCLUDED
#include <cmajor/symbols/Scope.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/TemplateSymbol.hpp>
#include <cmajor/symbols/ConceptSymbol.hpp>
#include <sngcm/ast/Concept.hpp>

namespace cmajor { namespace binder {

using namespace sngcm::ast;
using namespace cmajor::symbols;

class BoundCompileUnit;
class BoundFunction;
class BoundConstraint;

ConceptSymbol* ResolveConceptId(ConceptIdNode* conceptIdNode, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);

bool CheckConstraint(ConstraintNode* constraint, const NodeList<Node>& usingNodes, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope, BoundFunction* currentFunction,
    const std::vector<TemplateParameterSymbol*>& templateParameters, const std::unordered_map<TemplateParameterSymbol*, TypeSymbol*>& templateParameterMap, 
    std::unique_ptr<BoundConstraint>& boundConstraint, const Span& span, const boost::uuids::uuid& moduleId, FunctionSymbol* viableFunction, std::unique_ptr<Exception>& conceptCheckException);

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_CONCEPT_INCLUDED
