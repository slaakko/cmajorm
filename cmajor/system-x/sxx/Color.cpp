// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/sxx/Color.hpp>
#include <soulng/util/Ansi.hpp>
#include <soulng/util/Unicode.hpp>

namespace sxx {

using namespace soulng::unicode;
using namespace soulng::util;

std::u32string SetColors(ConsoleColor fgColor, ConsoleColor bgColor)
{
    std::u32string colors;
    switch (fgColor)
    {
        case ConsoleColor::black:
        {
            colors.append(ToUtf32(fgBlack));
            break;
        }
        case ConsoleColor::darkBlue:
        {
            colors.append(ToUtf32(fgDarkBlue));
            break;
        }
        case ConsoleColor::darkGreen:
        {
            colors.append(ToUtf32(fgDarkGreen));
            break;
        }
        case ConsoleColor::darkCyan:
        {
            colors.append(ToUtf32(fgDarkCyan));
            break;
        }
        case ConsoleColor::darkRed:
        {
            colors.append(ToUtf32(fgDarkRed));
            break;
        }
        case ConsoleColor::darkYellow:
        {
            colors.append(ToUtf32(fgDarkYellow));
            break;
        }
        case ConsoleColor::gray:
        {
            colors.append(ToUtf32(fgGray));
            break;
        }
        case ConsoleColor::darkGray:
        {
            colors.append(ToUtf32(fgGray));
            break;
        }
        case ConsoleColor::blue:
        {
            colors.append(ToUtf32(fgBlue));
            break;
        }
        case ConsoleColor::green:
        {
            colors.append(ToUtf32(fgGreen));
            break;
        }
        case ConsoleColor::cyan:
        {
            colors.append(ToUtf32(fgCyan));
            break;
        }
        case ConsoleColor::red:
        {
            colors.append(ToUtf32(fgRed));
            break;
        }
        case ConsoleColor::magenta:
        {
            colors.append(ToUtf32(fgMagenta));
            break;
        }
        case ConsoleColor::yellow:
        {
            colors.append(ToUtf32(fgYellow));
            break;
        }
        case ConsoleColor::white:
        {
            colors.append(ToUtf32(fgWhite));
            break;
        }
    }
    switch (bgColor)
    {
        case ConsoleColor::black:
        {
            colors.append(ToUtf32(bgBlack));
            break;
        }
        case ConsoleColor::darkBlue:
        {
            colors.append(ToUtf32(bgDarkBlue));
            break;
        }
        case ConsoleColor::darkGreen:
        {
            colors.append(ToUtf32(bgDarkGreen));
            break;
        }
        case ConsoleColor::darkCyan:
        {
            colors.append(ToUtf32(bgDarkCyan));
            break;
        }
        case ConsoleColor::darkRed:
        {
            colors.append(ToUtf32(bgDarkRed));
            break;
        }
        case ConsoleColor::darkYellow:
        {
            colors.append(ToUtf32(bgDarkYellow));
            break;
        }
        case ConsoleColor::gray:
        {
            colors.append(ToUtf32(bgGray));
            break;
        }
        case ConsoleColor::darkGray:
        {
            colors.append(ToUtf32(bgGray));
            break;
        }
        case ConsoleColor::blue:
        {
            colors.append(ToUtf32(bgBlue));
            break;
        }
        case ConsoleColor::green:
        {
            colors.append(ToUtf32(bgGreen));
            break;
        }
        case ConsoleColor::cyan:
        {
            colors.append(ToUtf32(bgCyan));
            break;
        }
        case ConsoleColor::red:
        {
            colors.append(ToUtf32(bgRed));
            break;
        }
        case ConsoleColor::magenta:
        {
            colors.append(ToUtf32(bgMagenta));
            break;
        }
        case ConsoleColor::yellow:
        {
            colors.append(ToUtf32(bgYellow));
            break;
        }
        case ConsoleColor::white:
        {
            colors.append(ToUtf32(bgWhite));
            break;
        }
    }
    return colors;
}

std::u32string ResetColors()
{
    return ToUtf32(reset);
}

} // namespace sxx
