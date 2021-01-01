// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RTS_PROFILE_INCLUDED
#define CMAJOR_RTS_PROFILE_INCLUDED
#include <cmajor/rts/RtsApi.hpp>
#include <stdint.h>

extern "C" RT_API void RtStartProfiling(int64_t numberOfPolymorphicClassIds, const uint64_t* polymorphicClassIdArray, 
    int64_t numberOfStaticClassIds, const uint64_t* staticClassIdArray);
extern "C" RT_API void RtEndProfiling();
extern "C" RT_API void RtProfileStartFunction(void* functionId);
extern "C" RT_API void RtProfileEndFunction(void* functionId);

#endif // CMAJOR_RTS_PROFILE_INCLUDED
