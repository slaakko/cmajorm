// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_FILE_PTR_INCLUDED
#define SOULNG_FILE_PTR_INCLUDED
#include <soulng/util/FileLocking.hpp>
#include <cstdio>
#include <cstring>
#include <string>

namespace soulng { namespace util {

class UTIL_API FilePtr
{
public:
    FilePtr(FILE* fp_, const std::string& fileName_, LockKind lockKind_) : fp(fp_), fileName(fileName_), lockKind(lockKind_)
    {
    }
    ~FilePtr()
    {
        if (fp != nullptr)
        {
            std::fclose(fp);
            UnlockFile(fileName, lockKind);
        }
    }
    operator FILE* () const { return fp; }
private:
    FILE* fp;
    std::string fileName;
    LockKind lockKind;
};

UTIL_API FILE* OpenRead(const char* fileName);
UTIL_API FILE* OpenWrite(const char* fileName);

} } // namespace soulng::util

#endif // SOULNG_FILE_PTR_INCLUDED
