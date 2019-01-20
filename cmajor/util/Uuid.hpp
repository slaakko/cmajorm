// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_UTIL_UUID_INCLUDED
#define CMAJOR_UTIL_UUID_INCLUDED
#include <boost/uuid/uuid.hpp>
#include <stdint.h>

namespace cmajor { namespace util {

void UuidToInts(const boost::uuids::uuid& id, uint64_t& int1, uint64_t& int2);
void IntsToUuid(uint64_t int1, uint64_t int2, boost::uuids::uuid& id);

} } // namespace cmajor::util

#endif // CMAJOR_UTIL_UUID_INCLUDED
