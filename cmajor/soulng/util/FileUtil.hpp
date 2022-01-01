// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_FILE_UTIL_INCLUDED
#define SOULNG_UTIL_FILE_UTIL_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <string>

namespace soulng { namespace util {

UTIL_API void CopyFile(const std::string& source, const std::string& dest, bool force, bool verbose);

} } // namespace soulng::util

#endif // SOULNG_UTIL_FILE_UTIL_INCLUDED
