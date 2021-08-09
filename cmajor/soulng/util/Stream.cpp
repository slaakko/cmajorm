// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/Stream.hpp>
<<<<<<< HEAD
#include <algorithm>
=======
>>>>>>> c8b260dfbbd561b2e053dafc4b764c71755fbac7
#include <stdexcept>
#include <memory>

namespace soulng { namespace util {

<<<<<<< HEAD
StreamObserver::StreamObserver()
{
}

StreamObserver::~StreamObserver()
{
}

Stream::Stream() : position(0)
=======
Stream::Stream()
>>>>>>> c8b260dfbbd561b2e053dafc4b764c71755fbac7
{
}

Stream::~Stream()
{
}

void Stream::Flush()
{
}

void Stream::Seek(int64_t pos, Origin origin)
{
    throw std::runtime_error("seek not supported");
}

int64_t Stream::Tell()
{
    throw std::runtime_error("tell not supported");
}

<<<<<<< HEAD
void Stream::AddObserver(StreamObserver* observer)
{
    if (std::find(observers.begin(), observers.end(), observer) == observers.end())
    {
        observers.push_back(observer);
    }
}

void Stream::RemoveObserver(StreamObserver* observer)
{
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void Stream::SetPosition(int64_t position_)
{
    if (position != position_)
    {
        position = position_;
        for (StreamObserver* observer : observers)
        {
            observer->PositionChanged(this);
        }
    }
}

=======
>>>>>>> c8b260dfbbd561b2e053dafc4b764c71755fbac7
void Stream::CopyTo(Stream& destination)
{
    CopyTo(destination, 16384);
}

void Stream::CopyTo(Stream& destination, int64_t bufferSize)
{
    std::unique_ptr<uint8_t[]> buf(new uint8_t[bufferSize]);
    int64_t bytesRead = Read(buf.get(), bufferSize);
    while (bytesRead > 0)
    {
        destination.Write(buf.get(), bytesRead);
        bytesRead = Read(buf.get(), bufferSize);
    }
}

} } // namespace soulng::util
