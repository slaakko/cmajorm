// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RTS_COMPRESSION_INCLUDED
#define CMAJOR_RTS_COMPRESSION_INCLUDED
#include <cmajor/rts/RtsApi.hpp>
#include <stdint.h>

//  ZLIB:

extern "C" RT_API int32_t RtInitZlib(int32_t  mode, int32_t  level, void** handle);
extern "C" RT_API void RtDoneZlib(int32_t  mode, void* handle);
extern "C" RT_API void RtSetInputZlib(void* inChunk, uint32_t inAvail, void* handle);
extern "C" RT_API int32_t RtDeflateZlib(void* outChunk, uint32_t outChunkSize, uint32_t* have, uint32_t* outAvail, void* handle, int32_t flush);
extern "C" RT_API int32_t RtInflateZlib(void* outChunk, uint32_t outChunkSize, uint32_t* have, uint32_t* outAvail, uint32_t* inAvail, void* handle);
extern "C" RT_API const char* RtRetvalStrZlib(int32_t retVal);

// BZIP2:

extern "C" RT_API int32_t RtInitBZip2(int32_t mode, int32_t compressionLevel, int32_t compressionWorkFactor, void** handle);
extern "C" RT_API void RtDoneBZip2(int32_t mode, void* handle);
extern "C" RT_API void RtSetInputBZip2(void* inChunk, uint32_t inAvail, void* handle);
extern "C" RT_API int32_t RtCompressBZip2(void* outChunk, uint32_t outChunkSize, uint32_t* have, uint32_t* outAvail, void* handle, int32_t action);
extern "C" RT_API int32_t RtDecompressBZip2(void* outChunk, uint32_t outChunkSize, uint32_t* have, uint32_t* outAvail, uint32_t* inAvail, void* handle);
extern "C" RT_API const char* RtRetvalStrBZip2(int32_t retVal);


#endif // CMAJOR_RTS_COMPRESSION_INCLUDED
