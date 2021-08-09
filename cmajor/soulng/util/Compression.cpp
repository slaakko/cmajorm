// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/Compression.hpp>
#include <soulng/util/ZlibInterface.h>
#include <soulng/util/BZ2Interface.h>

//  ZLIB:

int32_t RtInitZlib(int32_t mode, int32_t level, void** handle)
{
    return zlib_init(mode, level, handle);
}

void RtDoneZlib(int32_t  mode, void* handle)
{
    zlib_done(mode, handle);
}

void RtSetInputZlib(void* inChunk, uint32_t inAvail, void* handle)
{
    zlib_set_input(inChunk, inAvail, handle);
}

int32_t RtDeflateZlib(void* outChunk, uint32_t outChunkSize, uint32_t * have, uint32_t * outAvail, void* handle, int32_t flush)
{
    return zlib_deflate(outChunk, outChunkSize, have, outAvail, handle, flush);
}

int32_t RtInflateZlib(void* outChunk, uint32_t outChunkSize, uint32_t * have, uint32_t * outAvail, uint32_t * inAvail, void* handle)
{
    return zlib_inflate(outChunk, outChunkSize, have, outAvail, inAvail, handle);
}

const char* RtRetvalStrZlib(int32_t retVal)
{
    return zlib_retval_str(retVal);
}

// BZIP2:

int32_t RtInitBZip2(int32_t mode, int32_t compressionLevel, int32_t compressionWorkFactor, void** handle)
{
    return bz2_init(mode, compressionLevel, compressionWorkFactor, handle);
}

void RtDoneBZip2(int32_t mode, void* handle)
{
    bz2_done(mode, handle);
}

void RtSetInputBZip2(void* inChunk, uint32_t inAvail, void* handle)
{
    bz2_set_input(inChunk, inAvail, handle);
}

int32_t RtCompressBZip2(void* outChunk, uint32_t outChunkSize, uint32_t * have, uint32_t * outAvail, void* handle, int32_t action)
{
    return bz2_compress(outChunk, outChunkSize, have, outAvail, handle, action);
}

int32_t RtDecompressBZip2(void* outChunk, uint32_t outChunkSize, uint32_t * have, uint32_t * outAvail, uint32_t * inAvail, void* handle)
{
    return bz2_decompress(outChunk, outChunkSize, have, outAvail, inAvail, handle);
}

const char* RtRetvalStrBZip2(int32_t retVal)
{
    return bz2_retval_str(retVal);
}
