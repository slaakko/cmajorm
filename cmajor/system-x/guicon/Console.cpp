// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/guicon/Console.hpp>

namespace cmsx::guicon {

const char32_t colorChar = static_cast<char32_t>(0x100000u);
const char32_t resetColors = static_cast<char32_t>(0x10FFFDu);

ConsoleFile::ConsoleFile() :
    File("CONSOLE"), console(nullptr), backColor(cmajor::wing::DefaultConsoleBackColor()), textColor(cmajor::wing::DefaultConsoleTextColor())
{
}

void ConsoleFile::Close(cmsx::machine::Process* process)
{
}

int64_t ConsoleFile::Write(const std::vector<uint8_t>& buffer, cmsx::machine::Process* process)
{
    for (uint8_t x : buffer)
    {
        utfEngine.Put(x);
        if (utfEngine.ResulReady())
        {
            char32_t c = utfEngine.Result();
            if (c < colorChar)
            {
                console->OutputChar(textColor, backColor, 1, c);
            }
            else if (c < resetColors)
            {
                uint8_t foreByte = static_cast<uint8_t>(c);
                uint8_t backByte = static_cast<uint8_t>(c >> 8);
                if (foreByte <= static_cast<uint8_t>(cmajor::wing::ConsoleColor::white))
                {
                    textColor = static_cast<cmajor::wing::ConsoleColor>(foreByte);
                }
                if (backByte <= static_cast<uint8_t>(cmajor::wing::ConsoleColor::white))
                {
                    backColor = static_cast<cmajor::wing::ConsoleColor>(backByte);
                }
            }
            else if (c == resetColors)
            {
                backColor = cmajor::wing::DefaultConsoleBackColor();
                textColor = cmajor::wing::DefaultConsoleTextColor();
            }
        }
    }
    console->OutputLines();
    return buffer.size();
}

std::vector<uint8_t> ConsoleFile::Read(int64_t count, cmsx::machine::Process* process)
{
    // todo
    return std::vector<uint8_t>();
}

} // namespace cmsx::guicon
