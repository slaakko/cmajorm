// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/Prime.hpp>

namespace soulng { namespace util {
        
bool IsPrime(uint64_t x)
{
    uint64_t i = 3;
    while (true)
    {
        uint64_t q = x / i;
        if (q < i)
        {
            return true;
        }
        if (x == q * i)
        {
            return false;
        }
        i += 2;
    }
}

uint64_t NextPrime(uint64_t x)
{
    if (x <= 2)
    {
        return 2;
    }
    if ((x & 1) == 0)
    {
        ++x;
    }
    while (!IsPrime(x))
    {
        x += 2;
    }
    return x;
}

} } // namespace soulng::util
