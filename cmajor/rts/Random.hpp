// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RTS_RANDOM_INCLUDED
#define CMAJOR_RTS_RANDOM_INCLUDED
#include <cmajor/rts/RtsApi.hpp>
#include <stdint.h>

extern "C" RT_API uint32_t RtGetRandomSeed();
extern "C" RT_API void RtInitRand(uint32_t seed);
extern "C" RT_API uint32_t RtRandom();
extern "C" RT_API uint64_t RtRandom64();

#endif // CMAJOR_RTS_RANDOM_INCLUDED
