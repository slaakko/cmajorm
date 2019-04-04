// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/Os.hpp>
#include <cmajor/rt/Screen.hpp>
#include <cstring>
#ifdef _WIN32
#include <Windows.h>
#endif
#include <cmajor/util/Unicode.hpp>

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

extern "C" RT_API bool OsGetConsoleScreenBufferInfo(void* consoleOutputHandle, int32_t* cursorPosX, int32_t* cursorPosY, int32_t* screenSizeX, int32_t* screenSizeY)
{
    if (!cursorPosX || !cursorPosY || !screenSizeX || !screenSizeY) return false;
    CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
    bool retVal = GetConsoleScreenBufferInfo(consoleOutputHandle, &consoleScreenBufferInfo);
    if (!retVal) return false;
    *cursorPosX = consoleScreenBufferInfo.dwCursorPosition.X;
    *cursorPosY = consoleScreenBufferInfo.dwCursorPosition.Y;
    *screenSizeX = consoleScreenBufferInfo.dwSize.X;
    *screenSizeY = consoleScreenBufferInfo.dwSize.Y;
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
    std::u16string w(cmajor::unicode::ToUtf16(s));
    return WriteConsoleW(consoleOutputHandle, w.c_str(), w.length(), &numCharsWritten, nullptr);
}

extern "C" RT_API uint32_t OsGetConsoleCP()
{
    return GetConsoleCP();
}

extern "C" RT_API bool OsSetConsoleCP(uint32_t cp)
{
    return SetConsoleCP(cp);
}

extern "C" RT_API uint32_t OsGetConsoleOutputCP()
{
    return GetConsoleOutputCP();
}

extern "C" RT_API bool OsSetConsoleOutputCP(uint32_t cp)
{
    return SetConsoleOutputCP(cp);
}

#endif 
