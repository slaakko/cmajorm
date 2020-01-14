// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_LOG_INCLUDED
#define SOULNG_UTIL_LOG_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <string>

namespace soulng { namespace util {

enum class LogMode
{
    console, queue
};

UTIL_API void SetLogMode(LogMode mode);
UTIL_API void StartLog();
UTIL_API void EndLog();
UTIL_API void LogMessage(int logStreamId, const std::string& message);
UTIL_API void LogMessage(int logStreamId, const std::string& message, int indent);
UTIL_API int WaitForLogMessage();
UTIL_API std::string FetchLogMessage();
UTIL_API int FetchLogMessage(char16_t* buf, int size);

} } // namespace soulng::util

#endif // SOULNG_UTIL_LOG_INCLUDED
