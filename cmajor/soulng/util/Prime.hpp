// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_PRIME_INCLUDED
#define SOULNG_UTIL_PRIME_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <boost/multiprecision/cpp_int.hpp>

namespace soulng { namespace util {

// Returns smallest prime greater than x

UTIL_API void NextPrime(boost::multiprecision::uint128_t& x);

} } // namespace soulng::util

#endif // SOULNG_UTIL_PRIME_INCLUDED
