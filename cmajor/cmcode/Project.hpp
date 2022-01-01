// =================================
// Copyright (c) 2022 Seppo Laakko
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
    void Load(const std::string& projectSettingsFilePath);
    void Save(const std::string& projectSettingsFilePath);
    bool Changed();
    sngcm::ast::Project* GetProject() const { return project; }
    BreakpointCollection& GetBreakpointCollection() { return breakpointCollection; }
    const std::string& ProgramArguments() const { return programArguments; }
    void SetProgramArguments(const std::string& programArguments_);
private:
    bool changed;
    sngcm::ast::Project* project;
    BreakpointCollection breakpointCollection;
    std::string programArguments;
};

} // namespace cmcode

#endif // CMCODE_PROJECT_INCLUDED
