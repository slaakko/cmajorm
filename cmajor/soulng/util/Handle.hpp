// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_HANDLE_INCLUDED
#define SOULNG_UTIL_HANDLE_INCLUDED
#include <soulng/util/UtilApi.hpp>
#if defined(_WIN32)
#include <io.h>
#elif defined(__linux) || defined(__posix) || defined(__unix)
#include <unistd.h>
#else
#error unknown platform
#endif

namespace soulng { namespace util {

class UTIL_API Handle
{
public:
    Handle(int handle_) : handle(handle_)
    {
    }
    ~Handle()
    {
        if (handle != -1)
        {
            close(handle);
        }
    }
    Handle(Handle&& that)
    {
        handle = that.handle;
        that.handle = -1;
    }
    void operator=(Handle&& that)
    {
        if (handle != -1)
        {
            close(handle);
        }
        handle = that.handle;
        that.handle = -1;
    }
    operator int() const { return handle; }
private:
    int handle;
};

} } // namespace soulng::util

#endif // SOULNG_UTIL_HANDLE_INCLUDED
