// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMTOOLCHAINS_INCLUDED
#define CMAJOR_CMTOOLCHAINS_INCLUDED
#include <cmajor/cmtoolchain/ToolChain.hpp>

CMTOOLCHAIN_API ToolChains& GetToolChains();
CMTOOLCHAIN_API void CreateToolChains(bool verbose);
CMTOOLCHAIN_API void ShowToolChains();
CMTOOLCHAIN_API void ReadToolChains(bool verbose);
CMTOOLCHAIN_API void WriteToolChains(bool verbose);
CMTOOLCHAIN_API void SetToolChain(const std::string& toolChain_);
CMTOOLCHAIN_API std::string GetToolChain();
CMTOOLCHAIN_API const Tool& GetCompilerTool();
CMTOOLCHAIN_API const Tool& GetLibraryManagerTool();
CMTOOLCHAIN_API const Tool& GetLinkerTool();
CMTOOLCHAIN_API const Tool& GetProjectFileGeneratorTool();
CMTOOLCHAIN_API const Tool& GetSolutionFileGeneratorTool();

#endif // CMAJOR_CMTOOLCHAINS_INCLUDED
