// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/Prime.hpp>
#include <boost/multiprecision/miller_rabin.hpp>

namespace soulng { namespace util {

bool IsPrime64(uint64_t n)
{
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (uint64_t i = 5; i * i <= n; i = i + 6)
    {
        if (n % i == 0 || n % (i + 2) == 0)
        {
            return false;
        }
    }
    return true;
}

void NextPrime(boost::multiprecision::uint128_t& x)
{
    ++x;
    constexpr uint64_t max64 = std::numeric_limits<uint64_t>::max();
    while (x < max64)
    {
        uint64_t n = static_cast<uint64_t>(x);
        if (IsPrime64(n))
        {
            return;
        }
        ++x;
    }
    while (!boost::multiprecision::miller_rabin_test(x, 25))
    {
        ++x;
    }
}

} } // namespace soulng::util
