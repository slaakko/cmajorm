// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_TERMINAL_INCLUDED
#define CMSX_KERNEL_TERMINAL_INCLUDED
#include <system-x/kernel/File.hpp>

namespace cmsx::kernel {

class CMSX_KERNEL_API TerminalFile : public File
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
    bool HasColors() const override { return true; }
    int Columns() const override;
    int Rows() const override;
    int CursorX() const override;
    int CursorY() const override;
    void SetCursorPos(int32_t cursorX, int32_t cursorY) override;
    void SetCooked() override;
    void SetRaw() override;
    void SetEcho(bool echo) override;
    void SendKey(char32_t key) override;
    void PushLines() override;
    void PopLines() override;
    bool IsDirFile() const override { return false; }
    std::vector<uint8_t> Read(int64_t count, cmsx::machine::Process* process) override;
    int64_t Write(const std::vector<uint8_t>& buffer, cmsx::machine::Process* process) override;
};

CMSX_KERNEL_API void SetTerminalMachine(cmsx::machine::Machine* machine);
CMSX_KERNEL_API void StartTerminal();
CMSX_KERNEL_API void StopTerminal();
CMSX_KERNEL_API void InitTerminal();
CMSX_KERNEL_API void DoneTerminal();

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_TERMINAL_INCLUDED

