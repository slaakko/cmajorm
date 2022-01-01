// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/Ansi.hpp>
#include <memory>
#include <stdexcept>
#ifdef _WIN32
#include <Windows.h>
#endif

namespace soulng { namespace util {

const std::string fgBlack = "\033[30m";
const std::string fgDarkRed = "\033[31m";
const std::string fgDarkGreen = "\033[32m";
const std::string fgDarkYellow = "\033[33m";
const std::string fgDarkBlue = "\033[34m";
const std::string fgDarkMagenta = "\033[35m";
const std::string fgDarkCyan = "\033[36m";
const std::string fgGray = "\033[37m";
const std::string fgRed = "\033[91m";
const std::string fgGreen = "\033[92m";
const std::string fgYellow = "\033[93m";
const std::string fgBlue = "\033[94m";
const std::string fgMagenta = "\033[95m";
const std::string fgCyan = "\033[96m";
const std::string fgWhite = "\033[97m";

const std::string bgBlack = "\033[40m";
const std::string bgDarkRed = "\033[41m";
const std::string bgDarkGreen = "\033[42m";
const std::string bgDarkYellow = "\033[43m";
const std::string bgDarkBlue = "\033[44m";
const std::string bgDarkMagenta = "\033[45m";
const std::string bgDarkCyan = "\033[46m";
const std::string bgGray = "\033[47m";
const std::string bgRed = "\033[101m";
const std::string bgGreen = "\033[102m";
const std::string bgYellow = "\033[103m";
const std::string bgBlue = "\033[104m";
const std::string bgMagenta = "\033[105m";
const std::string bgCyan = "\033[106m";
const std::string bgWhite = "\033[107m";

const std::string reset = "\033[0m";

#ifdef _WIN32

enum class Color
{
    none, black, red, green, yellow, blue, magenta, cyan, white
};

enum class Intensity
{
    none, dark, bright
};

enum class ForeBack
{
    none, foreground, background
};

enum class ConsoleColor : uint8_t
{
    black = 0u, darkBlue = 1u, darkGreen = 2u, darkCyan = 3u, darkRed = 4u, darkYellow = 6u, gray = 7u,
    darkGray = 8u, blue = 9u, green = 10u, cyan = 11u, red = 12u, magenta = 13u, yellow = 14u, white = 15u
};

struct Attributes
{
    Attributes() : color(Color::none), intensity(Intensity::none), foreback(ForeBack::none) {}
    Attributes(Color color_, Intensity intensity_, ForeBack foreback_) : color(color_), intensity(intensity_), foreback(foreback_) {}
    Color color;
    Intensity intensity;
    ForeBack foreback;
};

ConsoleColor ToConsoleColor(Color color, Intensity intensity)
{
    switch (intensity)
    {
    case Intensity::dark:
    {
        switch (color)
        {
        case Color::black: return ConsoleColor::black;
        case Color::red: return ConsoleColor::darkRed;
        case Color::green: return ConsoleColor::darkGreen;
        case Color::yellow: return ConsoleColor::darkYellow;
        case Color::blue: return ConsoleColor::darkBlue;
        case Color::magenta: return ConsoleColor::magenta;
        case Color::cyan: return ConsoleColor::darkCyan;
        case Color::white: return ConsoleColor::gray;
        }
        break;
    }
    case Intensity::bright:
    {
        switch (color)
        {
        case Color::black: return ConsoleColor::darkGray;
        case Color::red: return ConsoleColor::red;
        case Color::green: return ConsoleColor::green;
        case Color::yellow: return ConsoleColor::yellow;
        case Color::blue: return ConsoleColor::blue;
        case Color::magenta: return ConsoleColor::magenta;
        case Color::cyan: return ConsoleColor::cyan;
        case Color::white: return ConsoleColor::white;
        }
    }
    }
    return ConsoleColor::gray;
}

class AnsiEngine
{
public:
    static void Init();
    static void Done();
    static AnsiEngine& Out() { return *out; }
    static AnsiEngine& Error() { return *error; }
    std::string Process(const std::string& text);
    void SetHandle(HANDLE handle_) { handle = handle_; }
private:
    static std::unique_ptr<AnsiEngine> out;
    static std::unique_ptr<AnsiEngine> error;
    AnsiEngine();
    void Put(char c);
    void Emit(char c);
    void SetAttributes();
    void ResetAttributes();
    int state;
    std::string outChars;
    bool charsReady;
    Attributes attributes;
    bool attributesReset;
    WORD prevAttrs;
    HANDLE handle;
};

std::string AnsiIntercept(int handle, const std::string& text)
{
    if (handle == 1)
    {
        return AnsiEngine::Out().Process(text);
    }
    else if (handle == 2)
    {
        return AnsiEngine::Error().Process(text);
    }
    else
    {
        return text;
    }
}

std::unique_ptr<AnsiEngine> AnsiEngine::out;
std::unique_ptr<AnsiEngine> AnsiEngine::error;

void AnsiEngine::Init()
{
    out.reset(new AnsiEngine());
    out->SetHandle(GetStdHandle(STD_OUTPUT_HANDLE));
    error.reset(new AnsiEngine());
    error->SetHandle(GetStdHandle(STD_ERROR_HANDLE));
}

void AnsiEngine::Done()
{
    out.reset();
    error.reset();
}

AnsiEngine::AnsiEngine() : state(0), outChars(), charsReady(false), attributes(), prevAttrs(0), attributesReset(true), handle(nullptr)
{
}

std::string AnsiEngine::Process(const std::string& text)
{
    std::string output;
    for (char c : text)
    {
        Put(c);
        if (charsReady)
        {
            charsReady = false;
            output.append(outChars);
            outChars.clear();
        }
    }
    return output;
}

void AnsiEngine::SetAttributes()
{
    if (attributesReset)
    {
        attributesReset = false;
        CONSOLE_SCREEN_BUFFER_INFO info;
        if (GetConsoleScreenBufferInfo(handle, &info))
        {
            prevAttrs = info.wAttributes;
        }
        else
        {
            throw std::runtime_error("AnsiEngine: failed to fetch console attributes");
        }
    }
    WORD attrs = prevAttrs;
    if (attributes.foreback == ForeBack::foreground)
    {
        attrs = (attrs & 0xFF00) | static_cast<uint8_t>(ToConsoleColor(attributes.color, attributes.intensity));
    }
    else if (attributes.foreback == ForeBack::background)
    {
        attrs = (attrs & 0x00FF) | (static_cast<uint8_t>(ToConsoleColor(attributes.color, attributes.intensity)) << 8);
    }
    if (!SetConsoleTextAttribute(handle, attrs))
    {
        throw std::runtime_error("AnsiEngine: failed to set console attributes");
    }
}

void AnsiEngine::ResetAttributes()
{
    attributesReset = true;
    if (!SetConsoleTextAttribute(handle, prevAttrs))
    {
        throw std::runtime_error("AnsiEngine: failed to reset console attributes");
    }
}

void AnsiEngine::Emit(char c)
{
    outChars.append(1, c);
    charsReady = true;
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
                case '0': attributes = Attributes(Color::black, Intensity::dark, ForeBack::foreground); state = 1000; break;
                case '1': attributes = Attributes(Color::red, Intensity::dark, ForeBack::foreground); state = 1001; break;
                case '2': attributes = Attributes(Color::green, Intensity::dark, ForeBack::foreground); state = 1002; break;
                case '3': attributes = Attributes(Color::yellow, Intensity::dark, ForeBack::foreground); state = 1003; break;
                case '4': attributes = Attributes(Color::blue, Intensity::dark, ForeBack::foreground); state = 1004; break;
                case '5': attributes = Attributes(Color::magenta, Intensity::dark, ForeBack::foreground); state = 1005; break;
                case '6': attributes = Attributes(Color::cyan, Intensity::dark, ForeBack::foreground); state = 1006; break;
                case '7': attributes = Attributes(Color::white, Intensity::dark, ForeBack::foreground); state = 1007; break;
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
                case '1': attributes = Attributes(Color::red, Intensity::bright, ForeBack::foreground); state = 9001; break;
                case '2': attributes = Attributes(Color::green, Intensity::bright, ForeBack::foreground); state = 9002; break;
                case '3': attributes = Attributes(Color::yellow, Intensity::bright, ForeBack::foreground); state = 9003; break;
                case '4': attributes = Attributes(Color::blue, Intensity::bright, ForeBack::foreground); state = 9004; break;
                case '5': attributes = Attributes(Color::magenta, Intensity::bright, ForeBack::foreground); state = 9005; break;
                case '6': attributes = Attributes(Color::cyan, Intensity::bright, ForeBack::foreground); state = 9006; break;
                case '7': attributes = Attributes(Color::white, Intensity::bright, ForeBack::foreground); state = 9007; break;
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
                case '0': attributes = Attributes(Color::black, Intensity::dark, ForeBack::background); state = 4000; break;
                case '1': attributes = Attributes(Color::red, Intensity::dark, ForeBack::background); state = 4001; break;
                case '2': attributes = Attributes(Color::green, Intensity::dark, ForeBack::background); state = 4002; break;
                case '3': attributes = Attributes(Color::yellow, Intensity::dark, ForeBack::background); state = 4003; break;
                case '4': attributes = Attributes(Color::blue, Intensity::dark, ForeBack::background); state = 4004; break;
                case '5': attributes = Attributes(Color::magenta, Intensity::dark, ForeBack::background); state = 4005; break;
                case '6': attributes = Attributes(Color::cyan, Intensity::dark, ForeBack::background); state = 4006; break;
                case '7': attributes = Attributes(Color::white, Intensity::dark, ForeBack::background); state = 4007; break;
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
                case '1': attributes = Attributes(Color::red, Intensity::bright, ForeBack::background); state = 10001; break;
                case '2': attributes = Attributes(Color::green, Intensity::bright, ForeBack::background); state = 10002; break;
                case '3': attributes = Attributes(Color::yellow, Intensity::bright, ForeBack::background); state = 10003; break;
                case '4': attributes = Attributes(Color::blue, Intensity::bright, ForeBack::background); state = 10004; break;
                case '5': attributes = Attributes(Color::magenta, Intensity::bright, ForeBack::background); state = 10005; break;
                case '6': attributes = Attributes(Color::cyan, Intensity::bright, ForeBack::background); state = 10006; break;
                case '7': attributes = Attributes(Color::white, Intensity::bright, ForeBack::background); state = 10007; break;
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
                ResetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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
                SetAttributes();
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

#endif

void AnsiInit()
{
#ifdef _WIN32
    AnsiEngine::Init();
#endif
}

void AnsiDone()
{
#ifdef _WIN32
    AnsiEngine::Done();
#endif
}

} } // namespace soulng::util
