// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_ROOT_FS_INCLUDED
#define CMSX_KERNEL_ROOT_FS_INCLUDED
#include <system-x/kernel/Fs.hpp>
#include <system-x/kernel/File.hpp>

namespace cmsx::kernel {

class CMSX_KERNEL_API RootFilesystem : public Filesystem
{
public:
    RootFilesystem();
    void SetMachine(cmsx::machine::Machine* machine_) override { machine = machine_; }
    void Initialize() override;
    File* Create(const std::string& path, INode* dirINode, int32_t mode, cmsx::machine::Process* process) override;
    File* Open(const std::string& path, INode* dirINode, int32_t flags, int32_t mode, cmsx::machine::Process* process) override;
    INodePtr SearchDirectory(const std::string& name, INode* dirINode, const std::string& dirPath, cmsx::machine::Process* process) override;
    void Stat(INode* inode, cmsx::machine::Process* process) override;
    BlockFile* HostFile() const override { return hostFile; }
    DirFile* OpenDir(const std::string& path, INode* dirINode, cmsx::machine::Process* process) override;
    void MkDir(INode* parentDirINode, const std::string& dirName, cmsx::machine::Process* process, int32_t mode) override;
    void CloseFile(int32_t id, cmsx::kernel::Process* process);
    std::string GetHostFilePath(int32_t inodeNumber, cmsx::machine::Process* process) override { return std::string(); }
    INodePtr ReadINode(INodeKey inodeKey, cmsx::machine::Process* process) override;
    std::string INodeToPath(INodeKey inodeKey, cmsx::machine::Process* process) override;
private:
    cmsx::machine::Machine* machine;
    Filesystem* hostFs;
    BlockFile* hostFile;
    int32_t nextFileId;
    std::map<int32_t, File*> fileMap;
};

CMSX_KERNEL_API std::string RootFsHostFilePath();
CMSX_KERNEL_API bool RootFsExists();
CMSX_KERNEL_API void MakeRootFs(RootFilesystem& rootFs);

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_ROOT_FS_INCLUDED
