// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_MEMORY_STREAM_INCLUDED
#define SOULNG_UTIL_MEMORY_STREAM_INCLUDED
#include <soulng/util/Stream.hpp>
#include <vector>

namespace soulng { namespace util {

class UTIL_API MemoryStream : public Stream
{
public:
    MemoryStream();
    MemoryStream(uint8_t* data, int64_t count);
    int ReadByte() override;
    int64_t Read(uint8_t* buf, int64_t count) override;
    void Write(uint8_t x) override;
    void Write(uint8_t* buf, int64_t count) override;
    void Seek(int64_t pos, Origin origin) override;
    int64_t Tell() override;
    uint8_t* Data() { return content.data(); }
    int64_t Count() const { return content.size(); }
    int64_t ReadPos() const { return readPos; }
private:
    std::vector<uint8_t> content;
    int64_t readPos;
};

} } // namespace soulng::util

#endif // SOULNG_UTIL_MEMORY_STREAM_INCLUDED
