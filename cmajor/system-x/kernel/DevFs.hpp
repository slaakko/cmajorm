// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_DEV_FS_INCLUDED
#define CMSX_KERNEL_DEV_FS_INCLUDED
#include <system-x/kernel/Fs.hpp>

namespace cmsx::kernel {

class CMSX_KERNEL_API DevFilesystem : public Filesystem
{
public:
    DevFilesystem();
    void SetMachine(cmsx::machine::Machine* machine_) override { machine = machine_; }
    void Initialize() override;
    File* Create(const std::string& path, INode* dirINode, int32_t mode, cmsx::machine::Process* process) override;
    File* Open(const std::string& path, INode* dirINode, int32_t flags, int32_t mode, cmsx::machine::Process* process) override;
    INodePtr SearchDirectory(const std::string& name, INode* dirINode, const std::string& dirPath, cmsx::machine::Process* process) override;
    void Stat(INode* inode, cmsx::machine::Process* process) override;
    BlockFile* HostFile() const override;
    INodeKey MountPoint() const override { return mountPoint; }
    void SetMountPoint(INodeKey mountPoint_) override { mountPoint = mountPoint_; }
    DirFile* OpenDir(const std::string& path, INode* dirINode, cmsx::machine::Process* process) override;
    void MkDir(INode* parentDirINode, const std::string& dirName, cmsx::machine::Process* process, int32_t mode) override;
    std::string GetHostFilePath(int32_t inodeNumber, cmsx::machine::Process* process) override;
    INodePtr ReadINode(INodeKey inodeKey, cmsx::machine::Process* process) override;
    void ClearProcessData(cmsx::machine::Process* process) override;
private:
    cmsx::machine::Machine* machine;
    INodeKey mountPoint;
    INodeKey devINodeKey;
};

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_DEV_FS_INCLUDED
