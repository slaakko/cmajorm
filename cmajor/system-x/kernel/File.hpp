// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_FILE_INCLUDED
#define CMSX_KERNEL_FILE_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <system-x/machine/Process.hpp>
#include <string>
#include <vector>
#include <stdint.h>

namespace cmsx::kernel {

class Process;

enum class OpenFlags : int32_t
{
    none = 0, read = 1 << 0, write = 1 << 1, create = 1 << 2, append = 1 << 3, truncate = 1 << 4, text = 1 << 5, random_access = 1 << 6, opened = 1 << 7
};

CMSX_KERNEL_API inline OpenFlags operator|(OpenFlags left, OpenFlags right)
{
    return OpenFlags(int32_t(left) | int32_t(right));
}

CMSX_KERNEL_API inline OpenFlags operator&(OpenFlags left, OpenFlags right)
{
    return OpenFlags(int32_t(left) & int32_t(right));
}

CMSX_KERNEL_API inline OpenFlags operator~(OpenFlags flags)
{
    return OpenFlags(~int32_t(flags));
}

enum class Origin : int32_t
{
    seekSet = 0, seekCur = 1, seekEnd = 2
};

class CMSX_KERNEL_API File
{
public:
    File(const std::string& name_);
    const std::string& Name() const { return name; }
    virtual ~File();
    virtual bool IsShareable() const { return true; }
    virtual File* Share() { return this; }
    virtual void Release(cmsx::kernel::Process* process) { }
    virtual void Close(cmsx::kernel::Process* process) = 0;
    virtual bool IsReadable() const = 0;
    virtual bool IsWritable() const = 0;
    virtual bool IsConsole() const = 0;
    virtual bool IsHostTextFile() const = 0;
    virtual bool IsResource() const = 0;
    virtual bool IsDecompressionFile() const = 0;
    virtual bool HasColors() const = 0;
    virtual int Columns() const = 0;
    virtual int Rows() const = 0;
    virtual int CursorX() const = 0;
    virtual int CursorY() const = 0;
    virtual void SetCursorPos(int32_t cursorX, int32_t cursorY);
    virtual void SetCooked();
    virtual void SetRaw();
    virtual void SetEcho(bool echo);
    virtual void PushLines();
    virtual void PopLines();
    virtual bool IsDirFile() const { return false; }
    virtual std::vector<uint8_t> Read(int64_t count, cmsx::machine::Process* process);
    virtual int64_t Write(const std::vector<uint8_t>& buffer, cmsx::machine::Process* process);
    virtual int64_t Seek(int64_t offset, Origin whence, cmsx::machine::Process* process);
    virtual int64_t Tell(cmsx::machine::Process* process);
private:
    std::string name;
};

class CMSX_KERNEL_API ProcessFileTable
{
public:
    ProcessFileTable();
    int32_t AddFile(File* file);
    void CloseFile(int32_t fd, cmsx::kernel::Process* process);
    File* GetFile(int32_t fd) const;
    void CopyFrom(const ProcessFileTable& that);
    void CloseFiles(cmsx::kernel::Process* process);
    int32_t Dup(int32_t fd);
private:
    std::vector<File*> files;
};

CMSX_KERNEL_API void SetTerminalFiles(File* terminalOutputFile, File* terminalInputFile);
CMSX_KERNEL_API void WriteToTerminal(const std::string& text, cmsx::machine::Process* process);

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_FILE_INCLUDED
