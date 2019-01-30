// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_UTIL_RANDOM_INCLUDED
#define CMAJOR_UTIL_RANDOM_INCLUDED
#include <cmajor/util/UtilApi.hpp>
#include <stdint.h>

namespace cmajor { namespace util {

UTIL_API void InitMt(uint32_t seed);

UTIL_API uint32_t Random();
UTIL_API uint64_t Random64();

} } // namespace cmajor::util

#endif // CMAJOR_UTIL_RANDOM_INCLUDED
