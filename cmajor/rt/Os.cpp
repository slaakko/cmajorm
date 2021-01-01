// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/Os.hpp>
#include <cmajor/rt/Screen.hpp>
#include <cstring>
#ifdef _WIN32
#include <Windows.h>
#endif
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Time.hpp>
#include <soulng/util/MemoryWriter.hpp>

using namespace soulng::unicode;

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
    *c = '\0';
    INPUT_RECORD buffer[1];
    DWORD numRead = 0;
    bool retVal = ReadConsoleInputW(consoleInputHandle, buffer, 1, &numRead); 
    if (retVal)
    {
        if (numRead > 0)
        {
            INPUT_RECORD* inputRecord = &buffer[0];
            if (inputRecord->EventType == KEY_EVENT)
            {
                KEY_EVENT_RECORD* keyEventRecord = reinterpret_cast<KEY_EVENT_RECORD*>(&inputRecord->Event);
                if (keyEventRecord->bKeyDown)
                {
                    bool ctrl = false;
                    bool shift = false;
                    if (keyEventRecord->dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED))
                    {
                        ctrl = true;
                    }
                    else if (keyEventRecord->dwControlKeyState & SHIFT_PRESSED)
                    {
                        shift = true;
                    }
                    switch (keyEventRecord->wVirtualKeyCode)
                    {
                        case VK_BACK: *c = static_cast<char32_t>(keyBackspace); break;
                        case VK_TAB: *c = static_cast<char32_t>(keyTab); break;
                        case VK_RETURN: *c = static_cast<char32_t>(keyEnter); break;
                        case VK_ESCAPE: *c = static_cast<char32_t>(keyEscape); break;
                        case VK_PRIOR: if (ctrl) *c = static_cast<char32_t>(keyControlPgUp); else if (shift) *c = static_cast<char32_t>(keyShiftPgUp); else *c = static_cast<char32_t>(keyPgUp); break;
                        case VK_NEXT: if (ctrl) *c = static_cast<char32_t>(keyControlPgDown); else if (shift) *c = static_cast<char32_t>(keyShiftPgDown); else *c = static_cast<char32_t>(keyPgDown); break;
                        case VK_END: if (ctrl) *c = static_cast<char32_t>(keyControlEnd); else if (shift) *c = static_cast<char32_t>(keyShiftEnd); else *c = static_cast<char32_t>(keyEnd); break;
                        case VK_HOME: if (ctrl) *c = static_cast<char32_t>(keyControlHome); else if (shift) *c = static_cast<char32_t>(keyShiftHome); else *c = static_cast<char32_t>(keyHome); break;
                        case VK_LEFT: if (ctrl) *c = static_cast<char32_t>(keyControlLeft); else if (shift) *c = static_cast<char32_t>(keyShiftLeft); else *c = static_cast<char32_t>(keyLeft); break;
                        case VK_RIGHT: if (ctrl) *c = static_cast<char32_t>(keyControlRight); else if (shift) *c = static_cast<char32_t>(keyShiftRight); else *c = static_cast<char32_t>(keyRight); break;
                        case VK_UP: if (ctrl) *c = static_cast<char32_t>(keyControlUp); else if (shift) *c = static_cast<char32_t>(keyShiftUp); else *c = static_cast<char32_t>(keyUp); break;
                        case VK_DOWN: if (ctrl) *c = static_cast<char32_t>(keyControlDown); else if (shift) *c = static_cast<char32_t>(keyShiftDown); else *c = static_cast<char32_t>(keyDown); break;
                        case VK_DELETE: if (ctrl) *c = static_cast<char32_t>(keyControlDel); else if (shift) *c = static_cast<char32_t>(keyShiftDel); else *c = static_cast<char32_t>(keyDel); break;
                        case VK_F1: if (ctrl) *c = static_cast<char32_t>(keyControlF1); else *c = static_cast<char32_t>(keyF1); break;
                        case VK_F2: if (ctrl) *c = static_cast<char32_t>(keyControlF2); else *c = static_cast<char32_t>(keyF2); break;
                        case VK_F3: if (ctrl) *c = static_cast<char32_t>(keyControlF3); else *c = static_cast<char32_t>(keyF3); break;
                        case VK_F4: if (ctrl) *c = static_cast<char32_t>(keyControlF4); else *c = static_cast<char32_t>(keyF4); break;
                        case VK_F5: if (ctrl) *c = static_cast<char32_t>(keyControlF5); else *c = static_cast<char32_t>(keyF5); break;
                        case VK_F6: if (ctrl) *c = static_cast<char32_t>(keyControlF6); else *c = static_cast<char32_t>(keyF6); break;
                        case VK_F7: if (ctrl) *c = static_cast<char32_t>(keyControlF7); else *c = static_cast<char32_t>(keyF7); break;
                        case VK_F8: if (ctrl) *c = static_cast<char32_t>(keyControlF8); else *c = static_cast<char32_t>(keyF8); break;
                        case VK_F9: if (ctrl) *c = static_cast<char32_t>(keyControlF9); else *c = static_cast<char32_t>(keyF9); break;
                        case VK_F10: if (ctrl) *c = static_cast<char32_t>(keyControlF10); else *c = static_cast<char32_t>(keyF10); break;
                        case VK_F11: if (ctrl) *c = static_cast<char32_t>(keyControlF11); else *c = static_cast<char32_t>(keyF11); break;
                        case VK_F12: if (ctrl) *c = static_cast<char32_t>(keyControlF12); else *c = static_cast<char32_t>(keyF12); break;
                    }
                    if (*c == '\0' && ctrl)
                    {
                        switch (keyEventRecord->wVirtualKeyCode)
                        {
                            case 'A': *c = static_cast<char32_t>(keyControlA); break;
                            case 'B': *c = static_cast<char32_t>(keyControlB); break;
                            case 'C': *c = static_cast<char32_t>(keyControlC); break;
                            case 'D': *c = static_cast<char32_t>(keyControlD); break;
                            case 'E': *c = static_cast<char32_t>(keyControlE); break;
                            case 'F': *c = static_cast<char32_t>(keyControlF); break;
                            case 'G': *c = static_cast<char32_t>(keyControlG); break;
                            case 'H': *c = static_cast<char32_t>(keyControlH); break;
                            case 'I': *c = static_cast<char32_t>(keyControlI); break;
                            case 'J': *c = static_cast<char32_t>(keyControlJ); break;
                            case 'K': *c = static_cast<char32_t>(keyControlK); break;
                            case 'L': *c = static_cast<char32_t>(keyControlL); break;
                            case 'M': *c = static_cast<char32_t>(keyControlM); break;
                            case 'N': *c = static_cast<char32_t>(keyControlN); break;
                            case 'O': *c = static_cast<char32_t>(keyControlO); break;
                            case 'P': *c = static_cast<char32_t>(keyControlP); break;
                            case 'Q': *c = static_cast<char32_t>(keyControlQ); break;
                            case 'R': *c = static_cast<char32_t>(keyControlR); break;
                            case 'S': *c = static_cast<char32_t>(keyControlS); break;
                            case 'T': *c = static_cast<char32_t>(keyControlT); break;
                            case 'U': *c = static_cast<char32_t>(keyControlU); break;
                            case 'V': *c = static_cast<char32_t>(keyControlV); break;
                            case 'W': *c = static_cast<char32_t>(keyControlW); break;
                            case 'X': *c = static_cast<char32_t>(keyControlX); break;
                            case 'Y': *c = static_cast<char32_t>(keyControlY); break;
                            case 'Z': *c = static_cast<char32_t>(keyControlZ); break;
                        }
                    }
                    if (*c == '\0')
                    {
                        *c = keyEventRecord->uChar.UnicodeChar;
                    }
                }
            }
        }
    }
    return retVal;
}

extern "C" RT_API bool OsGetConsoleScreenBufferInfo(void* consoleOutputHandle, int32_t* cursorPosX, int32_t* cursorPosY, int32_t* screenSizeX, int32_t* screenSizeY, uint16_t* attrs)
{
    if (!cursorPosX || !cursorPosY || !screenSizeX || !screenSizeY || !attrs) return false;
    CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
    bool retVal = GetConsoleScreenBufferInfo(consoleOutputHandle, &consoleScreenBufferInfo);
    if (!retVal) return false;
    *cursorPosX = consoleScreenBufferInfo.dwCursorPosition.X;
    *cursorPosY = consoleScreenBufferInfo.dwCursorPosition.Y;
    *screenSizeX = consoleScreenBufferInfo.dwSize.X;
    *screenSizeY = consoleScreenBufferInfo.dwSize.Y;
    *attrs = consoleScreenBufferInfo.wAttributes;
    return true;
}

extern "C" RT_API bool OsSetConsoleCursorPosition(void* consoleOutputHandle, int cursorPosX, int cursorPosY)
{
    COORD cursorPos;
    cursorPos.X = cursorPosX;
    cursorPos.Y = cursorPosY;
    return SetConsoleCursorPosition(consoleOutputHandle, cursorPos);
}

extern "C" RT_API bool OsWriteConsole(void* consoleOutputHandle, const char32_t* chars)
{
    DWORD numCharsWritten = 0;
    std::u32string s(chars);
    std::u16string w(soulng::unicode::ToUtf16(s));
    return WriteConsoleW(consoleOutputHandle, w.c_str(), w.length(), &numCharsWritten, nullptr);
}

extern "C" RT_API bool OsSetConsoleTextAttribute(uint16_t attrs)
{
    return SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attrs);
}

extern "C" RT_API void* OsCreateConsoleOutputHandle()
{
    void* result = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (result == INVALID_HANDLE_VALUE)
    {
        return nullptr;
    }
    else
    {
        return result;
    }
}

extern "C" RT_API void* OsCreateHostFile(const char* filePath, bool randomAccess)
{
    DWORD accessFlag = FILE_FLAG_SEQUENTIAL_SCAN;
    if (randomAccess)
    {
        accessFlag = FILE_FLAG_RANDOM_ACCESS;
    }
    HANDLE handle = CreateFileA(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL | accessFlag | FILE_FLAG_OVERLAPPED, NULL);
    if (handle == INVALID_HANDLE_VALUE)
    {
        return nullptr;
    }
    else
    {
        return handle;
    }
}

extern "C" RT_API void* OsOpenHostFile(const char* filePath, bool randomAccess)
{
    DWORD accessFlag = FILE_FLAG_SEQUENTIAL_SCAN;
    if (randomAccess)
    {
        accessFlag = FILE_FLAG_RANDOM_ACCESS;
    }
    HANDLE handle = CreateFileA(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | accessFlag | FILE_FLAG_OVERLAPPED, NULL);
    if (handle == INVALID_HANDLE_VALUE)
    {
        return nullptr;
    }
    else
    {
        return handle;
    }
}

extern "C" RT_API void OsCloseHostFile(void* fileHandle)
{
    CloseHandle(fileHandle);
}

extern "C" RT_API void* OsCreateIoCompletionPort()
{
    HANDLE handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
    if (handle == NULL)
    {
        return nullptr;
    }
    else
    {
        return handle;
    }
}

extern "C" RT_API void* OsAssociateFileWithCompletionPort(void* fileHandle, void* completionPort, uint64_t completionKey)
{
    HANDLE handle = CreateIoCompletionPort(fileHandle, completionPort, completionKey, 0);
    if (handle == NULL)
    {
        return nullptr;
    }
    else
    {
        return handle;
    }
}

extern "C" RT_API void OsCloseIoCompletionPort(void* completionPortHandle)
{
    CloseHandle(completionPortHandle);
}

extern "C" RT_API bool OsGetQueuedCompletionStatus(void* completionPortHandle, uint64_t* numberOfBytes, uint64_t* completionKey, void** overlappedPtr)
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

extern "C" RT_API bool OsPostQueuedCompletionStatus(void* completionPortHandle, uint64_t numberOfBytes, uint64_t completionKey)
{
    bool retval = PostQueuedCompletionStatus(completionPortHandle, numberOfBytes, completionKey, NULL);
    return retval;
}

extern "C" RT_API void* OsCreateOverlapped(uint64_t offset)
{
    OVERLAPPED* overlapped = new OVERLAPPED();
    std::memset(overlapped, 0, sizeof(OVERLAPPED));
    overlapped->Offset = static_cast<uint32_t>(offset);
    overlapped->OffsetHigh = static_cast<uint32_t>(offset >> 32);
    overlapped->hEvent = 0;
    return overlapped;
}

extern "C" RT_API void OsDestroyOverlapped(void* overlapped)
{
    delete overlapped;
}

extern "C" RT_API bool OsReadFile(void* fileHandle, void* buffer, uint32_t numberOfBytesToRead, void* overlapped)
{
    bool retval = ReadFile(fileHandle, buffer, numberOfBytesToRead, NULL, static_cast<LPOVERLAPPED>(overlapped));
    return retval;
}

extern "C" RT_API bool OsWriteFile(void* fileHandle, void* buffer, uint32_t numberOfBytesToWrite, void* overlapped)
{
    bool retval = WriteFile(fileHandle, buffer, numberOfBytesToWrite, NULL, static_cast<LPOVERLAPPED>(overlapped));
    return retval;
}

extern "C" RT_API void* OsCreateEvent()
{
    HANDLE handle = CreateEventA(NULL, false, false, NULL);
    return handle;
}

extern "C" RT_API bool OsSetEvent(void* eventHandle)
{
    bool retval = SetEvent(eventHandle);
    return retval;
}

extern "C" RT_API bool OsResetEvent(void* eventHandle)
{
    bool retval = ResetEvent(eventHandle);
    return retval;
}

extern "C" RT_API void OsWaitEvent(void* eventHandle)
{
    WaitForSingleObject(eventHandle, INFINITE);
}

extern "C" RT_API void OsCloseEvent(void* eventHandle)
{
    CloseHandle(eventHandle);
}

extern "C" RT_API int32_t OsWaitForMultipleObjects(uint32_t count, void** handles)
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

extern "C" RT_API void* OsConvertThreadToFiber(void* param)
{
    return ConvertThreadToFiber(param);
}

extern "C" RT_API void* OsCreateFiber(uint64_t stackSize, void* startAddress, void* param)
{
    return CreateFiber(stackSize, (LPFIBER_START_ROUTINE)startAddress, param);
}

extern "C" RT_API void OsSwitchToFiber(void* fiber)
{
    SwitchToFiber(fiber);
}

extern "C" RT_API void* OsGetFiberData()
{
    return GetFiberData();
}

extern "C" RT_API void OsDeleteFiber(void* fiber)
{
    DeleteFiber(fiber);
}

extern "C" RT_API uint64_t OsGetLastError()
{
    return GetLastError();
}

extern "C" RT_API void OsFormatMessage(uint64_t errorCode, char16_t* buffer)
{
    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errorCode, LANG_SYSTEM_DEFAULT, (LPWSTR)buffer, 4096, nullptr);
}

extern "C" RT_API bool OsGetLogicalDrives(char* buffer, int bufSize)
{
    char d[4096];
    int retval = GetLogicalDriveStringsA(4096, &d[0]);
    if (retval == 0)
    {
        return false;
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
        int i = 0;
        for (char c : s)
        {
            if (i >= bufSize)
            {
                return false;
            }
            *buffer++ = c;
            ++i;
        }
        if (i >= bufSize)
        {
            return false;
        }
        else
        {
            *buffer = '\0';
        }
        return true;
    }
}

extern "C" RT_API uint32_t OsGetDriveType(const char* rootPathName)
{
    return GetDriveTypeA(rootPathName);
}

extern "C" RT_API int64_t OsGetFileSize(void* fileHandle)
{
    LARGE_INTEGER fileSize;
    if (GetFileSizeEx(fileHandle, &fileSize))
    {
        return fileSize.QuadPart;
    }
    else
    {
        return -1;
    }
}

extern "C" RT_API uint32_t OsGetFileAttributes(const char* filePath)
{
    std::u16string fp = ToUtf16(std::string(filePath));
    uint32_t attrs = GetFileAttributes((LPWSTR)fp.c_str());
    if (attrs != INVALID_FILE_ATTRIBUTES)
    {
        return attrs;
    }
    else
    {
        return static_cast<uint32_t>(-1);
    }
}

extern "C" RT_API void* OsFindFirstFile(const char* pathMask, char* fileName)
{
    WIN32_FIND_DATAA findData;
    HANDLE handle = FindFirstFileA(pathMask, &findData);
    if (handle == INVALID_HANDLE_VALUE)
    {
        return nullptr;
    }
    else
    {
        std::string pathName = &findData.cFileName[0];
        std::string fname = soulng::util::Path::GetFileName(pathName);
        strcpy(fileName, fname.c_str());
        return handle;
    }
}

extern "C" RT_API bool OsFindNextFile(void* findHandle, char* fileName)
{
    WIN32_FIND_DATAA findData;
    bool result = FindNextFileA(findHandle, &findData);
    if (result)
    {
        std::string pathName = &findData.cFileName[0];
        std::string fname = soulng::util::Path::GetFileName(pathName);
        strcpy(fileName, fname.c_str());
        return true;
    }
    else
    {
        return false;
    }
}

extern "C" RT_API void OsFindClose(void* findHandle)
{
    FindClose(findHandle);
}

extern "C" RT_API bool OsGetFileTimes(const char* filePath, uint8_t* ctime, uint8_t* mtime, uint8_t* atime)
{
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    SYSTEMTIME systemTime;
    SYSTEMTIME localTime;
    std::u16string fp = ToUtf16(std::string(filePath));
    if (GetFileAttributesEx((LPWSTR)fp.c_str(), GetFileExInfoStandard, &fileInfo))
    {
        if (FileTimeToSystemTime(&fileInfo.ftCreationTime, &systemTime))
        {
            if (SystemTimeToTzSpecificLocalTime(NULL, &systemTime, &localTime))
            {
                soulng::util::DateTime dtCreationTime(soulng::util::Date(localTime.wYear, static_cast<soulng::util::Month>(localTime.wMonth), localTime.wDay),
                    60 * 60 * localTime.wHour + 60 * localTime.wMinute + localTime.wSecond);
                soulng::util::MemoryWriter ctimeWriter(ctime, 8);
                ctimeWriter.Write(dtCreationTime);
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
        if (FileTimeToSystemTime(&fileInfo.ftLastWriteTime, &systemTime))
        {
            if (SystemTimeToTzSpecificLocalTime(NULL, &systemTime, &localTime))
            {
                soulng::util::DateTime dtWriteTime(soulng::util::Date(localTime.wYear, static_cast<soulng::util::Month>(localTime.wMonth), localTime.wDay),
                    60 * 60 * localTime.wHour + 60 * localTime.wMinute + localTime.wSecond);
                soulng::util::MemoryWriter mtimeWriter(mtime, 8);
                mtimeWriter.Write(dtWriteTime);
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
        if (FileTimeToSystemTime(&fileInfo.ftLastAccessTime, &systemTime))
        {
            if (SystemTimeToTzSpecificLocalTime(NULL, &systemTime, &localTime))
            {
                soulng::util::DateTime dtAccessTime(soulng::util::Date(localTime.wYear, static_cast<soulng::util::Month>(localTime.wMonth), localTime.wDay),
                    60 * 60 * localTime.wHour + 60 * localTime.wMinute + localTime.wSecond);
                soulng::util::MemoryWriter atimeWriter(atime, 8);
                atimeWriter.Write(dtAccessTime);
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
        return true;
    }
    else
    {
        return false;
    }
}

extern "C" RT_API bool OsGetComputerName(char* buffer, int size)
{
    DWORD n = size;
    if (GetComputerNameExA(ComputerNameDnsHostname, buffer, &n))
    {
        return true;
    }
    else
    {
        return false;
    }
}

extern "C" RT_API int OsGetMaxComputerNameLength()
{
    DWORD n = 0;
    if (GetComputerNameExA(ComputerNameDnsHostname, NULL, &n))
    {
        return n;
    }
    else
    {
        return MAX_COMPUTERNAME_LENGTH;
    }
}

#endif 
