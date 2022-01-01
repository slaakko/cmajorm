// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_PROFILE_INCLUDED
#define CMAJOR_RT_PROFILE_INCLUDED
#include <cmajor/rt/RtApi.hpp>
#include <stdint.h>

extern "C" RT_API void RtStartProfiling(int64_t numberOfPolymorphicClassIds, const uint64_t* polymorphicClassIdArray, 
    int64_t numberOfStaticClassIds, const uint64_t* staticClassIdArray);
extern "C" RT_API void RtEndProfiling();
extern "C" RT_API void RtProfileStartFunction(void* functionId);
extern "C" RT_API void RtProfileEndFunction(void* functionId);

#endif // CMAJOR_RT_PROFILE_INCLUDED
