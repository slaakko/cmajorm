// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_UTIL_LOG_INCLUDED
#define CMAJOR_UTIL_LOG_INCLUDED
#include <cmajor/util/UtilApi.hpp>
#include <string>

namespace cmajor { namespace util {

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
UTIL_API int FetchLogMessage(char16_t* buf, int size);

} } // namespace cmajor::util

#endif // CMAJOR_UTIL_LOG_INCLUDED
