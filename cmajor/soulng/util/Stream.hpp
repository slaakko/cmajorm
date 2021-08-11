// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_STREAM_INCLUDED
#define SOULNG_UTIL_STREAM_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <stdint.h>
#include <memory>
#include <vector>

namespace soulng { namespace util {

enum class Origin : uint8_t
{
    seekSet, seekCur, seekEnd
};

class Stream;

class UTIL_API StreamObserver
{
public:
    StreamObserver();
    virtual ~StreamObserver();
    virtual void PositionChanged(Stream* stream) {}
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
    int64_t Position() const { return position; }
    void SetPosition(int64_t position_);
    void AddObserver(StreamObserver* observer);
    void RemoveObserver(StreamObserver* observer);
private:
    int64_t position;
    std::vector<StreamObserver*> observers;
};

class UTIL_API Streams
{
public:
    Streams();
    Streams(Streams&& that) noexcept;
    Streams(const Streams&) = delete;
    Streams& operator=(Streams&& that) noexcept;
    Streams& operator=(const Streams&) = delete;
    ~Streams();
    void Add(Stream* stream);
    Stream* Get(int index) const;
    Stream& Back() const;
    int Count() const { return streams.size(); }
private:
    std::vector<std::unique_ptr<Stream>> streams;
};

} } // namespace soulng::util

#endif // SOULNG_UTIL_STREAM_INCLUDED
