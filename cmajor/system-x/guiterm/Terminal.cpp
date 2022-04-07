// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/guiterm/Terminal.hpp>
#include <system-x/guiterm/TerminalControl.hpp>
#include <cmajor/wing/Application.hpp>

namespace cmsx::guiterm {

TerminalFile::TerminalFile() :
    cmsx::kernel::File("GUITERM"),
    terminalControl(nullptr)
{
}

int TerminalFile::Columns() const
{
    return terminalControl->Cols();
}

int TerminalFile::Rows() const
{
    return terminalControl->Rows();
}

int TerminalFile::CursorX() const
{
    return terminalControl->CursorX();
}

int TerminalFile::CursorY() const
{
    return terminalControl->CursorY();
}

void TerminalFile::SetCursorPos(int32_t cursorX, int32_t cursorY)
{
    terminalControl->SetCursorPos(cursorX, cursorY);
}

void TerminalFile::SetCooked()
{
    terminalControl->SetCooked();
}

void TerminalFile::SetRaw()
{
    terminalControl->SetRaw();
}

void TerminalFile::SetEcho(bool echo)
{
    terminalControl->SetEcho(echo);
}

void TerminalFile::PushLines()
{
    terminalControl->PushLines();
}

void TerminalFile::PopLines()
{
    terminalControl->PopLines();
}

std::vector<uint8_t> TerminalFile::Read(int64_t count, cmsx::machine::Process* process)
{
    return terminalControl->ReadInput(count);
}

void TerminalFile::Write(const std::u32string& utf32Chars)
{
    if (!utf32Chars.empty())
    {
        terminalControl->Write(utf32Chars);
    }
}

int64_t TerminalFile::Write(const std::vector<uint8_t>& buffer, cmsx::machine::Process* process)
{
    std::u32string utf32Chars;
    for (uint8_t x : buffer)
    {
        utfEngine.Put(x);
        if (utfEngine.ResulReady())
        {
            char32_t ch = utfEngine.Result();
            if (ch < static_cast<char32_t>(0x100000u))
            {
                utf32Chars.append(1, ch);
            }
            else if (ch < 0x10FFFD)
            {
                uint8_t fgColor = static_cast<uint32_t>(ch) & 0xFF;
                uint8_t bgColor = (static_cast<uint32_t>(ch) >> 8) & 0xFF;
                Write(utf32Chars);
                utf32Chars.clear();
                terminalControl->SetColors(static_cast<ConsoleColor>(fgColor), static_cast<ConsoleColor>(bgColor));
            }
            else if (ch == 0x10FFFD)
            {
                Write(utf32Chars);
                utf32Chars.clear();
                terminalControl->SetColors(terminalControl->DefaultForeColor(), terminalControl->DefaultBackColor());
            }
        }
    }
    Write(utf32Chars);
    utf32Chars.clear();
    terminalControl->SetOutputEndCursorPos(terminalControl->CursorX(), terminalControl->CursorY());
    terminalControl->ScrollToCursor();
    return buffer.size();
}

} // cmsx::guiterm
