// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/Os.hpp>
#include <cstring>
#ifdef _WIN32
#include <Windows.h>
#endif 

#ifdef _WIN32

extern "C" RT_API uint64_t OsAllocateMemoryPage(uint64_t pageSize)
{
    uint64_t addr = reinterpret_cast<uint64_t>(VirtualAlloc(nullptr, pageSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    return addr;
}

extern "C" RT_API bool OsFreeMemoryPage(uint64_t pageAddress)
{
    bool retval = VirtualFree(reinterpret_cast<void*>(pageAddress), 0, MEM_RELEASE);
    return retval;
}

extern "C" RT_API void OsCopyMemoryPage(uint64_t fromPageAddress, uint64_t toPageAddress, uint64_t pageSize)
{
    void* from = reinterpret_cast<void*>(fromPageAddress);
    void* to = reinterpret_cast<void*>(toPageAddress);
    std::memcpy(to, from, pageSize);
}

extern "C" RT_API void OsWriteMemoryByte(uint64_t address, uint8_t b)
{
    void* ptr = reinterpret_cast<void*>(address);
    uint8_t* bytePtr = reinterpret_cast<uint8_t*>(ptr);
    *bytePtr = b;
}

extern "C" RT_API uint8_t OsReadMemoryByte(uint64_t address) 
{
    void* ptr = reinterpret_cast<void*>(address);
    uint8_t* bytePtr = reinterpret_cast<uint8_t*>(ptr);
    return *bytePtr;
}

extern "C" RT_API void* OsGetStdHandle(int32_t stdInOutOrErr)
{
    switch (stdInOutOrErr)
    {
        case 0: return GetStdHandle(STD_INPUT_HANDLE);
        case 1: return GetStdHandle(STD_OUTPUT_HANDLE);
        case 2: return GetStdHandle(STD_ERROR_HANDLE);
    }
    return nullptr;
}

typedef void(*userCallbackPtr)(void);

VOID CALLBACK WaitOrTimerCallback(PVOID context, BOOLEAN timerOrWaitFired)
{
    userCallbackPtr ucb = reinterpret_cast<userCallbackPtr>(context);
    ucb();
}

extern "C" RT_API bool OsRegisterConsoleCallback(void* consoleInputHandle, uint64_t callBackValue)
{
    HANDLE handle = nullptr;
    PHANDLE newObject = &handle;
    HANDLE hConsole = reinterpret_cast<HANDLE>(consoleInputHandle);
    PVOID context = reinterpret_cast<PVOID>(callBackValue);
    WAITORTIMERCALLBACK consoleCallback = WaitOrTimerCallback;
    bool retVal = RegisterWaitForSingleObject(newObject, hConsole, consoleCallback, context, INFINITE, WT_EXECUTEDEFAULT);
    return retVal;
}

extern "C" RT_API bool OsReadConsoleInput(void* consoleInputHandle, char32_t* c)
{
    if (consoleInputHandle == INVALID_HANDLE_VALUE || c == nullptr) return false;
    INPUT_RECORD buffer[1];
    DWORD numRead = 0;
    bool retVal = ReadConsoleInput(consoleInputHandle, buffer, 1, &numRead);
    if (retVal)
    {
        if (numRead > 0)
        {
            INPUT_RECORD* inputRecord = &buffer[0];
            if (inputRecord->EventType == KEY_EVENT)
            {
                KEY_EVENT_RECORD* keyEventRecord = reinterpret_cast<KEY_EVENT_RECORD*>(&inputRecord->Event);
                *c = keyEventRecord->uChar.UnicodeChar;
            }
        }
        else
        {
            *c = '\0';
        }
    }
    return retVal;
}

#endif 
