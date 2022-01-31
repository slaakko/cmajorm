// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/sxx/Console.hpp>

namespace sxx {

Console::Console() : cmsx::kernel::File("CONSOLE")
{
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
    for (uint8_t x : buffer)
    {
        utf8Engine.Put(x);
        if (utf8Engine.ResulReady())
        {
            char32_t c = utf8Engine.Result();
            if (c < static_cast<char32_t>(0x100000u))
            {
                std::string s = ToUtf8(std::u32string(1, c));
                std::cout << s;
            }
        }
    }
    return buffer.size();
}

} // namespace sxx
