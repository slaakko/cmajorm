// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_ZLIB_INTERFACE_INCLUDED
#define SOULNG_UTIL_ZLIB_INTERFACE_INCLUDED
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t zlib_init(int32_t mode, int32_t level, void** handle);
void zlib_done(int32_t mode, void* handle);
void zlib_set_input(void* inChunk, uint32_t inAvail, void* handle);
int32_t zlib_deflate(void* outChunk, uint32_t outChunkSize, uint32_t* have, uint32_t* outAvail, void* handle, int32_t flush);
int32_t zlib_inflate(void* outChunk, uint32_t outChunkSize, uint32_t* have, uint32_t* outAvail, uint32_t* inAvail, void* handle);
const char* zlib_retval_str(int32_t retVal);

#ifdef __cplusplus
}
#endif

#endif // SOULNG_UTIL_ZLIB_INTERFACE_INCLUDED
