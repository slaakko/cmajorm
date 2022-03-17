// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_FS_INCLUDED
#define CMSX_KERNEL_FS_INCLUDED
#include <system-x/kernel/BlockManager.hpp>
#include <system-x/kernel/INodeManager.hpp>

namespace cmsx::kernel {

class File;
class BlockFile;
class DirFile;

class CMSX_KERNEL_API Filesystem
{
public:
    Filesystem(int32_t id_);
    virtual ~Filesystem();
    int32_t Id() const { return id; }
    virtual void SetMachine(cmsx::machine::Machine* machine) = 0;
    virtual void Initialize() = 0;
    virtual File* Create(const std::string& path, INode* dirINode, int32_t mode, cmsx::machine::Process* process) = 0;
    virtual File* Open(const std::string& path, INode* dirINode, int32_t flags, int32_t mode, cmsx::machine::Process* process) = 0;
    virtual BlockFile* HostFile() const = 0;
    virtual INodeKey MountPoint() const { return INodeKey(); }
    virtual void SetMountPoint(INodeKey mountPoint_) {}
    virtual INodePtr SearchDirectory(const std::string& name, INode* dirINode, const std::string& dirPath, cmsx::machine::Process* process) = 0;
    virtual void Stat(INode* inode, cmsx::machine::Process* process) = 0;
    virtual DirFile* OpenDir(const std::string& path, INode* dirINode, cmsx::machine::Process* process) = 0;
    virtual void MkDir(INode* parentDirINode, const std::string& dirName, cmsx::machine::Process* process, int32_t mode) = 0;
    virtual std::string GetHostFilePath(int32_t inodeNumber, cmsx::machine::Process* process) = 0;
    virtual INodePtr ReadINode(INodeKey inodeKey, cmsx::machine::Process* process) = 0;
    virtual void ClearProcessData(cmsx::machine::Process* process) = 0;
private:
    int32_t id;
};

class CMSX_KERNEL_API SuperBlock 
{
public:
    SuperBlock();
    void Read(Block* block);
    void Write(Block* block);
    void SetNumBlocks(int32_t numBlocks_) { numBlocks = numBlocks_; }
    int32_t NumBlocks() const { return numBlocks; }
    void SetNumINodes(int32_t numINodes_) { numINodes = numINodes_; }
    int32_t NumINodes() const { return numINodes; }
    void SetFirstBlockNumberBlockNumber(int32_t firstBlockNumberBlockNumber_) { firstBlockNumberBlockNumber = firstBlockNumberBlockNumber_;  }
    int32_t FirstBlockNumberBlockNumber() const { return firstBlockNumberBlockNumber; }
    void SetCurrentBlockNumberBlockNumber(int32_t currentBlockNumberBlockNumber_) { currentBlockNumberBlockNumber = currentBlockNumberBlockNumber_; }
    int32_t CurrentBlockNumberBlockNumber() const { return currentBlockNumberBlockNumber; }
    int32_t LastBlockNumberBlockNumber() const { return FirstBlockNumberBlockNumber() + NumberOfBlockNumberBlocks() - 1; }
    void SetFirstINodeBlockNumber(int32_t firstINodeBlockNumber_) { firstINodeBlockNumber = firstINodeBlockNumber_; }
    int32_t FirstINodeBlockNumber() const { return firstINodeBlockNumber; }
    void SetCurrentINodeBlockNumber(int32_t currentINodeBlockNumber_) { currentINodeBlockNumber = currentINodeBlockNumber_; }
    int32_t CurrentINodeBlockNumber() const { return currentINodeBlockNumber; }
    int32_t LastINodeBlockNumber() const { return FirstINodeBlockNumber() + NumberOfINodeBlocks() - 1; }
    void SetFirstDataBlockNumber(int32_t firstDataBlockNumber_) { firstDataBlockNumber = firstDataBlockNumber_; }
    int32_t FirstDataBlockNumber() const { return firstDataBlockNumber; }
    int32_t NumberOfBlockNumberBlocks() const;
    int32_t NumberOfINodeBlocks() const;
    int32_t NumberOfDataBlocks() const;
    int32_t GetINodeBlockNumber(int32_t inodeNumber) const;
    int32_t GetINodeIndexInBlock(int32_t inodeNumber) const;
    int32_t MakeINodeNumber(int32_t inodeBlockNumber, int32_t inodeIndexInBlock) const;
    int32_t CurrentNumberOfFiles() const { return currentNumberOfFiles; }
    void IncrementNumberOfFiles() { ++currentNumberOfFiles; }
    void DecrementNumberOfFiles() { --currentNumberOfFiles; }
    int32_t INodeNumberOfRootDirectory() const { return inodeNumberOfRootDirectory; }
    void SetINodeNumberOfRootDirectory(int32_t inodeNumberOfRootDirectory_) { inodeNumberOfRootDirectory = inodeNumberOfRootDirectory_; }
private:
    int32_t numBlocks;
    int32_t numINodes;
    int32_t firstBlockNumberBlockNumber;
    int32_t currentBlockNumberBlockNumber;
    int32_t firstINodeBlockNumber;
    int32_t currentINodeBlockNumber;
    int32_t firstDataBlockNumber;
    int32_t currentNumberOfFiles;
    int32_t inodeNumberOfRootDirectory;
};

class CMSX_KERNEL_API BlockNumberBlock
{
public:
    BlockNumberBlock();
    static constexpr int32_t BlockNumberCount() { return Block::Size() / 4; }
    void Read(Block* block);
    void Write(Block* block);
    int32_t GetBlockNumber(int index) const;
    void SetBlockNumber(int32_t blockNumber, int index);
    int32_t GetFreeBlockNumber();
    bool PutFreeBlockNumber(int32_t blockNumber);
private:
    std::vector<int32_t> blockNumbers;
};

const int numINodesInBlock = Block::Size() / INode::Size();

class CMSX_KERNEL_API INodeBlock
{
public:
    INodeBlock();
    void Read(Block* block);
    void Write(Block* block);
    int FirstFreeINodeIndex() const;
    static constexpr int Count() { return numINodesInBlock; }
    const INode& GetINode(int index) const { return inodes[index]; }
    void SetINode(const INode& inode, int index) { inodes[index] = inode; }
private:
    INode inodes[numINodesInBlock];
};

class CMSX_KERNEL_API DirectoryEntry
{
public:
    DirectoryEntry();
    void Read(Block* block, int index);
    void Write(Block* block, int index);
    static constexpr int Size() { return 256; }
    static constexpr int MaxNameLength() { return Size() - 4 - 1; }
    bool IsFree() const { return inodeNumber == -1; }
    void SetFree() { inodeNumber = -1; }
    int32_t INodeNumber() const { return inodeNumber; }
    void SetINodeNumber(int32_t inodeNumber_) { inodeNumber = inodeNumber_; }
    const std::string& Name() const { return name; }
    void SetName(const std::string& name_);
    void SetNameTruncate(const std::string& name_);
private:
    int32_t inodeNumber;
    std::string name;
};

class CMSX_KERNEL_API DirectoryBlock
{
public:
    DirectoryBlock();
    static constexpr int NumDirectoryEntriesInBlock() { return Block::Size() / DirectoryEntry::Size(); }
    void Read(Block* block);
    void Write(Block* block);
    const std::vector<DirectoryEntry>& Entries() const { return entries; }
    void AddEntry(const DirectoryEntry& entry);
    void RemoveEntry(int index);
    bool IsEmpty() const { return entries.empty(); }
    bool IsFull() const { return entries.size() == NumDirectoryEntriesInBlock(); }
private:
    std::vector<DirectoryEntry> entries;
};

CMSX_KERNEL_API uint64_t ToULong(const INodeKey& key);
CMSX_KERNEL_API INodeKey ToINodeKey(uint64_t key);
CMSX_KERNEL_API Filesystem* GetFs(int32_t fsNumber);
CMSX_KERNEL_API void ReadSuperBlock(SuperBlock& superBlock, Filesystem* fs, cmsx::machine::Process* process);
CMSX_KERNEL_API void WriteSuperBlock(SuperBlock& superBlock, Filesystem* fs, cmsx::machine::Process* process);
CMSX_KERNEL_API BlockPtr ReadBlockNumberBlock(BlockNumberBlock& blockNumberBlock, int32_t blockNumberBlockNumber, Filesystem* fs, cmsx::machine::Process* process);
CMSX_KERNEL_API void WriteBlockNumberBlock(BlockNumberBlock& blockNumberBlock, BlockPtr& bnb, Filesystem* fs, cmsx::machine::Process* process);
CMSX_KERNEL_API BlockPtr ReadINodeBlock(INodeBlock& inodeBlock, int32_t inodeBlockNumber, Filesystem* fs, cmsx::machine::Process* process);
CMSX_KERNEL_API void WriteINodeBlock(INodeBlock& inodeBlock, BlockPtr& inb, Filesystem* fs, cmsx::machine::Process* process);
CMSX_KERNEL_API BlockPtr ReadDirectoryBlock(DirectoryBlock& directoryBlock, int32_t directoryBlockNumber, Filesystem* fs, cmsx::machine::Process* process);
CMSX_KERNEL_API void WriteDirectoryBlock(DirectoryBlock& directoryBlock, BlockPtr& db, Filesystem* fs, cmsx::machine::Process* process);
CMSX_KERNEL_API int32_t GetFreeBlockNumber(int32_t fsNumber, cmsx::machine::Process* process);
CMSX_KERNEL_API void PutFreeBlockNumber(int32_t fsNumber, int32_t freeBlockNumber, cmsx::machine::Process* process);
CMSX_KERNEL_API INodePtr AllocateINode(int32_t fsNumber, cmsx::machine::Process* process);
CMSX_KERNEL_API void FreeINode(INode* inode, cmsx::machine::Process* process);
CMSX_KERNEL_API INodePtr ReadINode(const INodeKey& inodeKey, cmsx::machine::Process* process);
CMSX_KERNEL_API void WriteINode(INode* inode, cmsx::machine::Process* process);
CMSX_KERNEL_API BlockPtr AllocateBlock(int32_t fsNumber, cmsx::machine::Process* process);
CMSX_KERNEL_API void FreeBlock(Block* block, cmsx::machine::Process* process);
CMSX_KERNEL_API void FreeBlocks(INode* inode, Filesystem* fs, cmsx::machine::Process* process);
CMSX_KERNEL_API int32_t MapBlockNumber(int32_t logicalBlockNumber, INode* inode, Filesystem* fs, cmsx::machine::Process* process);
CMSX_KERNEL_API void SetBlockNumber(int32_t logicalBlockNumber, int32_t blockNumber, INode* inode, Filesystem* fs, cmsx::machine::Process* process);
CMSX_KERNEL_API INodePtr SearchDirectory(const std::string& name, INode* dirINode, const std::string& dirPath, Filesystem* fs, cmsx::machine::Process* process);
CMSX_KERNEL_API DirectoryEntry GetDirectoryEntry(INode* dirINode, int32_t inodeNumber, Filesystem* fs, cmsx::machine::Process* process);
CMSX_KERNEL_API void AddDirectoryEntry(const DirectoryEntry& entry, INode* dirINode, Filesystem* fs, cmsx::machine::Process* process);
CMSX_KERNEL_API void RemoveDirectoryEntry(const std::string& name, const std::string& filePath, INode* dirINode, Filesystem* fs, cmsx::machine::Process* process);
CMSX_KERNEL_API void RemoveDirectoryEntry(const std::string& name, const std::string& filePath, INode* dirINode, Filesystem* fs, cmsx::machine::Process* process, bool unlink);
CMSX_KERNEL_API void RenameDirectoryEntry(INode* dirINode, const std::string& oldName, const std::string& newName, Filesystem* fs, cmsx::machine::Process* process);
CMSX_KERNEL_API INodePtr MakeDirectory(const std::string& path, Filesystem* fs, cmsx::machine::Process* process, int32_t mode);
CMSX_KERNEL_API bool DirectoryExists(const std::string& path, Filesystem* fs, cmsx::machine::Process* process);
CMSX_KERNEL_API std::string GetHostFilePath(const INodeKey& key, cmsx::machine::Process* process);

enum class PathToINodeFlags : int32_t
{
    none = 0, ignoreMountPoint = 1 << 0, stat = 1 << 1
};

CMSX_KERNEL_API inline PathToINodeFlags operator|(PathToINodeFlags left, PathToINodeFlags right)
{
    return PathToINodeFlags(int32_t(left) | int32_t(right));
}

CMSX_KERNEL_API inline PathToINodeFlags operator&(PathToINodeFlags left, PathToINodeFlags right)
{
    return PathToINodeFlags(int32_t(left) & int32_t(right));
}

CMSX_KERNEL_API inline PathToINodeFlags operator~(PathToINodeFlags flags)
{
    return PathToINodeFlags(~int32_t(flags));
}

CMSX_KERNEL_API INodePtr PathToINode(const std::string& path, Filesystem* fs, cmsx::machine::Process* process);
CMSX_KERNEL_API INodePtr PathToINode(const std::string& path, Filesystem* fs, cmsx::machine::Process* process, PathToINodeFlags flags);

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_FS_INCLUDED
