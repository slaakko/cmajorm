// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_PROJECT_INCLUDED
#define SNGCM_AST_PROJECT_INCLUDED
#include <sngcm/ast/AstApi.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <vector>
#include <memory>
#include <set>

namespace sngcm { namespace ast {

using namespace soulng::util;

enum class BackEnd : int8_t
{
    llvm = 0, cmsx = 1, cppcm = 2
};

enum class Config : int8_t
{
    debug = 0, release = 1, profile = 2, trace = 3
};

class SNGCM_AST_API ModuleVersionTagVerifier
{
public:
    virtual void VerifyModuleVersionTag(const std::string& moduleFilePath) = 0;
};

SNGCM_AST_API void SetModuleVersionTagVerifier(ModuleVersionTagVerifier* verifier);

enum class SystemDirKind : int
{
    regular = 0, repository = 1
};

class Project;
class Solution;

SNGCM_AST_API std::string CmajorRootDir();
SNGCM_AST_API std::string CmajorSystemLibDir(const std::string& config, BackEnd backend, const std::string& toolChain, SystemDirKind systemDirKind);
SNGCM_AST_API std::string CmajorResourceDir();
SNGCM_AST_API std::string CmajorLogFileDir();
SNGCM_AST_API std::string CmajorSystemModuleFilePath(const std::string& config, BackEnd backend, const std::string& toolChain, SystemDirKind systemDirKind);
SNGCM_AST_API std::string CmajorSystemWindowsModuleFilePath(const std::string& config, const std::string& toolChanin, SystemDirKind systemDirKind);
SNGCM_AST_API std::string MakeCmajorRootRelativeFilePath(const std::string& filePath);
SNGCM_AST_API std::string ExpandCmajorRootRelativeFilePath(const std::string& filePath);
SNGCM_AST_API std::vector<Project*> GetReferencedProjects(Project* project, Solution* solution);
SNGCM_AST_API std::set<Project*> GetAllReferencedProjects(Project* project, Solution* solution);
SNGCM_AST_API void AddReferencedProjects(std::set<Project*>& allReferencedProjects, Project* project, Solution* solution);
SNGCM_AST_API void SetOutDir(const std::string& outDir_);
SNGCM_AST_API const std::string& OutDir();

enum class ProjectDeclarationType : uint8_t
{
    referenceDeclaration, sourceFileDeclaration, resourceFileDeclaration, textFileDeclaration, targetDeclaration
};

class SNGCM_AST_API ProjectDeclaration
{
public:
    ProjectDeclaration(ProjectDeclarationType declarationType_);
    ProjectDeclaration(const ProjectDeclaration&) = delete;
    ProjectDeclaration& operator=(const ProjectDeclaration&) = delete;
    virtual ~ProjectDeclaration();
    ProjectDeclarationType GetDeclarationType() const { return declarationType; }
    virtual void Write(CodeFormatter& formatter) = 0;
private:
    ProjectDeclarationType declarationType;
};

class SNGCM_AST_API ReferenceDeclaration : public ProjectDeclaration
{
public:
    ReferenceDeclaration(const std::string& filePath_);
    const std::string& FilePath() const { return filePath; }
    void Write(CodeFormatter& formatter) override;
private:
    std::string filePath;
};

class SNGCM_AST_API SourceFileDeclaration : public ProjectDeclaration
{
public:
    SourceFileDeclaration(const std::string& filePath_);
    const std::string& FilePath() const { return filePath; }
    void Write(CodeFormatter& formatter) override;
private:
    std::string filePath;
};

class SNGCM_AST_API ResourceFileDeclaration : public ProjectDeclaration
{
public:
    ResourceFileDeclaration(const std::string& filePath_);
    const std::string& FilePath() const { return filePath; }
    void Write(CodeFormatter& formatter) override;
private:
    std::string filePath;
};

class SNGCM_AST_API TextFileDeclaration : public ProjectDeclaration
{
public:
    TextFileDeclaration(const std::string& filePath_);
    const std::string& FilePath() const { return filePath; }
    void Write(CodeFormatter& formatter) override;
private:
    std::string filePath;
};

enum class Target
{
    program, winguiapp, winapp, library, winlib, unitTest
};

SNGCM_AST_API std::string TargetStr(Target target);

SNGCM_AST_API Target ParseTarget(const std::string& targetStr);

class SNGCM_AST_API TargetDeclaration : public ProjectDeclaration
{
public:
    TargetDeclaration(Target target_);
    Target GetTarget() const { return target; }
    void Write(CodeFormatter& formatter) override;
private:
    Target target;
};

class SNGCM_AST_API Project
{
public:
    Project(const std::u32string& name_, const std::string& filePath_, const std::string& config_, BackEnd backend_, const std::string& toolChain_,
        SystemDirKind systemDirKind);
    Project(const Project&) = delete;
    Project& operator=(const Project&) = delete;
    const std::u32string& Name() const { return name; }
    const std::string& FilePath() const { return filePath; }
    const boost::filesystem::path& SourceBasePath() const { return sourceBasePath; }
    const boost::filesystem::path& OutdirBasePath() const { return outdirBasePath; }
    void AddDeclaration(ProjectDeclaration* declaration);
    void ResolveDeclarations();
    void Write(const std::string& projectFilePath);
    const std::string& ModuleFilePath() const { return moduleFilePath; }
    const std::string& LibraryFilePath() const { return libraryFilePath; }
    const std::string& ExecutableFilePath() const { return executableFilePath; }
    const std::vector<std::string>& References() const { return references; }
    const std::vector<std::string>& ReferencedProjectFilePaths() const { return referencedProjectFilePaths; }
    const std::vector<std::string>& RelativeReferencedProjectFilePaths() const { return relativeReferencedProjectFilePaths; }
    const std::vector<std::string>& SourceFilePaths() const { return sourceFilePaths; }
    const std::vector<std::string>& RelativeSourceFilePaths() const { return relativeSourceFilePaths; }
    const std::vector<std::string>& ResourceFilePaths() const { return resourceFilePaths;  }
    const std::vector<std::string>& RelativeResourceFilePaths() const { return relativeResourceFilePaths; }
    const std::vector<std::string>& RelativeTextFilePaths() const { return relativeTextFilePaths; }
    const std::vector<std::string>& TextFilePaths() const { return textFilePaths; }
    Target GetTarget() const { return target; }
    void SetTarget(Target target_) { target = target_; }
    bool DependsOn(Project* that) const;
    void AddDependsOnProjects(Project* dependsOnProject);
    bool IsSystemProject() const { return isSystemProject; }
    void SetSystemProject() { isSystemProject = true; }
    void SetRelativeFilePath(const std::string& relativeFilePath_) { relativeFilePath = relativeFilePath_; }
    const std::string& RelativeFilePath() const { return relativeFilePath; }
    void SetModuleFilePath(const std::string& moduleFilePath_);
    void SetLibraryFilePath(const std::string& libraryFilePath_);
    void SetReferencedProjects(const std::vector<Project*>& referencedProjects);
    bool IsUpToDate(const std::string& systemModuleFilePath) const;
    int LogStreamId() const { return logStreamId; }
    void SetLogStreamId(int logStreamId_) { logStreamId = logStreamId_; }
    void SetIndex(int index_) { index = index_; }
    int Index() const { return index; }
    const std::vector<Project*>& DependsOnProjects() { return dependsOn; }
    bool Built();
    void SetBuilt();
    bool Ready();
    void SetExcludeSourceFilePath(const std::string& excludeSourceFilePath_);
    void SetHash(const std::string& hash_) { hash = hash_; }
    const std::string& Hash() const { return hash; }
    std::string Id() const;
    void AddDependsOnId(const std::string& dependsOnId);
    const std::vector<std::string>& DependsOnIds() const { return dependsOnIds; }
    bool HasSourceFile(const std::string& sourceFilePath) const;
    bool HasResourceFile(const std::string& resourceFilePath) const;
    bool HasTextFile(const std::string& textFilePath) const;
    void AddSourceFileName(const std::string& sourceFileName, const std::string& sourceFilePath);
    void AddResourceFileName(const std::string& resourceFileName, const std::string& resourceFilePath);
    void AddTextFileName(const std::string& textFileName, const std::string& textFilePath);
    void RemoveFile(const std::string& filePath, const std::string& fileName);
    void Save();
private:
    BackEnd backend;
    std::string toolChain;
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
    std::vector<std::string> resourceFilePaths;
    std::vector<std::string> relativeResourceFilePaths;
    std::vector<std::string> relativeReferencedProjectFilePaths;
    std::vector<std::string> relativeTextFilePaths;
    std::vector<std::string> textFilePaths;
    std::vector<Project*> dependsOn;
    bool built;
    bool isSystemProject;
    int logStreamId;
    int index;
    std::string hash;
    std::vector<std::string> dependsOnIds;
    std::mutex mtx;
};


} } // namespace sngcm::ast

#endif // SNGCM_AST_PROJECT_INCLUDED
