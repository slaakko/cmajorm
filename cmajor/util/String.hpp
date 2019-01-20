// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_UTIL_STRING_INCLUDED
#define CMAJOR_UTIL_STRING_INCLUDED
#include <cmajor/util/Defines.hpp>
#include <cmajor/util/Error.hpp>

namespace cmajor { namespace util {

inline uint64_t StringLen(const char32_t* s)
{
    uint64_t n = 0;
    Assert(s, "null string ptr");
    while (*s++) ++n;
    return n;
}

class StringPtr
{
public:
    StringPtr(const char32_t* value_) : value(value_) {}
    const char32_t* Value() const { return value; }
    bool IsEmpty() const
    {
        Assert(value, "null string ptr");
        return !*value;
    }
private:
    const char32_t* value;
};

inline bool operator==(StringPtr left, StringPtr right)
{
    const char32_t* p = left.Value();
    const char32_t* q = right.Value();
    Assert(p, "null string ptr");
    Assert(q, "null string ptr");
    while (*p && *q && *p == *q)
    {
        ++p;
        ++q;
    }
    return !*p && !*q;
}

inline bool operator!=(StringPtr left, StringPtr right)
{
    return !(left == right);
}

inline bool operator<(StringPtr left, StringPtr right)
{
    const char32_t* p = left.Value();
    const char32_t* q = right.Value();
    Assert(p, "null string ptr");
    Assert(q, "null string ptr");
    while (*p && *q && *p == *q)
    {
        ++p;
        ++q;
    }
    return *p < *q;
}

struct StringPtrHash
{
#if defined(BITS_64)
    const size_t offset = 14695981039346656037ULL;
    const size_t prime = 1099511628211ULL;
#elif defined(BITS_32)
    const size_t offset = 2166136261U;
    const size_t prime = 16777619U;
#else
    #error either BITS_64 or BITS_32 must be defined
#endif
    size_t operator()(StringPtr s) const
    {
        size_t value = offset;
        const char32_t* p = s.Value();
        Assert(p, "null string ptr");
        while (*p)
        {    
            value ^= (size_t)*p;
            value *= prime;
            ++p;
        }
        return value;
    }
};

} } // namespace cmajor::util

#endif // CMAJOR_UTIL_STRING_INCLUDED
