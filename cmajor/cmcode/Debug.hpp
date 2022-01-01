// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_DEBUG_INCLUDED
#define CMCODE_DEBUG_INCLUDED
#include <cmajor/cmsvc/Breakpoint.hpp>
#include <cmajor/cmmsg/CmdbMessage.hpp>
#include <sngcm/ast/Project.hpp>

namespace cmcode {

using namespace cmajor::service;

void StartDebugService(int pid, const std::string& backend, const std::string& config, sngcm::ast::Project* project, const std::string& programArguments, const std::vector<Breakpoint*>& breakpoints);

} // namespace cmcode

#endif // CMCODE_DEBUG_INCLUDED
