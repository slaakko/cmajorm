// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/Color.hpp>

namespace cmajor { namespace wing {

Color GetColor(ConsoleColor consoleColor)
{
    switch (consoleColor)
    {
        case ConsoleColor::black: return Color(12, 12, 12);
        case ConsoleColor::darkBlue: return Color(0, 55, 218);
        case ConsoleColor::darkGreen: return Color(19, 161, 14);
        case ConsoleColor::darkCyan: return Color(58, 150, 221);
        case ConsoleColor::darkRed: return Color(197, 15, 31);
        case ConsoleColor::darkYellow: return Color(193, 156, 0);
        case ConsoleColor::gray: return Color(204, 204, 204);
        case ConsoleColor::darkGray: return Color(169, 169, 169);
        case ConsoleColor::blue: return Color(59, 120, 255);
        case ConsoleColor::green: return Color(22, 198, 12);
        case ConsoleColor::cyan: return Color(97, 214, 214);
        case ConsoleColor::red: return Color(231, 72, 86);
        case ConsoleColor::magenta: return Color(180, 0, 158);
        case ConsoleColor::yellow: return Color(249, 241, 165);
        case ConsoleColor::white: return Color(242, 242, 242);
    }
    return Color::Black;
}

} } // cmajor::wing
