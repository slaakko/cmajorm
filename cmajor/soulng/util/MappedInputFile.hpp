// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_MAPPED_INPUT_FILE_INCLUDED
#define SOULNG_UTIL_MAPPED_INPUT_FILE_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <string>
#include <stdint.h>

namespace soulng { namespace util {

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

} } // namespace soulng::util

#endif // SOULNG_UTIL_MAPPED_INPUT_FILE_INCLUDED
