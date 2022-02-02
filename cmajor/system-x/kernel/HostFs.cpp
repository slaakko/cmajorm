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
#include <system-x/kernel/Kernel.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <boost/filesystem.hpp>

namespace cmsx::kernel {

using namespace soulng::util;

class HostFilesystemFile : public BlockFile
{
public:
    HostFilesystemFile(HostFilesystem* fs_, int32_t fileId_, const std::string& filePath, OpenFlags flags_, INodeKey inodeKey_);
    void Close(cmsx::machine::Process* process) override;
    bool IsReadable() const override { return (flags & OpenFlags::read) != OpenFlags::none; }
    bool IsWritable() const override { return (flags & OpenFlags::write) != OpenFlags::none; }
    bool IsConsole() const override { return false; }
    bool IsHostTextFile() const override { return (flags & OpenFlags::text) != OpenFlags::none; }
    bool HasColors() const override { return false; }
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

void HostFilesystemFile::Close(cmsx::machine::Process* process)
{
    INodePtr inode = GetINode(process);
    CloseHostFile(hostFileId);
    fs->Close(fileId, inode.Get());
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
    std::unique_lock<std::recursive_mutex> lock(fs->GetMachine()->Lock());
    int32_t requestId = cmsx::kernel::Read(hostFileId, block);
    cmsx::machine::Event evnt(cmsx::machine::EventKind::ioEvent, requestId);
    Sleep(evnt, process, lock);
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
    std::unique_lock<std::recursive_mutex> lock(fs->GetMachine()->Lock());
    int32_t requestId = cmsx::kernel::Write(hostFileId, block);
    cmsx::machine::Event evnt(cmsx::machine::EventKind::ioEvent, requestId);
    Sleep(evnt, process, lock);
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

HostFilesystem::HostFilesystem(int32_t id_, const std::string& prefix_) : Filesystem(id_), nextINodeId(0), machine(nullptr), prefix(prefix_)
{
}

void HostFilesystem::Initialize()
{
}

BlockFile* HostFilesystem::Create(const std::string& path, INode* dirINode, int32_t mode, cmsx::machine::Process* process)
{
    OpenFlags openFlags = OpenFlags::create | OpenFlags::truncate | OpenFlags::write | OpenFlags::random_access;
    return Open(path, dirINode, static_cast<int32_t>(openFlags), mode, process);
}

BlockFile* HostFilesystem::Open(const std::string& path, INode* dirINode, int32_t flags, int32_t mode, cmsx::machine::Process* process)
{
    std::lock_guard<std::recursive_mutex> lock(machine->Lock());
    std::string fullPath;
    if (dirINode)
    {
        auto it = inodePathMap.find(dirINode->Key().inodeNumber);
        if (it != inodePathMap.cend())
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
    int32_t fileId = nextINodeId++;
    INodeKey inodeKey(Id(), fileId);
    INodePtr inodePtr = GetINode(inodeKey, process);
    INode* inode = inodePtr.Get();
    inode->IncrementReferenceCount();
    inode->SetValid();
    OpenFlags openFlags = static_cast<OpenFlags>(flags);
    inodePathMap[inode->Key().inodeNumber] = fullPath;
    HostFilesystemFile* file = new HostFilesystemFile(this, fileId, fullPath, openFlags, inodeKey);
    fileMap[fileId] = file;
    return file;
}

INodePtr HostFilesystem::SearchDirectory(const std::string& name, INode* dirINode, cmsx::machine::Process* process)
{
    std::string fullPath;
    if (dirINode->Key() == mountPoint)
    {
        fullPath = GetFullPath(Path::Combine(prefix, name));
    }
    else if (dirINode->Key().fsNumber == Id())
    {
        auto it = inodePathMap.find(dirINode->Key().inodeNumber);
        if (it != inodePathMap.cend())
        {
            fullPath = Path::Combine(it->second, name);
        }
    }
    if (boost::filesystem::exists(fullPath))
    {
        INodeKey inodeKey(Id(), nextINodeId++);
        inodePathMap[inodeKey.inodeNumber] = fullPath;
        INodePtr inode = GetINode(inodeKey, process);
        Stat(inode.Get());
        return inode;
    }
    else
    {
        return INodePtr(nullptr);
    }
}

void HostFilesystem::Stat(INode* inode)
{
    auto it = inodePathMap.find(inode->Key().inodeNumber);
    if (it != inodePathMap.cend())
    {
        std::string fullPath = it->second;
        boost::filesystem::file_status status = boost::filesystem::status(fullPath);
        if (status.type() == boost::filesystem::file_type::regular_file)
        {
            inode->SetFileType(FileType::regular);
            inode->SetFileSize(boost::filesystem::file_size(fullPath));
        }
        else if (status.type() == boost::filesystem::file_type::directory_file)
        {
            inode->SetFileType(FileType::directory);
        }
        else
        {
            throw SystemError(EFAIL, "path '" + fullPath + "' has unknown file type");
        }
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

void HostFilesystem::Close(int32_t fileId, INode* inode)
{
    std::lock_guard<std::recursive_mutex> lock(machine->Lock());
    auto it = fileMap.find(fileId);
    if (it != fileMap.cend())
    {
        File* file = it->second;
        inode->DecrementReferenceCount();
        if (inode->ReferenceCount() == 0)
        {
            delete file;
            fileMap.erase(fileId);
        }
    }
}

BlockFile* HostFilesystem::HostFile() const
{
    throw SystemError(EFAIL, "host filesystem does not provide a host file");
}

} // namespace cmsx::kernel
