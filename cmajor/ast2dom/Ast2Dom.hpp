// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_AST2DOM_AST2DOM_INCLUDED
#define CMAJOR_AST2DOM_AST2DOM_INCLUDED
#include <cmajor/ast/Node.hpp>
#include <cmajor/dom/Document.hpp>

namespace cmajor { namespace ast2dom {

using namespace cmajor::ast;
using namespace cmajor::dom;

std::unique_ptr<dom::Document> GenerateAstDocument(cmajor::ast::Node* node);

} } // namespace cmajor::ast2dom

#endif // CMAJOR_AST2DOM_AST2DOM_INCLUDED
