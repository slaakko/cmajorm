#include <cmajor/rt/ZlibInterface.h>
#include <cmajor/system/ext/zlib-1.2.11/zlib.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define COMPRESS    0
#define DECOMPRESS  1

int32_t zlib_init(int32_t mode, int32_t level, void** handle)
{
    int32_t ret = Z_OK;
    if (!handle)
    {
        ret = Z_MEM_ERROR;
    }
    else
    {
        z_stream* strm = (z_stream*)malloc(sizeof(z_stream));
        switch (mode)
        {
            case COMPRESS:
            {
                strm->zalloc = Z_NULL;
                strm->zfree = Z_NULL;
                strm->opaque = Z_NULL;
                ret = deflateInit(strm, level);                
                break;
            }
            case DECOMPRESS:
            {
                strm->zalloc = Z_NULL;
                strm->zfree = Z_NULL;
                strm->opaque = Z_NULL;
                strm->avail_in = 0;
                strm->next_in = Z_NULL;
                ret = inflateInit(strm);
                break;
            }
        }
        if (ret != Z_OK)
        {
            free(strm);
            *handle = NULL;
        }
        else
        {
            *handle = strm;
        }    
    }
    return ret;
}

void zlib_done(int32_t mode, void* handle)
{
    z_stream* strm = (z_stream*)handle;
    switch (mode)
    {
        case COMPRESS:
        {
            deflateEnd(strm);
            break;
        }
        case DECOMPRESS:
        {
            inflateEnd(strm);
            break;
        }
    }    
    free(strm);
}

void zlib_set_input(void* inChunk, uint32_t inAvail, void* handle)
{
    z_stream* strm = (z_stream*)handle;
    strm->next_in = inChunk;
    strm->avail_in = inAvail;
}

int32_t zlib_deflate(void* outChunk, uint32_t outChunkSize, uint32_t* have, uint32_t* outAvail, void* handle, int32_t flush)
{
    z_stream* strm = (z_stream*)handle;
    strm->next_out = outChunk;
    strm->avail_out = outChunkSize;
    int ret = deflate(strm, flush);
    *have = outChunkSize - strm->avail_out;
    *outAvail = strm->avail_out;
    return ret;
}

int32_t zlib_inflate(void* outChunk, uint32_t outChunkSize, uint32_t* have, uint32_t* outAvail, uint32_t* inAvail, void* handle)
{
    z_stream* strm = (z_stream*)handle;
    strm->next_out = outChunk;
    strm->avail_out = outChunkSize;
    int ret = inflate(strm, Z_NO_FLUSH);
    *have = outChunkSize - strm->avail_out;
    *outAvail = strm->avail_out;
    *inAvail = strm->avail_in;
    return ret;
}

const char* zlib_retval_str(int32_t retVal)
{
    switch (retVal)
    {
        case Z_OK: return "Z_OK";
        case Z_STREAM_END: return "Z_STREAM_END";
        case Z_NEED_DICT: return "Z_NEED_DICT";
        case Z_ERRNO: return strerror(errno);
        case Z_STREAM_ERROR: return "Z_STREAM_ERROR";
        case Z_DATA_ERROR: return "Z_DATA_ERROR";
        case Z_MEM_ERROR: return "Z_MEM_ERROR";
        case Z_BUF_ERROR: return "Z_BUF_ERROR";
        case Z_VERSION_ERROR: return "Z_VERSION_ERROR";
    }
    return "";
}
