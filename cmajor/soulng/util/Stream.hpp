// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_STREAM_INCLUDED
#define SOULNG_UTIL_STREAM_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <stdint.h>

namespace soulng { namespace util {

enum class Origin : uint8_t
{
    seekSet, seekCur, seekEnd
};

class UTIL_API Stream
{
public:
    Stream();
    virtual ~Stream();
    Stream(const Stream&) = delete;
    Stream& operator=(const Stream&) = delete;
    virtual int ReadByte() = 0;
    virtual int64_t Read(uint8_t* buf, int64_t count) = 0;
    virtual void Write(uint8_t x) = 0;
    virtual void Write(uint8_t* buf, int64_t count) = 0;
    virtual void Flush();
    virtual void Seek(int64_t pos, Origin origin);
    virtual int64_t Tell();
    void CopyTo(Stream& destination);
    void CopyTo(Stream& destination, int64_t bufferSize);
};

} } // namespace soulng::util

#endif // SOULNG_UTIL_STREAM_INCLUDED
