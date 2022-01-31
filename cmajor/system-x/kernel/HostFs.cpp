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
#include <boost/filesystem.hpp>

namespace cmsx::kernel {

class HostFilesystemFile : public BlockFile
{
public:
    HostFilesystemFile(HostFilesystem* fs_, int32_t fileId_, const std::string& filePath, OpenFlags flags_, INodeKey inodeKey_);
    void Close(cmsx::machine::Process* process) override;
    bool IsReadable() const override { return (flags & OpenFlags::read) != OpenFlags::none; }
    bool IsWritable() const override { return (flags & OpenFlags::write) != OpenFlags::none; }
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
    if (IsReadable() && boost::filesystem::exists(Name()))
    {
        inode.Get()->SetFileSize(boost::filesystem::file_size(Name()));
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

HostFilesystem::HostFilesystem() : Filesystem(hostFSNumber), nextFileId(0), machine(nullptr)
{
}

void HostFilesystem::Initialize()
{
}

BlockFile* HostFilesystem::Create(const std::string& path, int32_t mode, cmsx::machine::Process* process)
{
    OpenFlags openFlags = OpenFlags::create | OpenFlags::truncate | OpenFlags::write | OpenFlags::random_access;
    return Open(path, static_cast<int32_t>(openFlags), mode, process);
}

BlockFile* HostFilesystem::Open(const std::string& path, int32_t flags, int32_t mode, cmsx::machine::Process* process)
{
    std::lock_guard<std::recursive_mutex> lock(machine->Lock());
    int32_t fileId = nextFileId++;
    OpenFlags openFlags = static_cast<OpenFlags>(flags);
    INodeKey inodeKey(hostFSNumber, fileId);
    INodePtr inodePtr = GetINode(inodeKey, process);
    INode* inode = inodePtr.Get();
    inode->IncrementReferenceCount();
    inode->SetValid();
    HostFilesystemFile* file = new HostFilesystemFile(this, fileId, path, openFlags, inodeKey);
    fileMap[fileId] = file;
    return file;
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
