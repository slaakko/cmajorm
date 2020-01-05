// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_PRIME_INCLUDED
#define SOULNG_UTIL_PRIME_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <stdint.h>

namespace soulng { namespace util {

// Returns smallest prime greater than or equal to x

UTIL_API uint64_t NextPrime(uint64_t x);

} } // namespace soulng::util

#endif // SOULNG_UTIL_PRIME_INCLUDED


