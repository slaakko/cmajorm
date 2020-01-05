// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_MERGE_INCLUDED
#define SNGCM_AST_MERGE_INCLUDED
#include <sngcm/ast/AstApi.hpp>

namespace sngcm { namespace ast {

class CompileUnitNode;

SNGCM_AST_API void Merge(CompileUnitNode& source, CompileUnitNode& target);

} } // namespace sngcm::ast

#endif // SNGCM_AST_MERGE_INCLUDED
