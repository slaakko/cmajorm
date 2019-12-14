// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_MEMORY_READER_INCLUDED
#define SOULNG_UTIL_MEMORY_READER_INCLUDED
#include <soulng/util/Time.hpp>
#include <soulng/util/UtilApi.hpp>
#include <stdint.h>

namespace soulng { namespace util {

class UTIL_API MemoryReader
{
public:
    MemoryReader(uint8_t* ptr_, int64_t count_);
    uint8_t ReadByte();
    int8_t ReadSByte();
    uint16_t ReadUShort();
    int16_t ReadShort();
    uint32_t ReadUInt();
    int32_t ReadInt();
    uint64_t ReadULong();
    int64_t ReadLong();
    DateTime ReadDateTime();
private:
    uint8_t* ptr;
    uint8_t* pos;
    int64_t count;
};

} } // namespace soulng::util

#endif // SOULNG_UTIL_MEMORY_READER_INCLUDED
