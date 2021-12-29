// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_OBJECT_COPY_RANGE_INCLUDED
#define CMSX_OBJECT_COPY_RANGE_INCLUDED
#include <system-x/object/Api.hpp>
#include <stdint.h>

namespace cmsx::object {

class Section;

class CMSX_OBJECT_API CopyRange
{
public:
    CopyRange(Section* fromSection_, Section* toSection_, int64_t startPos_, int64_t length_, uint8_t alignment_);
    Section* FromSection() const { return fromSection; }
    Section* ToSection() const { return toSection; }
    int64_t StartPos() const { return startPos; }
    int64_t Length() const { return length; }
    void SetLength(int64_t length_) { length = length_; }
    int8_t Alignment() const { return alignment; }
private:
    Section* fromSection;
    Section* toSection;
    int64_t startPos;
    int64_t length;
    int8_t alignment;
};

} // namespace cmsx::object

#endif // CMSX_OBJECT_COPY_RANGE_INCLUDED
