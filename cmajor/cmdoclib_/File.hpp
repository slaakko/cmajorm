// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMDOCLIB_FILE_INCLUDED
#define CMAJOR_CMDOCLIB_FILE_INCLUDED
#include <string>

namespace cmajor { namespace cmdoclib {

struct File
{
    std::u32string name;
    std::string htmlFilePath;
};

} } // namespace cmajor::cmdoclib

#endif // CMAJOR_CMDOCLIB_FILE_INCLUDED
