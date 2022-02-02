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
    BlockFile* file = fs->Create(path, nullptr, mode, process);
    ProcessFileTable& fileTable = process->GetFileTable();
    int32_t fd = fileTable.AddFile(file);
    if (fd == -1)
    {
        throw SystemError(ELIMITEXCEEDED, "maximum number of open files exceeded");
    }
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
    File* file = fs->Open(path, nullptr, flags, mode, process);
    ProcessFileTable& fileTable = process->GetFileTable();
    int32_t fd = fileTable.AddFile(file);
    if (fd == -1)
    {
        throw SystemError(ELIMITEXCEEDED, "maximum number of open files exceeded");
    }
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

int32_t IOCtl(Process* process, int32_t fd, int32_t item)
{
    ProcessFileTable& fileTable = process->GetFileTable();
    File* file = fileTable.GetFile(fd);
    switch (static_cast<IOControlItem>(item))
    {
        case IOControlItem::isConsole:
        {
            return static_cast<int32_t>(file->IsConsole());
        }
        case IOControlItem::isHostTextFile:
        {
            return static_cast<int32_t>(file->IsHostTextFile());
        }
        case IOControlItem::hasColors:
        {
            return static_cast<int32_t>(file->HasColors());
        }
        default:
        {
            throw SystemError(EPARAM, "unknown ioctl item");
        }
    }
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

int64_t Seek(Process* process, int32_t fd, int64_t offset, int32_t whence)
{
    ProcessFileTable& fileTable = process->GetFileTable();
    File* file = fileTable.GetFile(fd);
    return file->Seek(offset, static_cast<Origin>(whence), process);
}

int64_t Tell(Process* process, int32_t fd)
{
    ProcessFileTable& fileTable = process->GetFileTable();
    File* file = fileTable.GetFile(fd);
    return file->Tell(process);
}

void Stat(Process* process, int64_t pathAddr, int64_t statBufAddr, int32_t statBufSize)
{
    if (pathAddr == 0)
    {
        throw SystemError(EPARAM, "path is null");
    }
    if (statBufAddr == 0)
    {
        throw SystemError(EPARAM, "stat buffer is null");
    }
    if (statBufSize < INode::StatBufSize())
    {
        throw SystemError(EPARAM, "stat buffer too small");
    }
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    std::string path = ReadString(process, pathAddr, mem);
    Filesystem* fs = GetFs(rootFSNumber);
    INodePtr inode = PathToINode(path, GetFs(rootFSNumber), process, PathToINodeFlags::stat);
    if (!inode.Get())
    {
        throw SystemError(ENOTFOUND, "path '" + path + "' not found");
    }
    std::vector<uint8_t> statBuffer(statBufSize, 0);
    MemoryWriter writer(statBuffer.data(), statBufSize);
    try
    {
        inode.Get()->WriteStat(writer);
    }
    catch (const std::exception& ex)
    {
        throw SystemError(EPARAM, "memory writer exception: " + std::string(ex.what()));
    }
    WriteProcessMemory(process, statBufAddr, statBuffer);
}

int32_t DirStat(Process* process, int64_t pathAddr)
{
    if (pathAddr == 0)
    {
        throw SystemError(EPARAM, "path is null");
    }
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    std::string path = ReadString(process, pathAddr, mem);
    bool directoryExists = DirectoryExists(path, GetFs(rootFSNumber), process);
    if (directoryExists)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void GetCWD(Process* process, int64_t bufAddr, int64_t bufSize)
{
    if (bufAddr == 0)
    {
        throw SystemError(EPARAM, "path buffer is null");
    }
    if (bufSize <= 1)
    {
        throw SystemError(EPARAM, "invalid buffer size");
    }
    INodeKey cwdINodeKey = ToINodeKey(process->GetINodeKeyOfWorkingDir());
    Filesystem* fs = GetFs(cwdINodeKey.fsNumber);
    if (fs->Id() != rootFSNumber)
    {
        throw SystemError(EFAIL, "process cwd not in root filesystem");
    }
    INodePtr dirINodePtr = ReadINode(cwdINodeKey, process);
    if (dirINodePtr.Get())
    {
        INodePtr parentDirINodePtr = SearchDirectory("..", dirINodePtr.Get(), fs, process);
        if (parentDirINodePtr.Get())
        {
            std::string cwd;
            while (dirINodePtr.Get()->Key() != parentDirINodePtr.Get()->Key())
            {
                DirectoryEntry entry = GetDirectoryEntry(parentDirINodePtr.Get(), dirINodePtr.Get()->Key().inodeNumber, fs, process);
                if (!entry.IsFree())
                {
                    cwd = Path::Combine(entry.Name(), cwd);
                }
                else
                {
                    throw SystemError(EFAIL, "parent directory entry not found");
                }
                INodePtr grandParentDirINodePtr = SearchDirectory("..", parentDirINodePtr.Get(), fs, process);
                if (!grandParentDirINodePtr.Get())
                {
                    throw SystemError(EFAIL, "grand parent directory not found");
                }
                dirINodePtr = std::move(parentDirINodePtr);
                parentDirINodePtr = std::move(grandParentDirINodePtr);
            }
            cwd = Path::Combine("/", cwd);
            std::vector<uint8_t> buffer;
            for (int64_t i = 0; i < bufSize - 1; ++i)
            {
                if (i >= cwd.length())
                {
                    break;
                }
                buffer.push_back(static_cast<uint8_t>(cwd[i]));
            }
            buffer.push_back(0);
            WriteProcessMemory(process, bufAddr, buffer);
        }
        else
        {
            throw SystemError(EFAIL, "parent directory not found from current directory");
        }
    }
    else
    {
        throw SystemError(EFAIL, "current directory not found from process");
    }
}

void ChDir(Process* process, int64_t pathAddr)
{
    if (pathAddr == 0)
    {
        throw SystemError(EPARAM, "path is null");
    }
    Filesystem* fs = GetFs(rootFSNumber);
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    std::string path = ReadString(process, pathAddr, mem);
    INodePtr inodePtr = PathToINode(path, fs, process);
    if (inodePtr.Get())
    {
        INodeKey inodeKey = inodePtr.Get()->Key();
        process->SetINodeKeyOfWorkingDir(ToULong(inodeKey));
    }
    else
    {
        throw SystemError(ENOTFOUND, "path '" + path + "' not found");
    }
}

} // namespace cmsx::kernel
