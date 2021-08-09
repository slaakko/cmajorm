// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/Ansi.hpp>

namespace cmajor { namespace wing {

AnsiEngine::AnsiEngine(int handle_) : handle(handle_), state(0), defaultTextColor(ConsoleColor::gray), defaultBackColor(ConsoleColor::black)
{
    ResetColors();
}

void AnsiEngine::ResetColors()
{
    textColor = defaultTextColor;
    backColor = defaultBackColor;
}

void AnsiEngine::SetColors()
{
    textColor = currentTextColor;
    backColor = currentBackColor;
}

void AnsiEngine::SetColorCharOutputMethod(const ColorCharOutputMethod& outputMethod_)
{
    outputMethod = outputMethod_;
}

void AnsiEngine::Process(const std::string& text)
{
    for (char c : text)
    {
        Put(c);
    }
}

void AnsiEngine::Put(char c)
{
    switch (state)
    {
        case 0:
        {
            if (c == '\033')
            {
                state = 1;
            }
            else
            {
                Emit(c);
            }
            break;
        }
        case 1:
        {
            if (c == '[')
            {
                state = 2;
            }
            else if (c == '\033')
            {
                Emit('\033');
            }
            else
            {
                Emit('\033');
                Emit(c);
                state = 0;
            }
            break;
        }
        case 2:
        {
            switch (c)
            {
            case '3': state = 3; break;
            case '9': state = 9; break;
            case '4': state = 4; break;
            case '1': state = 10; break;
            case '0': state = 999; break;
            default:
            {
                Emit('\033');
                Emit('[');
                Emit(c);
                state = 0;
                break;
            }
            }
            break;
        }
        case 3:
        {
            switch (c)
            {
            case '0': currentTextColor = ConsoleColor::black; state = 1000; break;
            case '1': currentTextColor = ConsoleColor::darkRed; state = 1001; break;
            case '2': currentTextColor = ConsoleColor::darkGreen; state = 1002; break;
            case '3': currentTextColor = ConsoleColor::darkYellow; state = 1003; break;
            case '4': currentTextColor = ConsoleColor::darkBlue; state = 1004; break;
            case '5': currentTextColor = ConsoleColor::darkGray; state = 1005; break;
            case '6': currentTextColor = ConsoleColor::darkCyan; state = 1006; break;
            case '7': currentTextColor = ConsoleColor::gray; state = 1007; break;
            default:
            {
                Emit('\033');
                Emit('[');
                Emit('3');
                Emit(c);
                state = 0;
                break;
            }
            }
            break;
        }
        case 9:
        {
            switch (c)
            {
            case '1': currentTextColor = ConsoleColor::red; state = 9001; break;
            case '2': currentTextColor = ConsoleColor::green; state = 9002; break;
            case '3': currentTextColor = ConsoleColor::yellow; state = 9003; break;
            case '4': currentTextColor = ConsoleColor::blue; state = 9004; break;
            case '5': currentTextColor = ConsoleColor::magenta; state = 9005; break;
            case '6': currentTextColor = ConsoleColor::cyan; state = 9006; break;
            case '7': currentTextColor = ConsoleColor::white; state = 9007; break;
            default:
            {
                Emit('\033');
                Emit('[');
                Emit('9');
                Emit(c);
                state = 0;
                break;
            }
            }
            break;
        }
        case 4:
        {
            switch (c)
            {
            case '0': currentBackColor = ConsoleColor::black; state = 4000; break;
            case '1': currentBackColor = ConsoleColor::darkRed; state = 4001; break;
            case '2': currentBackColor = ConsoleColor::darkGreen; state = 4002; break;
            case '3': currentBackColor = ConsoleColor::darkYellow; state = 4003; break;
            case '4': currentBackColor = ConsoleColor::darkBlue; state = 4004; break;
            case '5': currentBackColor = ConsoleColor::darkGray; state = 4005; break;
            case '6': currentBackColor = ConsoleColor::darkCyan; state = 4006; break;
            case '7': currentBackColor = ConsoleColor::gray; state = 4007; break;
            default:
            {
                Emit('\033');
                Emit('[');
                Emit('4');
                Emit(c);
                state = 0;
                break;
            }
            }
            break;
        }
        case 10:
        {
            if (c == '0')
            {
                state = 100;
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('1');
                Emit(c);
                state = 0;
                break;
            }
            break;
        }
        case 100:
        {
            switch (c)
            {
            case '1': currentBackColor = ConsoleColor::red; state = 10001; break;
            case '2': currentBackColor = ConsoleColor::green; state = 10002; break;
            case '3': currentBackColor = ConsoleColor::yellow; state = 10003; break;
            case '4': currentBackColor = ConsoleColor::blue; state = 10004; break;
            case '5': currentBackColor = ConsoleColor::magenta; state = 10005; break;
            case '6': currentBackColor = ConsoleColor::cyan; state = 10006; break;
            case '7': currentBackColor = ConsoleColor::white; state = 10007; break;
            default:
            {
                Emit('\033');
                Emit('[');
                Emit('1');
                Emit('0');
                Emit(c);
                state = 0;
                break;
            }
            }
            break;
        }
        case 999:
        {
            if (c == 'm')
            {
                ResetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('0');
                Emit(c);
                break;
            }
            state = 0;
            break;
        }
        case 1000:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('3');
                Emit('0');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 1001:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('3');
                Emit('1');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 1002:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('3');
                Emit('2');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 1003:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('3');
                Emit('3');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 1004:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('3');
                Emit('4');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 1005:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('3');
                Emit('5');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 1006:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('3');
                Emit('6');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 1007:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('3');
                Emit('7');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 9001:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('9');
                Emit('1');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 9002:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('9');
                Emit('2');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 9003:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('9');
                Emit('3');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 9004:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('9');
                Emit('4');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 9005:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('9');
                Emit('5');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 9006:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('9');
                Emit('6');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 9007:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('9');
                Emit('7');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 4000:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('4');
                Emit('0');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 4001:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('4');
                Emit('1');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 4002:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('4');
                Emit('2');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 4003:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('4');
                Emit('3');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 4004:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('4');
                Emit('4');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 4005:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('4');
                Emit('5');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 4006:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('4');
                Emit('6');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 4007:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('4');
                Emit('7');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 10001:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('1');
                Emit('0');
                Emit('1');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 10002:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('1');
                Emit('0');
                Emit('2');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 10003:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('1');
                Emit('0');
                Emit('3');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 10004:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('1');
                Emit('0');
                Emit('4');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 10005:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('1');
                Emit('0');
                Emit('5');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 10006:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('1');
                Emit('0');
                Emit('6');
                Emit(c);
            }
            state = 0;
            break;
        }
        case 10007:
        {
            if (c == 'm')
            {
                SetColors();
            }
            else
            {
                Emit('\033');
                Emit('[');
                Emit('1');
                Emit('0');
                Emit('7');
                Emit(c);
            }
            state = 0;
            break;
        }
    }

}

void AnsiEngine::Emit(char c)
{
    unicodeEngine.Put(static_cast<uint8_t>(c));
    if (unicodeEngine.ResulReady())
    {
        outputMethod(textColor, backColor, handle, unicodeEngine.Result());
    }
}

} } // cmajor::wing
