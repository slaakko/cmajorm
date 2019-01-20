// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_UTIL_LOG_INCLUDED
#define CMAJOR_UTIL_LOG_INCLUDED
#include <string>

namespace cmajor { namespace util {

enum class LogMode
{
    console, queue
};

void SetLogMode(LogMode mode);
void StartLog();
void EndLog();
void LogMessage(int logStreamId, const std::string& message);
void LogMessage(int logStreamId, const std::string& message, int indent);
int WaitForLogMessage();
int FetchLogMessage(char16_t* buf, int size);

} } // namespace cmajor::util

#endif // CMAJOR_UTIL_LOG_INCLUDED
