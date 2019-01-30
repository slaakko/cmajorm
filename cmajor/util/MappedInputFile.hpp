// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_UTIL_MAPPED_INPUT_FILE_INCLUDED
#define CMAJOR_UTIL_MAPPED_INPUT_FILE_INCLUDED
#include <cmajor/util/UtilApi.hpp>
#include <string>
#include <stdint.h>

namespace cmajor { namespace util {

class MappedInputFileImpl;

class UTIL_API MappedInputFile
{
public:
    MappedInputFile(const std::string& fileName_);
    ~MappedInputFile();
    const char* Begin() const;
    const char* End() const;
private:
    MappedInputFileImpl* impl;
};

UTIL_API std::string ReadFile(const std::string& fileName);

} } // namespace cmajor::util

#endif // CMAJOR_UTIL_MAPPED_INPUT_FILE_INCLUDED
