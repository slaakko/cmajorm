// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_FILE_STREAM_INCLUDED
#define SOULNG_UTIL_FILE_STREAM_INCLUDED
#include <soulng/util/Stream.hpp>
#include <string>
#include <cstdio>

namespace soulng { namespace util {

enum class OpenMode : uint8_t
{
    none = 0u,
    read = 1u << 0u,
    write = 1u << 1u,
    append = 1u << 2u,
    binary = 1u << 3u
};

UTIL_API inline OpenMode operator|(OpenMode left, OpenMode right)
{
    return OpenMode(uint8_t(left) | uint8_t(right));
}

UTIL_API inline OpenMode operator&(OpenMode left, OpenMode right)
{
    return OpenMode(uint8_t(left) & uint8_t(right));
}

UTIL_API inline OpenMode operator~(OpenMode operand)
{
    return OpenMode(~uint8_t(operand));
}

class UTIL_API FileStream : public Stream
{
public:
    FileStream(int handle);
    FileStream(const std::string& filePath_, OpenMode openMode);
    ~FileStream() override;
    int ReadByte() override;
    int64_t Read(uint8_t* buf, int64_t count) override;
    void Write(uint8_t x) override;
    void Write(uint8_t* buf, int64_t count) override;
    void Flush() override;
    void Seek(int64_t pos, Origin origin) override;
    int64_t Tell() override;
    int64_t Size() const;
private:
    std::string filePath;
    FILE* file;
    bool needToClose;
};

} } // namespace soulng::util

#endif // SOULNG_UTIL_FILE_STREAM_INCLUDED
