// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/Mutex.hpp>
#include <soulng/util/Error.hpp>
#include <mutex>

void* RtAllocateMutex()
{
    return new std::mutex();
}

void RtFreeMutex(void* nativeHandle)
{
    if (nativeHandle)
    {
        delete static_cast<std::mutex*>(nativeHandle);
    }
}

void RtLockMutex(void* nativeHandle)
{
    std::mutex* mtx = static_cast<std::mutex*>(nativeHandle);
    mtx->lock();
}

void RtUnlockMutex(void* nativeHandle)
{
    std::mutex* mtx = static_cast<std::mutex*>(nativeHandle);
    mtx->unlock();
}

void* RtAllocateRecursiveMutex()
{
    return new std::recursive_mutex();
}

void RtFreeRecursiveMutex(void* nativeHandle)
{
    if (nativeHandle)
    {
        delete static_cast<std::recursive_mutex*>(nativeHandle);
    }
}

void RtLockRecursiveMutex(void* nativeHandle)
{
    std::recursive_mutex* mtx = static_cast<std::recursive_mutex*>(nativeHandle);
    mtx->lock();
}

void RtUnlockRecursiveMutex(void* nativeHandle)
{
    std::recursive_mutex* mtx = static_cast<std::recursive_mutex*>(nativeHandle);
    mtx->unlock();
}
