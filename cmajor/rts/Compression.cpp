// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rts/Compression.hpp>
#include <cmajor/rts/ZlibInterface.h>
#include <cmajor/rts/BZ2Interface.h>

//  ZLIB:

extern "C" RT_API int32_t RtInitZlib(int32_t mode, int32_t level, void** handle)
{
    return zlib_init(mode, level, handle);
}

extern "C" RT_API void RtDoneZlib(int32_t  mode, void* handle)
{
    zlib_done(mode, handle);
}

extern "C" RT_API void RtSetInputZlib(void* inChunk, uint32_t inAvail, void* handle)
{
    zlib_set_input(inChunk, inAvail, handle);
}

extern "C" RT_API int32_t RtDeflateZlib(void* outChunk, uint32_t outChunkSize, uint32_t* have, uint32_t* outAvail, void* handle, int32_t flush)
{
    return zlib_deflate(outChunk, outChunkSize, have, outAvail, handle, flush);
}

extern "C" RT_API int32_t RtInflateZlib(void* outChunk, uint32_t outChunkSize, uint32_t* have, uint32_t* outAvail, uint32_t* inAvail, void* handle)
{
    return zlib_inflate(outChunk, outChunkSize, have, outAvail, inAvail, handle);
}

extern "C" RT_API const char* RtRetvalStrZlib(int32_t retVal)
{
    return zlib_retval_str(retVal);
}

// BZIP2:

extern "C" RT_API int32_t RtInitBZip2(int32_t mode, int32_t compressionLevel, int32_t compressionWorkFactor, void** handle)
{
    return bz2_init(mode, compressionLevel, compressionWorkFactor, handle);
}

extern "C" RT_API void RtDoneBZip2(int32_t mode, void* handle)
{
    bz2_done(mode, handle);
}

extern "C" RT_API void RtSetInputBZip2(void* inChunk, uint32_t inAvail, void* handle)
{
    bz2_set_input(inChunk, inAvail, handle);
}

extern "C" RT_API int32_t RtCompressBZip2(void* outChunk, uint32_t outChunkSize, uint32_t* have, uint32_t* outAvail, void* handle, int32_t action)
{
    return bz2_compress(outChunk, outChunkSize, have, outAvail, handle, action);
}

extern "C" RT_API int32_t RtDecompressBZip2(void* outChunk, uint32_t outChunkSize, uint32_t* have, uint32_t* outAvail, uint32_t* inAvail, void* handle)
{
    return bz2_decompress(outChunk, outChunkSize, have, outAvail, inAvail, handle);
}

extern "C" RT_API const char* RtRetvalStrBZip2(int32_t retVal)
{
    return bz2_retval_str(retVal);
}
