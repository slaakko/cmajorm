// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_HOST_FS_INCLUDED
#define CMSX_KERNEL_HOST_FS_INCLUDED
#include <system-x/kernel/Fs.hpp>

namespace cmsx::kernel {

struct CMSX_KERNEL_API ProcessHostFilesystemData
{
    std::map<int32_t, std::string> inodePathMap;
};

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
    void Stat(INode* inode, cmsx::machine::Process* process) override;
    void Close(int32_t fileId, INode* inode, cmsx::kernel::Process* process);
    BlockFile* HostFile() const override;
    INodeKey MountPoint() const override { return mountPoint; }
    void SetMountPoint(INodeKey mountPoint_) { mountPoint = mountPoint_; }
    DirFile* OpenDir(const std::string& path, INode* dirINode, cmsx::machine::Process* process) override;
    void MkDir(INode* parentDirINode, const std::string& dirName, cmsx::machine::Process* process) override;
    void CloseDir(int32_t dirId);
    std::string GetHostFilePath(int32_t inodeNumber, cmsx::machine::Process* process) override;
    INodePtr ReadINode(INodeKey inodeKey, cmsx::machine::Process* process) override;
    void ClearProcessData(cmsx::machine::Process* process);
private:
    cmsx::machine::Machine* machine;
    int32_t nextINodeId;
    int32_t nextDirId;
    std::map<int32_t, ProcessHostFilesystemData*> processDataMap;
    std::map<int32_t, BlockFile*> fileMap;
    std::map<int32_t, DirFile*> dirFileMap;
    std::string prefix;
    INodeKey mountPoint;
};

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_HOST_FS_INCLUDED
