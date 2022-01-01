// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/Fiber.hpp>
#ifdef _WIN32
#include <Windows.h>
#else
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#endif

namespace soulng { namespace util {

#ifdef _WIN32

void* ConvertThreadToFiber(void* param)
{
    return ::ConvertThreadToFiber(param);
}

void* CreateFiber(uint64_t stackSize, void* startAddress, void* param)
{
    return ::CreateFiber(stackSize, (LPFIBER_START_ROUTINE)startAddress, param);
}

void SwitchToFiber(void* fiber)
{
    ::SwitchToFiber(fiber);
}

void* GetFiberData()
{
    return ::GetFiberData();
}

void DeleteFiber(void* fiber)
{
    ::DeleteFiber(fiber);
}

#else

const int maxFibers = 64;
static void* fiberParam[maxFibers];
static ucontext_t contexts[maxFibers];
static int fiberParamIndex = 0;
static int currentFiber = 0;

void* ConvertThreadToFiber(void* param)
{
    return reinterpret_cast<void*>(-1);
}

void* CreateFiber(uint64_t stackSize, void* startAddress, void* param)
{
    fiberParamIndex = fiberParamIndex + 1;
    if (fiberParamIndex >= maxFibers)
    {
        return nullptr;
    }
    if (stackSize == 0)
    {
        stackSize = SIGSTKSZ;
    }
    fiberParam[fiberParamIndex] = param;
    ucontext_t* ucp = &contexts[fiberParamIndex];
    int retval = getcontext(ucp);
    if (retval == -1)
    {
        return nullptr;
    }
    ucp->uc_link = &contexts[0];
    ucp->uc_stack.ss_sp = new uint8_t[stackSize];
    ucp->uc_stack.ss_size = stackSize;
    makecontext(ucp, (void (*)())startAddress, 1, fiberParamIndex);
    return reinterpret_cast<void*>(fiberParamIndex);
}

void SwitchToFiber(void* fiber)
{
    int fiberParamIndex = *reinterpret_cast<int*>(&fiber);
    if (fiberParamIndex == -1)
    {
        fiberParamIndex = 0;
    }
    ucontext_t* ucp = &contexts[fiberParamIndex];
    ucontext_t* oucp = &contexts[currentFiber];
    currentFiber = fiberParamIndex;
    swapcontext(oucp, ucp);
}

void* GetFiberData()
{
    return fiberParam[currentFiber];
}

void DeleteFiber(void* fiber)
{
    int fiberParamIndex = *reinterpret_cast<int*>(&fiber);
    ucontext_t* ucp = &contexts[fiberParamIndex];
    delete[] static_cast<uint8_t*>(ucp->uc_stack.ss_sp);
}

#endif

} } // namespace soulng::util

