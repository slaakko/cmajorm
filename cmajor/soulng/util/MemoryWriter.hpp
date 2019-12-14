// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_MEMORY_WRITER_INCLUDED
#define SOULNG_UTIL_MEMORY_WRITER_INCLUDED
#include <soulng/util/Time.hpp>
#include <soulng/util/UtilApi.hpp>
#include <stdint.h>

namespace soulng { namespace util {

class UTIL_API MemoryWriter
{
public:
    MemoryWriter(uint8_t* ptr_, int64_t count_);
    void Write(uint8_t x);
    void Write(int8_t x);
    void Write(uint16_t x);
    void Write(int16_t x);
    void Write(uint32_t x);
    void Write(int32_t x);
    void Write(uint64_t x);
    void Write(int64_t x);
    void Write(const DateTime& dt);
private:
    uint8_t* ptr;
    uint8_t* pos;
    int64_t count;
};

} } // namespace soulng::util

#endif // SOULNG_UTIL_MEMORY_WRITER_INCLUDED
