// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_RUN_INCLUDED
#define CMCODE_RUN_INCLUDED
#include <sngcm/ast/Project.hpp>

namespace cmcode {

void RunProgram(const std::string& backend, const std::string& config, sngcm::ast::Project* project, const std::string& programArguments);

} // namespace cmcode

#endif // CMCODE_RUN_INCLUDED
