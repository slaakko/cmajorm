// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/MemoryStream.hpp>

namespace soulng { namespace util {

MemoryStream::MemoryStream() : Stream(), data(), size(), readPos(0)
{
}

MemoryStream::MemoryStream(uint8_t* data_, int64_t size_) : Stream(), data(data_), size(size_), readPos(0)
{
}

MemoryStream::MemoryStream(uint8_t* data, int64_t count) : Stream(), content(), readPos(0)
{
}

int MemoryStream::ReadByte()
{
    if (readPos < size)
    {
        SetPosition(Position() + 1);
        return data[readPos++];
    }
    return -1;
}

int64_t MemoryStream::Read(uint8_t* buf, int64_t count)
{
    int64_t bytesRead = 0;
    int64_t n = count;
    while (count > 0 && readPos < n)
    {
        *buf++ = data[readPos++];
        ++bytesRead;
        --count;
    }
    SetPosition(Position() + bytesRead);
    return bytesRead;
}

void MemoryStream::Write(uint8_t x)
{
    content.push_back(x);
    SetPosition(Position() + 1);
}

void MemoryStream::Write(uint8_t* buf, int64_t count)
{
    int64_t bytesWritten = 0;
    while (count > 0)
    {
        content.push_back(*buf++);
        --count;
        ++bytesWritten;
    }
    SetPosition(Position() + bytesWritten);
}

void MemoryStream::Seek(int64_t pos, Origin origin)
{
    switch (origin)
    {
        case Origin::seekSet:
        {
            readPos = pos;
            SetPosition(readPos);
            break;
        }
        case Origin::seekCur:
        {
            readPos = readPos + pos;
            SetPosition(readPos);
            break;
        }
        case Origin::seekEnd:
        {
            readPos = size + pos;
            SetPosition(readPos);
            break;
        }
    }
}

int64_t MemoryStream::Tell()
{
    return readPos;
}

void MemoryStream::SetFromContent()
{ 
    data = content.data(); 
    size = content.size(); 
    readPos = 0;
    SetPosition(readPos);
}

} } // namespace soulng::util
