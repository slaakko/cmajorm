// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/BZip2Stream.hpp>
#include <soulng/util/BZ2Interface.h>
#include <stdexcept>
#include <string>

namespace soulng { namespace util {

BZip2Stream::BZip2Stream(CompressionMode mode_, Stream& underlyingStream_) : BZip2Stream(mode_, underlyingStream_, 16384)
{
}

BZip2Stream::BZip2Stream(CompressionMode mode_, Stream& underlyingStream_, int64_t bufferSize_) : 
    BZip2Stream(mode_, underlyingStream_, bufferSize_, defaultBZip2CompressionLevel, defaultBZip2WorkFactor)
{
}

BZip2Stream::BZip2Stream(CompressionMode mode_, Stream& underlyingStream_, int64_t bufferSize_, int compressionLevel_, int compressionWorkFactor_) :
    Stream(), mode(mode_), underlyingStream(underlyingStream_), bufferSize(bufferSize_), compressionLevel(compressionLevel_), compressionWorkFactor(compressionWorkFactor_),
    inAvail(0), endOfInput(false), endOfStream(false), in(new uint8_t[bufferSize]), outHave(0), outAvail(0), outPos(0), out(new uint8_t[bufferSize]),
    handle(nullptr)
{
    int ret = bz2_init(int32_t(mode), compressionLevel, compressionWorkFactor, &handle);
    if (ret < 0)
    {
        throw std::runtime_error("could not create bzip2 stream: bzip2 initialization returned error code " + std::to_string(ret));
    }
}

BZip2Stream::~BZip2Stream()
{
    if (handle)
    {
        try
        {
            if (mode == CompressionMode::compress)
            {
                Finish();
            }
        }
        catch (...)
        {
        }
        bz2_done(int32_t(mode), handle);
    }
}

int BZip2Stream::ReadByte()
{
    uint8_t x = 0;
    int64_t bytesRead = Read(&x, 1);
    if (bytesRead == 0)
    {
        return -1;
    }
    return x;
}

int64_t BZip2Stream::Read(uint8_t* buf, int64_t count)
{
    if (mode != CompressionMode::decompress)
    {
        throw std::runtime_error("bzip2 stream: cannot read in 'compress' compression mode");
    }
    int64_t bytesRead = 0;
    do
    {
        if (inAvail == 0 && !endOfInput)
        {
            inAvail = static_cast<uint32_t>(underlyingStream.Read(in.get(), static_cast<uint32_t>(bufferSize)));
            if (inAvail == 0)
            {
                endOfInput = true;
            }
            bz2_set_input(in.get(), inAvail, handle);
        }
        do
        {
            if (outHave == 0 && !endOfStream)
            {
                int ret = bz2_decompress(out.get(), static_cast<uint32_t>(bufferSize), &outHave, &outAvail, &inAvail, handle);
                if (ret < 0)
                {
                    throw std::runtime_error("bzip2 stream: could not decompress: decompress returned error code " + std::to_string(ret));
                }
                if (ret == BZ_STREAM_END)
                {
                    endOfStream = true;
                }
                outPos = 0;
            }
            while (count > 0 && outHave > 0)
            {
                *buf++ = out[outPos++];
                --count;
                --outHave;
                ++bytesRead;
            }
        }
        while (count > 0 && outAvail == 0);
    }
    while (count > 0 && !endOfStream && !endOfInput);
    if (endOfInput && !endOfStream)
    {
        throw std::runtime_error("bzip2 stream: unexpected end of input");
    }
    return bytesRead;
}

void BZip2Stream::Write(uint8_t x)
{
    Write(&x, 1);
}

void BZip2Stream::Write(uint8_t* buf, int64_t count)
{
    if (mode != CompressionMode::compress)
    {
        throw std::runtime_error("bzip2 stream: cannot write in 'decompress' compression mode");
    }
    while (count > 0)
    {
        uint8_t* inP = in.get();
        inAvail = 0;
        while (count > 0 && inAvail < static_cast<uint32_t>(bufferSize))
        {
            *inP++ = *buf++;
            --count;
            ++inAvail;
        }
        bz2_set_input(in.get(), inAvail, handle);
        do
        {
            uint32_t have = 0;
            int ret = bz2_compress(out.get(), static_cast<uint32_t>(bufferSize), &have, &outAvail, handle, BZ_RUN);
            if (ret < 0)
            {
                throw std::runtime_error("bzip2 stream: could not compress: compress returned error code " + std::to_string(ret));
            }
            underlyingStream.Write(out.get(), static_cast<int64_t>(have));
        } 
        while (outAvail == 0);
    }
}

void BZip2Stream::Finish()
{
    int ret = 0;
    do
    {
        uint32_t have = 0;
        ret = bz2_compress(out.get(), static_cast<uint32_t>(bufferSize), &have, &outAvail, handle, BZ_FINISH);
        if (ret < 0)
        {
            throw std::runtime_error("bzip2 stream: could not compress: compress returned error code " + std::to_string(ret));
        }
        underlyingStream.Write(out.get(), static_cast<int64_t>(have));
    }
    while (ret != BZ_STREAM_END);
}

} } // namespace soulng::util

