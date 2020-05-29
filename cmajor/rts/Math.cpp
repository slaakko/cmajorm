// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rts/Math.hpp>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#include <limits>

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

double RtPi()
{
    return M_PI;
}

double RtCos(double angleRad)
{
    return std::cos(angleRad);
}

double RtSin(double angleRad)
{
    return std::sin(angleRad);
}

double RtAtan2(double y, double x)
{
    return std::atan2(y, x);
}

double RtEpsilon()
{
    return std::numeric_limits<double>::epsilon();
}