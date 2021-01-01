// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/Trace.hpp>
#include <soulng/util/Trace.hpp>

void RtBeginTracing()
{
    soulng::util::BeginTracing();
}

void RtEndTracing()
{
    soulng::util::EndTracing();
}

void RtSetThreadId(char threadId)
{
    soulng::util::SetThreadId(threadId);
}

void RtBeginTraceFunction(int32_t traceFunctionId)
{
    soulng::util::AddTraceEntry(soulng::util::EntryKind::begin, traceFunctionId);
}

void RtEndTraceFunction(int32_t traceFunctionId)
{
    soulng::util::AddTraceEntry(soulng::util::EntryKind::end, traceFunctionId);
}

void RtWindowsMessage(int32_t messageId)
{
    soulng::util::AddTraceEntry(soulng::util::EntryKind::message, messageId);
}
