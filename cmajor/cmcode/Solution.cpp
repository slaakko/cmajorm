// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/Solution.hpp>
#include <cmajor/cmcode/SolutionSettings.hpp>
#include <cmajor/cmcode/SolutionExplorer.hpp>
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

SolutionData::SolutionData(std::unique_ptr<sngcm::ast::Solution>&& solution_, SolutionExplorer* solutionExplorer_) : 
    solution(std::move(solution_)), solutionExplorer(solutionExplorer_), treeViewData(), treeViewDataMap(), activeProjectNode(nullptr),
    callStackOpen(false), localsViewOpen(false), currentCursorLine(0)
{
    solution->SortByProjectName();
    SetActiveProject();
    std::unique_ptr<TreeViewNode> solutionNode(new TreeViewNode(ToUtf8(solution->Name())));
    std::unique_ptr<SolutionTreeViewNodeData> solutionData(new SolutionTreeViewNodeData(SolutionTreeViewNodeDataKind::solution, solution.get(), nullptr, std::string(), std::string()));
    solutionNode->SetData(solutionData.get());
    solutionNode->SetImageIndex(solutionExplorer->GetImageList()->GetImageIndex("cmsolution.bitmap"));
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
        projectNodeMap[projectName] = projectNode.get();
        std::unique_ptr<SolutionTreeViewNodeData> projectTreeViewData(new SolutionTreeViewNodeData(SolutionTreeViewNodeDataKind::project, nullptr, project.get(), std::string(), std::string()));
        projectNode->SetData(projectTreeViewData.get());
        projectNode->SetImageIndex(solutionExplorer->GetImageList()->GetImageIndex("cmproject.bitmap"));
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
            if (Path::GetExtension(file->fileName) == ".xml")
            {
                fileNode->SetImageIndex(solutionExplorer->GetImageList()->GetImageIndex("xmlfile.bitmap"));
            }
            else
            {
                fileNode->SetImageIndex(solutionExplorer->GetImageList()->GetImageIndex("file.bitmap"));
            }
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
    //solutionTreeView->SetRoot(solutionNode.release());
    solutionExplorer->SetRoot(solutionNode.release());
}

bool SolutionData::Changed()
{
    if (solutionBreakpointCollection.Changed())
    {
        changed = true;
    }
    return changed;
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

void SolutionData::SetCallStackOpen(bool callStackOpen_)
{
    if (callStackOpen != callStackOpen_)
    {
        callStackOpen = callStackOpen_;
        changed = true;
    }
}

void SolutionData::SetLocalsViewOpen(bool localsViewOpen_)
{
    if (localsViewOpen != localsViewOpen_)
    {
        localsViewOpen = localsViewOpen_;
        changed = true;
    }
}

void SolutionData::AddOpenFile(const std::string& filePath)
{
    if (openFiles.find(filePath) == openFiles.cend())
    {
        openFiles.insert(filePath);
        changed = true;
    }
}

void SolutionData::RemoveOpenFile(const std::string& filePath)
{
    if (openFiles.find(filePath) != openFiles.cend())
    {
        openFiles.erase(filePath);
        changed = true;
    }
}

void SolutionData::SetCurrentOpenFile(const std::string& openFile)
{
    if (currentOpenFile != openFile)
    {
        currentOpenFile = openFile;
        changed = true;
    }
}

void SolutionData::SetCurrentCursorLine(int line)
{
    if (currentCursorLine != line)
    {
        currentCursorLine = line;
        changed = true;
    }
}

void SolutionData::AddExpandedProject(const std::string& expandedProject)
{
    if (expandedProjects.find(expandedProject) == expandedProjects.cend())
    {
        expandedProjects.insert(expandedProject);
        changed = true;
    }
}

void SolutionData::RemoveExpandedProject(const std::string& project)
{
    if (expandedProjects.find(project) != expandedProjects.cend())
    {
        expandedProjects.erase(project);
        changed = true;
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

TreeViewNode* SolutionData::GetProjectNodeByName(const std::string& projectName) const
{
    auto it = projectNodeMap.find(projectName);
    if (it != projectNodeMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void SolutionData::RemoveProjectNode(const std::string& projectName)
{
    projectNodeMap.erase(projectName);
}

void SolutionData::Load(const std::string& solutionSettingsFilePath)
{
    if (boost::filesystem::exists(solutionSettingsFilePath))
    {
        std::unique_ptr<sngxml::dom::Document> solutionSettingsDoc = sngxml::dom::ReadDocument(solutionSettingsFilePath); 
        SolutionSettings solutionSettings;
        solutionSettings.FromXml(solutionSettingsDoc->DocumentElement());
        callStackOpen = solutionSettings.callStackOpen;
        localsViewOpen = solutionSettings.localsViewOpen;
        openFiles.clear();
        for (const std::string& openFile : solutionSettings.openFiles)
        {
            if (boost::filesystem::exists(openFile))
            {
                openFiles.insert(openFile);
            }
        }
        currentOpenFile = std::string();
        currentCursorLine = 0;
        if (boost::filesystem::exists(solutionSettings.currentOpenFile))
        {
            currentOpenFile = solutionSettings.currentOpenFile;
            currentCursorLine = solutionSettings.currentCursorLine;
        }
        expandedProjects.clear();
        for (const std::string& expandedProject : solutionSettings.expandedProjects)
        {
            expandedProjects.insert(expandedProject);
        }
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
    solutionSettings.callStackOpen = callStackOpen;
    solutionSettings.localsViewOpen = localsViewOpen;
    for (const std::string& openFile : openFiles)
    {
        solutionSettings.openFiles.push_back(openFile);
    }
    solutionSettings.currentOpenFile = currentOpenFile;
    solutionSettings.currentCursorLine = currentCursorLine;
    for (const std::string& expandedProject : expandedProjects)
    {
        solutionSettings.expandedProjects.push_back(expandedProject);
    }
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
    changed = false;
}

} // namespace cmcode
