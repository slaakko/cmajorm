// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/BinaryReader.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Error.hpp>
#include <cstring>

namespace soulng { namespace util {

using namespace soulng::unicode;

BinaryReader::BinaryReader(const std::string& fileName_) : 
    fileName(fileName_), file(fileName), begin(reinterpret_cast<const uint8_t*>(file.Begin())), end(reinterpret_cast<const uint8_t*>(file.End())), pos(0)
{
}

BinaryReader::~BinaryReader()
{
}

bool BinaryReader::ReadBool()
{
    uint8_t x = ReadByte();
    return static_cast<bool>(x);
}

uint8_t BinaryReader::ReadByte()
{
    CheckEof();
    uint8_t x = *begin++;
    ++pos;
    return x;
}

int8_t BinaryReader::ReadSByte()
{
    uint8_t x = ReadByte();
    return static_cast<int8_t>(x);
}

uint16_t BinaryReader::ReadUShort()
{
    uint8_t h = ReadByte();
    uint8_t l = ReadByte();
    return (static_cast<uint16_t>(h) << 8) | static_cast<uint16_t>(l);
}

int16_t BinaryReader::ReadShort()
{
    uint16_t x = ReadUShort();
    return static_cast<int16_t>(x);
}

uint32_t BinaryReader::ReadUInt()
{
    uint8_t b0 = ReadByte();
    uint8_t b1 = ReadByte();
    uint8_t b2 = ReadByte();
    uint8_t b3 = ReadByte();
    return (static_cast<uint32_t>(b0) << 24) | (static_cast<uint32_t>(b1) << 16) | (static_cast<uint32_t>(b2) << 8) | static_cast<uint32_t>(b3);
}

int32_t BinaryReader::ReadInt()
{
    uint32_t x = ReadUInt();
    return static_cast<int32_t>(x);
}

uint64_t BinaryReader::ReadULong()
{
    uint8_t b0 = ReadByte();
    uint8_t b1 = ReadByte();
    uint8_t b2 = ReadByte();
    uint8_t b3 = ReadByte();
    uint8_t b4 = ReadByte();
    uint8_t b5 = ReadByte();
    uint8_t b6 = ReadByte();
    uint8_t b7 = ReadByte();
    return (static_cast<uint64_t>(b0) << 56) | (static_cast<uint64_t>(b1) << 48) | (static_cast<uint64_t>(b2) << 40) | (static_cast<uint64_t>(b3) << 32) | (static_cast<uint64_t>(b4) << 24) |
        (static_cast<uint64_t>(b5) << 16) | (static_cast<uint64_t>(b6) << 8) | static_cast<uint64_t>(b7);
}

int64_t BinaryReader::ReadLong()
{
    uint64_t x = ReadULong();
    return static_cast<int64_t>(x);
}

float BinaryReader::ReadFloat()
{
    uint32_t x = ReadUInt();
    return *reinterpret_cast<float*>(&x);
}

double BinaryReader::ReadDouble()
{
    uint64_t x = ReadULong();
    return *reinterpret_cast<double*>(&x);
}

char BinaryReader::ReadChar()
{
    uint8_t x = ReadByte();
    return static_cast<char>(x);
}

char16_t BinaryReader::ReadWChar()
{
    uint16_t x = ReadUShort();
    return static_cast<char16_t>(x);
}

char32_t BinaryReader::ReadUChar()
{
    uint32_t x = ReadUInt();
    return static_cast<char32_t>(x);
}

std::string BinaryReader::ReadUtf8String()
{
    std::string s;
    uint8_t x = ReadByte();
    while (x != 0)
    {
        s.append(1, static_cast<char>(x));
        x = ReadByte();
    }
    return s;
}

std::u16string BinaryReader::ReadUtf16String()
{
    std::string s = ReadUtf8String();
    return ToUtf16(s);
}

std::u32string BinaryReader::ReadUtf32String()
{
    std::string s = ReadUtf8String();
    return ToUtf32(s);
}

uint32_t BinaryReader::ReadULEB128UInt()
{
    uint32_t result = 0;
    uint32_t shift = 0;
    while (true)
    {
        uint8_t b = ReadByte();
        result |= ((b & 0x7F) << shift);
        if ((b & 0x80) == 0) break;
        shift += 7;
    }
    return result;
}

uint64_t BinaryReader::ReadULEB128ULong()
{
    uint64_t result = 0;
    uint64_t shift = 0;
    while (true)
    {
        uint8_t b = ReadByte();
        result |= ((b & 0x7F) << shift);
        if ((b & 0x80) == 0) break;
        shift += 7;
    }
    return result;
}

int32_t BinaryReader::ReadSLEB128Int()
{
    int32_t result = 0;
    int32_t shift = 0;
    uint8_t b = 0;
    do
    {
        b = ReadByte();
        result |= (b & 0x7F) << shift;
        shift += 7;
    } 
    while ((b & 0x80) != 0);
    if ((shift < 32) && (b & 0x40) != 0)
    {
        result |= ~int32_t(0) << shift;
    }
    return result;
}

int64_t BinaryReader::ReadSLEB128Long()
{
    int64_t result = 0;
    int64_t shift = 0;
    uint8_t b = 0;
    do 
    {
        b = ReadByte();
        result |= (b & 0x7F) << shift;
        shift += 7;
    }
    while ((b & 0x80) != 0);
    if ((shift < 64) && (b & 0x40) != 0)
    {
        result |= ~int64_t(0) << shift;
    }
    return result;
}

void BinaryReader::ReadUuid(boost::uuids::uuid& uuid)
{
    for (boost::uuids::uuid::value_type& x : uuid)
    {
        x = ReadByte();
    }
}

void BinaryReader::Skip(uint32_t size)
{
    begin += size;
    pos += size;
}

void BinaryReader::CheckEof()
{
    if (begin == end)
    {
        throw std::runtime_error("unexpected end of file '" + fileName + "'");
    }
}

} } // namespace soulng::util
