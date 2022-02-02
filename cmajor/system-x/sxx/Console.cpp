// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/sxx/Console.hpp>
#include <io.h>
#include <fcntl.h>

namespace sxx {

Console::Console() : cmsx::kernel::File("CONSOLE"), stdInInUtf16Mode(false), stdOutInUtf16Mode(false), stdErrInUtf16Mode(false)
{
}

void Console::SetToUtf16Mode()
{
    if (_isatty(0))
    {
        _setmode(0, _O_U16TEXT);
        stdInInUtf16Mode = true;
    }
    if (_isatty(1))
    {
        _setmode(1, _O_U16TEXT);
        stdOutInUtf16Mode = true;
    }
    if (_isatty(2))
    {
        _setmode(2, _O_U16TEXT);
        stdErrInUtf16Mode = true;
    }
}

void Console::SetToTextMode()
{
    _setmode(0, _O_TEXT);
    _setmode(1, _O_TEXT);
    _setmode(2, _O_TEXT);
}

void Console::Close(cmsx::machine::Process* process)
{
}

std::vector<uint8_t> Console::Read(int64_t count, cmsx::machine::Process* process)
{
    // todo
    return std::vector<uint8_t>();
}

int64_t Console::Write(const std::vector<uint8_t>& buffer, cmsx::machine::Process* process)
{
    std::u32string utf32Chars;
    for (uint8_t x : buffer)
    {
        utf8Engine.Put(x);
        if (utf8Engine.ResulReady())
        {
            char32_t c = utf8Engine.Result();
            if (c < static_cast<char32_t>(0x100000u))
            {
                utf32Chars.push_back(c);
            }
        }
    }
    if (stdOutInUtf16Mode)
    {
        std::u16string utf16Chars(ToUtf16(utf32Chars));
        if (!utf16Chars.empty())
        {
            int64_t utf16result = std::fwrite(utf16Chars.c_str(), sizeof(char16_t), utf16Chars.length(), stdout);
        }
    }
    else
    {
        std::string utf8Chars = ToUtf8(utf32Chars);
        std::cout << utf8Chars;
    }
    return buffer.size();
}

} // namespace sxx
