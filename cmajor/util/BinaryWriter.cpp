// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/util/BinaryWriter.hpp>
#include <cmajor/util/Unicode.hpp>
#include <cstring>
 
namespace cmajor { namespace util {

using namespace cmajor::unicode;

BinaryWriter::BinaryWriter(const std::string& fileName_) : fileName(fileName_), file(std::fopen(fileName.c_str(), "wb")), bufp(buffer), bufend(buffer + N), pos(0)
{
    if (!file)
    {
        throw std::runtime_error("could not open '" + fileName + "' for writing: " + strerror(errno));
    }
}

BinaryWriter::~BinaryWriter()
{
    FlushBuffer();
}

void BinaryWriter::Write(bool x)
{
    Write(uint8_t(x));
}

void BinaryWriter::Write(uint8_t x)
{
    if (BufferFull())
    {
        FlushBuffer();
    }
    *bufp++ = x;
    ++pos;
}

void BinaryWriter::Write(int8_t x)
{
    Write(static_cast<uint8_t>(x));
}

void BinaryWriter::Write(uint16_t x)
{
    uint8_t b0 = static_cast<uint8_t>(x >> 8);
    uint8_t b1 = static_cast<uint8_t>(x);
    Write(b0);
    Write(b1);
}

void BinaryWriter::Write(int16_t x)
{
    Write(static_cast<uint16_t>(x));
}

void BinaryWriter::Write(uint32_t x)
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

void BinaryWriter::Write(int32_t x)
{
    Write(static_cast<uint32_t>(x));
}

void BinaryWriter::Write(uint64_t x)
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

void BinaryWriter::Write(int64_t x)
{
    Write(static_cast<uint64_t>(x));
}

void BinaryWriter::Write(float x)
{
    uint32_t* u = reinterpret_cast<uint32_t*>(&x);
    Write(*u);
}

void BinaryWriter::Write(double x)
{
    uint64_t* u = reinterpret_cast<uint64_t*>(&x);
    Write(*u);
}

void BinaryWriter::Write(char x)
{
    Write(static_cast<uint8_t>(x));
}

void BinaryWriter::Write(char16_t x)
{
    Write(static_cast<uint16_t>(x));
}

void BinaryWriter::Write(char32_t x)
{
    Write(static_cast<uint32_t>(x));
}

void BinaryWriter::Write(const std::string& s)
{
    for (char c : s)
    {
        uint8_t x = static_cast<uint8_t>(c);
        Write(x);
    }
    Write(static_cast<uint8_t>(0));
}

void BinaryWriter::Write(const std::u16string& s)
{
    std::string utf8_str = ToUtf8(s);
    Write(utf8_str);
}

void BinaryWriter::Write(const std::u32string& s)
{
    std::string utf8_str = ToUtf8(s);
    Write(utf8_str);
}

void BinaryWriter::WriteULEB128UInt(uint32_t x)
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
    } 
    while (x != 0);
}

void BinaryWriter::WriteULEB128ULong(uint64_t x)
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
    } 
    while (x != 0);
}

void BinaryWriter::WriteSLEB128Int(int32_t x)
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

void BinaryWriter::WriteSLEB128Long(int64_t x)
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

void BinaryWriter::Write(const boost::uuids::uuid& uuid)
{
    for (boost::uuids::uuid::value_type x : uuid)
    {
        Write(x);
    }
}

void BinaryWriter::Seek(uint32_t pos_)
{
    FlushBuffer();
    pos = pos_;
    int result = fseek(file, pos, SEEK_SET);
    if (result != 0)
    {
        throw std::runtime_error("seek failed: " + std::string(strerror(errno)));
    }
}

void BinaryWriter::FlushBuffer()
{
    if (bufp != buffer)
    {
        size_t n = bufp - buffer;
        size_t numWritten = fwrite(buffer, 1, n, file);
        if (numWritten != n)
        {
            throw std::runtime_error("could not write to '" + fileName + "': " + strerror(errno));
        }
        BufferReset();
    }
}

} } // namespace cmajor::util

