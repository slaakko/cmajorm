// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_DEBUG_INCLUDED
#define CMCODE_DEBUG_INCLUDED
#include <cmajor/cmmsg/CmdbMessage.hpp>
#include <sngcm/ast/Project.hpp>

namespace cmcode {

void StartDebugService(int pid, const std::string& backend, const std::string& config, sngcm::ast::Project* project, const std::string& programArguments, const std::vector<SourceLoc>& breakpoints);

} // namespace cmcode

#endif // CMCODE_DEBUG_INCLUDED