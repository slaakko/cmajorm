// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SXX_COLOR_INCLUDED
#define SXX_COLOR_INCLUDED
#include <stdint.h>
#include <string>

namespace sxx {

enum class ConsoleColor : uint8_t
{
    black = 0u, darkBlue = 1u, darkGreen = 2u, darkCyan = 3u, darkRed = 4u, darkYellow = 6u, gray = 7u,
    darkGray = 8u, blue = 9u, green = 10u, cyan = 11u, red = 12u, magenta = 13u, yellow = 14u, white = 15u
};

std::u32string SetColors(ConsoleColor fgColor, ConsoleColor bgColor);
std::u32string ResetColors();

} // namespace sxx

#endif // SXX_COLOR_INCLUDED
