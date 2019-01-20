// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_UTIL_MEMORY_READER_INCLUDED
#define CMAJOR_UTIL_MEMORY_READER_INCLUDED
#include <stdint.h>

namespace cmajor { namespace util {

class MemoryReader
{
public:
    MemoryReader(const uint8_t* begin_, const uint8_t* end_);
    uint8_t GetByte();
    int8_t GetSByte();
    uint16_t GetUShort();
    int16_t GetShort();
    uint32_t GetUInt();
    int32_t GetInt();
    uint64_t GetULong();
    int64_t GetLong();
    uint32_t Pos() const { return pos; }
private:
    const uint8_t* begin;
    const uint8_t* end;
    uint32_t pos;
};

} } // namespace cmajor::util

#endif // CMAJOR_UTIL_MEMORY_READER_INCLUDED
