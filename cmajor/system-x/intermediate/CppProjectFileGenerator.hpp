// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_CPP_PROJECT_FILE_GENERATOR_INCLUDED
#define CMSX_INTERMEDIATE_CPP_PROJECT_FILE_GENERATOR_INCLUDED
#include <system-x/intermediate/Api.hpp>
#include <sngcm/ast/Project.hpp>
#include <symbols/Module.hpp>

namespace cmsx::intermediate {

using namespace sngcm::ast;
using namespace cmajor::symbols;

CMSX_INTERMEDIATE_API void GenerateProjectFile(Project* project, Module* module, const std::string& mainObjectFilePath, bool verbose);

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_CPP_PROJECT_FILE_GENERATOR_INCLUDED
