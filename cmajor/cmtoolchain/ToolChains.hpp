// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMTOOLCHAINS_INCLUDED
#define CMAJOR_CMTOOLCHAINS_INCLUDED
#include <cmajor/cmtoolchain/ToolChain.hpp>

CMTOOLCHAIN_API Platforms& GetPlatforms();
CMTOOLCHAIN_API std::string GetPlatform();
CMTOOLCHAIN_API Platform& GetOrInsertPlatform(const std::string& name);
CMTOOLCHAIN_API void CreateToolChains(bool verbose);
CMTOOLCHAIN_API void ShowToolChains();
CMTOOLCHAIN_API void ReadToolChains(bool verbose);
CMTOOLCHAIN_API void WriteToolChains(bool verbose);
CMTOOLCHAIN_API void SetToolChain(const std::string& toolChain_);
CMTOOLCHAIN_API std::string GetToolChain();
CMTOOLCHAIN_API void ResetToolChain();
CMTOOLCHAIN_API const Configuration& GetToolConfiguration(const Tool& tool, const std::string& config);
CMTOOLCHAIN_API const Tool& GetCompilerTool(const std::string& platform, const std::string& toolChain);
CMTOOLCHAIN_API const Tool& GetLibraryManagerTool(const std::string& platform, const std::string& toolChain);
CMTOOLCHAIN_API const Tool& GetLinkerTool(const std::string& platform, const std::string& toolChain);
CMTOOLCHAIN_API const Tool& GetProjectFileGeneratorTool(const std::string& platform, const std::string& toolChain);
CMTOOLCHAIN_API const Tool& GetSolutionFileGeneratorTool(const std::string& platform, const std::string& toolChain);

#endif // CMAJOR_CMTOOLCHAINS_INCLUDED
