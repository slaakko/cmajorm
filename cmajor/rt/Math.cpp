// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/Math.hpp>
#include <cmath>

double RtPow(double x, int32_t exponent)
{
    return std::pow(x, exponent);
}

double Pow(double x, double y)
{
    return std::pow(x, y);
}

double RtSqrt(double x)
{
    return std::sqrt(x);
}
