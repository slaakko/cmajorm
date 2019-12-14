// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_RANDOM_INCLUDED
#define SOULNG_UTIL_RANDOM_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <stdint.h>

namespace soulng { namespace util {

UTIL_API void InitMt(uint32_t seed);

UTIL_API uint32_t Random();
UTIL_API uint64_t Random64();

} } // namespace soulng::util

#endif // SOULNG_UTIL_RANDOM_INCLUDED
