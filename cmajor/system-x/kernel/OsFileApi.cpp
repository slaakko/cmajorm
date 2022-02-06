// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/OsFileApi.hpp>
#include <system-x/kernel/Error.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Path.hpp>
#include <Windows.h>
#include <cstring>

namespace cmsx::kernel {

using namespace soulng::unicode;
using namespace soulng::util;

const uint32_t fixedDriveType = DRIVE_FIXED;

void* OsCreateHostFile(const char* filePath, bool randomAccess)
{
    DWORD accessFlag = FILE_FLAG_SEQUENTIAL_SCAN;
    if (randomAccess)
    {
        accessFlag = FILE_FLAG_RANDOM_ACCESS;
    }
    HANDLE handle = CreateFileA(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL | accessFlag | FILE_FLAG_OVERLAPPED, NULL);
    if (handle == INVALID_HANDLE_VALUE)
    {
        ThrowLastHostError();
        return nullptr;
    }
    else
    {
        return handle;
    }
}

void* OsOpenHostFile(const char* filePath, bool randomAccess)
{
    DWORD accessFlag = FILE_FLAG_SEQUENTIAL_SCAN;
    if (randomAccess)
    {
        accessFlag = FILE_FLAG_RANDOM_ACCESS;
    }
    HANDLE handle = CreateFileA(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | accessFlag | FILE_FLAG_OVERLAPPED, NULL);
    if (handle == INVALID_HANDLE_VALUE)
    {
        ThrowLastHostError();
        return nullptr;
    }
    else
    {
        return handle;
    }
}

void OsCloseHostFile(void* fileHandle)
{
    CloseHandle(fileHandle);
}

void* OsCreateIoCompletionPort()
{
    HANDLE handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
    if (handle == NULL)
    {
        ThrowLastHostError();
        return nullptr;
    }
    else
    {
        return handle;
    }
}

void* OsAssociateFileWithCompletionPort(void* fileHandle, void* completionPort, uint64_t completionKey)
{
    HANDLE handle = CreateIoCompletionPort(fileHandle, completionPort, completionKey, 0);
    if (handle == NULL)
    {
        ThrowLastHostError();
        return nullptr;
    }
    else
    {
        return handle;
    }
}

void OsCloseIoCompletionPort(void* completionPortHandle)
{
    CloseHandle(completionPortHandle);
}

bool OsGetQueuedCompletionStatus(void* completionPortHandle, uint64_t* numberOfBytes, uint64_t* completionKey, void** overlappedPtr)
{
    DWORD numBytes = 0;
    LPOVERLAPPED overlapped = nullptr;
    bool retval = GetQueuedCompletionStatus(completionPortHandle, &numBytes, completionKey, &overlapped, INFINITE);
    if (retval)
    {
        *numberOfBytes = numBytes;
        *overlappedPtr = static_cast<void*>(overlapped);
    }
    return retval;
}

bool OsPostQueuedCompletionStatus(void* completionPortHandle, uint64_t numberOfBytes, uint64_t completionKey)
{
    bool retval = PostQueuedCompletionStatus(completionPortHandle, numberOfBytes, completionKey, NULL);
    if (!retval)
    {
        ThrowLastHostError();
        return false;
    }
    return retval;
}

void* OsCreateOverlapped(uint64_t offset)
{
    OVERLAPPED* overlapped = new OVERLAPPED();
    std::memset(overlapped, 0, sizeof(OVERLAPPED));
    overlapped->Offset = static_cast<uint32_t>(offset);
    overlapped->OffsetHigh = static_cast<uint32_t>(offset >> 32);
    overlapped->hEvent = 0;
    return overlapped;
}

void OsDestroyOverlapped(void* overlapped)
{
    delete overlapped;
}

bool OsReadFile(void* fileHandle, void* buffer, uint32_t numberOfBytesToRead, void* overlapped)
{
    bool retval = ReadFile(fileHandle, buffer, numberOfBytesToRead, NULL, static_cast<LPOVERLAPPED>(overlapped));
    if (!retval)
    {
        uint64_t errorCode = GetLastHostErrorCode();
        if (errorCode == ERROR_IO_PENDING)
        {
            return true;
        }
        else
        {
            ThrowLastHostError();
        }
    }
    return retval;
}

bool OsWriteFile(void* fileHandle, void* buffer, uint32_t numberOfBytesToWrite, void* overlapped)
{
    bool retval = WriteFile(fileHandle, buffer, numberOfBytesToWrite, NULL, static_cast<LPOVERLAPPED>(overlapped));
    if (!retval)
    {
        uint64_t errorCode = GetLastHostErrorCode();
        if (errorCode == ERROR_IO_PENDING)
        {
            return true;
        }
        else
        {
            ThrowLastHostError();
        }
    }
    return retval;
}

std::string OsGetLogicalDrives()
{
    char d[4096];
    int retval = GetLogicalDriveStringsA(4096, &d[0]);
    if (retval == 0)
    {
        return std::string();
    }
    else
    {
        std::string s;
        for (int i = 0; i < retval; ++i)
        {
            char c = d[i];
            if (c == '\0')
            {
                s.append(1, ';');
            }
            else
            {
                s.append(1, c);
            }
        }
        if (!s.empty() && s.back() == ';')
        {
            s.erase(s.end() - 1);
        }
        return s;
    }
}

uint32_t OsGetDriveType(const char* rootPathName)
{
    return GetDriveTypeA(rootPathName);
}

bool OsFindFirstFile(const std::string& directoryName, std::string& entry, void*& searchHandle)
{
    std::u16string s = ToUtf16(MakeNativePath(directoryName) + "\\*.*");
    WIN32_FIND_DATAW findData;
    HANDLE handle = FindFirstFile((LPCWSTR)s.c_str(), &findData);
    if (handle == INVALID_HANDLE_VALUE)
    {
        searchHandle = nullptr;
        return false;
    }
    else
    {
        searchHandle = handle;
        std::u16string entryStr((char16_t*)&findData.cFileName[0]);
        entry = ToUtf8(entryStr);
        return true;
    }
}

bool OsFindNextFile(void* searchHandle, std::string& entry)
{
    WIN32_FIND_DATAW findData;
    if (FindNextFile(searchHandle, &findData))
    {
        std::u16string entryStr((char16_t*)&findData.cFileName[0]);
        entry = ToUtf8(entryStr);
        return true;
    }
    else
    {
        FindClose(searchHandle);
        return false;
    }
}

int OsGetConsoleNumberOfColumns()
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
    int columns = info.srWindow.Right - info.srWindow.Left + 1;
    return columns;
}

int OsGetConsoleNumberOfRows()
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
    int rows = info.srWindow.Bottom - info.srWindow.Top + 1;
    return rows;
}

} // namespace cmsx::kernel
