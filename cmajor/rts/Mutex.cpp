// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rts/Mutex.hpp>
#include <soulng/util/Error.hpp>
#include <mutex>

void* RtAllocateMutex()
{
#ifndef __MINGW32__
    return new std::mutex();
#else
    return nullptr;
#endif
}

void RtFreeMutex(void* nativeHandle)
{
#ifndef __MINGW32__
    if (nativeHandle)
    {
        delete static_cast<std::mutex*>(nativeHandle);
    }
#endif
}

void RtLockMutex(void* nativeHandle)
{
#ifndef __MINGW32__
    std::mutex* mtx = static_cast<std::mutex*>(nativeHandle);
    mtx->lock();
#endif
}

void RtUnlockMutex(void* nativeHandle)
{
#ifndef __MINGW32__
    std::mutex* mtx = static_cast<std::mutex*>(nativeHandle);
    mtx->unlock();
#endif
}

void* RtAllocateRecursiveMutex()
{
#ifndef __MINGW32__
    return new std::recursive_mutex();
#else 
    return nullptr;
#endif
}

void RtFreeRecursiveMutex(void* nativeHandle)
{
#ifndef __MINGW32__
    if (nativeHandle)
    {
        delete static_cast<std::recursive_mutex*>(nativeHandle);
    }
#endif
}

void RtLockRecursiveMutex(void* nativeHandle)
{
#ifndef __MINGW32__
    std::recursive_mutex* mtx = static_cast<std::recursive_mutex*>(nativeHandle);
    mtx->lock();
#endif
}

void RtUnlockRecursiveMutex(void* nativeHandle)
{
#ifndef __MINGW32__
    std::recursive_mutex* mtx = static_cast<std::recursive_mutex*>(nativeHandle);
    mtx->unlock();
#endif
}
