// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_KEYS_INCLUDED
#define CMAJOR_WING_KEYS_INCLUDED
#include <wing/WingApi.hpp>
#include <string>

namespace cmajor { namespace wing {

enum class Keys : int
{
    a = 65,
    add = 107,
    altModifier = 262144, // 0x0004 0000 ALT modifier key
    apps = 93,
    attn = 246,
    b = 66,
    back = 8,
    browserBack = 166,
    browserFavorites = 171,
    browserForward = 167,
    browserHome = 172,
    browserRefresh = 168,
    browserSearch = 170,
    browserStop = 169,
    c = 67,
    cancel = 3,
    capital = 20,
    capsLock = 20,
    clear = 12,
    controlModifier = 131072, // 0x0002 0000 CTRL modifier key
    controlKey = 17, // CTRL key
    crsel = 247,
    d = 68,
    d0 = 48,
    d1 = 49,
    d2 = 50,
    d3 = 51,
    d4 = 52,
    d5 = 53,
    d6 = 54,
    d7 = 55,
    d8 = 56,
    d9 = 57,
    decimal = 110,
    delete_ = 46,
    divide = 111,
    down = 40,
    e = 69,
    end = 35,
    enter = 13,
    eraseEof = 249,
    escape = 27,
    execute = 43,
    exsel = 248,
    f = 70,
    f1 = 112,
    f10 = 121,
    f11 = 122,
    f12 = 123,
    f13 = 124,
    f14 = 125,
    f15 = 126,
    f16 = 127,
    f17 = 128,
    f18 = 129,
    f19 = 130,
    f2 = 113,
    f20 = 131,
    f21 = 132,
    f22 = 133,
    f23 = 134,
    f24 = 135,
    f3 = 114,
    f4 = 115,
    f5 = 116,
    f6 = 117,
    f7 = 118,
    f8 = 119,
    f9 = 120,
    finalMode = 24,
    g = 71,
    h = 72,
    hanguelMode = 21,
    hangulMode = 21,
    hanjaMode = 25,
    help = 47,
    home = 36,
    i = 73,
    imeAccept = 30,
    imeConvert = 28,
    imeModeChange = 31,
    imeNonconvert = 29,
    insert = 45,
    j = 74,
    junjaMode = 23,
    k = 75,
    kanaMode = 21,
    kanjiMode = 25,
    keyCode = 65535,
    l = 76,
    launchApplication1 = 182,
    launchApplication2 = 183,
    launchMail = 180,
    lButton = 1,
    lControlKey = 162, // LEFT CTRL KEY
    left = 37,
    lineFeed = 10,
    lMenu = 164, // LEFT ALT KEY
    lShiftKey = 160, // LEFT SHIFT KEY
    lWin = 91,
    m = 77,
    mButton = 4,
    mediaNextTrack = 176,
    mediaPlayPause = 179,
    mediaPreviousTrack = 177,
    mediaStop = 178,
    menu = 18, // ALT KEY
    modifiers = -65536,
    multiply = 106,
    n = 78,
    next = 34, // PAGE DOWN
    noName = 252,
    none = 0,
    numLock = 144,
    numPad0 = 96,
    numPad1 = 97,
    numPad2 = 98,
    numPad3 = 99,
    numPad4 = 100,
    numPad5 = 101,
    numPad6 = 102,
    numPad7 = 103,
    numPad8 = 104,
    numPad9 = 105,
    o = 79,
    oem1 = 186,
    oem102 = 226,
    oem2 = 191,
    oem3 = 192,
    oem4 = 219,
    oem5 = 220,
    oem6 = 221,
    oem7 = 222,
    oem8 = 223,
    oemBackslash = 226,
    oemClear = 254,
    oemCloseBrackets = 221,
    oemComma = 188,
    oemMinus = 189,
    oemOpenBrackets = 219,
    oemPediod = 190,
    oemPipe = 220,
    oemPlus = 187,
    oemQuestion = 191,
    oemQuotes = 222,
    oemSemicolon = 186,
    oemTilde = 192,
    p = 80,
    pa1 = 253,
    packet = 231,
    pageDown = 34,
    pageUp = 33,
    pause = 19,
    play = 250,
    print = 42,
    printScreen = 44,
    prior = 33,
    processKey = 229,
    q = 81,
    r = 82,
    rButton = 2,
    rControlKey = 163,
    return_ = 13,
    right = 39,
    rMenu = 165, // RIGHT ALT
    rShiftKey = 161, // RIGHT SHIFT
    rWin = 92,
    s = 83,
    scroll = 145,
    select = 41,
    selectMedia = 181,
    separator = 108,
    shiftModifier = 65536, // 0x0001 0000 SHIFT modifier key
    shiftKey = 16, // SHIFT key
    sleep = 95,
    snapshot = 44,
    space = 32,
    subtract = 109,
    t = 84,
    tab = 9,
    u = 85,
    up = 38,
    v = 86,
    volumeDown = 174,
    volumeMute = 173,
    volumeUp = 175,
    w = 87,
    x = 88,
    xButton1 = 5,
    xButton2 = 6,
    y = 89,
    z = 90,
    zoom = 251
};

inline constexpr Keys operator|(Keys left, Keys right)
{
    return Keys(int(left) | int(right));
}

inline constexpr Keys operator&(Keys left, Keys right)
{
    return Keys(int(left) & int(right));
}

inline constexpr Keys operator~(Keys k)
{
    return Keys(~int(k));
}

std::string KeyText(Keys key);

} } // cmajor::wing

#endif // CMAJOR_WING_KEYS_INCLUDED
