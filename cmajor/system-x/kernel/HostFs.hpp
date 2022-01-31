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
    HostFilesystem();
    void SetMachine(cmsx::machine::Machine* machine_) override { machine = machine_; }
    cmsx::machine::Machine* GetMachine() const { return machine; }
    void Initialize() override;
    BlockFile* Create(const std::string& path, int32_t mode, cmsx::machine::Process* process) override;
    BlockFile* Open(const std::string& path, int32_t flags, int32_t mode, cmsx::machine::Process* process) override;
    void Close(int32_t fileId, INode* inode);
    BlockFile* HostFile() const override;
private:
    cmsx::machine::Machine* machine;
    int32_t nextFileId;
    std::map<int32_t, BlockFile*> fileMap;
};

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_HOST_FS_INCLUDED
