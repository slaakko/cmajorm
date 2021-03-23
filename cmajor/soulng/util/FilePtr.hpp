// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_FILE_PTR_INCLUDED
#define SOULNG_FILE_PTR_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <cstdio>
#include <cstring>

namespace soulng { namespace util {

class UTIL_API FilePtr
{
public:
    FilePtr(FILE* fp_) : fp(fp_)
    {
    }
    ~FilePtr()
    {
        if (fp != nullptr)
        {
            std::fclose(fp);
        }
    }
    operator FILE* () const { return fp; }
private:
    FILE* fp;
};

UTIL_API FILE* OpenRead(const char* fileName);
UTIL_API FILE* OpenWrite(const char* fileName);

} } // namespace soulng::util

#endif // SOULNG_FILE_PTR_INCLUDED
