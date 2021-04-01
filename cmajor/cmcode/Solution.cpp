// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/Solution.hpp>
#include <cmajor/cmcode/SolutionSettings.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngcm/cmlexer/ContainerFileLexer.hpp>
#include <sngcm/cmparser/ProjectFile.hpp>
#include <sngcm/cmparser/SolutionFile.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Sha1.hpp>
#include <soulng/util/Unicode.hpp>
#include <algorithm>

namespace cmcode {

using namespace soulng::util;
using namespace soulng::unicode;

std::unique_ptr<sngcm::ast::Project> ReadProject(const std::string& projectFilePath, std::vector<std::unique_ptr<std::u32string>>& contents)
{
    std::unique_ptr<std::u32string> content(new std::u32string(ToUtf32(ReadFile(projectFilePath))));
    ContainerFileLexer lexer(content->c_str(), content->c_str() + content->length(), projectFilePath, 0);
    std::unique_ptr<sngcm::ast::Project> project = ProjectFileParser::Parse(lexer, "debug", sngcm::ast::BackEnd::llvm, "gcc", sngcm::ast::SystemDirKind::regular);
    project->ResolveDeclarations();
    contents.push_back(std::move(content));
    return project;
}

std::unique_ptr<sngcm::ast::Solution> ReadSolution(const std::string& solutionFilePath, std::vector<std::unique_ptr<std::u32string>>& contents)
{
    std::unique_ptr<std::u32string> content(new std::u32string(ToUtf32(ReadFile(solutionFilePath))));
    ContainerFileLexer lexer(content->c_str(), content->c_str() + content->length(), solutionFilePath, 0);
    std::unique_ptr<sngcm::ast::Solution> solution = SolutionFileParser::Parse(lexer);
    solution->ResolveDeclarations();
    for (const std::string& projectFilePath : solution->ProjectFilePaths())
    {
        std::unique_ptr<sngcm::ast::Project> project = ReadProject(projectFilePath, contents);
        solution->AddProject(std::move(project));
    }
    contents.push_back(std::move(content));
    return solution;
}

SolutionTreeViewNodeData::SolutionTreeViewNodeData(SolutionTreeViewNodeDataKind kind_, sngcm::ast::Solution* solution_, sngcm::ast::Project* project_, 
    const std::string& filePath_, const std::string& fileName_) :
    kind(kind_), solution(solution_), project(project_), filePath(filePath_), fileName(fileName_)
{
    switch (kind)
    {
        case SolutionTreeViewNodeDataKind::solution: 
        {
            key = GetSha1MessageDigest("solution." + ToUtf8(solution->Name()));
            break;
        }
        case SolutionTreeViewNodeDataKind::project:
        {
            key = GetSha1MessageDigest("project." + ToUtf8(project->Name()));
            break;
        }
        case SolutionTreeViewNodeDataKind::file:
        {
            key = filePath;
            break;
        }
    }
}

struct ByFileName
{
    bool operator()(const std::unique_ptr<SolutionTreeViewNodeData>& left, const std::unique_ptr<SolutionTreeViewNodeData>& right) const
    {
        return left->fileName < right->fileName;
    }
};

SolutionData::SolutionData(std::unique_ptr<sngcm::ast::Solution>&& solution_, TreeView* solutionTreeView_) : 
    solution(std::move(solution_)), solutionTreeView(solutionTreeView_), treeViewData(), treeViewDataMap(), activeProjectNode(nullptr)
{
    solution->SortByProjectName();
    SetActiveProject();
    std::unique_ptr<TreeViewNode> solutionNode(new TreeViewNode(ToUtf8(solution->Name())));
    std::unique_ptr<SolutionTreeViewNodeData> solutionData(new SolutionTreeViewNodeData(SolutionTreeViewNodeDataKind::solution, solution.get(), nullptr, std::string(), std::string()));
    solutionNode->SetData(solutionData.get());
    treeViewData.push_back(std::move(solutionData));
    std::string solutionSettingsFilePath = solution->FilePath();
    solutionSettingsFilePath.append(".settings.xml");
    Load(solutionSettingsFilePath);
    for (const auto& project : solution->Projects())
    {
        std::string projectName = ToUtf8(project->Name());
        std::unique_ptr<ProjectData> projectData(new ProjectData(project.get()));
        std::string projectSettingsFilePath = project->FilePath();
        projectSettingsFilePath.append(".settings.xml");
        projectData->Load(projectSettingsFilePath);
        projectDataMap[project.get()] = projectData.get();
        projectDataVec.push_back(std::move(projectData));
        std::unique_ptr<TreeViewNode> projectNode(new TreeViewNode(projectName));
        std::unique_ptr<SolutionTreeViewNodeData> projectTreeViewData(new SolutionTreeViewNodeData(SolutionTreeViewNodeDataKind::project, nullptr, project.get(), std::string(), std::string()));
        projectNode->SetData(projectTreeViewData.get());
        if (project.get() == solution->ActiveProject())
        {
            activeProjectNode = projectNode.get();
            projectNode->SetActive();
        }
        treeViewData.push_back(std::move(projectTreeViewData));
        std::vector<std::unique_ptr<SolutionTreeViewNodeData>> fileData;
        for (const std::string& sourceFilePath : project->SourceFilePaths())
        {
            std::unique_ptr<SolutionTreeViewNodeData> file(new SolutionTreeViewNodeData(SolutionTreeViewNodeDataKind::file, solution.get(), project.get(), sourceFilePath,
                Path::GetFileName(sourceFilePath)));
            fileData.push_back(std::move(file));
        }
        for (const std::string& resourceFilePath : project->ResourceFilePaths())
        {
            std::unique_ptr<SolutionTreeViewNodeData> file(new SolutionTreeViewNodeData(SolutionTreeViewNodeDataKind::file, solution.get(), project.get(), resourceFilePath,
                Path::GetFileName(resourceFilePath)));
            fileData.push_back(std::move(file));
        }
        for (const std::string& textFilePath : project->TextFilePaths())
        {
            std::unique_ptr<SolutionTreeViewNodeData> file(new SolutionTreeViewNodeData(SolutionTreeViewNodeDataKind::file, solution.get(), project.get(), textFilePath,
                Path::GetFileName(textFilePath)));
            fileData.push_back(std::move(file));
        }
        std::sort(fileData.begin(), fileData.end(), ByFileName());
        for (auto& file : fileData)
        {
            std::unique_ptr<TreeViewNode> fileNode(new TreeViewNode(file->fileName));
            fileNode->SetData(file.get());
            projectNode->AddChild(fileNode.release());
        }
        for (auto& file : fileData)
        {
            treeViewData.push_back(std::move(file));
        }
        solutionNode->AddChild(projectNode.release());
    }
    solutionNode->Expand();
    for (const auto& nodeData : treeViewData)
    {
        treeViewDataMap[nodeData->key] = nodeData.get();
    }
    solutionTreeView->SetRoot(solutionNode.release());
}

void SolutionData::SetActiveProject()
{
    if (solution->ActiveProjectName().empty())
    {
        if (!solution->Projects().empty())
        {
            solution->SetActiveProject(solution->Projects()[0].get());
        }
    }
    else
    {
        for (const auto& project : solution->Projects())
        {
            if (project->Name() == solution->ActiveProjectName())
            {
                solution->SetActiveProject(project.get());
                break;
            }
        }
    }
    if (solution->ActiveProject() == nullptr)
    {
        if (!solution->Projects().empty())
        {
            solution->SetActiveProject(solution->Projects()[0].get());
        }
    }
}

SolutionTreeViewNodeData* SolutionData::GetSolutionTreeViewNodeDataByKey(const std::string& key) const
{
    auto it = treeViewDataMap.find(key);
    if (it != treeViewDataMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

ProjectData* SolutionData::GetProjectDataByProject(sngcm::ast::Project* project) const
{
    auto it = projectDataMap.find(project);
    if (it != projectDataMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void SolutionData::AddTreeViewNodeData(SolutionTreeViewNodeData* data)
{
    treeViewDataMap[data->key] = data;
    treeViewData.push_back(std::unique_ptr<SolutionTreeViewNodeData>(data));
}

std::vector<Breakpoint*> SolutionData::GetBreakpoints() 
{
    std::vector<Breakpoint*> breakpoints;
    for (auto& bm : solutionBreakpointCollection.BreakpointListMap())
    {
        BreakpointList& list = bm.second;
        for (Breakpoint* breakpoint : list.Breakpoints())
        {
            breakpoints.push_back(breakpoint);
        }
    }
    for (auto& project : projectDataVec)
    {
        BreakpointCollection& breakpointCollection = project->GetBreakpointCollection();
        for (auto& bm : breakpointCollection.BreakpointListMap())
        {
            BreakpointList& list = bm.second;
            for (Breakpoint* breakpoint : list.Breakpoints())
            {
                breakpoints.push_back(breakpoint);
            }
        }
    }
    return breakpoints;
}

void SolutionData::Load(const std::string& solutionSettingsFilePath)
{
    if (boost::filesystem::exists(solutionSettingsFilePath))
    {
        std::unique_ptr<sngxml::dom::Document> solutionSettingsDoc = sngxml::dom::ReadDocument(solutionSettingsFilePath); 
        SolutionSettings solutionSettings(solutionSettingsDoc->DocumentElement());
        for (const SolutionBreakpoint& breakpoint : solutionSettings.breakpoints)
        {
            BreakpointList& breakpointList = solutionBreakpointCollection.GetBreakpointList(breakpoint.file);
            breakpointList.AddBreakpoint(new Breakpoint(breakpoint.line, breakpoint.condition, breakpoint.disabled));
        }
        solutionBreakpointCollection.ResetChanged();
    }
}

void SolutionData::Save(const std::string& solutionSettingsFilePath)
{
    if (!Changed()) return;
    SolutionSettings solutionSettings;
    for (auto& bm : solutionBreakpointCollection.BreakpointListMap())
    {
        BreakpointList& breakpointList = bm.second;
        for (Breakpoint* breakpoint : breakpointList.Breakpoints())
        {
            SolutionBreakpoint solutionBreakpoint;
            solutionBreakpoint.file = breakpointList.FilePath();
            solutionBreakpoint.line = breakpoint->line;
            solutionBreakpoint.condition = breakpoint->condition;
            solutionBreakpoint.disabled = breakpoint->disabled;
            solutionSettings.breakpoints.push_back(solutionBreakpoint);
        }
    }
    sngxml::dom::Document solutionSettingsDoc;
    std::unique_ptr<sngxml::dom::Element> solutionSettingsElement = solutionSettings.ToXml("solutionSettings");
    solutionSettingsDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(solutionSettingsElement.release()));
    std::ofstream solutionSettingsFile(solutionSettingsFilePath);
    CodeFormatter formatter(solutionSettingsFile);
    solutionSettingsDoc.Write(formatter);
    solutionBreakpointCollection.ResetChanged();
}

} // namespace cmcode
