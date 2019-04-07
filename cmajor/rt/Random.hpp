// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_RANDOM_INCLUDED
#define CMAJOR_RT_RANDOM_INCLUDED
#include <cmajor/rt/RtApi.hpp>
#include <stdint.h>

extern "C" RT_API uint32_t RtGetRandomSeed();
extern "C" RT_API void RtInitRand(uint32_t seed);
extern "C" RT_API uint32_t RtRandom();
extern "C" RT_API uint64_t RtRandom64();

#endif // CMAJOR_RT_RANDOM_INCLUDED
