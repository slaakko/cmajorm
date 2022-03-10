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
#include <system-x/kernel/DirFile.hpp>
#include <system-x/machine/Machine.hpp>
#include <system-x/machine/Processor.hpp>
#include <soulng/util/MemoryReader.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Util.hpp>

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

std::vector<std::string> ReadStringPointerArray(Process* process, int64_t arrayAddress, cmsx::machine::Memory& mem)
{
    std::vector<std::string> strings;
    std::vector<uint8_t> stringAddrItem = ReadProcessMemory(process, arrayAddress, 8);
    MemoryReader reader(stringAddrItem.data(), 8);
    int64_t stringAddr = reader.ReadLong();
    while (stringAddr != 0)
    {
        std::string s = ReadString(process, stringAddr, mem);
        strings.push_back(std::move(s));
        arrayAddress += 8;
        stringAddrItem = ReadProcessMemory(process, arrayAddress, 8);
        MemoryReader reader(stringAddrItem.data(), 8);
        stringAddr = reader.ReadLong();
    }
    return strings;
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
    return Open(process, path, flags, mode);
}

int32_t Open(Process* process, const std::string& path, int32_t flags, int32_t mode)
{
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

int32_t IOCtl(Process* process, int32_t fd, int32_t item, int64_t argAddr, int64_t argSize)
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
        case IOControlItem::columns:
        {
            return static_cast<int32_t>(file->Columns());
        }
        case IOControlItem::rows:
        {
            return static_cast<int32_t>(file->Rows());
        }
        case IOControlItem::cursorX:
        {
            return static_cast<int32_t>(file->CursorX());
        }
        case IOControlItem::cursorY:
        {
            return static_cast<int32_t>(file->CursorY());
        }
        case IOControlItem::set_cursor_pos:
        {
            std::vector<uint8_t> arg = ReadProcessMemory(process, argAddr, 8);
            MemoryReader reader(arg.data(), 8);
            int32_t cursorX = reader.ReadInt();
            int32_t cursorY = reader.ReadInt();
            file->SetCursorPos(cursorX, cursorY);
            break;
        }
        case IOControlItem::set_cooked:
        {
            file->SetCooked();
            break;
        }
        case IOControlItem::set_raw:
        {
            file->SetRaw();
            break;
        }
        case IOControlItem::set_echo:
        {
            std::vector<uint8_t> arg = ReadProcessMemory(process, argAddr, 1);
            MemoryReader reader(arg.data(), 1);
            uint8_t echo = reader.ReadByte();
            if (echo == 1)
            {
                file->SetEcho(true);
            }
            else
            {
                file->SetEcho(false);
            }
        }
        case IOControlItem::push_lines:
        {
            file->PushLines();
            break;
        }
        case IOControlItem::pop_lines:
        {
            file->PopLines();
            break;
        }
        default:
        {
            throw SystemError(EPARAM, "unknown ioctl item");
        }
    }
    return 0;
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
    CheckAccess(Access::write, process->EUID(), process->EGID(), dirINode.Get(), "could not open directory '" + dirName + "' for writing");
    std::string fileName = Path::GetFileName(path);
    RemoveDirectoryEntry(fileName, path, dirINode.Get(), GetFs(rootFSNumber), process, true);
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
        INodePtr parentDirINodePtr = SearchDirectory("..", dirINodePtr.Get(), "..", fs, process);
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
                INodePtr grandParentDirINodePtr = SearchDirectory("..", parentDirINodePtr.Get(), "..", fs, process);
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

void MkDir(Process* process, int64_t pathAddr, int32_t mode)
{
    if (pathAddr == 0)
    {
        throw SystemError(EPARAM, "path is null");
    }
    Filesystem* fs = GetFs(rootFSNumber);
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    std::string path = ReadString(process, pathAddr, mem);
    if (DirectoryExists(path, fs, process))
    {
        throw SystemError(EALREADYEXISTS, "directory path '" + path + "' already exists");
    }
    std::string parentPath = Path::GetDirectoryName(path);
    std::string dirName = Path::GetFileName(path);
    INodePtr inodePtr = PathToINode(parentPath, fs, process);
    INode* parentINode = inodePtr.Get();
    if (parentINode)
    {
        Filesystem* ds = GetFs(parentINode->Key().fsNumber);
        CheckAccess(Access::write, process->EUID(), process->EGID(), parentINode, "could not create directory '" + path + "'");
        ds->MkDir(parentINode, dirName, process, mode);
    }
    else
    {
        throw SystemError(ENOTFOUND, "directory path '" + parentPath + "' not found");
    }
}

int32_t OpenDir(Process* process, int64_t pathAddr)
{
    if (pathAddr == 0)
    {
        throw SystemError(EPARAM, "path is null");
    }
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    std::string path = ReadString(process, pathAddr, mem);
    Filesystem* fs = GetFs(rootFSNumber);
    INodePtr inodePtr = PathToINode(path, fs, process);
    INode* inode = inodePtr.Get();
    if (inode)
    {
        CheckAccess(Access::read, process->EUID(), process->EGID(), inode, "could not open directory '" + path + "' for reading");
        Filesystem* ds = GetFs(inode->Key().fsNumber);
        DirFile* dirFile = ds->OpenDir(path, inode, process);
        ProcessFileTable& fileTable = process->GetFileTable();
        int32_t dd = fileTable.AddFile(dirFile);
        if (dd == -1)
        {
            throw SystemError(ELIMITEXCEEDED, "maximum number of open files exceeded");
        }
        return dd;
    }
    else
    {
        throw SystemError(ENOTFOUND, "path '" + path + "' not found");
    }
    return 0;
}

void CloseDir(Process* process, int32_t dfd)
{
    ProcessFileTable& fileTable = process->GetFileTable();
    fileTable.CloseFile(dfd, process);
}

int32_t ReadDir(Process* process, int32_t dfd, int64_t dirEntryBufAddr, int64_t dirEntryBufSize)
{
    if (dirEntryBufAddr == 0)
    {
        throw SystemError(EPARAM, "directory entry buffer is null");
    }
    if (dirEntryBufSize < DirectoryEntry::Size())
    {
        throw SystemError(EPARAM, "invalid directory entry buffer size");
    }
    ProcessFileTable& fileTable = process->GetFileTable();
    File* file = fileTable.GetFile(dfd);
    if (file->IsDirFile())
    {
        DirFile* dirFile = static_cast<DirFile*>(file);
        DirectoryEntry dirEntry;
        int32_t result = dirFile->Read(dirEntry, process);
        if (result == 0)
        {
            return result;
        }
        std::vector<uint8_t> buffer;
        buffer.push_back(0);
        buffer.push_back(0);
        buffer.push_back(0);
        buffer.push_back(0);
        MemoryWriter writer(buffer.data(), 4);
        writer.Write(dirEntry.INodeNumber());
        std::string name = dirEntry.Name();
        for (char c : name)
        {
            uint8_t b = static_cast<uint8_t>(c);
            buffer.push_back(b);
        }
        buffer.push_back(static_cast<uint8_t>(0));
        WriteProcessMemory(process, dirEntryBufAddr, buffer);
        return 1;
    }
    else
    {
        throw SystemError(EBADF, "invalid directory file descriptor");
    }
}

void UTime(Process* process, int64_t pathAddr, int64_t timeBufAddr, int64_t timeBufSize)
{
    if (pathAddr == 0)
    {
        throw SystemError(EPARAM, "path is null");
    }
    if (timeBufAddr == 0)
    {
        throw SystemError(EPARAM, "time buffer is null");
    }
    if (timeBufSize < 16)
    {
        throw SystemError(EPARAM, "invalid time buffer size");
    }
    std::vector<uint8_t> times = ReadProcessMemory(process, timeBufAddr, std::min(static_cast<int64_t>(16), timeBufSize));
    Filesystem* fs = GetFs(rootFSNumber);
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    std::string path = ReadString(process, pathAddr, mem);
    INodePtr inodePtr = PathToINode(path, fs, process);
    INode* inode = inodePtr.Get();
    if (inode)
    {
        MemoryReader reader(times.data(), times.size());
        DateTime aTime = reader.ReadDateTime();
        DateTime mTime = reader.ReadDateTime();
        inode->SetATime(aTime);
        inode->SetMTime(mTime);
        inode->SetCTime(GetCurrentDateTime());
        WriteINode(inode, process);
    }
    else
    {
        throw SystemError(ENOTFOUND, "path '" + path + "' not found");
    }
}

std::vector<uint8_t> ReadFile(Process* process, int64_t pathAddr)
{
    std::vector<uint8_t> content;
    int32_t fd = Open(process, pathAddr, static_cast<int32_t>(OpenFlags::read), 0); // todo mode
    ProcessFileTable& fileTable = process->GetFileTable();
    File* file = fileTable.GetFile(fd);
    int64_t fileSize = Seek(process, fd, 0, static_cast<int32_t>(Origin::seekEnd));
    Seek(process, fd, 0, static_cast<int32_t>(Origin::seekSet));
    content = file->Read(fileSize, process);
    Close(process, fd);
    return content;
}

int32_t Dup(Process* process, int32_t fd)
{
    ProcessFileTable& fileTable = process->GetFileTable();
    return fileTable.Dup(fd);
}

void ChMod(Process* process, int64_t pathAddr, int32_t mode)
{
    if (pathAddr == 0)
    {
        throw SystemError(EPARAM, "path is null");
    }
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    std::string path = ReadString(process, pathAddr, mem);
    Filesystem* fs = GetFs(rootFSNumber);
    INodePtr inodePtr = PathToINode(path, fs, process);
    INode* inode = inodePtr.Get();
    if (inode)
    {
        if (process->EUID() == 0 || process->EUID() == inode->UID())
        {
            inode->SetMode(mode);
            inode->SetCTime(GetCurrentDateTime());
            WriteINode(inode, process);
        }
        else
        {
            throw SystemError(EPERMISSION, "unauthorized");
        }
    }
    else
    {
        throw SystemError(ENOTFOUND, "path '" + path + "' not found");
    }
}

void ChOwn(Process* process, int64_t pathAddr, int32_t uid, int32_t gid)
{
    if (process->EUID() != 0)
    {
        throw SystemError(EPERMISSION, "unauthorized");
    }
    if (pathAddr == 0)
    {
        throw SystemError(EPARAM, "path is null");
    }
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    std::string path = ReadString(process, pathAddr, mem);
    Filesystem* fs = GetFs(rootFSNumber);
    INodePtr inodePtr = PathToINode(path, fs, process);
    INode* inode = inodePtr.Get();
    if (inode)
    {
        inode->SetUID(uid);
        inode->SetGID(gid);
        inode->SetCTime(GetCurrentDateTime());
        WriteINode(inode, process);
    }
    else
    {
        throw SystemError(ENOTFOUND, "path '" + path + "' not found");
    }
}

void Rename(Process* process, int64_t sourcePathAddr, int64_t targetPathAddr)
{
    if (sourcePathAddr == 0)
    {
        throw SystemError(EPARAM, "source path is null");
    }
    if (targetPathAddr == 0)
    {
        throw SystemError(EPARAM, "target path is null");
    }
    Filesystem* fs = GetFs(rootFSNumber);
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    std::string sourcePath = ReadString(process, sourcePathAddr, mem);
    std::string targetPath = ReadString(process, targetPathAddr, mem);
    std::string sourceDirPath = Path::GetDirectoryName(sourcePath);
    if (sourceDirPath.empty())
    {
        if (sourcePath.starts_with("/"))
        {
            sourceDirPath = "/";
        }
        else
        {
            sourceDirPath = ".";
        }
    }
    std::string targetDirPath = Path::GetDirectoryName(targetPath);
    if (targetDirPath.empty())
    {
        if (targetPath.starts_with("/"))
        {
            targetDirPath = "/";
        }
        else
        {
            targetDirPath = ".";
        }
    }
    INodePtr sourceDirINodePtr = PathToINode(sourceDirPath, fs, process);
    INode* sourceDirINode = sourceDirINodePtr.Get();
    if (sourceDirINode)
    {
        CheckAccess(Access::write, process->EUID(), process->EGID(), sourceDirINode, "could not open directory '" + sourceDirPath + "' for writing");
        INodePtr targetDirINodePtr = PathToINode(targetDirPath, fs, process);
        INode* targetDirINode = targetDirINodePtr.Get();
        if (targetDirINode)
        {
            CheckAccess(Access::write, process->EUID(), process->EGID(), sourceDirINode, "could not open directory '" + targetDirPath + "' for writing");
            if (sourceDirINode == targetDirINode)
            {
                RenameDirectoryEntry(sourceDirINode, Path::GetFileName(sourcePath), Path::GetFileName(targetPath), fs, process);
            }
            else
            {
                if (sourceDirINode->Key().fsNumber == targetDirINode->Key().fsNumber)
                {
                    INodePtr inodePtr = SearchDirectory(Path::GetFileName(sourcePath), sourceDirINode, sourceDirPath, fs, process);
                    INode* inode = inodePtr.Get();
                    if (inode)
                    {
                        DirectoryEntry entry;
                        entry.SetINodeNumber(inode->Key().inodeNumber);
                        entry.SetName(Path::GetFileName(targetPath));
                        AddDirectoryEntry(entry, targetDirINode, fs, process);
                        RemoveDirectoryEntry(Path::GetFileName(sourcePath), sourcePath, sourceDirINode, fs, process);
                    }
                    else
                    {
                        throw SystemError(ENOTFOUND, "file name '" + Path::GetFileName(sourcePath) + "' not found from directory '" + sourceDirPath + "'");
                    }
                }
                else
                {
                    throw SystemError(EFAIL, "cannot rename across filesystems");
                }
            }
        }
        else
        {
            throw SystemError(ENOTFOUND, "target directory '" + targetDirPath + "' not found");
        }
    }
    else
    {
        throw SystemError(ENOTFOUND, "source directory '" + sourceDirPath + "' not found");
    }
}

void AddDirChangeNotification(Process* process, int64_t directoryPathsAddr)
{
    if (directoryPathsAddr == 0)
    {
        throw SystemError(EPARAM, "directory path is null");
    }
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    std::string directoryPaths = ReadString(process, directoryPathsAddr, mem);
    std::vector<std::string> directories = Split(directoryPaths, ':');
    Filesystem* fs = GetFs(rootFSNumber);
    for (const std::string& directory : directories)
    {
        INodePtr dirINodePtr = PathToINode(directory, fs, process);
        if (dirINodePtr.Get())
        {
            INodeKey dirINodeKey = dirINodePtr.Get()->Key();
            cmsx::kernel::AddDirChangeNotification(dirINodeKey, process->Id());
        }
    }
}

} // namespace cmsx::kernel
