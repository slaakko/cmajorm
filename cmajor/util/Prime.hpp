// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_UTIL_PRIME_INCLUDED
#define CMAJOR_UTIL_PRIME_INCLUDED
#include <cmajor/util/UtilApi.hpp>
#include <stdint.h>

namespace cmajor { namespace util {

// Returns smallest prime greater than or equal to x

UTIL_API uint64_t NextPrime(uint64_t x);

} } // namespace cmajor::util

#endif // CMAJOR_UTIL_PRIME_INCLUDED

