// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/util/MemoryReader.hpp>
#include <stdexcept>

namespace cmajor { namespace util {

MemoryReader::MemoryReader(const uint8_t* begin_, const uint8_t* end_) : begin(begin_), end(end_), pos(0)
{
}

uint8_t MemoryReader::GetByte()
{
    if (begin >= end)
    {
        throw std::runtime_error("MemoryReader: unexpected end of data");
    }
    ++pos;
    return *begin++;
}

int8_t MemoryReader::GetSByte()
{
    uint8_t x = GetByte();
    return static_cast<int8_t>(x);
}

uint16_t MemoryReader::GetUShort()
{
    uint8_t x[2];
    x[0] = GetByte();
    x[1] = GetByte();
    return *reinterpret_cast<uint16_t*>(&x[0]);
}

int16_t MemoryReader::GetShort()
{
    uint16_t x = GetUShort();
    return static_cast<int16_t>(x);
}

uint32_t MemoryReader::GetUInt()
{
    uint8_t x[4];
    x[0] = GetByte();
    x[1] = GetByte();
    x[2] = GetByte();
    x[3] = GetByte();
    return *reinterpret_cast<uint32_t*>(&x[0]);
}

int32_t MemoryReader::GetInt()
{
    uint32_t x = GetUInt();
    return static_cast<int32_t>(x);
}

uint64_t MemoryReader::GetULong()
{
    uint8_t x[8];
    x[0] = GetByte();
    x[1] = GetByte();
    x[2] = GetByte();
    x[3] = GetByte();
    x[4] = GetByte();
    x[5] = GetByte();
    x[6] = GetByte();
    x[7] = GetByte();
    return *reinterpret_cast<uint64_t*>(&x[0]);
}

int64_t MemoryReader::GetLong()
{
    uint64_t x = GetULong();
    return static_cast<int64_t>(x);
}

} } // namespace cmajor::util
