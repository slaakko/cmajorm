// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/MemoryStream.hpp>

namespace soulng { namespace util {

<<<<<<< HEAD
MemoryStream::MemoryStream() : Stream(), data(), size(), readPos(0)
{
}

MemoryStream::MemoryStream(uint8_t* data_, int64_t size_) : Stream(), data(data_), size(size_), readPos(0)
{
=======
MemoryStream::MemoryStream() : Stream(), content(), readPos(0)
{
}

MemoryStream::MemoryStream(uint8_t* data, int64_t count) : Stream(), content(), readPos(0)
{
    Write(data, count);
>>>>>>> c8b260dfbbd561b2e053dafc4b764c71755fbac7
}

int MemoryStream::ReadByte()
{
<<<<<<< HEAD
    if (readPos < size)
    {
        SetPosition(Position() + 1);
        return data[readPos++];
=======
    if (readPos < int64_t(content.size()))
    {
        return content[readPos++];
>>>>>>> c8b260dfbbd561b2e053dafc4b764c71755fbac7
    }
    return -1;
}

int64_t MemoryStream::Read(uint8_t* buf, int64_t count)
{
    int64_t bytesRead = 0;
<<<<<<< HEAD
    int64_t n = count;
    while (count > 0 && readPos < n)
    {
        *buf++ = data[readPos++];
        ++bytesRead;
        --count;
    }
    SetPosition(Position() + bytesRead);
=======
    int64_t n = content.size();
    while (count > 0 && readPos < n)
    {
        *buf++ = content[readPos++];
        ++bytesRead;
        --count;
    }
>>>>>>> c8b260dfbbd561b2e053dafc4b764c71755fbac7
    return bytesRead;
}

void MemoryStream::Write(uint8_t x)
{
    content.push_back(x);
<<<<<<< HEAD
    SetPosition(Position() + 1);
=======
>>>>>>> c8b260dfbbd561b2e053dafc4b764c71755fbac7
}

void MemoryStream::Write(uint8_t* buf, int64_t count)
{
<<<<<<< HEAD
    int64_t bytesWritten = 0;
=======
>>>>>>> c8b260dfbbd561b2e053dafc4b764c71755fbac7
    while (count > 0)
    {
        content.push_back(*buf++);
        --count;
<<<<<<< HEAD
        ++bytesWritten;
    }
    SetPosition(Position() + bytesWritten);
=======
    }
>>>>>>> c8b260dfbbd561b2e053dafc4b764c71755fbac7
}

void MemoryStream::Seek(int64_t pos, Origin origin)
{
    switch (origin)
    {
        case Origin::seekSet:
        {
            readPos = pos;
<<<<<<< HEAD
            SetPosition(readPos);
=======
>>>>>>> c8b260dfbbd561b2e053dafc4b764c71755fbac7
            break;
        }
        case Origin::seekCur:
        {
            readPos = readPos + pos;
<<<<<<< HEAD
            SetPosition(readPos);
=======
>>>>>>> c8b260dfbbd561b2e053dafc4b764c71755fbac7
            break;
        }
        case Origin::seekEnd:
        {
<<<<<<< HEAD
            readPos = size + pos;
            SetPosition(readPos);
=======
            readPos = content.size() + pos;
>>>>>>> c8b260dfbbd561b2e053dafc4b764c71755fbac7
            break;
        }
    }
}

int64_t MemoryStream::Tell()
{
    return readPos;
}

<<<<<<< HEAD
void MemoryStream::SetFromContent()
{ 
    data = content.data(); 
    size = content.size(); 
    readPos = 0;
    SetPosition(readPos);
}

=======
>>>>>>> c8b260dfbbd561b2e053dafc4b764c71755fbac7
} } // namespace soulng::util
