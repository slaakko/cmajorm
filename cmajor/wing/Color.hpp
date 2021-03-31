// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_COLOR_INCLUDED
#define CMAJOR_WING_COLOR_INCLUDED
#include <cmajor/wing/Graphics.hpp>
#include <stdint.h>

namespace cmajor { namespace wing {

enum class ConsoleColor : uint8_t
{
    black = 0u, darkBlue = 1u, darkGreen = 2u, darkCyan = 3u, darkRed = 4u, darkYellow = 6u, gray = 7u,
    darkGray = 8u, blue = 9u, green = 10u, cyan = 11u, red = 12u, magenta = 13u, yellow = 14u, white = 15u
};

Color GetColor(ConsoleColor consoleColor);

} } // cmajor::wing

#endif // CMAJOR_WING_COLOR_INCLUDED
