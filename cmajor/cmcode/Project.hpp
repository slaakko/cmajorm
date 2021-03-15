// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_PROJECT_INCLUDED
#define CMCODE_PROJECT_INCLUDED
#include <cmajor/cmsvc/Breakpoint.hpp>
#include <sngcm/ast/Project.hpp>

namespace cmcode {

using namespace cmajor::service;

class ProjectData
{
public:
    ProjectData(sngcm::ast::Project* project_);
    sngcm::ast::Project* GetProject() const { return project; }
    BreakpointCollection& GetBreakpointCollection() { return breakpointCollection; }
private:
    sngcm::ast::Project* project;
    BreakpointCollection breakpointCollection;
};

} // namespace cmcode

#endif // CMCODE_PROJECT_INCLUDED
