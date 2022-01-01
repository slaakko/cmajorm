// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/BZ2Interface.h>
#include <cmajor/system/ext/bzip2-1.0.6/bzlib.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define COMPRESS    0
#define DECOMPRESS  1

int32_t bz2_init(int32_t mode, int32_t compressionLevel, int32_t compressionWorkFactor, void** handle)
{
    int32_t ret = BZ_OK;
    if (!handle)
    {
        ret = BZ_MEM_ERROR;
    }
    else
    {
        bz_stream* stream = (bz_stream*)malloc(sizeof(bz_stream));
        switch (mode)
        {
            case COMPRESS:
            {
                stream->bzalloc = NULL;
                stream->bzfree = NULL;
                stream->opaque = NULL;
                ret = BZ2_bzCompressInit(stream, compressionLevel, 0, compressionWorkFactor);
                break;
            }
            case DECOMPRESS:
            {
                stream->bzalloc = NULL;
                stream->bzfree = NULL;
                stream->opaque = NULL;
                ret = BZ2_bzDecompressInit(stream, 0, 0);
                break;
            }
        }
        if (ret != BZ_OK)
        {
            free(stream);
            *handle = NULL;
        }
        else
        {
            *handle = stream;
        }
    }
    return ret;
}

void bz2_done(int32_t mode, void* handle)
{
    bz_stream* strm = (bz_stream*)handle;
    switch (mode)
    {
        case COMPRESS:
        {
            BZ2_bzCompressEnd(strm);
            break;
        }
        case DECOMPRESS:
        {
            BZ2_bzDecompressEnd(strm);
            break;
        }
    }    
    free(strm);
}

void bz2_set_input(void* inChunk, uint32_t inAvail, void* handle)
{
    bz_stream* strm = (bz_stream*)handle;
    strm->next_in = (char*)inChunk;
    strm->avail_in = inAvail;
}

int32_t bz2_compress(void* outChunk, uint32_t outChunkSize, uint32_t* have, uint32_t* outAvail, void* handle, int32_t action)
{
    bz_stream* strm = (bz_stream*)handle;
    strm->next_out = outChunk;
    strm->avail_out = outChunkSize;
    int ret = BZ2_bzCompress(strm, action);
    *have = outChunkSize - strm->avail_out;
    *outAvail = strm->avail_out;
    return ret;
}

int32_t bz2_decompress(void* outChunk, uint32_t outChunkSize, uint32_t* have, uint32_t* outAvail, uint32_t* inAvail, void* handle)
{
    bz_stream* strm = (bz_stream*)handle;
    strm->next_out = outChunk;
    strm->avail_out = outChunkSize;
    int32_t ret = BZ2_bzDecompress(strm);
    *have = outChunkSize - strm->avail_out;
    *outAvail = strm->avail_out;
    *inAvail = strm->avail_in;
    return ret;
}

const char* bz2_retval_str(int32_t retVal)
{
    switch (retVal)
    {
        case BZ_OK: return "BZ_OK";
        case BZ_RUN_OK: return "BZ_RUN_OK";
        case BZ_FLUSH_OK: return "BZ_FLUSH_OK";
        case BZ_FINISH_OK: return "BZ_FINISH_OK";
        case BZ_STREAM_END: return "BZ_STREAM_END";
        case BZ_SEQUENCE_ERROR: return "BZ_SEQUENCE_ERROR";
        case BZ_PARAM_ERROR: return "BZ_PARAM_ERROR";
        case BZ_MEM_ERROR: return "BZ_MEM_ERROR";
        case BZ_DATA_ERROR: return "BZ_DATA_ERROR";
        case BZ_DATA_ERROR_MAGIC: return "BZ_DATA_ERROR_MAGIC";
        case BZ_IO_ERROR: return strerror(errno);
        case BZ_UNEXPECTED_EOF: return "BZ_UNEXPECTED_EOF";
        case BZ_OUTBUFF_FULL: return "BZ_OUTBUFF_FULL";
        case BZ_CONFIG_ERROR: return "BZ_CONFIG_ERROR";
    }
    return "";
}
