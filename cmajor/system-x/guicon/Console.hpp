// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_GUICON_CONSOLE_INCLUDED
#define CMSX_GUICON_CONSOLE_INCLUDED
#include <system-x/guicon/Api.hpp>
#include <system-x/kernel/File.hpp>
#include <wing/Console.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmsx::guicon {

class CMSX_GUICON_API ConsoleFile : public cmsx::kernel::File
{
public:
    ConsoleFile();
    void SetConsole(cmajor::wing::Console* console_) { console = console_; }
    int64_t Write(const std::vector<uint8_t>& buffer) override;
    std::vector<uint8_t> Read(int64_t count) override;
private:
    soulng::unicode::Utf8ToUtf32Engine utfEngine;
    cmajor::wing::Console* console;
    cmajor::wing::ConsoleColor backColor;
    cmajor::wing::ConsoleColor textColor;
};

} // namespace cmsx::guicon

#endif // CMSX_GUICON_CONSOLE_INCLUDED
