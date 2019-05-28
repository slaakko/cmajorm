// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_UTIL_MEMORY_WRITER_INCLUDED
#define CMAJOR_UTIL_MEMORY_WRITER_INCLUDED
#include <cmajor/util/Time.hpp>
#include <cmajor/util/UtilApi.hpp>
#include <stdint.h>

namespace cmajor { namespace util {

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

} } // namespace cmajor::util

#endif // CMAJOR_UTIL_MEMORY_WRITER_INCLUDED
