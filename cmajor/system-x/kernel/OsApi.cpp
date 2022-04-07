// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/OsApi.hpp>
#include <system-x/kernel/Error.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Path.hpp>
#include <Windows.h>
#include <lmcons.h>
#include <cstring>

namespace cmsx::kernel {

using namespace soulng::unicode;
using namespace soulng::util;

const uint32_t fixedDriveType = DRIVE_FIXED;

std::string OsKeyName(int key)
{
    switch (key)
    {
        case keyBackspace: return "<backspace>";
        case keyTab: return "<tab>";
        case keyNewLine: return "<newline>";
        case keyControlA: return "<ctrl-A>";
        case keyControlB: return "<ctrl-B>";
        case keyControlC: return "<ctrl-C>";
        case keyControlD: return "<ctrl-D>";
        case keyControlE: return "<ctrl-E>";
        case keyControlF: return "<ctrl-F>";
        case keyControlG: return "<ctrl-G>";
        case keyControlK: return "<ctrl-K>";
        case keyControlL: return "<ctrl-L>";
        case keyControlM: return "<ctrl-M>";
        case keyControlN: return "<ctrl-N>";
        case keyControlO: return "<ctrl-O>";
        case keyControlQ: return "<ctrl-Q>";
        case keyControlR: return "<ctrl-R>";
        case keyControlS: return "<ctrl-S>";
        case keyControlT: return "<ctrl-T>";
        case keyControlU: return "<ctrl-U>";
        case keyControlV: return "<ctrl-V>";
        case keyControlW: return "<ctrl-W>";
        case keyControlX: return "<ctrl-X>";
        case keyControlY: return "<ctrl-Y>";
        case keyControlZ: return "<ctrl-Z>";
        case keyEscape: return "<escape>";
        case keyFS: return "<fs>";
        case keyGS: return "<gs>";
        case keyRS: return "<rs>";
        case keyUS: return "<us>";
        case keyDown: return "<down>";
        case keyUp: return "<up>";
        case keyLeft: return "<left>";
        case keyRight: return "<right>";
        case keyHome: return "<home>";
        case keyMsg: return "<msg>";
        case keyF1: return "<f1>";
        case keyF2: return "<f2>";
        case keyF3: return "<f3>";
        case keyF4: return "<f4>";
        case keyF5: return "<f5>";
        case keyF6: return "<f6>";
        case keyF7: return "<f7>";
        case keyF8: return "<f8>";
        case keyF9: return "<f9>";
        case keyF10: return "<f10>";
        case keyF11: return "<f11>";
        case keyF12: return "<f12>";
        case keyDel: return "<del>";
        case keyIns: return "<ins>";
        case keyPgDown: return "<pgdown>";
        case keyPgUp: return "<pgup>";
        case keyPrint: return "<print>";
        case keyEnd: return "<end>";
        case keyShiftDel: return "<shift-del>";
        case keyShiftEnd: return "<shift-end>";
        case keyShiftHome: return "<shift-home>";
        case keyShiftLeft: return "<shift-left>";
        case keyShiftRight: return "<shift-right>";
        case keyResize: return "<shift-resize>";
        case keyShiftUp: return "<shift-up>";
        case keyShiftDown: return "<shift-down>";
        case keyControlUp: return "<ctrl-up>";
        case keyControlDown: return "<ctrl-down>";
        case keyControlLeft: return "<ctrl-left>";
        case keyControlRight: return "<ctrl-right>";
        case keyControlPgUp: return "<ctrl-pgup>";
        case keyControlPgDown: return "<ctrl-pgdown>";
        case keyControlHome: return "<ctrl-home>";
        case keyControlEnd: return "<ctrl-end>";
        case keyControlDel: return "<ctrl-del>";
        case keyControlF1: return "<ctrl-f1>";
        case keyControlF2: return "<ctrl-f2>";
        case keyControlF3: return "<ctrl-f3>";
        case keyControlF4: return "<ctrl-f4>";
        case keyControlF5: return "<ctrl-f5>";
        case keyControlF6: return "<ctrl-f6>";
        case keyControlF7: return "<ctrl-f7>";
        case keyControlF8: return "<ctrl-f8>";
        case keyControlF9: return "<ctrl-f9>";
        case keyControlF10: return "<ctrl-f10>";
        case keyControlF11: return "<ctrl-f11>";
        case keyControlF12: return "<ctrl-f12>";
        case keyShiftPgUp: return "<shift-pgup>";
        case keyShiftPgDown: return "<shift-pgdown>";
        case keyShiftF1: return "<shift-f1>";
        case keyShiftF2: return "<shift-f2>";
        case keyShiftF3: return "<shift-f3>";
        case keyShiftF4: return "<shift-f4>";
        case keyShiftF5: return "<shift-f5>";
        case keyShiftF6: return "<shift-f6>";
        case keyShiftF7: return "<shift-f7>";
        case keyShiftF8: return "<shift-f8>";
        case keyShiftF9: return "<shift-f9>";
        case keyShiftF10: return "<shift-f10>";
        case keyShiftF11: return "<shift-f11>";
        case keyShiftF12: return "<shift-f12>";
        case keyAltA: return "<alt-A>";
        case keyAltB: return "<alt-B>";
        case keyAltC: return "<alt-C>";
        case keyAltD: return "<alt-D>";
        case keyAltE: return "<alt-E>";
        case keyAltF: return "<alt-F>";
        case keyAltG: return "<alt-G>";
        case keyAltH: return "<alt-H>";
        case keyAltI: return "<alt-I>";
        case keyAltJ: return "<alt-J>";
        case keyAltK: return "<alt-K>";
        case keyAltL: return "<alt-L>";
        case keyAltM: return "<alt-M>";
        case keyAltN: return "<alt-N>";
        case keyAltO: return "<alt-O>";
        case keyAltP: return "<alt-P>";
        case keyAltQ: return "<alt-Q>";
        case keyAltR: return "<alt-R>";
        case keyAltS: return "<alt-S>";
        case keyAltT: return "<alt-T>";
        case keyAltU: return "<alt-U>";
        case keyAltV: return "<alt-V>";
        case keyAltW: return "<alt-W>";
        case keyAltX: return "<alt-X>";
        case keyAltY: return "<alt-Y>";
        case keyAltZ: return "<alt-Z>";
        case keyAltF1: return "<alt-f1>";
        case keyAltF2: return "<alt-f2>";
        case keyAltF3: return "<alt-f3>";
        case keyAltF4: return "<alt-f4>";
        case keyAltF5: return "<alt-f5>";
        case keyAltF6: return "<alt-f6>";
        case keyAltF7: return "<alt-f7>";
        case keyAltF8: return "<alt-f8>";
        case keyAltF9: return "<alt-f9>";
        case keyAltF10: return "<alt-f10>";
        case keyAltF11: return "<alt-f11>";
        case keyAltF12: return "<alt-f12>";
        case keyControlShiftLeft: return "<ctrl-shift-left>";
        case keyControlShiftRight: return "<ctrl-shift-right>";
        case keyControlShiftHome: return "<ctrl-shift-home>";
        case keyControlShiftEnd: return "<ctrl-shift-end>";
        case keyControlTab: return "<ctrl-tab>";
        case keyShiftTab: return "<shift-tab>";
        case keyControlIns: return "<ctrl-ins>";
        case keyShiftIns: return "<shift-ins>";
        case ' ': return "<key-space>";
        default:
        {
            if (key > ' ' && key < specialKeyStart)
            {
                return "<key-" + ToUtf8(std::u32string(1, key)) + ">";
            }
            return "<unknown-key>";
        }
    }
}

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
        ThrowLastHostError(filePath);
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
        ThrowLastHostError(filePath);
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
        ThrowLastHostError(std::string());
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
        ThrowLastHostError(std::string());
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
        ThrowLastHostError(std::string());
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
            ThrowLastHostError(std::string());
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
            ThrowLastHostError(std::string());
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

std::string OsGetComputerName()
{
    char buf[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
    GetComputerNameExA(ComputerNameDnsHostname, & buf[0], &size);
    return std::string(&buf[0]);
}

std::string OsGetUserName()
{
    char buf[UNLEN + 1];
    DWORD size = UNLEN + 1;
    GetUserNameA(&buf[0], &size);
    return std::string(&buf[0]);
}

void* OsGetStdHandle(int inOutOrErr)
{
    switch (inOutOrErr)
    {
        case 0: return GetStdHandle(STD_INPUT_HANDLE);
        case 1: return GetStdHandle(STD_OUTPUT_HANDLE);
        case 2: return GetStdHandle(STD_ERROR_HANDLE);
    }
    return nullptr;
}

VOID CALLBACK WaitOrTimerCallback(PVOID context, BOOLEAN timerOrWaitFired)
{
    CallbackFunc* cb = reinterpret_cast<CallbackFunc*>(context);
    cb();
}

void OsRegisterConsoleCallBack(void* consoleInputHandle, CallbackFunc* cb, void*& waitHandle)
{
    HANDLE handle = nullptr;
    PHANDLE newObject = &handle;
    HANDLE hConsole = reinterpret_cast<HANDLE>(consoleInputHandle);
    PVOID context = reinterpret_cast<PVOID>(cb);
    WAITORTIMERCALLBACK consoleCallback = WaitOrTimerCallback;
    bool retVal = RegisterWaitForSingleObject(newObject, hConsole, consoleCallback, context, INFINITE, WT_EXECUTEDEFAULT);
    if (!retVal)
    {
        ThrowLastHostError(std::string());
    }
    waitHandle = handle;
}

void OsUnregisterConsoleCallBack(void* waitHandle)
{
    UnregisterWait(waitHandle);
}

char32_t OsReadConsoleInput(void* consoleInputHandle)
{
    char32_t ch = 0;
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
                    bool alt = false;
                    if (keyEventRecord->dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED))
                    {
                        ctrl = true;
                    }
                    if (keyEventRecord->dwControlKeyState & SHIFT_PRESSED)
                    {
                        shift = true;
                    }
                    if (keyEventRecord->dwControlKeyState & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))
                    {
                        alt = true;
                    }
                    switch (keyEventRecord->wVirtualKeyCode)
                    {
                        case VK_BACK: ch = static_cast<char32_t>(keyBackspace); break;
                        case VK_TAB: if (ctrl) ch = static_cast<char32_t>(keyControlTab); else if (shift) ch = static_cast<char32_t>(keyShiftTab); else ch = static_cast<char32_t>(keyTab); break;
                        case VK_RETURN: ch = static_cast<char32_t>(keyNewLine); break;
                        case VK_ESCAPE: ch = static_cast<char32_t>(keyEscape); break;
                        case VK_PRIOR: if (ctrl) ch = static_cast<char32_t>(keyControlPgUp); else if (shift) ch = static_cast<char32_t>(keyShiftPgUp); else ch = static_cast<char32_t>(keyPgUp); break;
                        case VK_NEXT: if (ctrl) ch = static_cast<char32_t>(keyControlPgDown); else if (shift) ch = static_cast<char32_t>(keyShiftPgDown); else ch = static_cast<char32_t>(keyPgDown); break;
                        case VK_END: if (ctrl && shift) ch = static_cast<char32_t>(keyControlShiftEnd); else if (ctrl) ch = static_cast<char32_t>(keyControlEnd); else if (shift) ch = static_cast<char32_t>(keyShiftEnd); else ch = static_cast<char32_t>(keyEnd); break;
                        case VK_HOME: if (ctrl && shift) ch = static_cast<char32_t>(keyControlShiftHome); else if (ctrl) ch = static_cast<char32_t>(keyControlHome); else if (shift) ch = static_cast<char32_t>(keyShiftHome); else ch = static_cast<char32_t>(keyHome); break;
                        case VK_LEFT: if (ctrl && shift) ch = static_cast<char32_t>(keyControlShiftLeft); else if (ctrl) ch = static_cast<char32_t>(keyControlLeft); else if (shift) ch = static_cast<char32_t>(keyShiftLeft); else ch = static_cast<char32_t>(keyLeft); break;
                        case VK_RIGHT: if (ctrl && shift) ch = static_cast<char32_t>(keyControlShiftRight); else if (ctrl) ch = static_cast<char32_t>(keyControlRight); else if (shift) ch = static_cast<char32_t>(keyShiftRight); else ch = static_cast<char32_t>(keyRight); break;
                        case VK_UP: if (ctrl) ch = static_cast<char32_t>(keyControlUp); else if (shift) ch = static_cast<char32_t>(keyShiftUp); else ch = static_cast<char32_t>(keyUp); break;
                        case VK_DOWN: if (ctrl) ch = static_cast<char32_t>(keyControlDown); else if (shift) ch = static_cast<char32_t>(keyShiftDown); else ch = static_cast<char32_t>(keyDown); break;
                        case VK_DELETE: if (ctrl) ch = static_cast<char32_t>(keyControlDel); else if (shift) ch = static_cast<char32_t>(keyShiftDel); else ch = static_cast<char32_t>(keyDel); break;
                        case VK_INSERT: if (ctrl) ch = static_cast<char32_t>(keyControlIns); else if (shift) ch = static_cast<char32_t>(keyShiftIns); else ch = static_cast<char32_t>(keyIns); break;
                        case VK_F1: if (ctrl) ch = static_cast<char32_t>(keyControlF1); else if (alt) ch = static_cast<char32_t>(keyAltF1); else ch = static_cast<char32_t>(keyF1); break;
                        case VK_F2: if (ctrl) ch = static_cast<char32_t>(keyControlF2); else if (alt) ch = static_cast<char32_t>(keyAltF2); else ch = static_cast<char32_t>(keyF2); break;
                        case VK_F3: if (ctrl) ch = static_cast<char32_t>(keyControlF3); else if (alt) ch = static_cast<char32_t>(keyAltF3); else ch = static_cast<char32_t>(keyF3); break;
                        case VK_F4: if (ctrl) ch = static_cast<char32_t>(keyControlF4); else if (alt) ch = static_cast<char32_t>(keyAltF4); else ch = static_cast<char32_t>(keyF4); break;
                        case VK_F5: if (ctrl) ch = static_cast<char32_t>(keyControlF5); else if (alt) ch = static_cast<char32_t>(keyAltF5); else ch = static_cast<char32_t>(keyF5); break;
                        case VK_F6: if (ctrl) ch = static_cast<char32_t>(keyControlF6); else if (alt) ch = static_cast<char32_t>(keyAltF6); else ch = static_cast<char32_t>(keyF6); break;
                        case VK_F7: if (ctrl) ch = static_cast<char32_t>(keyControlF7); else if (alt) ch = static_cast<char32_t>(keyAltF7); else ch = static_cast<char32_t>(keyF7); break;
                        case VK_F8: if (ctrl) ch = static_cast<char32_t>(keyControlF8); else if (alt) ch = static_cast<char32_t>(keyAltF8); else ch = static_cast<char32_t>(keyF8); break;
                        case VK_F9: if (ctrl) ch = static_cast<char32_t>(keyControlF9); else if (alt) ch = static_cast<char32_t>(keyAltF9); else ch = static_cast<char32_t>(keyF9); break;
                        case VK_F10: if (ctrl) ch = static_cast<char32_t>(keyControlF10); else if (alt) ch = static_cast<char32_t>(keyAltF10); else ch = static_cast<char32_t>(keyF10); break;
                        case VK_F11: if (ctrl) ch = static_cast<char32_t>(keyControlF11); else if (alt) ch = static_cast<char32_t>(keyAltF11); else ch = static_cast<char32_t>(keyF11); break;
                        case VK_F12: if (ctrl) ch = static_cast<char32_t>(keyControlF12); else if (alt) ch = static_cast<char32_t>(keyAltF12); else ch = static_cast<char32_t>(keyF12); break;
                    }
                    if (ch == '\0' && ctrl)
                    {
                        switch (keyEventRecord->wVirtualKeyCode)
                        {
                            case 'A': ch = static_cast<char32_t>(keyControlA); break;
                            case 'B': ch = static_cast<char32_t>(keyControlB); break;
                            case 'C': ch = static_cast<char32_t>(keyControlC); break;
                            case 'D': ch = static_cast<char32_t>(keyControlD); break;
                            case 'E': ch = static_cast<char32_t>(keyControlE); break;
                            case 'F': ch = static_cast<char32_t>(keyControlF); break;
                            case 'G': ch = static_cast<char32_t>(keyControlG); break;
                            case 'H': ch = static_cast<char32_t>(keyControlH); break;
                            case 'I': ch = static_cast<char32_t>(keyControlI); break;
                            case 'J': ch = static_cast<char32_t>(keyControlJ); break;
                            case 'K': ch = static_cast<char32_t>(keyControlK); break;
                            case 'L': ch = static_cast<char32_t>(keyControlL); break;
                            case 'M': ch = static_cast<char32_t>(keyControlM); break;
                            case 'N': ch = static_cast<char32_t>(keyControlN); break;
                            case 'O': ch = static_cast<char32_t>(keyControlO); break;
                            case 'P': ch = static_cast<char32_t>(keyControlP); break;
                            case 'Q': ch = static_cast<char32_t>(keyControlQ); break;
                            case 'R': ch = static_cast<char32_t>(keyControlR); break;
                            case 'S': ch = static_cast<char32_t>(keyControlS); break;
                            case 'T': ch = static_cast<char32_t>(keyControlT); break;
                            case 'U': ch = static_cast<char32_t>(keyControlU); break;
                            case 'V': ch = static_cast<char32_t>(keyControlV); break;
                            case 'W': ch = static_cast<char32_t>(keyControlW); break;
                            case 'X': ch = static_cast<char32_t>(keyControlX); break;
                            case 'Y': ch = static_cast<char32_t>(keyControlY); break;
                            case 'Z': ch = static_cast<char32_t>(keyControlZ); break;
                        }
                    }
                    else if (ch == '\0' && alt)
                    {
                        switch (keyEventRecord->wVirtualKeyCode)
                        {
                            case 'A': ch = static_cast<char32_t>(keyAltA); break;
                            case 'B': ch = static_cast<char32_t>(keyAltB); break;
                            case 'C': ch = static_cast<char32_t>(keyAltC); break;
                            case 'D': ch = static_cast<char32_t>(keyAltD); break;
                            case 'E': ch = static_cast<char32_t>(keyAltE); break;
                            case 'F': ch = static_cast<char32_t>(keyAltF); break;
                            case 'G': ch = static_cast<char32_t>(keyAltG); break;
                            case 'H': ch = static_cast<char32_t>(keyAltH); break;
                            case 'I': ch = static_cast<char32_t>(keyAltI); break;
                            case 'J': ch = static_cast<char32_t>(keyAltJ); break;
                            case 'K': ch = static_cast<char32_t>(keyAltK); break;
                            case 'L': ch = static_cast<char32_t>(keyAltL); break;
                            case 'M': ch = static_cast<char32_t>(keyAltM); break;
                            case 'N': ch = static_cast<char32_t>(keyAltN); break;
                            case 'O': ch = static_cast<char32_t>(keyAltO); break;
                            case 'P': ch = static_cast<char32_t>(keyAltP); break;
                            case 'Q': ch = static_cast<char32_t>(keyAltQ); break;
                            case 'R': ch = static_cast<char32_t>(keyAltR); break;
                            case 'S': ch = static_cast<char32_t>(keyAltS); break;
                            case 'T': ch = static_cast<char32_t>(keyAltT); break;
                            case 'U': ch = static_cast<char32_t>(keyAltU); break;
                            case 'V': ch = static_cast<char32_t>(keyAltV); break;
                            case 'W': ch = static_cast<char32_t>(keyAltW); break;
                            case 'X': ch = static_cast<char32_t>(keyAltX); break;
                            case 'Y': ch = static_cast<char32_t>(keyAltY); break;
                            case 'Z': ch = static_cast<char32_t>(keyAltZ); break;
                        }
                    }
                    if (ch == '\0')
                    {
                        ch = keyEventRecord->uChar.UnicodeChar;
                    }
                }
            }
        }
    }
    else
    {
        ThrowLastHostError(std::string());
    }
    return ch;
}

void OsGetConsoleScreenBufferInfo(void* consoleOutputHandle, int32_t* cursorPosX, int32_t* cursorPosY, int32_t* screenSizeX, int32_t* screenSizeY, uint16_t* attrs)
{
    if (!cursorPosX || !cursorPosY || !screenSizeX || !screenSizeY || !attrs) throw SystemError(EPARAM, "invalid parameter", __FUNCTION__);
    CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
    bool retVal = GetConsoleScreenBufferInfo(consoleOutputHandle, &consoleScreenBufferInfo);
    if (!retVal) ThrowLastHostError(std::string());
    *cursorPosX = consoleScreenBufferInfo.dwCursorPosition.X;
    *cursorPosY = consoleScreenBufferInfo.dwCursorPosition.Y;
    *screenSizeX = consoleScreenBufferInfo.dwMaximumWindowSize.X;
    *screenSizeY = consoleScreenBufferInfo.dwMaximumWindowSize.Y;
    *attrs = consoleScreenBufferInfo.wAttributes;
}

void OsSetConsoleCursorPosition(void* consoleOutputHandle, int cursorPosX, int cursorPosY)
{
    COORD cursorPos;
    cursorPos.X = cursorPosX;
    cursorPos.Y = cursorPosY;
    bool retval = SetConsoleCursorPosition(consoleOutputHandle, cursorPos);
    if (!retval)
    {
        ThrowLastHostError(std::string());
    }
}

void OsWriteConsole(void* consoleOutputHandle, const char32_t* chars)
{
    DWORD numCharsWritten = 0;
    std::u32string s(chars);
    std::u16string w(soulng::unicode::ToUtf16(s));
    bool retval = WriteConsoleW(consoleOutputHandle, w.c_str(), w.length(), &numCharsWritten, nullptr);
    if (!retval)
    {
        ThrowLastHostError(std::string());
    }
}

void OsSetConsoleTextAttribute(void* consoleOutputHandle, uint16_t attrs)
{
    bool retval = SetConsoleTextAttribute(consoleOutputHandle, attrs);
    if (!retval)
    {
        ThrowLastHostError(std::string());
    }
}

} // namespace cmsx::kernel
