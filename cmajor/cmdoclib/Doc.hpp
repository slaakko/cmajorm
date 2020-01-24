// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMDOCLIB_DOC_INCLUDED
#define CMAJOR_CMDOCLIB_DOC_INCLUDED
#include <cmajor/cmdoclib/CmDocLibApi.hpp>
#include <string>
#include <vector>

namespace cmajor { namespace cmdoclib {

CMDOCLIB_API void BuildDocs(const std::u32string& solutionName, const std::vector<std::u32string>& moduleNames);

} } // namespace cmajor::cmdoclib

#endif // CMAJOR_CMDOCLIB_DOC_INCLUDED
