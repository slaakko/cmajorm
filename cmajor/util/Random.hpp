// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_UTIL_RANDOM_INCLUDED
#define CMAJOR_UTIL_RANDOM_INCLUDED
#include <stdint.h>

namespace cmajor { namespace util {

void InitMt(uint32_t seed);

uint32_t Random();
uint64_t Random64();

} } // namespace cmajor::util

#endif // CMAJOR_UTIL_RANDOM_INCLUDED
