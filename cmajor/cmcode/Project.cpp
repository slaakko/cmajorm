// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/Project.hpp>
#include <cmajor/cmcode/ProjectSettings.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/Parser.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

namespace cmcode {

using namespace soulng::util;

ProjectData::ProjectData(sngcm::ast::Project* project_) : changed(false), project(project_), breakpointCollection(), programArguments()
{
}

void ProjectData::Load(const std::string& projectSettingsFilePath)
{
    if (boost::filesystem::exists(projectSettingsFilePath))
    {
        std::unique_ptr<sngxml::dom::Document> projectSettingsDoc = sngxml::dom::ReadDocument(projectSettingsFilePath);
        ProjectSettings projectSettings;
        projectSettings.FromXml(projectSettingsDoc->DocumentElement());
        programArguments = projectSettings.programArguments;
        for (const ProjectBreakpoint& breakpoint : projectSettings.breakpoints)
        {
            BreakpointList& breakpointList = breakpointCollection.GetBreakpointList(breakpoint.file);
            breakpointList.AddBreakpoint(new Breakpoint(breakpoint.line, breakpoint.condition, breakpoint.disabled));
        }
        breakpointCollection.ResetChanged();
    }
    changed = false;
}

void ProjectData::Save(const std::string& projectSettingsFilePath)
{
    if (!Changed()) return;
    ProjectSettings projectSettings;
    projectSettings.programArguments = programArguments;
    for (auto& bm : breakpointCollection.BreakpointListMap())
    {
        BreakpointList& breakpointList = bm.second;
        for (Breakpoint* breakpoint : breakpointList.Breakpoints())
        {
            ProjectBreakpoint projectBreakpoint;
            projectBreakpoint.file = breakpointList.FilePath();
            projectBreakpoint.line = breakpoint->line;
            projectBreakpoint.condition = breakpoint->condition;
            projectBreakpoint.disabled = breakpoint->disabled;
            projectSettings.breakpoints.push_back(projectBreakpoint);
        }
    }
    sngxml::dom::Document projectSettingsDoc;
    std::unique_ptr<sngxml::dom::Element> projectSettingsElement = projectSettings.ToXml("projectSettings");
    projectSettingsDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(projectSettingsElement.release()));
    std::ofstream projectSettingsFile(projectSettingsFilePath);
    CodeFormatter formatter(projectSettingsFile);
    projectSettingsDoc.Write(formatter);
    breakpointCollection.ResetChanged();
    changed = false;
}

bool ProjectData::Changed() 
{
    if (breakpointCollection.Changed()) changed = true;
    return changed;
}

void ProjectData::SetProgramArguments(const std::string& programArguments_)
{
    if (programArguments != programArguments_)
    {
        programArguments = programArguments_;
        changed = true;
    }
}

} // namespace cmcode
