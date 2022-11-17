// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rts/ConditionVariable.hpp>
#include <cmajor/rts/Mutex.hpp>
#include <cmajor/rts/Error.hpp>
#include <soulng/util/Error.hpp>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <stdexcept>

void* RtAllocateConditionVariable()
{
#ifndef __MINGW32__
    return new std::condition_variable_any();
#else
	return nullptr;
#endif
}

void RtFreeConditionVariable(void* nativeHandle)
{
#ifndef __MINGW32__
    if (nativeHandle)
    {
        delete static_cast<std::condition_variable_any*>(nativeHandle);
    }
#endif	
}

int32_t RtNotifyOne(void* nativeHandle)
{
#ifndef __MINGW32__
    try
    {
        std::condition_variable_any* conditionVariable = static_cast<std::condition_variable_any*>(nativeHandle);
        conditionVariable->notify_one();
        return 0;
    }
    catch (const std::exception& ex)
    {
        return cmajor::rt::InstallError(ex.what());
    }
#endif
	return 0;
}

int32_t RtNotifyAll(void* nativeHandle)
{
#ifndef __MINGW32__	
    try
    {
        std::condition_variable_any* conditionVariable = static_cast<std::condition_variable_any*>(nativeHandle);
        conditionVariable->notify_all();
        return 0;
    }
    catch (const std::exception& ex)
    {
        return cmajor::rt::InstallError(ex.what());
    }
#endif
	return 0;
}

int32_t RtWaitConditionVariable(void* nativeHandle, void* recursiveMutexHandle)
{
#ifndef __MINGW32__
    try
    {
        std::recursive_mutex* recursiveMutex = static_cast<std::recursive_mutex*>(recursiveMutexHandle);
        std::condition_variable_any* conditionVariable = static_cast<std::condition_variable_any*>(nativeHandle);
        std::unique_lock<std::recursive_mutex> lock(*recursiveMutex);
        conditionVariable->wait(lock);
        return 0;
    }
    catch (const std::exception& ex)
    {
        return cmajor::rt::InstallError(ex.what());
    }
#endif
	return 0;
}

enum class CondVarStatus : int32_t
{
    timeout = 0, no_timeout = 1
};

int32_t RtWaitConditionVariableDuration(void* nativeHandle, void* recursiveMutexHandle, int64_t nanoseconds)
{
#ifndef __MINGW32__
    try
    {
        CondVarStatus status = CondVarStatus::timeout;
        std::recursive_mutex* recursiveMutex = static_cast<std::recursive_mutex*>(recursiveMutexHandle);
        std::condition_variable_any* conditionVariable = static_cast<std::condition_variable_any*>(nativeHandle);
        std::chrono::nanoseconds duration(nanoseconds);
        std::unique_lock<std::recursive_mutex> lock(*recursiveMutex);
        if (conditionVariable->wait_for(lock, duration) == std::cv_status::no_timeout)
        {
            status = CondVarStatus::no_timeout;
        }
        return static_cast<int32_t>(status);
    }
    catch (const std::exception& ex)
    {
        return cmajor::rt::InstallError(ex.what());
    }
#endif
	return 0;
}
