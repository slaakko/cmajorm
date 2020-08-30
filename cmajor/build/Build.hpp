// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_BUILD_INCLUDED
#define CMAJOR_BUILD_BUILD_INCLUDED
#include <cmajor/build/BuildApi.hpp>
#include <cmajor/build/MessageBody.hpp>
#include <cmajor/symbols/Module.hpp>
#include <sngcm/ast/Project.hpp>
#include <set>

namespace cmajor { namespace build {

using namespace sngcm::ast;
using namespace cmajor::symbols;

BUILD_API void BuildProject(const std::string& projectFilePath, std::unique_ptr<Module>& rootModule, std::set<std::string>& builtProjects);
BUILD_API void BuildProject(Project* project, std::unique_ptr<Module>& rootModule, bool& stop, bool resetRootModule, std::set<std::string>& builtProjects);
BUILD_API void BuildSolution(const std::string& solutionFilePath, std::vector<std::unique_ptr<Module>>& rootModules);
BUILD_API void BuildSolution(const std::string& solutionFilePath, std::vector<std::unique_ptr<Module>>& rootModules, std::u32string& solutionName, std::vector<std::u32string>& moduleNames);
BUILD_API void BuildMsBuildProject(const std::string& projectName, const std::string& projectDirectory, const std::string& target,
    const std::vector<std::string>& sourceFiles, const std::vector<std::string>& resourceFiles, const std::vector<std::string>& referenceFiles,
    std::unique_ptr<Module>& rootModule);
BUILD_API void GenerateLibrary(Module* module, const std::vector<std::string>& objectFilePaths, const std::string& libraryFilePath);
BUILD_API void Link(Target target,const std::string& executableFilePath, const std::string& libraryFilePath, const std::vector<std::string>& libraryFilePaths,
    const std::string& mainObjectFilePath, cmajor::symbols::Module& module);
BUILD_API void StopBuild();
BUILD_API void ResetStopBuild();
BUILD_API std::unique_ptr<Project> ReadProject(const std::string& projectFilePath);
BUILD_API ProjectInfo ReadPojectInfo(Project* project, const std::string& projectInfoFilePath);

} } // namespace cmajor::build

#endif // CMAJOR_BUILD_BUILD_INCLUDED
