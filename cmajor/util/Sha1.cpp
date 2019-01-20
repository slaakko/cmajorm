// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/util/Sha1.hpp>
#include <cmajor/util/TextUtils.hpp>

namespace cmajor {namespace util {

inline uint32_t LeftRotate(uint32_t  x, uint32_t  n)
{
    return (x << n) ^ (x >> (32 - n));
}

Sha1::Sha1()
{
    Reset();
}

void Sha1::Reset()
{
    digest[0] = 0x67452301u;
    digest[1] = 0xEFCDAB89u;
    digest[2] = 0x98BADCFEu;
    digest[3] = 0x10325476u;
    digest[4] = 0xC3D2E1F0u;
    byteIndex = 0u;
    bitCount = 0u;
}

void Sha1::Process(void* begin, void* end)
{
    uint8_t* b = static_cast<uint8_t*>(begin);
    uint8_t* e = static_cast<uint8_t*>(end);
    while (b != e)
    {
        Process(*b);
        ++b;
    }
}

std::string Sha1::GetDigest()
{
    ProcessByte(0x80u);
    if (byteIndex > 56u)
    {
        while (byteIndex != 0u)
        {
            ProcessByte(0u);
        }
        while (byteIndex < 56u)
        {
            ProcessByte(0u);
        }
    }
    else
    {
        while (byteIndex < 56u)
        {
            ProcessByte(0u);
        }
    }
    ProcessByte(static_cast<uint8_t>((bitCount >> 56u) & 0xFFu));
    ProcessByte(static_cast<uint8_t>((bitCount >> 48u) & 0xFFu));
    ProcessByte(static_cast<uint8_t>((bitCount >> 40u) & 0xFFu));
    ProcessByte(static_cast<uint8_t>((bitCount >> 32u) & 0xFFu));
    ProcessByte(static_cast<uint8_t>((bitCount >> 24u) & 0xFFu));
    ProcessByte(static_cast<uint8_t>((bitCount >> 16u) & 0xFFu));
    ProcessByte(static_cast<uint8_t>((bitCount >> 8u) & 0xFFu));
    ProcessByte(static_cast<uint8_t>(bitCount & 0xFFu));
    std::string s = ToHexString(digest[0]);
    s.append(ToHexString(digest[1]));
    s.append(ToHexString(digest[2]));
    s.append(ToHexString(digest[3]));
    s.append(ToHexString(digest[4]));
    return s;
}

void Sha1::ProcessBlock()
{
    uint32_t w[80];
    for (int i = 0; i < 16; ++i)
    {
        w[i] = static_cast<uint32_t>(block[4 * i]) << 24u;
        w[i] = w[i] | static_cast<uint32_t>(block[4 * i + 1]) << 16u;
        w[i] = w[i] | static_cast<uint32_t>(block[4 * i + 2]) << 8u;
        w[i] = w[i] | static_cast<uint32_t>(block[4 * i + 3]);
    }
    for (int i = 16; i < 80; ++i)
    {
        w[i] = LeftRotate(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1u);
    }
    uint32_t a = digest[0];
    uint32_t b = digest[1];
    uint32_t c = digest[2];
    uint32_t d = digest[3];
    uint32_t e = digest[4];
    for (int i = 0; i < 80; ++i)
    {
        uint32_t f;
        uint32_t k;
        if (i < 20)
        {
            f = (b & c) | (~b & d);
            k = 0x5A827999u;
        }
        else if (i < 40)
        {
            f = b ^ c ^ d;
            k = 0x6ED9EBA1u;
        }
        else if (i < 60)
        {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDCu;
        }
        else
        {
            f = b ^ c ^ d;
            k = 0xCA62C1D6u;
        }
        uint32_t temp = LeftRotate(a, 5u) + f + e + k + w[i];
        e = d;
        d = c;
        c = LeftRotate(b, 30u);
        b = a;
        a = temp;
    }
    digest[0] = digest[0] + a;
    digest[1] = digest[1] + b;
    digest[2] = digest[2] + c;
    digest[3] = digest[3] + d;
    digest[4] = digest[4] + e;
}

std::string GetSha1MessageDigest(const std::string& message)
{
    Sha1 sha1;
    sha1.Process((void*)message.c_str(), int(message.length()));
    return sha1.GetDigest();
}

} } // namespace cmajor::util
