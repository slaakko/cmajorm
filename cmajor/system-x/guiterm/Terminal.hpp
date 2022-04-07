// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_GUITERM_TERMINAL_INCLUDED
#define CMSX_GUITERM_TERMINAL_INCLUDED
#include <system-x/guiterm/Api.hpp>
#include <system-x/kernel/File.hpp>
#include <wing/Control.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmsx::guiterm {

class TerminalControl;

class CMSX_GUITERM_API TerminalFile : public cmsx::kernel::File
{
public:
    TerminalFile();
    bool IsShareable() const override { return true; }
    File* Share() override { return this; }
    void Release(cmsx::kernel::Process* process) override { }
    void Close(cmsx::kernel::Process* process) override { }
    bool IsReadable() const override { return true; }
    bool IsWritable() const override { return true; }
    bool IsConsole() const override { return true; }
    bool IsHostTextFile() const override { return false; }
    bool IsResource() const override { return false; }
    bool IsDecompressionFile() const override { return false; }
    bool IsDirFile() const override { return false; }
    bool HasColors() const override { return true; }
    int Columns() const override;
    int Rows() const override;
    int CursorX() const override;
    int CursorY() const override;
    void SetCursorPos(int32_t cursorX, int32_t cursorY) override;
    void SetCooked() override;
    void SetRaw() override;
    void SetEcho(bool echo) override;
    void PushLines() override;
    void PopLines() override;
    std::vector<uint8_t> Read(int64_t count, cmsx::machine::Process* process) override;
    int64_t Write(const std::vector<uint8_t>& buffer, cmsx::machine::Process* process) override;
    TerminalControl* GetTerminalControl() const { return terminalControl; }
    void SetTerminalControl(TerminalControl* terminalControl_) { terminalControl = terminalControl_; }
private:
    void Write(const std::u32string& utf32Chars);
    TerminalControl* terminalControl;
    soulng::unicode::Utf8ToUtf32Engine utfEngine;
};

} // cmsx::guiterm

#endif // CMSX_GUITERM_GUITERMINAL_INCLUDED
