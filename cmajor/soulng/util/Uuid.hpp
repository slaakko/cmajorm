// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_UUID_INCLUDED
#define SOULNG_UTIL_UUID_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <boost/uuid/uuid.hpp>
#include <stdint.h>

namespace soulng { namespace util {

UTIL_API void UuidToInts(const boost::uuids::uuid& id, uint64_t& int1, uint64_t& int2);
UTIL_API void IntsToUuid(uint64_t int1, uint64_t int2, boost::uuids::uuid& id);

} } // namespace soulng::util

#endif // SOULNG_UTIL_UUID_INCLUDED
