// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_AST_SOLUTION_INCLUDED
#define CMAJOR_AST_SOLUTION_INCLUDED
#include <cmajor/ast/Project.hpp>
#include <unordered_map>

namespace cmajor { namespace ast {

class SolutionDeclaration
{
public:
    virtual ~SolutionDeclaration();
};

class SolutionProjectDeclaration : public SolutionDeclaration
{
public:
    SolutionProjectDeclaration(const std::string& filePath_);
    const std::string& FilePath() const { return filePath; }
private:
    std::string filePath;
};

class SolutionActiveProjectDeclaration : public SolutionDeclaration
{
public:
    SolutionActiveProjectDeclaration(const std::u32string& activeProjectName_);
    const std::u32string& ActiveProjectName() const { return activeProjectName; }
private:
    std::u32string activeProjectName;
};

class ProjectDependencyDeclaration : public SolutionDeclaration
{
public:
    ProjectDependencyDeclaration(const std::u32string& projectName_);
    void AddDependency(const std::u32string& dependsOn);
    const std::u32string& ProjectName() const { return projectName; }
    const std::vector<std::u32string>& DependsOnProjects() const { return dependsOnProjects; }
private:
    std::u32string projectName;
    std::vector<std::u32string> dependsOnProjects;
};

class Solution
{
public:
    Solution(const std::u32string& name_, const std::string& filePath_);
    const std::u32string& Name() const { return name; }
    const std::string& FilePath() const { return filePath; }
    const boost::filesystem::path& BasePath() const { return basePath; }
    const std::vector<std::string>& ProjectFilePaths() const { return projectFilePaths; }
    const std::vector<std::string>& RelativeProjectFilePaths() const { return relativeProjectFilePaths; }
    const std::u32string& ActiveProjectName() const { return activeProjectName; }
    void AddProject(std::unique_ptr<Project>&& project);
    std::vector<Project*> CreateBuildOrder();
    void AddDeclaration(SolutionDeclaration* declaration);
    void ResolveDeclarations();
private:
    std::u32string name;
    std::string filePath;
    boost::filesystem::path basePath;
    std::vector<std::unique_ptr<SolutionDeclaration>> declarations;
    std::vector<std::string> projectFilePaths;
    std::vector<std::string> relativeProjectFilePaths;
    std::vector<std::unique_ptr<Project>> projects;
    std::u32string activeProjectName;
    std::vector<std::unique_ptr<ProjectDependencyDeclaration>> additionalDependencyDeclarations;
    std::unordered_map<std::u32string, ProjectDependencyDeclaration*> dependencyMap;
    void AddDependencies();
};

} } // namespace cmajor::ast

#endif // CMAJOR_AST_SOLUTION_INCLUDED
