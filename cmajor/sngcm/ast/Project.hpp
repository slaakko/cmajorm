// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_PROJECT_INCLUDED
#define SNGCM_AST_PROJECT_INCLUDED
#include <sngcm/ast/AstApi.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <vector>
#include <memory>

namespace sngcm { namespace ast {

enum class BackEnd
{
    llvm, cmsx
};

class SNGCM_AST_API ModuleVersionTagVerifier
{
public:
    virtual void VerifyModuleVersionTag(const std::string& moduleFilePath) = 0;
};

SNGCM_AST_API void SetModuleVersionTagVerifier(ModuleVersionTagVerifier* verifier);

SNGCM_AST_API std::string CmajorRootDir();
SNGCM_AST_API std::string CmajorSystemLibDir(const std::string& config, BackEnd backend);
SNGCM_AST_API std::string CmajorSystemModuleFilePath(const std::string& config, BackEnd backend);
SNGCM_AST_API std::string CmajorSystemWindowsModuleFilePath(const std::string& config);
SNGCM_AST_API void SetOutDir(const std::string& outDir_);
SNGCM_AST_API const std::string& OutDir();

enum class ProjectDeclarationType : uint8_t
{
    referenceDeclaration, sourceFileDeclaration, textFileDeclaration, targetDeclaration
};

class SNGCM_AST_API ProjectDeclaration
{
public:
    ProjectDeclaration(ProjectDeclarationType declarationType_);
    ProjectDeclaration(const ProjectDeclaration&) = delete;
    ProjectDeclaration& operator=(const ProjectDeclaration&) = delete;
    virtual ~ProjectDeclaration();
    ProjectDeclarationType GetDeclarationType() const { return declarationType; }
private:
    ProjectDeclarationType declarationType;
};

class SNGCM_AST_API ReferenceDeclaration : public ProjectDeclaration
{
public:
    ReferenceDeclaration(const std::string& filePath_);
    const std::string& FilePath() const { return filePath; }
private:
    std::string filePath;
};

class SNGCM_AST_API SourceFileDeclaration : public ProjectDeclaration
{
public:
    SourceFileDeclaration(const std::string& filePath_);
    const std::string& FilePath() const { return filePath; }
private:
    std::string filePath;
};

class SNGCM_AST_API TextFileDeclaration : public ProjectDeclaration
{
public:
    TextFileDeclaration(const std::string& filePath_);
    const std::string& FilePath() const { return filePath; }
private:
    std::string filePath;
};

enum class Target
{
    program, winguiapp, winapp, library, winlib, unitTest
};

class SNGCM_AST_API TargetDeclaration : public ProjectDeclaration
{
public:
    TargetDeclaration(Target target_);
    Target GetTarget() const { return target; }
private:
    Target target;
};

class SNGCM_AST_API Project
{
public:
    Project(const std::u32string& name_, const std::string& filePath_, const std::string& config_, BackEnd backend);
    Project(const Project&) = delete;
    Project& operator=(const Project&) = delete;
    const std::u32string& Name() const { return name; }
    const std::string& FilePath() const { return filePath; }
    const boost::filesystem::path& SourceBasePath() const { return sourceBasePath; }
    const boost::filesystem::path& OutdirBasePath() const { return outdirBasePath; }
    void AddDeclaration(ProjectDeclaration* declaration);
    void ResolveDeclarations();
    const std::string& ModuleFilePath() const { return moduleFilePath; }
    const std::string& LibraryFilePath() const { return libraryFilePath; }
    const std::string& ExecutableFilePath() const { return executableFilePath; }
    const std::vector<std::string>& References() const { return references; }
    const std::vector<std::string>& ReferencedProjectFilePaths() const { return referencedProjectFilePaths; }
    const std::vector<std::string>& RelativeReferencedProjectFilePaths() const { return relativeReferencedProjectFilePaths; }
    const std::vector<std::string>& SourceFilePaths() const { return sourceFilePaths; }
    const std::vector<std::string>& RelativeSourceFilePaths() const { return relativeSourceFilePaths; }
    const std::vector<std::string>& RelativeTextFilePaths() const { return relativeTextFilePaths; }
    const std::vector<std::string>& TextFilePaths() const { return textFilePaths; }
    Target GetTarget() const { return target; }
    bool DependsOn(Project* that) const;
    void AddDependsOnProjects(Project* dependsOnProject);
    bool IsSystemProject() const { return isSystemProject; }
    void SetSystemProject() { isSystemProject = true; }
    void SetRelativeFilePath(const std::string& relativeFilePath_) { relativeFilePath = relativeFilePath_; }
    const std::string& RelativeFilePath() const { return relativeFilePath; }
    void SetModuleFilePath(const std::string& moduleFilePath_);
    void SetLibraryFilePath(const std::string& libraryFilePath_);
    bool IsUpToDate(const std::string& systemModuleFilePath) const;
    int LogStreamId() const { return logStreamId; }
    void SetLogStreamId(int logStreamId_) { logStreamId = logStreamId_; }
    void SetIndex(int index_) { index = index_; }
    int Index() const { return index; }
    const std::vector<Project*>& DependsOnProjects() { return dependsOn; }
    bool Built() const { return built; }
    void SetBuilt() { built = true; }
    bool Ready() const;
    void SetExcludeSourceFilePath(const std::string& excludeSourceFilePath_);
private:
    std::u32string name;
    std::string filePath;
    std::string config;
    Target target;
    boost::filesystem::path sourceBasePath;
    boost::filesystem::path outdirBasePath;
    boost::filesystem::path systemLibDir;
    std::vector<std::unique_ptr<ProjectDeclaration>> declarations;
    std::string relativeFilePath;
    std::string moduleFilePath;
    std::string libraryFilePath;
    std::string executableFilePath;
    std::string excludeSourceFilePath;
    std::vector<std::string> references;
    std::vector<std::string> referencedProjectFilePaths;
    std::vector<std::string> sourceFilePaths;
    std::vector<std::string> relativeSourceFilePaths;
    std::vector<std::string> relativeReferencedProjectFilePaths;
    std::vector<std::string> relativeTextFilePaths;
    std::vector<std::string> textFilePaths;
    std::vector<Project*> dependsOn;
    bool built;
    bool isSystemProject;
    int logStreamId;
    int index;
};


} } // namespace sngcm::ast

#endif // SNGCM_AST_PROJECT_INCLUDED
