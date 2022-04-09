// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_DEBUG_INCLUDED
#define CMSX_KERNEL_DEBUG_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <string>

namespace cmsx::kernel {

const int debugSystemErrorMode = 1 << 0;    // 1
const int debugTerminalMode = 1 << 1;       // 2
const int debugMsgQueueMode = 1 << 2;       // 4
const int debugAppMode = 1 << 3;            // 8

CMSX_KERNEL_API void SetDebugMode(int debugMode);
CMSX_KERNEL_API int GetDebugMode();
CMSX_KERNEL_API void SetDebugLogPort(int port);
CMSX_KERNEL_API void DebugWrite(const std::string& debugMessage);
CMSX_KERNEL_API void StartDebug();
CMSX_KERNEL_API void StopDebug();
CMSX_KERNEL_API void InitDebug();
CMSX_KERNEL_API void DoneDebug();

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_DEBUG_INCLUDED
