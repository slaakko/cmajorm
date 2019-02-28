// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMDOCLIB_DOC_INCLUDED
#define CMAJOR_CMDOCLIB_DOC_INCLUDED
#include <string>
#include <vector>

namespace cmajor { namespace cmdoclib {

void BuildDocs(const std::u32string& solutionName, const std::vector<std::u32string>& moduleNames, std::vector<std::string>& grammarFilePaths);

} } // namespace cmajor::cmdoclib

#endif // CMAJOR_CMDOCLIB_DOC_INCLUDED
