// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RTS_COMMAND_LINE_INCLUDED
#define CMAJOR_RTS_COMMAND_LINE_INCLUDED
#include <cmajor/rts/RtsApi.hpp>
#include <stdint.h>

extern "C" RT_API int32_t RtArgc();
extern "C" RT_API const char** RtArgv();

namespace cmajor { namespace rt {

void InitCommandLine();
void DoneCommandLine();

} } // namespace cmajor::rt

#endif // CMAJOR_RTS_COMMAND_LINE_INCLUDED
