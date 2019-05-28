// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_UTIL_MEMORY_READER_INCLUDED
#define CMAJOR_UTIL_MEMORY_READER_INCLUDED
#include <cmajor/util/Time.hpp>
#include <cmajor/util/UtilApi.hpp>
#include <stdint.h>

namespace cmajor { namespace util {

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

} } // namespace cmajor::util

#endif // CMAJOR_UTIL_MEMORY_READER_INCLUDED
