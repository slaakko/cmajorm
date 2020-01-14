// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/WindowsAPI.hpp>
#include <Windows.h>
#include <map>

struct CodeMapping
{
    CodeMapping(long extCode_, long winCode_) : extCode(extCode_), winCode(winCode_)
    {
    }
    long extCode;
    long winCode;
};

long ERROR_OUT_OF_RESOURCES = 0;

CodeMapping code_mapping[] =
{
    CodeMapping(0, ERROR_OUT_OF_RESOURCES),
    CodeMapping(1, ERROR_FILE_NOT_FOUND),
    CodeMapping(2, ERROR_PATH_NOT_FOUND),
    CodeMapping(3, ERROR_BAD_FORMAT),
    CodeMapping(4, SE_ERR_ACCESSDENIED),
    CodeMapping(5, SE_ERR_ASSOCINCOMPLETE),
    CodeMapping(6, SE_ERR_DDEBUSY),
    CodeMapping(7, SE_ERR_DDEFAIL),
    CodeMapping(8, SE_ERR_DDETIMEOUT),
    CodeMapping(9, SE_ERR_DLLNOTFOUND),
    CodeMapping(10, SE_ERR_FNF),
    CodeMapping(11, SE_ERR_NOASSOC),
    CodeMapping(12, SE_ERR_OOM),
    CodeMapping(13, SE_ERR_PNF),
    CodeMapping(14, SE_ERR_SHARE)
};

bool codeMappingInitialized = false;

std::map<long, long> winToExtCodeMap;
std::map<long, long> extToWinCodeMap;

bool InitCodeMapping()
{
    for (const CodeMapping& mapping : code_mapping)
    {
        auto it1 = winToExtCodeMap.find(mapping.winCode);
        if (it1 != winToExtCodeMap.cend())
        {
            return false;
        }
        winToExtCodeMap[mapping.winCode] = mapping.extCode;
        auto it2 = extToWinCodeMap.find(mapping.extCode);
        if (it2 != extToWinCodeMap.cend())
        {
            return false;
        }
        extToWinCodeMap[mapping.extCode] = mapping.winCode;
    }
    codeMappingInitialized = true;
    return true;
}

long MapWinCodeToExtCode(long winCode)
{
    if (!codeMappingInitialized)
    {
        if (!InitCodeMapping())
        {
            return -2;
        }
    }
    auto it = winToExtCodeMap.find(winCode);
    if (it != winToExtCodeMap.cend())
    {
        return it->second;
    }
    return -1;
}

long MapExtCodeToWinCode(long extCode)
{
    if (!codeMappingInitialized)
    {
        if (!InitCodeMapping())
        {
            return -2;
        }
    }
    auto it = extToWinCodeMap.find(extCode);
    if (it != extToWinCodeMap.cend())
    {
        return it->second;
    }
    return -1;
}

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

bool WinShellExecute(const char* filePath, int64_t& errorCode)
{
    int code = (int)ShellExecuteA(NULL, "open", filePath, NULL, NULL, SW_SHOWNORMAL);
    if (code > 32)
    {
        errorCode = 0;
        return true;
    }
    errorCode = MapWinCodeToExtCode(code);
    return false;
}
