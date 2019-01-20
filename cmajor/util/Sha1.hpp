// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_UTIL_SHA1_INCLUDED
#define CMAJOR_UTIL_SHA1_INCLUDED
#include <stdint.h>
#include <string>

namespace cmajor { namespace util {

class Sha1
{
public:
    Sha1();
    void Reset();
    void Process(uint8_t x)
    {
        ProcessByte(x);
        bitCount = bitCount + 8u;
    }
    void Process(void* begin, void* end);
    void Process(void* buf, int count)
    {
        uint8_t* b = static_cast<uint8_t*>(buf);
        Process(b, b + count);
    }
    std::string GetDigest();
private:
    void ProcessByte(uint8_t x)
    {
        block[byteIndex++] = x;
        if (byteIndex == 64u)
        {
            byteIndex = 0u;
            ProcessBlock();
        }
    }
    void ProcessBlock();
    uint32_t digest[5];
    uint8_t block[64];
    uint8_t byteIndex;
    uint64_t bitCount;
};

std::string GetSha1MessageDigest(const std::string& message);

} } // namespace cmajor::util

#endif // CMAJOR_UTIL_SHA1_INCLUDED
