// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_SCREEN_INCLUDED
#define CMAJOR_RT_SCREEN_INCLUDED
#include <cmajor/rt/RtApi.hpp>

extern "C" RT_API void RtInitScreen();
extern "C" RT_API void RtDoneScreen();
extern "C" RT_API void RtRaw();
extern "C" RT_API void RtNoRaw();
extern "C" RT_API void RtCBreak();
extern "C" RT_API void RtNoCBreak();
extern "C" RT_API void RtKeyPad();
extern "C" RT_API void RtEcho();
extern "C" RT_API void RtNoEcho();
extern "C" RT_API void RtCursSet(int visibility);
extern "C" RT_API void RtRefresh();
extern "C" RT_API void RtGetMaxYX(int* rows, int* cols);
extern "C" RT_API void RtErase();
extern "C" RT_API void RtClear();
extern "C" RT_API void RtClearToEol();
extern "C" RT_API void RtGetYX(int* row, int* col);
extern "C" RT_API void RtMove(int row, int col);
extern "C" RT_API void RtAddCh(int ch);
extern "C" RT_API void RtAddStr(const char* str);
extern "C" RT_API int RtGetCh(); // key codes translated to System.Runtime/Keys.cm
extern "C" RT_API int RtGetRawCh(); // key code from PDCurses/ncurses library (not translated)
extern "C" RT_API int RtTranslateCh(int ch); // translate a raw key code
extern "C" RT_API void RtGetNStr(char* str, int size);
extern "C" RT_API void RtAttrOn(int attrs);
extern "C" RT_API void RtAttrOff(int attrs);
extern "C" RT_API void RtAttrSet(int attrs);
extern "C" RT_API void RtStartColor();
extern "C" RT_API void RtInitPair(short n, short backgroundColor, short foregroundColor);
extern "C" RT_API int RtColorPair(short n);
extern "C" RT_API short RtWhite();
extern "C" RT_API short RtBlack();
extern "C" RT_API short RtBlue();
extern "C" RT_API short RtGreen();
extern "C" RT_API short RtRed();
extern "C" RT_API short RtCyan();
extern "C" RT_API short RtMagenta();
extern "C" RT_API short RtYellow();
extern "C" RT_API bool RtRunningOnWsl();

// Key codes returned by RtGetCh function:

const int keyBackspace = '\b';
const int keyTab = '\t';

const int keyControlA = 0x001;
const int keyControlB = 0x002;
const int keyControlC = 0x003;
const int keyControlD = 0x004;
const int keyControlE = 0x005;
const int keyControlF = 0x006;
const int keyControlG = 0x007;
const int keyControlH = 0x008;
const int keyControlI = 0x009;
const int keyControlJ = 0x00A;
const int keyControlK = 0x00B;
const int keyControlL = 0x00C;
const int keyControlM = 0x00D;
const int keyControlN = 0x00E;
const int keyControlO = 0x00F;
const int keyControlP = 0x010;
const int keyControlQ = 0x011;
const int keyControlR = 0x012;
const int keyControlS = 0x013;
const int keyControlT = 0x014;
const int keyControlU = 0x015;
const int keyControlV = 0x016;
const int keyControlW = 0x017;
const int keyControlX = 0x018;
const int keyControlY = 0x019;
const int keyControlZ = 0x01A;
const int keyEscape = 0x01B;
const int keyFS = 0x01C;
const int keyGS = 0x01D;
const int keyRS = 0x01E;
const int keyUS = 0x01F;

// special keys are mapped to Unicode Private Use Area

const int specialKeyStart = 0xE000;

const int keyEnter = specialKeyStart + 0;
const int keyDown = specialKeyStart + 1;
const int keyUp = specialKeyStart + 2;
const int keyLeft = specialKeyStart + 3;
const int keyRight = specialKeyStart + 4;
const int keyHome = specialKeyStart + 5;
const int keyF0 = specialKeyStart + 6;
const int keyF1 = specialKeyStart + 7;
const int keyF2 = specialKeyStart + 8;
const int keyF3 = specialKeyStart + 9;
const int keyF4 = specialKeyStart + 10;
const int keyF5 = specialKeyStart + 11;
const int keyF6 = specialKeyStart + 12;
const int keyF7 = specialKeyStart + 13;
const int keyF8 = specialKeyStart + 14;
const int keyF9 = specialKeyStart + 15;
const int keyF10 = specialKeyStart + 16;
const int keyF11 = specialKeyStart + 17;
const int keyF12 = specialKeyStart + 18;
const int keyDel = specialKeyStart + 19;
const int keyIns = specialKeyStart + 20;
const int keyPgDown = specialKeyStart + 21;
const int keyPgUp = specialKeyStart + 22;
const int keyPrint = specialKeyStart + 23;
const int keyEnd = specialKeyStart + 24;
const int keyShiftDel = specialKeyStart + 25;
const int keyShiftEnd = specialKeyStart + 26;
const int keyShiftHome = specialKeyStart + 27;
const int keyShiftLeft = specialKeyStart + 28;
const int keyShiftRight = specialKeyStart + 29;
const int keyResize = specialKeyStart + 30;
const int keyShiftUp = specialKeyStart + 31;
const int keyShiftDown = specialKeyStart + 32;
const int keyControlUp = specialKeyStart + 33;
const int keyControlDown = specialKeyStart + 34;
const int keyControlLeft = specialKeyStart + 35;
const int keyControlRight = specialKeyStart + 36;
const int keyControlPgUp = specialKeyStart + 37;
const int keyControlPgDown = specialKeyStart + 38;
const int keyControlHome = specialKeyStart + 39;
const int keyControlEnd = specialKeyStart + 40;

const int keyControlDel = specialKeyStart + 41;
const int keyControlF1 = specialKeyStart + 42;
const int keyControlF2 = specialKeyStart + 43;
const int keyControlF3 = specialKeyStart + 44;
const int keyControlF4 = specialKeyStart + 45;
const int keyControlF5 = specialKeyStart + 46;
const int keyControlF6 = specialKeyStart + 47;
const int keyControlF7 = specialKeyStart + 48;
const int keyControlF8 = specialKeyStart + 49;
const int keyControlF9 = specialKeyStart + 50;
const int keyControlF10 = specialKeyStart + 51;
const int keyControlF11 = specialKeyStart + 52;
const int keyControlF12 = specialKeyStart + 53;
const int keyShiftPgUp = specialKeyStart + 54;
const int keyShiftPgDown = specialKeyStart + 55;
const int keyShiftF1 = specialKeyStart + 56;
const int keyShiftF2 = specialKeyStart + 57;
const int keyShiftF3 = specialKeyStart + 58;
const int keyShiftF4 = specialKeyStart + 59;
const int keyShiftF5 = specialKeyStart + 60;
const int keyShiftF6 = specialKeyStart + 61;
const int keyShiftF7 = specialKeyStart + 62;
const int keyShiftF8 = specialKeyStart + 63;
const int keyShiftF9 = specialKeyStart + 64;
const int keyShiftF10 = specialKeyStart + 65;
const int keyShiftF11 = specialKeyStart + 66;
const int keyShiftF12 = specialKeyStart + 67;

const int specialKeyEnd = specialKeyStart + 67;

namespace cmajor { namespace rt {

void InitScreen();
void DoneScreen();

} } // namespace cmajor::rt

#endif // CMAJOR_RT_SCREEN_INCLUDED
