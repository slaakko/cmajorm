// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/Stream.hpp>
#include <stdexcept>
#include <memory>

namespace soulng { namespace util {

Stream::Stream()
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
