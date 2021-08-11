// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/Stream.hpp>
#include <algorithm>
#include <stdexcept>

namespace soulng { namespace util {

StreamObserver::StreamObserver()
{
}

StreamObserver::~StreamObserver()
{
}

Stream::Stream() : position(0)
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

Streams::Streams() : streams()
{
}

Streams::Streams(Streams&& that) noexcept : streams(std::move(that.streams))
{
}

Streams& Streams::operator=(Streams&& that) noexcept
{
    std::swap(streams, that.streams);
    return *this;
}

Streams::~Streams()
{
    int n = Count();
    for (int i = n - 1; i >= 0; --i)
    {
        streams[i].reset();
    }
}

void Streams::Add(Stream* stream)
{
    streams.push_back(std::unique_ptr<Stream>(stream));
}

Stream* Streams::Get(int index) const
{
    return streams[index].get();
}

Stream& Streams::Back() const
{ 
    if (streams.empty())
    {
        throw std::runtime_error("streams empty");
    }
    return *streams.back(); 
}

} } // namespace soulng::util
