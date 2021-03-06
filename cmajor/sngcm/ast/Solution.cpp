// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/Solution.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Path.hpp>
#include <algorithm>
#include <unordered_set>

namespace sngcm { namespace ast {

using namespace soulng::util;
using namespace soulng::unicode;

struct ByProjectName
{
    bool operator()(const std::unique_ptr<Project>& left, const std::unique_ptr<Project>& right) const
    {
        return left->Name() < right->Name();
    }
};

SolutionDeclaration::SolutionDeclaration()
{
}

SolutionDeclaration::~SolutionDeclaration()
{
}

SolutionProjectDeclaration::SolutionProjectDeclaration(const std::string& filePath_) : filePath(filePath_)
{
}

SolutionActiveProjectDeclaration::SolutionActiveProjectDeclaration(const std::u32string& activeProjectName_) : activeProjectName(activeProjectName_)
{
}

ProjectDependencyDeclaration::ProjectDependencyDeclaration(const std::u32string& projectName_) : projectName(projectName_)
{
}

void ProjectDependencyDeclaration::AddDependency(const std::u32string& dependsOn)
{
    dependsOnProjects.push_back(dependsOn);
}

Solution::Solution(const std::u32string& name_, const std::string& filePath_) : name(name_), filePath(filePath_), basePath(filePath), activeProject(nullptr)
{
    basePath.remove_filename();
}

void Solution::AddDeclaration(SolutionDeclaration* declaration)
{
    declarations.push_back(std::unique_ptr<SolutionDeclaration>(declaration));
}

void Solution::ResolveDeclarations()
{
    for (const std::unique_ptr<SolutionDeclaration>& declaration : declarations)
    {
        if (SolutionProjectDeclaration* solutionProjectDeclaration = dynamic_cast<SolutionProjectDeclaration*>(declaration.get()))
        {
            boost::filesystem::path pp(solutionProjectDeclaration->FilePath());
            relativeProjectFilePaths.push_back(pp.generic_string());
            if (pp.is_relative())
            {
                pp = basePath / pp;
            }
            if (pp.extension() != ".cmp")
            {
                throw std::runtime_error("invalid project file extension '" + pp.generic_string() + "' (not .cmp)");
            }
            if (!boost::filesystem::exists(pp))
            {
                throw std::runtime_error("project file '" + GetFullPath(pp.generic_string()) + "' not found");
            }
            std::string projectFilePath = GetFullPath(pp.generic_string());
            if (std::find(projectFilePaths.cbegin(), projectFilePaths.cend(), projectFilePath) == projectFilePaths.cend())
            {
                projectFilePaths.push_back(projectFilePath);
            }
        }
        else if (SolutionActiveProjectDeclaration* activeProjectDeclaration = dynamic_cast<SolutionActiveProjectDeclaration*>(declaration.get()))
        {
            activeProjectName = activeProjectDeclaration->ActiveProjectName();
        }
        else if (ProjectDependencyDeclaration* projectDependencyDeclaration = dynamic_cast<ProjectDependencyDeclaration*>(declaration.get()))
        {
            dependencyMap[projectDependencyDeclaration->ProjectName()] = projectDependencyDeclaration;
        }
        else
        {
            throw std::runtime_error("unknown solution declaration");
        }
    }
}

void Solution::SortByProjectName()
{
    std::sort(projects.begin(), projects.end(), ByProjectName());
}

void Solution::Save()
{
    std::ofstream file(filePath);
    CodeFormatter formatter(file);
    formatter.WriteLine("solution " + ToUtf8(name) + ";");
    std::string solutionDir = Path::GetDirectoryName(filePath);
    relativeProjectFilePaths.clear();
    for (const std::unique_ptr<Project>& project : projects)
    {
        std::string projectFilePath = project->FilePath();
        std::string projectDir = Path::GetDirectoryName(projectFilePath);
        std::string relativeProjectDir = MakeRelativeDirPath(projectDir, solutionDir);
        std::string relativeProjectFilePath = Path::Combine(relativeProjectDir, Path::GetFileName(projectFilePath));
        relativeProjectFilePaths.push_back(relativeProjectFilePath);
    }
    for (const std::string& relativeProjectFilePath : relativeProjectFilePaths)
    {
        formatter.WriteLine("project <" + relativeProjectFilePath + ">;");
    }
    if (activeProject)
    {
        formatter.WriteLine("activeProject " + ToUtf8(activeProject->Name()) + ";");
    }
    for (const std::unique_ptr<Project>& project : projects)
    {
        project->Save();
    }
}

void Solution::RemoveProject(Project* project)
{
    for (auto it = projects.begin(); it != projects.end(); ++it)
    {
        if (it->get() == project)
        {
            if (activeProject == project)
            {
                activeProject = nullptr;
            }
            projects.erase(it);
            break;
        }
    }
}

void Solution::AddProject(std::unique_ptr<Project>&& project)
{
    projects.push_back(std::move(project));
}

bool Solution::HasProject(const std::u32string& projectName) const
{
    for (const auto& project : projects)
    {
        if (project->Name() == projectName)
        {
            return true;
        }
    }
    return false;
}

void Visit(std::vector<std::u32string>& order, const std::u32string& projectName, std::unordered_set<std::u32string>& visited, std::unordered_set<std::u32string>& tempVisit,
    const std::unordered_map<std::u32string, ProjectDependencyDeclaration*>& dependencyMap, Solution* solution)
{
    if (tempVisit.find(projectName) == tempVisit.end())
    {
        if (visited.find(projectName) == visited.end())
        {
            tempVisit.insert(projectName);
            auto i = dependencyMap.find(projectName);
            if (i != dependencyMap.end())
            {
                ProjectDependencyDeclaration* dependencyDeclaration = i->second;
                for (const std::u32string& dependentProject : dependencyDeclaration->DependsOnProjects())
                {
                    Visit(order, dependentProject, visited, tempVisit, dependencyMap, solution);
                }
                tempVisit.erase(projectName);
                visited.insert(projectName);
                order.push_back(projectName);
            }
            else
            {
                throw std::runtime_error("project '" + ToUtf8(projectName) + "' not found in dependencies of solution '" + ToUtf8(solution->Name()) + "' (" +
                    GetFullPath(solution->FilePath()) + ")");
            }
        }
    }
    else
    {
        throw std::runtime_error("circular project dependency '" + ToUtf8(projectName) + "' detected in dependencies of solution '" + ToUtf8(solution->Name()) + "' (" +
            GetFullPath(solution->FilePath()) + ")");
    }
}

void Solution::AddDependencies()
{
    for (const std::unique_ptr<Project>& project : projects)
    {
        ProjectDependencyDeclaration* dependencyDeclaration = nullptr;
        auto it = dependencyMap.find(project->Name());
        if (it != dependencyMap.cend())
        {
            dependencyDeclaration = it->second;
        }
        else
        {
            ProjectDependencyDeclaration* additionalDeclaration = new ProjectDependencyDeclaration(project->Name());
            additionalDependencyDeclarations.push_back(std::unique_ptr<ProjectDependencyDeclaration>(additionalDeclaration));
            dependencyDeclaration = additionalDeclaration;
            dependencyMap[project->Name()] = dependencyDeclaration;
        }
        for (const std::unique_ptr<Project>& projectToCheck : projects)
        {
            if (projectToCheck != project)
            {
                if (project->DependsOn(projectToCheck.get()))
                {
                    project->AddDependsOnProjects(projectToCheck.get());
                    if (std::find(dependencyDeclaration->DependsOnProjects().cbegin(), dependencyDeclaration->DependsOnProjects().cend(), projectToCheck->Name()) == dependencyDeclaration->DependsOnProjects().cend())
                    {
                        dependencyDeclaration->AddDependency(projectToCheck->Name());
                    }
                }
            }
        }
    }
}

std::vector<Project*> Solution::CreateBuildOrder()
{
    AddDependencies();
    std::vector<Project*> buildOrder;
    std::unordered_map<std::u32string, Project*> projectMap;
    for (const std::unique_ptr<Project>& project : projects)
    {
        projectMap[project->Name()] = project.get();
    }
    std::vector<std::u32string> order;
    std::unordered_set<std::u32string> visited;
    std::unordered_set<std::u32string> tempVisit;
    for (const std::unique_ptr<Project>& project : projects)
    {
        if (visited.find(project->Name()) == visited.end())
        {
            Visit(order, project->Name(), visited, tempVisit, dependencyMap, this);
        }
    }
    for (const std::u32string& projectName : order)
    {
        auto i = projectMap.find(projectName);
        if (i != projectMap.end())
        {
            buildOrder.push_back(i->second);
        }
        else
        {
            throw std::runtime_error("project name '" + ToUtf8(projectName) + "' not found in solution '" + ToUtf8(Name()) + "' (" + GetFullPath(FilePath()) + ")");
        }
    }
    return buildOrder;
}

} } // namespace sngcm::ast
