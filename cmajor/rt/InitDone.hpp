// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_INIT_DONE_INCLUDED
#define CMAJOR_RT_INIT_DONE_INCLUDED
#include <cmajor/rt/RtApi.hpp>
#include <stdint.h>

using GlobalInitFunctionType = void(*)();
extern "C" RT_API void RtInit(int64_t numberOfPolymorphicClassIds, const uint64_t* polymorphicClassIdArray, int64_t numberOfStaticClassIds, const uint64_t* staticClassIdArray,
    GlobalInitFunctionType globalInitializationFunction);
extern "C" RT_API void RtDone();
extern "C" RT_API bool DynamicInitVmtsAndCompare(void* vmt1, void* vmt2);
extern "C" RT_API void RtExit(int32_t exitCode);
extern "C" RT_API void RtBeginUnwindInfoInit();
extern "C" RT_API void RtEndUnwindInfoInit();

namespace cmajor { namespace rt {

void Init(int64_t numberOfPolymorphicClassIds, const uint64_t* polymorphicClassIdArray, int64_t numberOfStaticClassIds, const uint64_t* staticClassIdArray);
void Done();

} } // namespace cmajor::rt

#endif // CMAJOR_RT_INIT_DONE_INCLUDED
