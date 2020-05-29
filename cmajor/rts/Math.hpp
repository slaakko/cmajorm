// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_MATH_INCLUDED
#define CMAJOR_RT_MATH_INCLUDED
#include <cmajor/rts/RtsApi.hpp>
#include <stdint.h>

extern "C" RT_API double RtPow(double x, int32_t exponent);
extern "C" RT_API double Pow(double x, double y);
extern "C" RT_API double RtSqrt(double x);
extern "C" RT_API double RtPi();
extern "C" RT_API double RtCos(double angleRad);
extern "C" RT_API double RtSin(double angleRad);
extern "C" RT_API double RtAtan2(double y, double x);
extern "C" RT_API double RtEpsilon();

#endif // CMAJOR_RT_MATH_INCLUDED