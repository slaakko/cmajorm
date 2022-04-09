// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/File.hpp>
#include <system-x/kernel/Error.hpp>
#include <system-x/machine/Config.hpp>

namespace cmsx::kernel {

File* terminalFile = nullptr;

File::File(const std::string& name_) : name(name_)
{
}

File::~File()
{
}

std::vector<uint8_t> File::Read(int64_t count, cmsx::machine::Process* process)
{
    if (!IsReadable())
    {
        throw SystemError(EBADF, name + " not open for reading", __FUNCTION__);
    }
    return std::vector<uint8_t>();
}

int64_t File::Write(const std::vector<uint8_t>& buffer, cmsx::machine::Process* process)
{
    if (!IsWritable())
    {
        throw SystemError(EBADF, name + " not open for writing", __FUNCTION__);
    }
    return 0;
}

void File::SetCursorPos(int32_t cursorX, int32_t cursorY)
{
    throw SystemError(EBADF, name + ": cannot set cursor position", __FUNCTION__);
}

void File::SetCooked()
{
    throw SystemError(EBADF, name + " cannot be set to cooked mode", __FUNCTION__);

}
void File::SetRaw()
{
    throw SystemError(EBADF, name + " cannot be set to raw mode", __FUNCTION__);
}

void File::SetEcho(bool echo)
{
    throw SystemError(EBADF, name + ": cannot set echo", __FUNCTION__);
}

void File::PushLines()
{
    throw SystemError(EBADF, name + ": cannot push lines", __FUNCTION__);
}

void File::PopLines()
{
    throw SystemError(EBADF, name + ": cannot pop lines", __FUNCTION__);
}

int64_t File::Seek(int64_t offset, Origin whence, cmsx::machine::Process* process)
{
    throw SystemError(EBADF, name + " not seekable", __FUNCTION__);
}

int64_t File::Tell(cmsx::machine::Process* process)
{
    throw SystemError(EBADF, name + " not seekable", __FUNCTION__);
}

void File::Bind(int32_t md)
{
    throw SystemError(EBADF, name + " cannot bind", __FUNCTION__);
}

void File::Unbind()
{
    throw SystemError(EBADF, name + " cannot unbind", __FUNCTION__);
}

void File::SetTabMsgQueues(int32_t putMd, int32_t getMd)
{
    throw SystemError(EBADF, name + " cannot set tab message queues", __FUNCTION__);
}

void File::ResetTabMsgQueues()
{
    throw SystemError(EBADF, name + " cannot reset tab message queues", __FUNCTION__);
}

void File::PushPid(int32_t pid)
{
    throw SystemError(EBADF, name + " cannot push pid", __FUNCTION__);
}

void File::PopPid()
{
    throw SystemError(EBADF, name + " cannot pop pid", __FUNCTION__);
}

ProcessFileTable::ProcessFileTable()
{
    files.resize(cmsx::machine::MaxOpenFiles());
    files[0] = terminalFile;
    files[1] = terminalFile;
    files[2] = terminalFile;
}

void ProcessFileTable::CloseFiles(cmsx::kernel::Process* process)
{
    for (File* file : files)
    {
        if (file)
        {
            file->Close(process);
        }
    }
    files.clear();
}

int32_t ProcessFileTable::Dup(int32_t fd)
{
    File* file = GetFile(fd);
    return AddFile(file->Share());
}

void ProcessFileTable::CopyFrom(const ProcessFileTable& that)
{
    for (int32_t fd = 0; fd < that.files.size(); ++fd)
    {
        File* file = that.files[fd];
        if (file && file->IsShareable())
        {
            files[fd] = file->Share();
        }
    }
}

int32_t ProcessFileTable::AddFile(File* file)
{
    for (int32_t fd = 0; fd < files.size(); ++fd)
    {
        if (!files[fd])
        {
            files[fd] = file;
            return fd;
        }
    }
    return -1;
}

void ProcessFileTable::CloseFile(int32_t fd, cmsx::kernel::Process* process)
{
    File* file = GetFile(fd);
    file->Close(process);
    files[fd] = nullptr;
}

File* ProcessFileTable::GetFile(int32_t fd) const
{
    if (fd < 0 || fd > files.size())
    {
        throw SystemError(EBADF, "invalid file descriptor", __FUNCTION__);
    }
    File* file = files[fd];
    if (!file)
    {
        throw SystemError(EBADF, "file not open", __FUNCTION__);
    }
    return file;
}

void SetTerminalFile(File* terminalFile)
{
    cmsx::kernel::terminalFile = terminalFile;
}

File* GetTerminalFile()
{
    return cmsx::kernel::terminalFile;
}

void WriteToTerminal(const std::string& text, cmsx::machine::Process* process)
{
    if (cmsx::kernel::terminalFile)
    {
        std::vector<std::uint8_t> buffer;
        for (char c : text)
        {
            buffer.push_back(static_cast<uint8_t>(c));
        }
        cmsx::kernel::terminalFile->Write(buffer, process);
    }
}

} // namespace cmsx::kernel
