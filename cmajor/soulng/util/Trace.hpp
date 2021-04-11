// =================================
// Copyright (c) 2021 Seppo Laakko
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

struct TraceEntry
{
    TraceEntry(EntryKind kind_, char threadId_, int32_t id_, int64_t nanosecs_);
    EntryKind kind;
    char threadId;
    int32_t id;
    int64_t nanosecs;
};

UTIL_API void BeginTracing();
UTIL_API void EndTracing();
UTIL_API void SetThreadId(char threadId_);
UTIL_API int32_t GetTraceFunctionId(const std::string& functionFullName);
UTIL_API const std::string& GetTraceFunctionName(int32_t traceFunctionId);
UTIL_API const std::string& GetTraceMessageName(int32_t traceMessageId);
UTIL_API void AddTraceEntry(EntryKind kind, int32_t id);
UTIL_API std::vector<TraceEntry> ReadTrace(const std::string& traceFilePath);
UTIL_API void ReadTraceTable();
UTIL_API void WriteTraceTable();
UTIL_API void InitTrace();
UTIL_API void DoneTrace();

class UTIL_API Tracer
{
public:
    Tracer(int32_t traceFunctionId_);
    ~Tracer();
private:
    int32_t traceFunctionId;
};

} } // namespace soulng::util

#endif // SOULNG_UTIL_TRACE_INCLUDED
