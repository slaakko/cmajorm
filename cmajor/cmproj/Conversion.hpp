// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMPROJ_CONVERSION_INCLUDED
#define CMAJOR_CMPROJ_CONVERSION_INCLUDED
#include <cmajor/ast/Project.hpp>

using namespace cmajor::ast;

namespace cmajor { namespace cmproj {

void ConvertProject(Project* project, const std::string& cmprojFilePath, const std::string& projectGuid, bool verbose);

} } // namespace cmajor::cmproj

#endif // CMAJOR_CMPROJ_CONVERSION_INCLUDED
