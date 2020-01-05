// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/Math.hpp>
#include <cmath>

extern "C" RT_API double RtPow(double x, int32_t exponent)
{
    return std::pow(x, exponent);
}

extern "C" RT_API double Pow(double x, double y)
{
    return std::pow(x, y);
}
