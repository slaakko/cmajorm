// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/File.hpp>
#include <system-x/kernel/Error.hpp>
#include <system-x/machine/Config.hpp>

namespace cmsx::kernel {

File* consoleOutputFile = nullptr;
File* consoleInputFile = nullptr;

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
        throw SystemError(EBADF, name + " not open for reading");
    }
    return std::vector<uint8_t>();
}

int64_t File::Write(const std::vector<uint8_t>& buffer, cmsx::machine::Process* process)
{
    if (!IsWritable())
    {
        throw SystemError(EBADF, name + " not open for writing");
    }
    return 0;
}

int64_t File::Seek(int64_t offset, Origin whence, cmsx::machine::Process* process)
{
    throw SystemError(EBADF, name + " not seekable");
}

int64_t File::Tell(cmsx::machine::Process* process)
{
    throw SystemError(EBADF, name + " not seekable");
}

ProcessFileTable::ProcessFileTable()
{
    files.resize(cmsx::machine::MaxOpenFiles());
    files[0] = consoleInputFile;
    files[1] = consoleOutputFile;
    files[2] = consoleOutputFile;
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
    return AddFile(file);
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
        throw SystemError(EBADF, "invalid file descriptor");
    }
    File* file = files[fd];
    if (!file)
    {
        throw SystemError(EBADF, "file not open");
    }
    return file;
}

void SetConsoleFiles(File* consoleOutputFile, File* consoleInputFile)
{
    cmsx::kernel::consoleOutputFile = consoleOutputFile;
    cmsx::kernel::consoleInputFile = consoleInputFile;
}

void WriteToConsole(const std::string& text, cmsx::machine::Process* process)
{
    if (cmsx::kernel::consoleOutputFile)
    {
        std::vector<std::uint8_t> buffer;
        for (char c : text)
        {
            buffer.push_back(static_cast<uint8_t>(c));
        }
        cmsx::kernel::consoleOutputFile->Write(buffer, process);
    }
}


} // namespace cmsx::kernel
