// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMDOCLIB_FILE_INCLUDED
#define CMAJOR_CMDOCLIB_FILE_INCLUDED
#include <cmajor/cmdoclib/CmDocLibApi.hpp>
#include <string>

namespace cmajor { namespace cmdoclib {

struct CMDOCLIB_API File
{
    std::u32string name;
    std::string htmlFilePath;
};

} } // namespace cmajor::cmdoclib

#endif // CMAJOR_CMDOCLIB_FILE_INCLUDED
