// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_BLOCK_FILE_INCLUDED
#define CMSX_KERNEL_BLOCK_FILE_INCLUDED
#include <system-x/kernel/File.hpp>
#include <system-x/kernel/INodeManager.hpp>

namespace cmsx::kernel {

class Block;

class CMSX_KERNEL_API BlockFile : public File
{
public:
    BlockFile(const std::string& name_, INodeKey inodeKey_);
    std::vector<uint8_t> Read(int64_t count, cmsx::machine::Process* process) override;
    int64_t Write(const std::vector<uint8_t>& buffer, cmsx::machine::Process* process) override;
    int64_t Seek(int64_t offset, Origin whence, cmsx::machine::Process* process) override;
    int64_t Tell(cmsx::machine::Process* process) override;
    virtual INodePtr GetINode(cmsx::machine::Process* process) = 0;
    virtual int32_t GetBlockNumber(INode* inode, cmsx::machine::Process* process, bool allocate) const = 0;
    virtual int64_t Read(Block* block, cmsx::machine::Process* process) = 0;
    virtual int64_t Write(Block* block, cmsx::machine::Process* process) = 0;
    const INodeKey& GetINodeKey() const { return inodeKey; }
    int64_t FilePos() const { return filePos; }
    void SetFilePos(int64_t filePos_) { filePos = filePos_; }
private:
    INodeKey inodeKey;
    int64_t filePos;
};

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_BLOCK_FILE_INCLUDED
