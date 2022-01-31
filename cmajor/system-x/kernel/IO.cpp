// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/IO.hpp>
#include <system-x/kernel/Error.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/kernel/Mount.hpp>
#include <system-x/kernel/Kernel.hpp>
#include <system-x/kernel/Fs.hpp>
#include <system-x/kernel/BlockFile.hpp>
#include <system-x/machine/Machine.hpp>
#include <system-x/machine/Processor.hpp>
#include <soulng/util/Path.hpp>

namespace cmsx::kernel {

using namespace soulng::util;

std::vector<std::uint8_t> MakeStringBuffer(const std::string& str, int64_t maxLength)
{
    std::vector<std::uint8_t> buffer;
    for (char c : str)
    {
        if (buffer.size() >= maxLength - 1)
        {
            break;
        }
        buffer.push_back(static_cast<uint8_t>(c));
    }
    buffer.push_back(0);
    return buffer;
}

std::vector<uint8_t> ReadProcessMemory(Process* process, int64_t addr, int64_t count)
{
    std::vector<uint8_t> buffer;
    uint64_t rv = process->RV();
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    for (int64_t i = 0; i < count; ++i)
    {
        try
        {
            buffer.push_back(mem.ReadByte(rv, addr + i, cmsx::machine::Protection::read));
        }
        catch (const std::exception& ex)
        {
            throw SystemError(EMEMORYACCESS, ex.what());
        }
    }
    return buffer;
}

void WriteProcessMemory(Process* process, int64_t addr, const std::vector<uint8_t>& buffer)
{
    uint64_t rv = process->RV();
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    int64_t count = buffer.size();
    for (int64_t i = 0; i < count; ++i)
    {
        try
        {
            mem.WriteByte(rv, addr + i, buffer[i], cmsx::machine::Protection::write);
        }
        catch (const std::exception& ex)
        {
            throw SystemError(EMEMORYACCESS, ex.what());
        }
    }
}

std::string ReadString(Process* process, int64_t address, cmsx::machine::Memory& mem)
{
    std::string s;
    uint64_t rv = process->RV();
    uint8_t x = mem.ReadByte(rv, address, cmsx::machine::Protection::read);
    while (x)
    {
        s.append(1, static_cast<char>(x));
        ++address;
        x = mem.ReadByte(rv, address, cmsx::machine::Protection::read);
    }
    return s;
}

int64_t WriteString(Process* process, const std::string& s, int64_t address, cmsx::machine::Memory& mem)
{
    uint64_t rv = process->RV();
    for (char c : s)
    {
        mem.WriteByte(rv, static_cast<uint64_t>(address), static_cast<uint8_t>(c), cmsx::machine::Protection::write);
        ++address;
    }
    mem.WriteByte(rv, static_cast<uint64_t>(address), static_cast<uint8_t>(0), cmsx::machine::Protection::write);
    ++address;
    return address;
}

int32_t Create(Process* process, int64_t pathAddr, int32_t mode)
{
    if (pathAddr == 0)
    {
        throw SystemError(EPARAM, "path is null");
    }
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    std::string path = ReadString(process, pathAddr, mem);
    Filesystem* fs = GetFs(rootFSNumber);
    BlockFile* file = fs->Create(path, mode, process);
    ProcessFileTable& fileTable = process->GetFileTable();
    int32_t fd = fileTable.AddFile(file);
    return fd;
}

int32_t Open(Process* process, int64_t pathAddr, int32_t flags, int32_t mode)
{
    if (pathAddr == 0)
    {
        throw SystemError(EPARAM, "path is null");
    }
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    std::string path = ReadString(process, pathAddr, mem);
    Filesystem* fs = GetFs(rootFSNumber);
    File* file = fs->Open(path, flags, mode, process);
    ProcessFileTable& fileTable = process->GetFileTable();
    int32_t fd = fileTable.AddFile(file);
    return fd;
}

void Close(Process* process, int32_t fd)
{
    ProcessFileTable& fileTable = process->GetFileTable();
    fileTable.CloseFile(fd, process);
}

int64_t Write(Process* process, int32_t fd, int64_t bufferAddr, int64_t count)
{
    if (bufferAddr == 0)
    {
        throw SystemError(EPARAM, "buffer is null");
    }
    if (count < 0)
    {
        throw SystemError(EPARAM, "count is negative");
    }
    ProcessFileTable& fileTable = process->GetFileTable();
    File* file = fileTable.GetFile(fd);
    std::vector<uint8_t> buffer = ReadProcessMemory(process, bufferAddr, count);
    return file->Write(buffer, process);
}

int64_t Read(Process* process, int32_t fd, int64_t bufferAddr, int64_t count)
{
    if (bufferAddr == 0)
    {
        throw SystemError(EPARAM, "buffer is null");
    }
    if (count < 0)
    {
        throw SystemError(EPARAM, "count is negative");
    }
    ProcessFileTable& fileTable = process->GetFileTable();
    File* file = fileTable.GetFile(fd);
    std::vector<uint8_t> buffer = file->Read(count, process);
    WriteProcessMemory(process, bufferAddr, buffer);
    return buffer.size();
}

void Unlink(Process* process, int64_t pathAddr)
{
    if (pathAddr == 0)
    {
        throw SystemError(EPARAM, "path is null");
    }
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    std::string path = ReadString(process, pathAddr, mem);
    std::string dirName = Path::GetDirectoryName(path);
    INodePtr dirINode = PathToINode(dirName, GetFs(rootFSNumber), process);
    if (!dirINode.Get())
    {
        throw SystemError(EFAIL, "could not unlink: directory '" + dirName + "' not found");
    }
    std::string fileName = Path::GetFileName(path);
    RemoveDirectoryEntry(fileName, dirINode.Get(), GetFs(rootFSNumber), process);
}

void Seek(Process* process, int32_t fd, int64_t offset, int32_t whence)
{
    ProcessFileTable& fileTable = process->GetFileTable();
    File* file = fileTable.GetFile(fd);
    file->Seek(offset, static_cast<Origin>(whence), process);
}

} // namespace cmsx::kernel
