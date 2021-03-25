// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/FilePtr.hpp>
#include <soulng/util/FileLocking.hpp>
#include <stdexcept>
#include <stdio.h>
#include <share.h>

namespace soulng { namespace util {

FILE* OpenRead(const char* fileName)
{
    LockFile(fileName, LockKind::read);
#if defined(_WIN32) && !defined(__MINGW32__)
    FILE* file = _fsopen(fileName, "rb", _SH_DENYWR);
#else
    FILE* file = std::fopen(fileName, "rb");
#endif
    if (!file)
    {
        throw std::runtime_error("could not open '" + std::string(fileName) + "' for reading: " + std::strerror(errno));
    }
    return file;
}

FILE* OpenWrite(const char* fileName)
{
    LockFile(fileName, LockKind::write);
#if defined(_WIN32) && !defined(__MINGW32__)
    FILE* file = _fsopen(fileName, "wb", _SH_DENYWR);
#else
    FILE* file = std::fopen(fileName, "wb");
#endif
    if (!file)
    {
        throw std::runtime_error("could not open '" + std::string(fileName) + "' for writing: " + std::strerror(errno));
    }
    return file;
}

} } // namespace soulng::util
