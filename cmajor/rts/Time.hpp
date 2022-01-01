// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RTS_TIME_INCLUDED
#define CMAJOR_RTS_TIME_INCLUDED

#include <cmajor/rts/RtsApi.hpp>
#include <stdint.h>

extern "C" RT_API int64_t RtNow();
extern "C" RT_API void RtSleep(int64_t nanoSeconds);
extern "C" RT_API void RtGetCurrentDate(int16_t* year, int8_t* month, int8_t* day);
extern "C" RT_API void RtGetCurrentDateTime(int16_t* year, int8_t* month, int8_t* day, int32_t* secs);
extern "C" RT_API int64_t RtGetCurrentTime();

#endif // CMAJOR_RTS_TIME_INCLUDED
