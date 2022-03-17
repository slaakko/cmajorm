// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/DevFs.hpp>
#include <system-x/kernel/Mount.hpp>
#include <system-x/kernel/Error.hpp>
#include <system-x/kernel/Terminal.hpp>
#include <soulng/util/Path.hpp>

namespace cmsx::kernel {

DevFilesystem::DevFilesystem() : Filesystem(devFSNumber), machine(nullptr), mountPoint(), devINodeKey(devFSNumber, 0)
{
}

void DevFilesystem::Initialize()
{
    // todo
}

File* DevFilesystem::Create(const std::string& path, INode* dirINode, int32_t mode, cmsx::machine::Process* process)
{
    throw SystemError(EFAIL, "device filesystem: cannot create '" + path + "'");
}

File* DevFilesystem::Open(const std::string& path, INode* dirINode, int32_t flags, int32_t mode, cmsx::machine::Process* process)
{
    if (path == "tty")
    {
        return GetTerminalFile();
    }
    else
    {
        throw SystemError(ENOTFOUND, "device filesystem: path '" + path + "' not found");
    }
}

INodePtr DevFilesystem::SearchDirectory(const std::string& name, INode* dirINode, const std::string& dirPath, cmsx::machine::Process* process)
{
#if (LOCK_DEBUG)
    DebugLock startDebugLock(&machine->Lock(), DEV_FILE_SYSTEM, process->Id(), NO_LOCK | SEARCH_DIRECTORY);
#endif 
    std::lock_guard<std::recursive_mutex> lock(machine->Lock());
#if (LOCK_DEBUG)
    DebugLock hasDebugLock(&machine->Lock(), DEV_FILE_SYSTEM, process->Id(), HAS_LOCK | SEARCH_DIRECTORY);
#endif 
    if (dirINode->Key() == mountPoint)
    {
        INodePtr inodePtr = GetINode(devINodeKey, process);
        INode* inode = inodePtr.Get();
        inode->SetFileType(FileType::directory);
        cmsx::kernel::Access access = static_cast<cmsx::kernel::Access>(cmsx::kernel::Access::read | kernel::Access::write | cmsx::kernel::Access::execute);
        inode->SetOwnerAccess(access);
        inode->SetGroupAccess(access);
        inode->SetOtherAccess(access);
        return inode;
    }
    else
    {
        return INodePtr(nullptr);
    }
}

void DevFilesystem::Stat(INode* inode, cmsx::machine::Process* process)
{
    // todo
}

BlockFile* DevFilesystem::HostFile() const 
{
    throw SystemError(EFAIL, "device filesystem does not provide a host file");
}

DirFile* DevFilesystem::OpenDir(const std::string& path, INode* dirINode, cmsx::machine::Process* process)
{
    throw SystemError(EFAIL, "device filesystem does not provide directories");
}

void DevFilesystem::MkDir(INode* parentDirINode, const std::string& dirName, cmsx::machine::Process* process, int32_t mode)
{
    throw SystemError(EFAIL, "device filesystem does not provide directories");
}

INodePtr DevFilesystem::ReadINode(INodeKey inodeKey, cmsx::machine::Process* process)
{
    return INodePtr(nullptr); // todo
}

std::string DevFilesystem::GetHostFilePath(int32_t inodeNumber, cmsx::machine::Process* process)
{
    return std::string();
}

void DevFilesystem::ClearProcessData(cmsx::machine::Process* process)
{
    // todo
}

} // namespace cmsx::kernel
