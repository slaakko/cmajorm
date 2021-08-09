// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/BinaryStreamWriter.hpp>
#include <soulng/util/Unicode.hpp>

namespace soulng { namespace util {

using namespace soulng::unicode;

BinaryStreamWriter::BinaryStreamWriter(Stream& stream_) : stream(stream_)
{
}

void BinaryStreamWriter::Write(bool x)
{
    Write(uint8_t(x));
}

void BinaryStreamWriter::Write(uint8_t x)
{
    stream.Write(x);
}

void BinaryStreamWriter::Write(int8_t x)
{
    Write(static_cast<uint8_t>(x));
}

void BinaryStreamWriter::Write(uint16_t x)
{
    uint8_t b0 = static_cast<uint8_t>(x >> 8);
    uint8_t b1 = static_cast<uint8_t>(x);
    Write(b0);
    Write(b1);
}

void BinaryStreamWriter::Write(int16_t x)
{
    Write(static_cast<uint16_t>(x));
}

void BinaryStreamWriter::Write(uint32_t x)
{
    uint8_t b0 = static_cast<uint8_t>(x >> 24);
    uint8_t b1 = static_cast<uint8_t>(x >> 16);
    uint8_t b2 = static_cast<uint8_t>(x >> 8);
    uint8_t b3 = static_cast<uint8_t>(x);
    Write(b0);
    Write(b1);
    Write(b2);
    Write(b3);
}

void BinaryStreamWriter::Write(int32_t x)
{
    Write(static_cast<uint32_t>(x));
}

void BinaryStreamWriter::Write(uint64_t x)
{
    uint8_t b0 = static_cast<uint8_t>(x >> 56);
    uint8_t b1 = static_cast<uint8_t>(x >> 48);
    uint8_t b2 = static_cast<uint8_t>(x >> 40);
    uint8_t b3 = static_cast<uint8_t>(x >> 32);
    uint8_t b4 = static_cast<uint8_t>(x >> 24);
    uint8_t b5 = static_cast<uint8_t>(x >> 16);
    uint8_t b6 = static_cast<uint8_t>(x >> 8);
    uint8_t b7 = static_cast<uint8_t>(x);
    Write(b0);
    Write(b1);
    Write(b2);
    Write(b3);
    Write(b4);
    Write(b5);
    Write(b6);
    Write(b7);
}

void BinaryStreamWriter::Write(int64_t x)
{
    Write(static_cast<uint64_t>(x));
}

void BinaryStreamWriter::Write(float x)
{
    uint32_t* u = reinterpret_cast<uint32_t*>(&x);
    Write(*u);
}

void BinaryStreamWriter::Write(double x)
{
    uint64_t* u = reinterpret_cast<uint64_t*>(&x);
    Write(*u);
}

void BinaryStreamWriter::Write(char x)
{
    Write(static_cast<uint8_t>(x));
}

void BinaryStreamWriter::Write(char16_t x)
{
    Write(static_cast<uint16_t>(x));
}

void BinaryStreamWriter::Write(char32_t x)
{
    Write(static_cast<uint32_t>(x));
}

void BinaryStreamWriter::Write(const std::string& s)
{
    Write(s, true);
}

void BinaryStreamWriter::Write(const std::string& s, bool writeNull)
{
    for (char c : s)
    {
        uint8_t x = static_cast<uint8_t>(c);
        Write(x);
    }
    if (writeNull)
    {
        Write(static_cast<uint8_t>(0));
    }
}

void BinaryStreamWriter::Write(const std::u16string& s)
{
    std::string utf8_str = ToUtf8(s);
    Write(utf8_str);
}

void BinaryStreamWriter::Write(const std::u32string& s)
{
    std::string utf8_str = ToUtf8(s);
    Write(utf8_str);
}

void BinaryStreamWriter::WriteULEB128UInt(uint32_t x)
{
    do
    {
        uint8_t b = x & 0x7F;
        x >>= 7;
        if (x != 0)
        {
            b |= 0x80;
        }
        Write(b);
    } while (x != 0);
}

void BinaryStreamWriter::WriteULEB128ULong(uint64_t x)
{
    do
    {
        uint8_t b = x & 0x7F;
        x >>= 7;
        if (x != 0)
        {
            b |= 0x80;
        }
        Write(b);
    } while (x != 0);
}

void BinaryStreamWriter::WriteSLEB128Int(int32_t x)
{
    bool more = true;
    bool negative = x < 0;
    while (more)
    {
        uint8_t b = x & 0x7F;
        x >>= 7;
        if (negative)
        {
            x |= ~int32_t(0) << (32 - 7);
        }
        if (x == 0 && (b & 0x40) == 0 || x == -1 && (b & 0x40) != 0)
        {
            more = false;
        }
        else
        {
            b |= 0x80;
        }
        Write(b);
    }
}

void BinaryStreamWriter::WriteSLEB128Long(int64_t x)
{
    bool more = true;
    bool negative = x < 0;
    while (more)
    {
        uint8_t b = x & 0x7F;
        x >>= 7;
        if (negative)
        {
            x |= ~int64_t(0) << (64 - 7);
        }
        if (x == 0 && (b & 0x40) == 0 || x == -1 && (b & 0x40) != 0)
        {
            more = false;
        }
        else
        {
            b |= 0x80;
        }
        Write(b);
    }
}

void BinaryStreamWriter::Write(const boost::uuids::uuid& uuid)
{
    for (boost::uuids::uuid::value_type x : uuid)
    {
        Write(x);
    }
}

void BinaryStreamWriter::WriteTime(time_t time)
{
    Write(static_cast<int64_t>(time));
}

} } // namespace soulng::util
