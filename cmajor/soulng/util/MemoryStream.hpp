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
<<<<<<< HEAD
    MemoryStream(uint8_t* data_, int64_t size_);
=======
    MemoryStream(uint8_t* data, int64_t count);
>>>>>>> c8b260dfbbd561b2e053dafc4b764c71755fbac7
    int ReadByte() override;
    int64_t Read(uint8_t* buf, int64_t count) override;
    void Write(uint8_t x) override;
    void Write(uint8_t* buf, int64_t count) override;
    void Seek(int64_t pos, Origin origin) override;
    int64_t Tell() override;
<<<<<<< HEAD
    uint8_t* Data() { return data; }
    int64_t Size() const { return size; }
    int64_t ReadPos() const { return readPos; }
    const std::vector<uint8_t>& Content() const { return content; }
    void SetFromContent();
private:
    uint8_t* data;
    int64_t size;
    int64_t readPos;
    std::vector<uint8_t> content;
=======
    uint8_t* Data() { return content.data(); }
    int64_t Count() const { return content.size(); }
    int64_t ReadPos() const { return readPos; }
private:
    std::vector<uint8_t> content;
    int64_t readPos;
>>>>>>> c8b260dfbbd561b2e053dafc4b764c71755fbac7
};

} } // namespace soulng::util

#endif // SOULNG_UTIL_MEMORY_STREAM_INCLUDED
