// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/BufferedStream.hpp>

namespace soulng { namespace util {

BufferedStream::BufferedStream(Stream& baseStream_) : BufferedStream(baseStream_, 4096)
{
}

BufferedStream::BufferedStream(Stream& baseStream_, int64_t bufferSize_) : 
    Stream(), baseStream(baseStream_), bufferSize(bufferSize_), buffer(new uint8_t[bufferSize]), pos(bufferSize), bytesAvailable(0), end(0)
{
}

BufferedStream::~BufferedStream()
{
    try
    {
        Flush();
    }
    catch (...)
    {
    }
}

int BufferedStream::ReadByte()
{
    Flush();
    if (bytesAvailable == 0)
    {
        FillBuf();
        if (bytesAvailable == 0)
        {
            return -1;
        }
    }
    uint8_t* b = buffer.get();
    uint8_t value = b[pos++];
    --bytesAvailable;
    SetPosition(Position() + 1);
    return value;
}

int64_t BufferedStream::Read(uint8_t* buf, int64_t count)
{
    Flush();
    if (bytesAvailable == 0)
    {
        FillBuf();
    }
    int64_t bytesRead = 0;
    int64_t n = std::min(bytesAvailable, count);
    for (int64_t i = 0; i < n; ++i)
    {
        uint8_t* b = buffer.get();
        buf[i] = b[pos++];
        ++bytesRead;
        --bytesAvailable;
    }
    SetPosition(Position() + bytesRead);
    return bytesRead;
}

void BufferedStream::Write(uint8_t x)
{
    if (end >= bufferSize)
    {
        Flush();
    }
    uint8_t* b = buffer.get();
    b[end++] = x;
    SetPosition(Position() + 1);
}

void BufferedStream::Write(uint8_t* buf, int64_t count)
{
    for (int64_t i = 0; i < count; ++i)
    {
        Write(buf[i]);
    }
}

void BufferedStream::Flush()
{
    if (end != 0)
    {
        baseStream.Write(buffer.get(), end);
        baseStream.Flush();
        end = 0;
    }
}

void BufferedStream::Seek(int64_t pos, Origin origin)
{
    Flush();
    bytesAvailable = 0;
    baseStream.Seek(pos, origin);
}

int64_t BufferedStream::Tell()
{
    Flush();
    return baseStream.Tell() - bytesAvailable;
}

void BufferedStream::FillBuf()
{
    bytesAvailable = baseStream.Read(buffer.get(), bufferSize);
    pos = 0;
}

} } // namespace soulng::util
