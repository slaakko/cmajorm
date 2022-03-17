// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/HostFs.hpp>
#include <system-x/kernel/Error.hpp>
#include <system-x/kernel/HostFileManager.hpp>
#include <system-x/kernel/IOManager.hpp>
#include <system-x/kernel/EventManager.hpp>
#include <system-x/kernel/Mount.hpp>
#include <system-x/kernel/BlockFile.hpp>
#include <system-x/kernel/DirFile.hpp>
#include <system-x/kernel/Kernel.hpp>
#include <system-x/kernel/OsApi.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/kernel/DebugHelp.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <boost/filesystem.hpp>

namespace cmsx::kernel {

using namespace soulng::util;

class HostFilesystemFile : public BlockFile
{
public:
    HostFilesystemFile(HostFilesystem* fs_, int32_t fileId_, const std::string& filePath, OpenFlags flags_, INodeKey inodeKey_);
    void Close(cmsx::kernel::Process* process) override;
    bool IsReadable() const override { return (flags & OpenFlags::read) != OpenFlags::none; }
    bool IsWritable() const override { return (flags & OpenFlags::write) != OpenFlags::none; }
    bool IsConsole() const override { return false; }
    bool IsHostTextFile() const override { return (flags & OpenFlags::text) != OpenFlags::none; }
    bool IsResource() const override { return false; }
    bool IsDecompressionFile() const override { return false; }
    bool HasColors() const override { return false; }
    int Columns() const override { return 0; }
    int Rows() const override { return 0; }
    int CursorX() const override { return 0; }
    int CursorY() const override { return 0; }
    int32_t GetBlockNumber(INode* inode, cmsx::machine::Process* process, bool allocate) const override;
    INodePtr GetINode(cmsx::machine::Process* process) override;
    int64_t Read(Block* block, cmsx::machine::Process* process) override;
    int64_t Write(Block* block, cmsx::machine::Process* process) override;
private:
    HostFilesystem* fs;
    int32_t fileId;
    OpenFlags flags;
    int32_t hostFileId;
};

HostFilesystemFile::HostFilesystemFile(HostFilesystem* fs_, int32_t fileId_, const std::string& filePath, OpenFlags flags_, INodeKey inodeKey_) :
    BlockFile(filePath, inodeKey_), fs(fs_), fileId(fileId_), flags(flags_), hostFileId(OpenHostFile(filePath, flags))
{
}

void HostFilesystemFile::Close(cmsx::kernel::Process* process)
{
    INodePtr inode = GetINode(process);
    CloseHostFile(hostFileId);
    fs->Close(fileId, inode.Get(), process);
}

int32_t HostFilesystemFile::GetBlockNumber(INode* inode, cmsx::machine::Process* process, bool allocate) const
{
    int64_t filePos = FilePos();
    return static_cast<int32_t>(filePos / Block::Size());
}

INodePtr HostFilesystemFile::GetINode(cmsx::machine::Process* process)
{
    INodePtr inode = cmsx::kernel::GetINode(GetINodeKey(), process);
    if (boost::filesystem::exists(Name()))
    {
        inode.Get()->SetFileSize(boost::filesystem::file_size(Name()));
    }
    if ((flags & OpenFlags::append) != OpenFlags::none)
    {
        SetFilePos(inode.Get()->FileSize());
    }
    return inode;
}

int64_t HostFilesystemFile::Read(Block* block, cmsx::machine::Process* process)
{
#if (LOCK_DEBUG)
    DebugLock startDebugLock(&fs->GetMachine()->Lock(), HOST_FILE_SYSTEM, process->Id(), NO_LOCK | READ);
#endif 
    std::unique_lock<std::recursive_mutex> lock(fs->GetMachine()->Lock());
#if (LOCK_DEBUG)
    DebugLock hasDebugLock(&fs->GetMachine()->Lock(), HOST_FILE_SYSTEM, process->Id(), HAS_LOCK | READ);
#endif 
    int32_t requestId = cmsx::kernel::Read(hostFileId, block);
    cmsx::machine::Event evnt(cmsx::machine::EventKind::ioEvent, requestId);
    Sleep(evnt, process, lock);
    lock.lock();
    IORequest* request = GetRequest(requestId);
    if (request->Failed())
    {
        SystemError error = request->Error();
        DeleteRequest(requestId);
        throw error;
    }
    else
    {
        block->SetValid();
        int64_t bytesTransferred = request->BytesTransferred();
        DeleteRequest(requestId);
        return bytesTransferred;
    }
}

int64_t HostFilesystemFile::Write(Block* block, cmsx::machine::Process* process)
{
#if (LOCK_DEBUG)
    DebugLock startDebugLock(&fs->GetMachine()->Lock(), HOST_FILE_SYSTEM, process->Id(), NO_LOCK | WRITE);
#endif 
    std::unique_lock<std::recursive_mutex> lock(fs->GetMachine()->Lock());
#if (LOCK_DEBUG)
    DebugLock hasDebugLock(&fs->GetMachine()->Lock(), HOST_FILE_SYSTEM, process->Id(), HAS_LOCK | WRITE);
#endif 
    int32_t requestId = cmsx::kernel::Write(hostFileId, block);
    cmsx::machine::Event evnt(cmsx::machine::EventKind::ioEvent, requestId);
    Sleep(evnt, process, lock);
    lock.lock();
    IORequest* request = GetRequest(requestId);
    if (request->Failed())
    {
        SystemError error = request->Error();
        DeleteRequest(requestId);
        throw error;
    }
    else
    {
        block->ResetDirty();
        int64_t bytesTransferred = request->BytesTransferred();
        DeleteRequest(requestId);
        return bytesTransferred;
    }
}

class HostFilesystemDirFile : public DirFile 
{
public:
    HostFilesystemDirFile(HostFilesystem* fs_, const std::string& name_, int32_t id_);
    void Close(cmsx::kernel::Process* process) override;
    int32_t Read(DirectoryEntry& dirEntry, cmsx::machine::Process* process) override;
    int32_t Id() const { return id; }
private:
    HostFilesystem* fs;
    int32_t id;
    int32_t dirEntryIndex;
    void* searchHandle;
};

HostFilesystemDirFile::HostFilesystemDirFile(HostFilesystem* fs_, const std::string& name_, int32_t id_) : DirFile(name_), fs(fs_), id(id_), dirEntryIndex(0), searchHandle(nullptr)
{
}

void HostFilesystemDirFile::Close(cmsx::kernel::Process* process)
{
    fs->CloseDir(id);
}

int32_t HostFilesystemDirFile::Read(DirectoryEntry& dirEntry, cmsx::machine::Process* process)
{
    std::string entryStr;
    if (searchHandle == nullptr)
    {
        if (OsFindFirstFile(Name(), entryStr, searchHandle))
        {
            dirEntry.SetINodeNumber(-1);
            dirEntry.SetNameTruncate(entryStr);
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        if (OsFindNextFile(searchHandle, entryStr))
        {
            dirEntry.SetINodeNumber(-1);
            dirEntry.SetNameTruncate(entryStr);
            return 1;
        }
        else
        {
            return 0;
        }
    }
}

HostFilesystem::HostFilesystem(int32_t id_, const std::string& prefix_) : Filesystem(id_), nextINodeId(0), nextDirId(0), machine(nullptr), prefix(prefix_)
{
}

void HostFilesystem::Initialize()
{
}

File* HostFilesystem::Create(const std::string& path, INode* dirINode, int32_t mode, cmsx::machine::Process* process)
{
    OpenFlags openFlags = OpenFlags::create | OpenFlags::truncate | OpenFlags::write | OpenFlags::random_access;
    return Open(path, dirINode, static_cast<int32_t>(openFlags), mode, process);
}

File* HostFilesystem::Open(const std::string& path, INode* dirINode, int32_t flags, int32_t mode, cmsx::machine::Process* process)
{
#if (LOCK_DEBUG)
    DebugLock startDebugLock(&machine->Lock(), HOST_FILE_SYSTEM, process->Id(), NO_LOCK | OPEN);
#endif 
    std::lock_guard<std::recursive_mutex> lock(machine->Lock());
#if (LOCK_DEBUG)
    DebugLock hasDebugLock(&machine->Lock(), HOST_FILE_SYSTEM, process->Id(), HAS_LOCK | OPEN);
#endif 
    ProcessHostFilesystemData* data = nullptr;
    auto it = processDataMap.find(process->Id());
    if (it != processDataMap.cend())
    {
        data = it->second;
    }
    else
    {
        data = new ProcessHostFilesystemData();
        processDataMap[process->Id()] = data;
    }
    std::string fullPath;
    if (dirINode)
    {
        auto it = data->inodePathMap.find(dirINode->Key().inodeNumber);
        if (it != data->inodePathMap.cend())
        {
            fullPath = Path::Combine(it->second, path);
        }
        else
        {
            throw SystemError(ENOTFOUND, "path '" + path + "' not found from host file system '" + prefix + "'");
        }
    }
    else
    {
        fullPath = Path::Combine(prefix, path);
    }
    OpenFlags openFlags = static_cast<OpenFlags>(flags);
    if ((openFlags & OpenFlags::truncate) != OpenFlags::none)
    {
        if (boost::filesystem::exists(fullPath))
        {
            boost::system::error_code ec;
            boost::filesystem::remove(fullPath, ec);
            if (ec)
            {
                throw SystemError(EFAIL, "could not remove file '" + fullPath + "' from host file system: " + soulng::util::PlatformStringToUtf8(ec.message()));
            }
        }
    }
    int32_t fileId = nextINodeId++;
    INodeKey inodeKey(Id(), fileId);
    INodePtr inodePtr = GetINode(inodeKey, process);
    INode* inode = inodePtr.Get();
    inode->IncrementReferenceCount();
    inode->SetValid();
    data->inodePathMap[inode->Key().inodeNumber] = fullPath;
    HostFilesystemFile* file = new HostFilesystemFile(this, fileId, fullPath, openFlags, inodeKey);
    fileMap[fileId] = file;
    return file;
}

INodePtr HostFilesystem::SearchDirectory(const std::string& name, INode* dirINode, const std::string& dirPath, cmsx::machine::Process* process)
{
#if (LOCK_DEBUG)
    DebugLock startDebugLock(&machine->Lock(), HOST_FILE_SYSTEM, process->Id(), NO_LOCK | SEARCH_DIRECTORY);
#endif 
    std::lock_guard<std::recursive_mutex> lock(machine->Lock());
#if (LOCK_DEBUG)
    DebugLock hasDebugLock(&machine->Lock(), HOST_FILE_SYSTEM, process->Id(), HAS_LOCK | SEARCH_DIRECTORY);
#endif 
    ProcessHostFilesystemData* data = nullptr;
    auto it = processDataMap.find(process->Id());
    if (it != processDataMap.cend())
    {
        data = it->second;
    }
    else
    {
        data = new ProcessHostFilesystemData();
        processDataMap[process->Id()] = data;
    }
    std::string fullPath;
    if (dirINode->Key() == mountPoint)
    {
        fullPath = GetFullPath(Path::Combine(prefix, name));
    }
    else if (dirINode->Key().fsNumber == Id())
    {
        auto it = data->inodePathMap.find(dirINode->Key().inodeNumber);
        if (it != data->inodePathMap.cend())
        {
            fullPath = Path::Combine(it->second, name);
        }
    }
    if (boost::filesystem::exists(fullPath))
    {
        INodeKey inodeKey(Id(), nextINodeId++);
        data->inodePathMap[inodeKey.inodeNumber] = fullPath;
        INodePtr inode = GetINode(inodeKey, process);
        Stat(inode.Get(), process);
        return inode;
    }
    else
    {
        return INodePtr(nullptr);
    }
}

void HostFilesystem::Stat(INode* inode, cmsx::machine::Process* process)
{
    auto it = processDataMap.find(process->Id());
    if (it != processDataMap.cend())
    {
        ProcessHostFilesystemData* data = it->second;
        auto it = data->inodePathMap.find(inode->Key().inodeNumber);
        if (it != data->inodePathMap.cend())
        {
            std::string fullPath = it->second;
            boost::filesystem::file_status status = boost::filesystem::status(fullPath);
            cmsx::kernel::Access access = static_cast<cmsx::kernel::Access>(cmsx::kernel::Access::read | kernel::Access::write);
            std::string ext = Path::GetExtension(fullPath);
            if (ext == ".exe")
            {
                access = static_cast<cmsx::kernel::Access>(access | cmsx::kernel::Access::execute);
            }
            if (status.type() == boost::filesystem::file_type::regular_file)
            {
                inode->SetFileType(FileType::regular);
                inode->SetFileSize(boost::filesystem::file_size(fullPath));
            }
            else if (status.type() == boost::filesystem::file_type::directory_file)
            {
                inode->SetFileType(FileType::directory);
                access = static_cast<cmsx::kernel::Access>(access | cmsx::kernel::Access::execute);
            }
            else
            {
                throw SystemError(EFAIL, "path '" + fullPath + "' has unknown file type");
            }
            inode->SetOwnerAccess(access);
            inode->SetGroupAccess(access);
            inode->SetOtherAccess(access);
            inode->SetNLinks(1);
            std::time_t lastWriteTime = boost::filesystem::last_write_time(fullPath);
            DateTime mtime = ToDateTime(lastWriteTime);
            inode->SetMTime(mtime);
        }
        else
        {
            throw SystemError(ENOTFOUND, "path not found");
        }
    }
    else
    {
        throw SystemError(ENOTFOUND, "process data for PID " + std::to_string(process->Id()) + " not found from host filesystem ");
    }
}

void HostFilesystem::Close(int32_t fileId, INode* inode, cmsx::kernel::Process* process)
{
#if (LOCK_DEBUG)
    DebugLock startDebugLock(&machine->Lock(), HOST_FILE_SYSTEM, 0, NO_LOCK | CLOSE);
#endif 
    std::lock_guard<std::recursive_mutex> lock(machine->Lock());
#if (LOCK_DEBUG)
    DebugLock hasDebugLock(&machine->Lock(), HOST_FILE_SYSTEM, 0, HAS_LOCK | CLOSE);
#endif 
    auto it = fileMap.find(fileId);
    if (it != fileMap.cend())
    {
        File* file = it->second;
        inode->DecrementReferenceCount();
        if (inode->ReferenceCount() == 0)
        {
            fileMap.erase(fileId);
            file->Release(process);
        }
    }
}

BlockFile* HostFilesystem::HostFile() const
{
    throw SystemError(EFAIL, "host filesystem does not provide a host file");
}

DirFile* HostFilesystem::OpenDir(const std::string& path, INode* dirINode, cmsx::machine::Process* process)
{
#if (LOCK_DEBUG)
    DebugLock startDebugLock(&machine->Lock(), HOST_FILE_SYSTEM, 0, NO_LOCK | OPEN_DIR);
#endif 
    std::lock_guard<std::recursive_mutex> lock(machine->Lock());
#if (LOCK_DEBUG)
    DebugLock hasDebugLock(&machine->Lock(), HOST_FILE_SYSTEM, 0, HAS_LOCK | OPEN_DIR);
#endif 
    auto it = processDataMap.find(process->Id());
    if (it != processDataMap.cend())
    {
        ProcessHostFilesystemData* data = it->second;
        auto it = data->inodePathMap.find(dirINode->Key().inodeNumber);
        if (it != data->inodePathMap.cend())
        {
            std::string fullPath = it->second;
            HostFilesystemDirFile* dirFile = new HostFilesystemDirFile(this, fullPath, nextDirId++);
            dirFileMap[dirFile->Id()] = dirFile;
            return dirFile;
        }
        else
        {
            throw SystemError(ENOTFOUND, "path not found from inode path map with inode number " + std::to_string(dirINode->Key().inodeNumber));
        }
    }
    else
    {
        throw SystemError(ENOTFOUND, "process data for PID " + std::to_string(process->Id()) + " not found from host filesystem ");
    }
}

void HostFilesystem::MkDir(INode* parentDirINode, const std::string& dirName, cmsx::machine::Process* process, int32_t mode)
{
#if (LOCK_DEBUG)
    DebugLock startDebugLock(&machine->Lock(), HOST_FILE_SYSTEM, 0, NO_LOCK | MK_DIR);
#endif 
    std::lock_guard<std::recursive_mutex> lock(machine->Lock());
#if (LOCK_DEBUG)
    DebugLock hasDebugLock(&machine->Lock(), HOST_FILE_SYSTEM, 0, HAS_LOCK | MK_DIR);
#endif 
    auto it = processDataMap.find(process->Id());
    if (it != processDataMap.cend())
    {
        ProcessHostFilesystemData* data = it->second;
        auto it = data->inodePathMap.find(parentDirINode->Key().inodeNumber);
        if (it != data->inodePathMap.cend())
        {
            std::string fullPath = Path::Combine(it->second, dirName);
            boost::system::error_code ec;
            boost::filesystem::create_directory(fullPath, ec);
            if (ec)
            {
                throw SystemError(EHOST, "could not create host directory '" + fullPath + "': " + PlatformStringToUtf8(ec.message()));
            }
        }
        else
        {
            throw SystemError(ENOTFOUND, "path not found from inode path map with inode number " + std::to_string(parentDirINode->Key().inodeNumber));
        }
    }
    else
    {
        throw SystemError(ENOTFOUND, "process data for PID " + std::to_string(process->Id()) + " not found from host filesystem ");
    }
}

void HostFilesystem::CloseDir(int32_t dirId)
{
#if (LOCK_DEBUG)
    DebugLock startDebugLock(&machine->Lock(), HOST_FILE_SYSTEM, 0, NO_LOCK | CLOSE_DIR);
#endif 
    std::lock_guard<std::recursive_mutex> lock(machine->Lock());
#if (LOCK_DEBUG)
    DebugLock hasDebugLock(&machine->Lock(), HOST_FILE_SYSTEM, 0, HAS_LOCK | CLOSE_DIR);
#endif 
    auto it = dirFileMap.find(dirId);
    if (it != dirFileMap.cend())
    {
        DirFile* dir = it->second;
        dirFileMap.erase(dirId);
        delete dir;
    }
}

std::string HostFilesystem::GetHostFilePath(int32_t inodeNumber, cmsx::machine::Process* process) 
{
#if (LOCK_DEBUG)
    DebugLock startDebugLock(&machine->Lock(), HOST_FILE_SYSTEM, process->Id(), NO_LOCK | GET_HOST_FILE_PATH);
#endif 
    std::lock_guard<std::recursive_mutex> lock(machine->Lock());
#if (LOCK_DEBUG)
    DebugLock hasDebugLock(&machine->Lock(), HOST_FILE_SYSTEM, process->Id(), HAS_LOCK | GET_HOST_FILE_PATH);
#endif 
    ProcessHostFilesystemData* data = nullptr;
    auto it = processDataMap.find(process->Id());
    if (it != processDataMap.end())
    {
        data = it->second;
    }
    else
    {
        data = new ProcessHostFilesystemData();
        processDataMap[process->Id()] = data;
    }
    auto iti = data->inodePathMap.find(inodeNumber);
    if (iti != data->inodePathMap.cend())
    {
        std::string fullPath = iti->second;
        return fullPath;
    }
    else
    {
        return std::string();
    }
}

INodePtr HostFilesystem::ReadINode(INodeKey inodeKey, cmsx::machine::Process* process)
{
#if (LOCK_DEBUG)
    DebugLock startDebugLock(&machine->Lock(), HOST_FILE_SYSTEM, process->Id(), NO_LOCK | READ_INODE);
#endif 
    std::lock_guard<std::recursive_mutex> lock(machine->Lock());
#if (LOCK_DEBUG)
    DebugLock hasDebugLock(&machine->Lock(), HOST_FILE_SYSTEM, process->Id(), HAS_LOCK | READ_INODE);
#endif 
    auto it = processDataMap.find(process->Id());
    if (it != processDataMap.cend())
    {
        ProcessHostFilesystemData* data = it->second;
        auto it = data->inodePathMap.find(inodeKey.inodeNumber);
        if (it != data->inodePathMap.cend())
        {
            INodePtr inode = GetINode(inodeKey, process);
            Stat(inode.Get(), process);
            return inode;
        }
        else
        {
            throw SystemError(EFAIL, "host file system could not retrieve inode");
        }
    }
    else
    {
        throw SystemError(ENOTFOUND, "process data for PID " + std::to_string(process->Id()) + " not found from host filesystem ");
    }
}

void HostFilesystem::ClearProcessData(cmsx::machine::Process* process)
{
#if (LOCK_DEBUG)
    DebugLock startDebugLock(&machine->Lock(), HOST_FILE_SYSTEM, process->Id(), NO_LOCK | CLEAR_PROCESS_DATA);
#endif 
    std::lock_guard<std::recursive_mutex> lock(machine->Lock());
#if (LOCK_DEBUG)
    DebugLock hasDebugLock(&machine->Lock(), HOST_FILE_SYSTEM, process->Id(), HAS_LOCK | CLEAR_PROCESS_DATA);
#endif 
    auto it = processDataMap.find(process->Id());
    if (it != processDataMap.cend())
    {
        ProcessHostFilesystemData* data = it->second;
        delete data;
        processDataMap.erase(process->Id());
    }
}

} // namespace cmsx::kernel
