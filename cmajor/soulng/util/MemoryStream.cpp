// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/MemoryStream.hpp>

namespace soulng { namespace util {

MemoryStream::MemoryStream() : Stream(), content(), readPos(0)
{
}

MemoryStream::MemoryStream(uint8_t* data, int64_t count) : Stream(), content(), readPos(0)
{
    Write(data, count);
}

int MemoryStream::ReadByte()
{
    if (readPos < int64_t(content.size()))
    {
        return content[readPos++];
    }
    return -1;
}

int64_t MemoryStream::Read(uint8_t* buf, int64_t count)
{
    int64_t bytesRead = 0;
    int64_t n = content.size();
    while (count > 0 && readPos < n)
    {
        *buf++ = content[readPos++];
        ++bytesRead;
        --count;
    }
    return bytesRead;
}

void MemoryStream::Write(uint8_t x)
{
    content.push_back(x);
}

void MemoryStream::Write(uint8_t* buf, int64_t count)
{
    while (count > 0)
    {
        content.push_back(*buf++);
        --count;
    }
}

void MemoryStream::Seek(int64_t pos, Origin origin)
{
    switch (origin)
    {
        case Origin::seekSet:
        {
            readPos = pos;
            break;
        }
        case Origin::seekCur:
        {
            readPos = readPos + pos;
            break;
        }
        case Origin::seekEnd:
        {
            readPos = content.size() + pos;
            break;
        }
    }
}

int64_t MemoryStream::Tell()
{
    return readPos;
}

} } // namespace soulng::util
