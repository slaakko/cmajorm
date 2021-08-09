// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_BZIP2_STREAM_INCLUDED
#define SOULNG_UTIL_BZIP2_STREAM_INCLUDED
#include <soulng/util/Stream.hpp>
#include <soulng/util/Compression.hpp>
#include <memory>

namespace soulng { namespace util {

const int minimumBZip2CompressionLevel = 1;
const int optimalBZip2CompressionLevel = 9;
const int defaultBZip2CompressionLevel = optimalBZip2CompressionLevel;

const int defaultBZip2WorkFactor = 0; // equal to 30 according to BZIP2 documentation
const int maximumBZip2WorkFactor = 250;
const int BZ_OK = 0;
const int BZ_SEQUENCE_ERROR = -1;
const int BZ_STREAM_END = 4;
const int BZ_UNEXPECTED_EOF = -7;
const int BZ_RUN = 0;
const int BZ_FINISH = 2;

class UTIL_API BZip2Stream : public Stream
{
public:
    BZip2Stream(CompressionMode mode_, Stream& underlyingStream_);
    BZip2Stream(CompressionMode mode_, Stream& underlyingStream_, int64_t bufferSize_);
    BZip2Stream(CompressionMode mode_, Stream& underlyingStream_, int64_t bufferSize_, int compressionLevel_, int compressionWorkFactor_);
    ~BZip2Stream() override;
    int ReadByte() override;
    int64_t Read(uint8_t* buf, int64_t count) override;
    void Write(uint8_t x) override;
    void Write(uint8_t* buf, int64_t count) override;
private:
    void Finish();
    CompressionMode mode;
    Stream& underlyingStream;
    int64_t bufferSize;
    int compressionLevel;
    int compressionWorkFactor;
    uint32_t inAvail;
    bool endOfInput;
    bool endOfStream;
    std::unique_ptr<uint8_t[]> in;
    uint32_t outHave;
    uint32_t outAvail;
    int64_t outPos;
    std::unique_ptr<uint8_t[]> out;
    void* handle;
};

} } // namespace soulng::util

#endif // SOULNG_UTIL_BZIP2_STREAM_INCLUDED
