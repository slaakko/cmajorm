// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/Math.hpp>
#include <cmath>

extern "C" RT_API double RtPow(double x, int32_t exponent)
{
    return std::pow(x, exponent);
}