// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMPROJ_CONVERSION_INCLUDED
#define CMAJOR_CMPROJ_CONVERSION_INCLUDED
#include <cmproj/CmprojApi.hpp>
#include <sngcm/ast/Project.hpp>

using namespace sngcm::ast;

namespace cmajor { namespace cmproj {

void ConvertProjectToCmProject(Project* project, const std::string& cmprojFilePath, const std::string& projectGuid, bool verbose);
void ConvertCmProjectToProject(const std::string& cmprojFilePath, const std::string& projectFilePath, bool verbose);

} } // namespace cmajor::cmproj

#endif // CMAJOR_CMPROJ_CONVERSION_INCLUDED
