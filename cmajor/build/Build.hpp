// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_BUILD_INCLUDED
#define CMAJOR_BUILD_BUILD_INCLUDED
#include <cmajor/build/BuildApi.hpp>
#include <cmajor/symbols/Module.hpp>
#include <sngcm/ast/Project.hpp>

namespace cmajor { namespace build {

using namespace sngcm::ast;
using namespace cmajor::symbols;

BUILD_API void BuildProject(const std::string& projectFilePath, std::unique_ptr<Module>& rootModule);
BUILD_API void BuildProject(Project* project, std::unique_ptr<Module>& rootModule, bool& stop, bool resetRootModule);
BUILD_API void BuildSolution(const std::string& solutionFilePath, std::vector<std::unique_ptr<Module>>& rootModules);
BUILD_API void BuildSolution(const std::string& solutionFilePath, std::vector<std::unique_ptr<Module>>& rootModules, std::u32string& solutionName, std::vector<std::u32string>& moduleNames);
BUILD_API void BuildMsBuildProject(const std::string& projectName, const std::string& projectDirectory, const std::string& target,
    const std::vector<std::string>& sourceFiles, const std::vector<std::string>& referenceFiles, std::unique_ptr<Module>& rootModule);
BUILD_API void GenerateLibrary(Module* module, const std::vector<std::string>& objectFilePaths, const std::string& libraryFilePath);
BUILD_API void Link(const std::string& executableFilePath, const std::string& libraryFilePath, const std::vector<std::string>& libraryFilePaths, cmajor::symbols::Module& module);
BUILD_API void StopBuild();

} } // namespace cmajor::build

#endif // CMAJOR_BUILD_BUILD_INCLUDED
