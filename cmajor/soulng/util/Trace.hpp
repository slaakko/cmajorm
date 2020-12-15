// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_TRACE_INCLUDED
#define SOULNG_UTIL_TRACE_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <stdint.h>
#include <string>
#include <vector>

namespace soulng { namespace util {

enum class EntryKind : int8_t
{
    begin = 0, end = 1, message = 2
};

UTIL_API void BeginTracing();
UTIL_API void EndTracing();
UTIL_API void SetThreadId(char threadId_);
UTIL_API int32_t GetTraceFunctionId(const std::string& functionFullName);
UTIL_API void AddTraceEntry(EntryKind kind, int32_t id);
UTIL_API void ConvertTraceToText(const std::string& traceFilePath);
UTIL_API void ReadTraceTable();
UTIL_API void WriteTraceTable();
UTIL_API void InitTrace();
UTIL_API void DoneTrace();

} } // namespace soulng::util

#endif // SOULNG_UTIL_TRACE_INCLUDED