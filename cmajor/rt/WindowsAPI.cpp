// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/WindowsAPI.hpp>
#include <Windows.h>

uint64_t WinGetLastError()
{
    return GetLastError();
}

void WinFormatMessage(uint64_t errorCode, char* buffer)
{
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errorCode, LANG_SYSTEM_DEFAULT, buffer, 4096, nullptr);
}

void* WinCreateEvent()
{
    HANDLE handle = CreateEventA(NULL, false, false, NULL);
    return handle;
}

bool WinSetEvent(void* eventHandle)
{
    bool retval = SetEvent(eventHandle);
    return retval;
}

bool WinResetEvent(void* eventHandle)
{
    bool retval = ResetEvent(eventHandle);
    return retval;
}

bool WinWaitEvent(void* eventHandle)
{
    DWORD retval = WaitForSingleObject(eventHandle, INFINITE);
    return retval == WAIT_OBJECT_0;
}

bool WinCloseHandle(void* handle)
{
    bool retval = CloseHandle(handle);
    return retval;
}

int32_t WinWaitForMultipleObjects(uint32_t count, void** handles)
{
    uint32_t retval = WaitForMultipleObjects(count, handles, false, INFINITE);
    if (retval == WAIT_FAILED)
    {
        return -1;
    }
    else if (retval == WAIT_TIMEOUT)
    {
        return -2;
    }
    else if (retval >= WAIT_OBJECT_0 && retval < WAIT_OBJECT_0 + count)
    {
        return retval - WAIT_OBJECT_0;
    }
    else if (retval >= WAIT_ABANDONED_0 && retval < WAIT_ABANDONED_0 + count)
    {
        return -3;
    }
    else
    {
        return -4;
    }
}

void* WinFindFirstChangeNotification(const char* directoryPath)
{
    void* handle = FindFirstChangeNotificationA(directoryPath, false, FILE_NOTIFY_CHANGE_FILE_NAME);
    if (handle == INVALID_HANDLE_VALUE)
    {
        return nullptr;
    }
    return handle;
}

bool WinFindNextChangeNotification(void* handle)
{
    bool retval = FindNextChangeNotification(handle);
    return retval;
}

bool WinFindCloseChangeNotification(void* handle)
{
    bool retval = FindCloseChangeNotification(handle);
    return retval;
}
