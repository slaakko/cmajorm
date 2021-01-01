// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/Screen.hpp>
#include <cmajor/rt/Environment.hpp>
#include <memory>
#include <unordered_map>
#include <fstream>
bool runningOnWsl = false;
int ksup = 0;
int ksdn = 0;
int clft = 0;
int crgt = 0;
int cup = 0;
int cdn = 0;
int cpgup = 0;
int cpgdn = 0;
int chome = 0;
int cend = 0;
#ifdef _WIN32
#include <cmajor/system/ext/pdcurs36/curses.h>
void SetKeys()
{
    ksup = KEY_SUP;
    ksdn = KEY_SDOWN;
    clft = CTL_LEFT;
    crgt = CTL_RIGHT;
    cup = CTL_UP;
    cdn = CTL_DOWN;
    cpgup = CTL_PGUP;
    cpgdn = CTL_PGDN;
    chome = CTL_HOME;
    cend = CTL_END;
}
#else
#include <ncurses.h>
void SetKeys()
{
    if (runningOnWsl) // running on Windows Subsystem for Linux
    {
        ksup = 337;
        ksdn = 336;
        clft = 545;
        crgt = 560;
        cup = 566;
        cdn = 525;
        cpgup = 555;
        cpgdn = 550;
        chome = 535;
        cend = 530;
    }
    else // running on native Linux
    {
        ksup = 337;
        ksdn = 336;
        clft = 544;
        crgt = 559;
        cup = 565;
        cdn = 524;
        cpgup = 554;
        cpgdn = 549;
        chome = 262;
        cend = 360;
    }
}
#endif

namespace cmajor { namespace rt {

std::unordered_map<int, int> keyMap;

void InitScreen()
{
    std::string osInfo = RtGetOsInfo();
    if (osInfo.find("Microsoft") != std::string::npos)
    {
        runningOnWsl = true;
    }
    SetKeys();
    keyMap['\r'] = keyEnter;
    keyMap['\n'] = keyEnter;
    keyMap[KEY_DOWN] = keyDown;
    keyMap[KEY_UP] = keyUp;
    keyMap[KEY_LEFT] = keyLeft;
    keyMap[KEY_RIGHT] = keyRight;
    keyMap[KEY_HOME] = keyHome;
    keyMap[KEY_F(0)] = keyF0;
    keyMap[KEY_F(1)] = keyF1;
    keyMap[KEY_F(2)] = keyF2;
    keyMap[KEY_F(3)] = keyF3;
    keyMap[KEY_F(4)] = keyF4;
    keyMap[KEY_F(5)] = keyF5;
    keyMap[KEY_F(6)] = keyF6;
    keyMap[KEY_F(7)] = keyF7;
    keyMap[KEY_F(8)] = keyF8;
    keyMap[KEY_F(9)] = keyF9;
    keyMap[KEY_F(10)] = keyF10;
    keyMap[KEY_F(11)] = keyF11;
    keyMap[KEY_F(12)] = keyF12;
    keyMap[KEY_DC] = keyDel;
    keyMap[KEY_IC] = keyIns;
    keyMap[KEY_NPAGE] = keyPgDown;
    keyMap[KEY_PPAGE] = keyPgUp;
    keyMap[KEY_PRINT] = keyPrint;
    keyMap[KEY_END] = keyEnd;
    keyMap[KEY_SDC] = keyShiftDel;
    keyMap[KEY_SEND] = keyShiftEnd;
    keyMap[KEY_SHOME] = keyShiftHome;
    keyMap[KEY_SLEFT] = keyShiftLeft;
    keyMap[KEY_SRIGHT] = keyShiftRight;
    keyMap[KEY_RESIZE] = keyResize;
    keyMap[ksup] = keyShiftUp;
    keyMap[ksdn] = keyShiftDown;
    keyMap[cup] = keyControlUp;
    keyMap[cdn] = keyControlDown;
    keyMap[clft] = keyControlLeft;
    keyMap[crgt] = keyControlRight;
    keyMap[cpgup] = keyControlPgUp;
    keyMap[cpgdn] = keyControlPgDown;
    keyMap[chome] = keyControlHome;
    keyMap[cend] = keyControlEnd;
}

void DoneScreen()
{
}

} } // namespace cmajor::rt

extern "C" RT_API void RtInitScreen()
{
    initscr();
}

extern "C" RT_API void RtDoneScreen()
{
    endwin();
}

extern "C" RT_API void RtRaw()
{
    raw();
}

extern "C" RT_API void RtNoRaw()
{
    noraw();
}

extern "C" RT_API void RtCBreak()
{
    cbreak();
}

extern "C" RT_API void RtNoCBreak()
{
    nocbreak();
}

extern "C" RT_API void RtKeyPad()
{
    keypad(stdscr, true);
}

extern "C" RT_API void RtEcho()
{
    echo();
}

extern "C" RT_API void RtNoEcho()
{
    noecho();
}

extern "C" RT_API void RtCursSet(int visibility)
{
    curs_set(visibility);
}

extern "C" RT_API void RtRefresh()
{
    refresh();
}

extern "C" RT_API void RtGetMaxYX(int* rows, int* cols)
{
    int r;
    int c;
    getmaxyx(stdscr, r, c);
    *rows = r;
    *cols = c;
}

extern "C" RT_API void RtErase()
{
    erase();
}

extern "C" RT_API void RtClear()
{
    clear();
}

extern "C" RT_API void RtClearToEol()
{
    clrtoeol();
}

extern "C" RT_API void RtGetYX(int* row, int* col)
{
    int r;
    int c;
    getyx(stdscr, r, c);
    *row = r;
    *col = c;
}

extern "C" RT_API void RtMove(int row, int col)
{
    move(row, col);
}

extern "C" RT_API void RtAddCh(int ch)
{
    addch(ch);
}

extern "C" RT_API void RtAddStr(const char* str)
{
    addstr(str);
}

extern "C" RT_API int RtGetRawCh()
{
    return getch();
}

extern "C" RT_API int RtTranslateCh(int ch)
{
    using cmajor::rt::keyMap;
    auto it = keyMap.find(ch);
    if (it != keyMap.cend())
    {
        ch = it->second;
    }
    return ch;
}

extern "C" RT_API int RtGetCh()
{
    int ch = RtGetRawCh();
    ch = RtTranslateCh(ch);
    return ch;
}

extern "C" RT_API void RtGetNStr(char* str, int size)
{
    getnstr(str, size);
}

extern "C" RT_API void RtAttrOn(int attrs)
{
    attron(attrs);
}

extern "C" RT_API void RtAttrOff(int attrs)
{
    attroff(attrs);
}

extern "C" RT_API void RtAttrSet(int attrs)
{
    attrset(attrs);
}

extern "C" RT_API void RtStartColor()
{
    start_color();
}

extern "C" RT_API void RtInitPair(short n, short foregroundColor, short backgroundColor)
{
    init_pair(n, foregroundColor, backgroundColor);
}

extern "C" RT_API int RtColorPair(short n)
{
    return COLOR_PAIR(n);
}

extern "C" RT_API short RtWhite()
{
    return COLOR_WHITE;
}

extern "C" RT_API short RtBlack()
{
    return COLOR_BLACK;
}

extern "C" RT_API short RtBlue()
{
    return COLOR_BLUE;
}

extern "C" RT_API short RtGreen()
{
    return COLOR_GREEN;
}

extern "C" RT_API short RtRed()
{
    return COLOR_RED;
}

extern "C" RT_API short RtCyan()
{
    return COLOR_CYAN;
}

extern "C" RT_API short RtMagenta()
{
    return COLOR_MAGENTA;
}

extern "C" RT_API short RtYellow()
{
    return COLOR_YELLOW;
}

extern "C" RT_API bool RtRunningOnWsl()
{
    return runningOnWsl;
}
