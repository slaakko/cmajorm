// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_AST2DOM_AST2DOM_INCLUDED
#define CMAJOR_AST2DOM_AST2DOM_INCLUDED
#include <cmajor/ast2dom/Ast2DomApi.hpp>
#include <sngcm/ast/Node.hpp>
#include <sngxml/dom/Document.hpp>

namespace cmajor { namespace ast2dom {

using namespace sngcm::ast;
using namespace sngxml::dom;

std::unique_ptr<sngxml::dom::Document> GenerateAstDocument(sngcm::ast::Node* node);

} } // namespace cmajor::ast2dom

#endif // CMAJOR_AST2DOM_AST2DOM_INCLUDED
