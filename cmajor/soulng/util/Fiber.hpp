// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_FIBER_INCLUDED
#define SOULNG_UTIL_FIBER_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <stdint.h>

namespace soulng { namespace util {

UTIL_API void* ConvertThreadToFiber(void* param);
UTIL_API void* CreateFiber(uint64_t stackSize, void* startAddress, void* param);
UTIL_API void SwitchToFiber(void* fiber);
UTIL_API void* GetFiberData();
UTIL_API void DeleteFiber(void* fiber);

} } // namespace soulng::util

#endif // SOULNG_UTIL_FIBER_INCLUDED
