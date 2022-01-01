// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_BZ2_INTERFACE_INCLUDED
#define CMAJOR_RT_BZ2_INTERFACE_INCLUDED
#include <stdint.h>

#if defined (__cplusplus)
extern "C" {
#endif

int32_t bz2_init(int32_t mode, int32_t compressionLevel, int32_t compressionWorkFactor, void** handle);
void bz2_done(int32_t mode, void* handle);
void bz2_set_input(void* inChunk, uint32_t inAvail, void* handle);
int32_t bz2_compress(void* outChunk, uint32_t outChunkSize, uint32_t* have, uint32_t* outAvail, void* handle, int32_t action);
int32_t bz2_decompress(void* outChunk, uint32_t outChunkSize, uint32_t* have, uint32_t* outAvail, uint32_t* inAvail, void* handle);
const char* bz2_retval_str(int32_t retVal);

#if defined (__cplusplus)
}
#endif

#endif // CMAJOR_RT_BZ2_INTERFACE_INCLUDED
