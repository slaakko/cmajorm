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

std::vector<uint8_t> File::Read(int64_t count)
{
    throw SystemError(EBADF, name + " not open for reading");
}

int64_t File::Write(const std::vector<uint8_t>& buffer)
{
    throw SystemError(EBADF, name + " not open for writing");
}

ProcessFileTable::ProcessFileTable()
{
    files.resize(cmsx::machine::MaxOpenFiles());
    files[0] = consoleInputFile;
    files[1] = consoleOutputFile;
    files[2] = consoleOutputFile;
}

void ProcessFileTable::CopyFrom(const ProcessFileTable& that)
{
    files = that.files;
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

} // namespace cmsx::kernel
