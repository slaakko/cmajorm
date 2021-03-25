// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_FILE_LOCKING_INCLUDED
#define SOULNG_UTIL_FILE_LOCKING_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <string>

namespace soulng { namespace util {

enum class LockKind : int
{
    read = 0, write = 1
};

UTIL_API void LockFile(const std::string& filePath, LockKind lockKind);
UTIL_API void UnlockFile(const std::string& filePath, LockKind lockKind);
UTIL_API void InitFileLocking();
UTIL_API void DoneFileLocking();

} } // namespace soulng::util

#endif // SOULNG_UTIL_FILE_LOCKING_INCLUDED
