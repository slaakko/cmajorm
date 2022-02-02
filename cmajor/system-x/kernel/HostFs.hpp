// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_HOST_FS_INCLUDED
#define CMSX_KERNEL_HOST_FS_INCLUDED
#include <system-x/kernel/Fs.hpp>

namespace cmsx::kernel {

class CMSX_KERNEL_API HostFilesystem : public Filesystem
{
public:
    HostFilesystem(int32_t id_, const std::string& prefix_);
    void SetMachine(cmsx::machine::Machine* machine_) override { machine = machine_; }
    cmsx::machine::Machine* GetMachine() const { return machine; }
    void Initialize() override;
    BlockFile* Create(const std::string& path, INode* dirINode, int32_t mode, cmsx::machine::Process* process) override;
    BlockFile* Open(const std::string& path, INode* dirINode, int32_t flags, int32_t mode, cmsx::machine::Process* process) override;
    INodePtr SearchDirectory(const std::string& name, INode* dirINode, cmsx::machine::Process* process) override;
    void Stat(INode* inode) override;
    void Close(int32_t fileId, INode* inode);
    BlockFile* HostFile() const override;
    INodeKey MountPoint() const override { return mountPoint; }
    void SetMountPoint(INodeKey mountPoint_) { mountPoint = mountPoint_; }
private:
    cmsx::machine::Machine* machine;
    int32_t nextINodeId;
    std::map<int32_t, BlockFile*> fileMap;
    std::map<int32_t, std::string> inodePathMap;
    std::string prefix;
    INodeKey mountPoint;
};

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_HOST_FS_INCLUDED
