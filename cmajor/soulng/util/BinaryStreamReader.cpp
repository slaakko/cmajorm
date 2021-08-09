// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/BinaryStreamReader.hpp>
#include <soulng/util/Unicode.hpp>
#include <stdexcept>

namespace soulng { namespace util {

using namespace soulng::unicode;

BinaryStreamReader::BinaryStreamReader(Stream& stream_) : stream(stream_)
{
}

bool BinaryStreamReader::ReadBool()
{
    return static_cast<bool>(ReadByte());
}

uint8_t BinaryStreamReader::ReadByte()
{
    int x = stream.ReadByte();
    if (x == -1)
    {
        throw std::runtime_error("unexpected end of stream");
    }
    return static_cast<uint8_t>(x);
}

int8_t BinaryStreamReader::ReadSByte()
{
    return static_cast<int8_t>(ReadByte());
}

uint16_t BinaryStreamReader::ReadUShort()
{
    uint8_t h = ReadByte();
    uint8_t l = ReadByte();
    return (static_cast<uint16_t>(h) << 8) | static_cast<uint16_t>(l);
}

int16_t BinaryStreamReader::ReadShort()
{
    uint16_t x = ReadUShort();
    return static_cast<int16_t>(x);
}

uint32_t BinaryStreamReader::ReadUInt()
{
    uint8_t b0 = ReadByte();
    uint8_t b1 = ReadByte();
    uint8_t b2 = ReadByte();
    uint8_t b3 = ReadByte();
    return (static_cast<uint32_t>(b0) << 24) | (static_cast<uint32_t>(b1) << 16) | (static_cast<uint32_t>(b2) << 8) | static_cast<uint32_t>(b3);
}

int32_t BinaryStreamReader::ReadInt()
{
    uint32_t x = ReadUInt();
    return static_cast<int32_t>(x);
}

uint64_t BinaryStreamReader::ReadULong()
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

int64_t BinaryStreamReader::ReadLong()
{
    uint64_t x = ReadULong();
    return static_cast<int64_t>(x);
}

float BinaryStreamReader::ReadFloat()
{
    uint32_t x = ReadUInt();
    return *reinterpret_cast<float*>(&x);
}

double BinaryStreamReader::ReadDouble()
{
    uint64_t x = ReadULong();
    return *reinterpret_cast<double*>(&x);
}

char BinaryStreamReader::ReadChar()
{
    uint8_t x = ReadByte();
    return static_cast<char>(x);
}

char16_t BinaryStreamReader::ReadWChar()
{
    uint16_t x = ReadUShort();
    return static_cast<char16_t>(x);
}

char32_t BinaryStreamReader::ReadUChar()
{
    uint32_t x = ReadUInt();
    return static_cast<char32_t>(x);
}

std::string BinaryStreamReader::ReadUtf8String()
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

std::u16string BinaryStreamReader::ReadUtf16String()
{
    std::string s = ReadUtf8String();
    return ToUtf16(s);
}

std::u32string BinaryStreamReader::ReadUtf32String()
{
    std::string s = ReadUtf8String();
    return ToUtf32(s);
}

uint32_t BinaryStreamReader::ReadULEB128UInt()
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

uint64_t BinaryStreamReader::ReadULEB128ULong()
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

int32_t BinaryStreamReader::ReadSLEB128Int()
{
    int32_t result = 0;
    int32_t shift = 0;
    uint8_t b = 0;
    do
    {
        b = ReadByte();
        result |= (b & 0x7F) << shift;
        shift += 7;
    } while ((b & 0x80) != 0);
    if ((shift < 32) && (b & 0x40) != 0)
    {
        result |= ~int32_t(0) << shift;
    }
    return result;
}

int64_t BinaryStreamReader::ReadSLEB128Long()
{
    int64_t result = 0;
    int64_t shift = 0;
    uint8_t b = 0;
    do
    {
        b = ReadByte();
        result |= (b & 0x7F) << shift;
        shift += 7;
    } while ((b & 0x80) != 0);
    if ((shift < 64) && (b & 0x40) != 0)
    {
        result |= ~int64_t(0) << shift;
    }
    return result;
}

void BinaryStreamReader::ReadUuid(boost::uuids::uuid& uuid)
{
    for (boost::uuids::uuid::value_type& x : uuid)
    {
        x = ReadByte();
    }
}

time_t BinaryStreamReader::ReadTime()
{
    return static_cast<time_t>(ReadLong());
}

} } // namespace soulng::util
