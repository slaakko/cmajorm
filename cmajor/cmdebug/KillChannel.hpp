// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_KILL_CHANNEL_INCLUDED
#define CMAJOR_DEBUG_KILL_CHANNEL_INCLUDED
#include <cmajor/cmdebug/DebugApi.hpp>

namespace cmajor { namespace debug {

DEBUG_API void StartKillChannel(int port);
DEBUG_API void StopKillChannel();
DEBUG_API void InitKillChannel();
DEBUG_API void DoneKillChannel();

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_KILL_CHANNEL_INCLUDED
