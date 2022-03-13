// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Fs.hpp>
#include <system-x/kernel/Error.hpp>
#include <system-x/kernel/Kernel.hpp>
#include <system-x/kernel/Mount.hpp>
#include <system-x/kernel/BlockFile.hpp>
#include <system-x/kernel/ProcessManager.hpp>
#include <soulng/util/MemoryReader.hpp>
#include <soulng/util/MemoryWriter.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Util.hpp>

namespace cmsx::kernel {

Filesystem::Filesystem(int32_t id_) : id(id_)
{
}

Filesystem::~Filesystem()
{
}

SuperBlock::SuperBlock() : 
    numBlocks(0), 
    numINodes(0), 
    firstBlockNumberBlockNumber(-1), 
    currentBlockNumberBlockNumber(-1), 
    firstINodeBlockNumber(-1), 
    currentINodeBlockNumber(-1), 
    firstDataBlockNumber(-1),
    currentNumberOfFiles(0),
    inodeNumberOfRootDirectory(-1)
{
}

void SuperBlock::Read(Block* block)
{
    soulng::util::MemoryReader reader(block->Data(), Block::Size());
    numBlocks = reader.ReadInt();
    numINodes = reader.ReadInt();
    firstBlockNumberBlockNumber = reader.ReadInt();
    currentBlockNumberBlockNumber = reader.ReadInt();
    firstINodeBlockNumber = reader.ReadInt();
    currentINodeBlockNumber = reader.ReadInt();
    firstDataBlockNumber = reader.ReadInt();
    currentNumberOfFiles = reader.ReadInt();
    inodeNumberOfRootDirectory = reader.ReadInt();
}

void SuperBlock::Write(Block* block)
{
    soulng::util::MemoryWriter writer(block->Data(), Block::Size());
    writer.Write(numBlocks);
    writer.Write(numINodes);
    writer.Write(firstBlockNumberBlockNumber);
    writer.Write(currentBlockNumberBlockNumber);
    writer.Write(firstINodeBlockNumber);
    writer.Write(currentINodeBlockNumber);
    writer.Write(firstDataBlockNumber);
    writer.Write(currentNumberOfFiles);
    writer.Write(inodeNumberOfRootDirectory);
    block->SetDirty();
}

int32_t SuperBlock::NumberOfBlockNumberBlocks() const
{
    int32_t numBlockNumberBlocks = (numBlocks - 1) / BlockNumberBlock::BlockNumberCount() + 1;
    return numBlockNumberBlocks;
}

int32_t SuperBlock::NumberOfINodeBlocks() const
{
    return (numINodes - 1) / numINodesInBlock + 1;
}

int32_t SuperBlock::NumberOfDataBlocks() const
{
    const int numReservedBlocks = 1; // block number 0 is super block  
    return numBlocks - numReservedBlocks - NumberOfBlockNumberBlocks() - NumberOfINodeBlocks();
}

int32_t SuperBlock::GetINodeBlockNumber(int32_t inodeNumber) const
{
    return firstINodeBlockNumber + (inodeNumber / numINodesInBlock);
}

int32_t SuperBlock::GetINodeIndexInBlock(int32_t inodeNumber) const
{
    return inodeNumber % numINodesInBlock;
}

int32_t SuperBlock::MakeINodeNumber(int32_t inodeBlockNumber, int32_t inodeIndexInBlock) const
{
    return (inodeBlockNumber - firstINodeBlockNumber) * numINodesInBlock + inodeIndexInBlock;
}

BlockNumberBlock::BlockNumberBlock() : blockNumbers(BlockNumberCount(), -1)
{
}

void BlockNumberBlock::Read(Block* block)
{
    MemoryReader reader(block->Data(), Block::Size());
    for (int32_t i = 0; i < BlockNumberCount(); ++i)
    {
        blockNumbers[i] = reader.ReadInt();
    }
}

void BlockNumberBlock::Write(Block* block)
{
    MemoryWriter writer(block->Data(), Block::Size());
    for (int32_t i = 0; i < BlockNumberCount(); ++i)
    {
        writer.Write(blockNumbers[i]);
    }
    block->SetDirty();
}

int32_t BlockNumberBlock::GetBlockNumber(int index) const
{
    if (index >= 0 && index < BlockNumberCount())
    {
        return blockNumbers[index];
    }
    else
    {
        throw SystemError(EFAIL, "invalid block number index");
    }
}

void BlockNumberBlock::SetBlockNumber(int32_t blockNumber, int index)
{
    if (index >= 0 && index < BlockNumberCount())
    {
        blockNumbers[index] = blockNumber;
    }
    else
    {
        throw SystemError(EFAIL, "invalid block number index");
    }
}

int32_t BlockNumberBlock::GetFreeBlockNumber()
{
    for (int i = 0; i < BlockNumberCount(); ++i)
    {
        if (blockNumbers[i] != -1)
        {
            int32_t blockNumber = blockNumbers[i];
            blockNumbers[i] = -1;
            return blockNumber;
        }
    }
    return -1;
}

bool BlockNumberBlock::PutFreeBlockNumber(int32_t blockNumber)
{
    for (int i = 0; i < BlockNumberCount(); ++i)
    {
        if (blockNumbers[i] == -1)
        {
            blockNumbers[i] = blockNumber;
            return true;
        }
    }
    return false;
}

INodeBlock::INodeBlock()
{
    constexpr int n = numINodesInBlock;
    for (int i = 0; i < n; ++i)
    {
        INode inode;
        SetINode(inode, i);
    }
}

void INodeBlock::Read(Block* block)
{
    constexpr int n = numINodesInBlock;
    for (int i = 0; i < n; ++i)
    {
        inodes[i].Read(block, i);
    }
}

void INodeBlock::Write(Block* block)
{
    constexpr int n = numINodesInBlock;
    for (int i = 0; i < n; ++i)
    {
        inodes[i].Write(block, i);
    }
    block->SetDirty();
}

int INodeBlock::FirstFreeINodeIndex() const
{
    constexpr int n = numINodesInBlock;
    for (int i = 0; i < n; ++i)
    {
        if (inodes[i].IsFree())
        {
            return i;
        }
    }
    return -1;
}

DirectoryEntry::DirectoryEntry() : name(), inodeNumber(-1)
{
}

void DirectoryEntry::Read(Block* block, int index)
{
    MemoryReader reader(block->Data() + Size() * index, Size());
    inodeNumber = reader.ReadInt();
    uint8_t x = reader.ReadByte();
    while (x)
    {
        name.append(1, static_cast<char>(x));
        x = reader.ReadByte();
    }
}

void DirectoryEntry::Write(Block* block, int index)
{
    MemoryWriter writer(block->Data() + Size() * index, Size());
    writer.Write(inodeNumber);
    for (char c : name)
    {
        uint8_t x = static_cast<uint8_t>(c);
        writer.Write(x);
    }
    writer.Write(static_cast<uint8_t>(0));
    block->SetDirty();
}

void DirectoryEntry::SetName(const std::string& name_)
{
    if (name_.length() > MaxNameLength())
    {
        throw SystemError(EFAIL, "name too long");
    }
    name = name_;
}

void DirectoryEntry::SetNameTruncate(const std::string& name_)
{
    name = name_.substr(0, MaxNameLength());
}

DirectoryBlock::DirectoryBlock()
{
}

void DirectoryBlock::Read(Block* block)
{
    constexpr int n = NumDirectoryEntriesInBlock();
    for (int i = 0; i < n; ++i)
    {
        DirectoryEntry entry;
        entry.Read(block, i);
        if (entry.INodeNumber() != -1)
        {
            entries.push_back(entry);
        }
    }
}

void DirectoryBlock::Write(Block* block)
{
    constexpr int n = NumDirectoryEntriesInBlock();
    int ne = entries.size();
    for (int i = 0; i < ne; ++i)
    {
        entries[i].Write(block, i);
    }
    for (int i = ne; i < n; ++i)
    {
        DirectoryEntry entry;
        entry.Write(block, i);
    }
    block->SetDirty();
}

void DirectoryBlock::AddEntry(const DirectoryEntry& entry)
{
    if (IsFull())
    {
        throw SystemError(EFAIL, "too many directory entries in directory block");
    }
    entries.push_back(entry);
}

void DirectoryBlock::RemoveEntry(int index)
{
    entries.erase(entries.begin() + index);
}

Filesystem* GetFs(int32_t fsNumber)
{
    MountTable& mountTable = Kernel::Instance().GetMountTable();
    return mountTable.GetFilesystem(fsNumber);
}

uint64_t ToULong(const INodeKey& key)
{
    return (static_cast<uint64_t>(key.fsNumber) << 32) | static_cast<uint64_t>(key.inodeNumber);
}

INodeKey ToINodeKey(uint64_t key)
{
    int32_t fsNumber = (key >> 32);
    int32_t inodeNumber = key & 0xFFFFFFFF;
    return MakeINodeKey(fsNumber, inodeNumber);
}

void ReadSuperBlock(SuperBlock& superBlock, Filesystem* fs, cmsx::machine::Process* process)
{
    BlockPtr sb = GetBlock(SuperBlockKey(fs->Id()), process);
    if (!sb.Get()->IsValid())
    {
        BlockFile* hostFile = fs->HostFile();
        hostFile->Read(sb.Get(), process);
    }
    superBlock.Read(sb.Get());
}

void WriteSuperBlock(SuperBlock& superBlock, Filesystem* fs, cmsx::machine::Process* process)
{
    BlockFile* hostFile = fs->HostFile();
    BlockPtr sb = GetBlock(SuperBlockKey(fs->Id()), process);
    superBlock.Write(sb.Get());
    hostFile->Write(sb.Get(), process);
    sb.Get()->SetValid();
}

BlockPtr ReadBlockNumberBlock(BlockNumberBlock& blockNumberBlock, int32_t blockNumberBlockNumber, Filesystem* fs, cmsx::machine::Process* process)
{
    BlockPtr bnb = GetBlock(MakeBlockKey(fs->Id(), blockNumberBlockNumber), process);
    if (!bnb.Get()->IsValid())
    {
        BlockFile* hostFile = fs->HostFile();
        hostFile->Read(bnb.Get(), process);
    }
    blockNumberBlock.Read(bnb.Get());
    return bnb;
}

void WriteBlockNumberBlock(BlockNumberBlock& blockNumberBlock, BlockPtr& bnb, Filesystem* fs, cmsx::machine::Process* process)
{
    BlockFile* hostFile = fs->HostFile();
    blockNumberBlock.Write(bnb.Get());
    hostFile->Write(bnb.Get(), process);
    bnb.Get()->SetValid();
}

BlockPtr ReadINodeBlock(INodeBlock& inodeBlock, int32_t inodeBlockNumber, Filesystem* fs, cmsx::machine::Process* process)
{
    BlockPtr inb = GetBlock(MakeBlockKey(fs->Id(), inodeBlockNumber), process);
    if (!inb.Get()->IsValid())
    {
        BlockFile* hostFile = fs->HostFile();
        hostFile->Read(inb.Get(), process);
    }
    inodeBlock.Read(inb.Get());
    return inb;
}

void WriteINodeBlock(INodeBlock& inodeBlock, BlockPtr& inb, Filesystem* fs, cmsx::machine::Process* process)
{
    BlockFile* hostFile = fs->HostFile();
    inodeBlock.Write(inb.Get());
    hostFile->Write(inb.Get(), process);
    inb.Get()->SetValid();
}

BlockPtr ReadDirectoryBlock(DirectoryBlock& directoryBlock, int32_t directoryBlockNumber, Filesystem* fs, cmsx::machine::Process* process)
{
    BlockPtr db = GetBlock(MakeBlockKey(fs->Id(), directoryBlockNumber), process);
    if (!db.Get()->IsValid())
    {
        BlockFile* hostFile = fs->HostFile();
        hostFile->Read(db.Get(), process);
    }
    directoryBlock.Read(db.Get());
    return db;
}

void WriteDirectoryBlock(DirectoryBlock& directoryBlock, BlockPtr& db, Filesystem* fs, cmsx::machine::Process* process)
{
    BlockFile* hostFile = fs->HostFile();
    directoryBlock.Write(db.Get());
    hostFile->Write(db.Get(), process);
    db.Get()->SetValid();
}

int32_t GetFreeBlockNumber(int32_t fsNumber, cmsx::machine::Process* process)
{
    Filesystem* fs = GetFs(fsNumber);
    SuperBlock superBlock;
    ReadSuperBlock(superBlock, fs, process);
    int32_t blockNumberBlockNumber = superBlock.CurrentBlockNumberBlockNumber();
    int32_t startBlockNumberBlockNumber = blockNumberBlockNumber;
    do
    {
        BlockNumberBlock blockNumberBlock;
        BlockPtr bnb = ReadBlockNumberBlock(blockNumberBlock, blockNumberBlockNumber, fs, process);
        int32_t blockNumber = blockNumberBlock.GetFreeBlockNumber();
        if (blockNumber != -1)
        {
            WriteBlockNumberBlock(blockNumberBlock, bnb, fs, process);
            return blockNumber;
        }
        ++blockNumberBlockNumber;
        if (blockNumberBlockNumber > superBlock.LastBlockNumberBlockNumber())
        {
            blockNumberBlockNumber = superBlock.FirstBlockNumberBlockNumber();
        }
        superBlock.SetCurrentBlockNumberBlockNumber(blockNumberBlockNumber);
        WriteSuperBlock(superBlock, fs, process);
    }
    while (blockNumberBlockNumber != startBlockNumberBlockNumber);
    throw SystemError(EFAIL, "out of free blocks in filesystem " + std::to_string(fsNumber));
}

void PutFreeBlockNumber(int32_t fsNumber, int32_t freeBlockNumber, cmsx::machine::Process* process)
{
    Filesystem* fs = GetFs(fsNumber);
    SuperBlock superBlock;
    ReadSuperBlock(superBlock, fs, process);
    int32_t blockNumberBlockNumber = superBlock.CurrentBlockNumberBlockNumber();
    int32_t startBlockNumberBlockNumber = blockNumberBlockNumber;
    do
    {
        BlockNumberBlock blockNumberBlock;
        BlockPtr bnb = ReadBlockNumberBlock(blockNumberBlock, blockNumberBlockNumber, fs, process);
        if (blockNumberBlock.PutFreeBlockNumber(freeBlockNumber))
        {
            WriteBlockNumberBlock(blockNumberBlock, bnb, fs, process);
            return;
        }
        --blockNumberBlockNumber;
        if (blockNumberBlockNumber < superBlock.FirstBlockNumberBlockNumber())
        {
            blockNumberBlockNumber = superBlock.LastBlockNumberBlockNumber();
        }
        superBlock.SetCurrentBlockNumberBlockNumber(blockNumberBlockNumber);
        WriteSuperBlock(superBlock, fs, process);
    }
    while (blockNumberBlockNumber != startBlockNumberBlockNumber);
    throw SystemError(EFAIL, "free block number list in filesystem " + std::to_string(fsNumber) + " is full");
}

INodePtr AllocateINode(int32_t fsNumber, cmsx::machine::Process* process)
{
    Filesystem* fs = GetFs(fsNumber);
    SuperBlock superBlock;
    ReadSuperBlock(superBlock, fs, process);
    if (superBlock.CurrentNumberOfFiles() == superBlock.NumINodes())
    {
        throw SystemError(EFAIL, "filesystem " + std::to_string(fsNumber) + " is out of inodes (maximum number of files is " + std::to_string(superBlock.NumINodes()) + ")");
    }
    int32_t currentINodeBlockNumber = superBlock.CurrentINodeBlockNumber();
    int32_t startINodeBlockNumber = currentINodeBlockNumber;
    do 
    {
        INodeBlock inodeBlock;
        BlockPtr inb = ReadINodeBlock(inodeBlock, currentINodeBlockNumber, fs, process);
        int32_t freeINodeIndex = inodeBlock.FirstFreeINodeIndex();
        if (freeINodeIndex != -1)
        {
            int32_t freeINodeNumber = superBlock.MakeINodeNumber(currentINodeBlockNumber, freeINodeIndex);
            superBlock.IncrementNumberOfFiles();
            WriteSuperBlock(superBlock, fs, process);
            INodePtr inode = GetINode(MakeINodeKey(fsNumber, freeINodeNumber), process);
            return inode;
        }
        else if (currentINodeBlockNumber == superBlock.LastINodeBlockNumber())
        {
            currentINodeBlockNumber = superBlock.FirstINodeBlockNumber();
            superBlock.SetCurrentINodeBlockNumber(currentINodeBlockNumber);
            WriteSuperBlock(superBlock, fs, process);
        }
        else
        {
            ++currentINodeBlockNumber;
            superBlock.SetCurrentINodeBlockNumber(currentINodeBlockNumber);
            WriteSuperBlock(superBlock, fs, process);
        }
    } 
    while (currentINodeBlockNumber != startINodeBlockNumber);
    throw SystemError(EFAIL, "filesystem " + std::to_string(fsNumber) + " is out of inodes (maximum number of files is " + std::to_string(superBlock.NumINodes()) + ")");
}

void FreeINode(INode* inode, cmsx::machine::Process* process)
{
    inode->SetFree();
    Filesystem* fs = GetFs(inode->Key().fsNumber);
    SuperBlock superBlock;
    ReadSuperBlock(superBlock, fs, process);
    superBlock.DecrementNumberOfFiles();
    WriteSuperBlock(superBlock, fs, process);
    INodeBlock inodeBlock;
    BlockPtr inb = ReadINodeBlock(inodeBlock, superBlock.GetINodeBlockNumber(inode->Key().inodeNumber), fs, process);
    inodeBlock.SetINode(*inode, superBlock.GetINodeIndexInBlock(inode->Key().inodeNumber));
    WriteINodeBlock(inodeBlock, inb, fs, process);
}

INodePtr ReadINode(const INodeKey& inodeKey, cmsx::machine::Process* process)
{
    INodePtr inodePtr = GetINode(inodeKey, process);
    if (!inodePtr.Get()->IsValid())
    {
        Filesystem* fs = GetFs(inodeKey.fsNumber);
        SuperBlock superBlock;
        ReadSuperBlock(superBlock, fs, process);
        int32_t inodeBlockNumber = superBlock.GetINodeBlockNumber(inodeKey.inodeNumber);
        INodeBlock inodeBlock;
        BlockPtr inb = ReadINodeBlock(inodeBlock, inodeBlockNumber, fs, process);
        int32_t inodeIndex = superBlock.GetINodeIndexInBlock(inodeKey.inodeNumber);
        inodePtr.SetINode(inodeBlock.GetINode(inodeIndex));
        inodePtr.Get()->SetKey(inodeKey);
        inodePtr.Get()->SetValid();
    }
    return inodePtr;
}

void WriteINode(INode* inode, cmsx::machine::Process* process)
{
    Filesystem* fs = GetFs(inode->Key().fsNumber);
    if (fs->Id() != rootFSNumber) return;
    SuperBlock superBlock;
    ReadSuperBlock(superBlock, fs, process);
    int32_t inodeBlockNumber = superBlock.GetINodeBlockNumber(inode->Key().inodeNumber);
    INodeBlock inodeBlock;
    BlockPtr inb = ReadINodeBlock(inodeBlock, inodeBlockNumber, fs, process);
    int32_t inodeIndex = superBlock.GetINodeIndexInBlock(inode->Key().inodeNumber);
    inode->SetValid();
    inodeBlock.SetINode(*inode, inodeIndex);
    WriteINodeBlock(inodeBlock, inb, fs, process);
}

BlockPtr AllocateBlock(int32_t fsNumber, cmsx::machine::Process* process)
{
    BlockPtr block = GetBlock(MakeBlockKey(fsNumber, GetFreeBlockNumber(fsNumber, process)), process);
    return block;
}

void FreeBlock(Block* block, cmsx::machine::Process* process)
{
    PutFreeBlockNumber(block->Key().fsNumber, block->Key().blockNumber, process);
}

void FreeBlocks(INode* inode, Filesystem* fs, cmsx::machine::Process* process)
{
    constexpr int32_t numDirectBlocks = INode::NumDirectBlockNumbers();
    for (int32_t i = 0; i < numDirectBlocks; ++i)
    {
        int32_t directBlockNumber = inode->GetDirectBlockNumber(i);
        if (directBlockNumber != -1)
        {
            PutFreeBlockNumber(fs->Id(), directBlockNumber, process);
            inode->SetDirectBlockNumber(-1, i);
        }
    }
    int32_t singleIndirectBlockNumber = inode->SingleIndirectBlockNumber();
    if (singleIndirectBlockNumber != -1)
    {
        BlockNumberBlock blockNumberBlock;
        BlockPtr sib = ReadBlockNumberBlock(blockNumberBlock, singleIndirectBlockNumber, fs, process);
        constexpr int32_t n = BlockNumberBlock::BlockNumberCount();
        for (int32_t i = 0; i < n; ++i)
        {
            int32_t blockNumber = blockNumberBlock.GetBlockNumber(i);
            if (blockNumber != -1)
            {
                PutFreeBlockNumber(fs->Id(), blockNumber, process);
                blockNumberBlock.SetBlockNumber(-1, i);
            }
        }
        PutFreeBlockNumber(fs->Id(), singleIndirectBlockNumber, process);
        inode->SetSingleIndirectBlockNumber(-1);
    }
    int32_t doubleIndirectBlockNumber = inode->DoubleIndirectBlockNumber();
    if (doubleIndirectBlockNumber != -1)
    {
        BlockNumberBlock blockNumberBlock;
        BlockPtr dib = ReadBlockNumberBlock(blockNumberBlock, doubleIndirectBlockNumber, fs, process);
        constexpr int32_t n = BlockNumberBlock::BlockNumberCount();
        for (int32_t i = 0; i < n; ++i)
        {
            int32_t singleIndirectBlockNumber = blockNumberBlock.GetBlockNumber(i);
            if (singleIndirectBlockNumber != -1)
            {
                BlockNumberBlock blockNumberBlock;
                BlockPtr sib = ReadBlockNumberBlock(blockNumberBlock, singleIndirectBlockNumber, fs, process);
                constexpr int32_t n = BlockNumberBlock::BlockNumberCount();
                for (int32_t i = 0; i < n; ++i)
                {
                    int32_t blockNumber = blockNumberBlock.GetBlockNumber(i);
                    if (blockNumber != -1)
                    {
                        PutFreeBlockNumber(fs->Id(), blockNumber, process);
                        blockNumberBlock.SetBlockNumber(-1, i);
                    }
                }
                PutFreeBlockNumber(fs->Id(), singleIndirectBlockNumber, process);
            }
            blockNumberBlock.SetBlockNumber(-1, i);
        }
        PutFreeBlockNumber(fs->Id(), doubleIndirectBlockNumber, process);
        inode->SetDoubleIndirectBlockNumber(-1);
    }
    WriteINode(inode, process);
}

int32_t MapBlockNumber(int32_t logicalBlockNumber, INode* inode, Filesystem* fs, cmsx::machine::Process* process)
{
    if (logicalBlockNumber < 0)
    {
        return -1;
    }
    if (logicalBlockNumber >= 0 && logicalBlockNumber < INode::NumDirectBlockNumbers())
    {
        return inode->GetDirectBlockNumber(logicalBlockNumber);
    }
    logicalBlockNumber -= INode::NumDirectBlockNumbers();
    if (logicalBlockNumber >= 0 && logicalBlockNumber < BlockNumberBlock::BlockNumberCount())
    {
        if (inode->SingleIndirectBlockNumber() == -1)
        {
            return -1;
        }
        BlockNumberBlock blockNumberBlock;
        BlockPtr sib = ReadBlockNumberBlock(blockNumberBlock, inode->SingleIndirectBlockNumber(), fs, process);
        return blockNumberBlock.GetBlockNumber(logicalBlockNumber);
    }
    if (inode->DoubleIndirectBlockNumber() == -1)
    {
        return -1;
    }
    logicalBlockNumber -= BlockNumberBlock::BlockNumberCount();
    int32_t n = logicalBlockNumber / BlockNumberBlock::BlockNumberCount();
    BlockNumberBlock blockNumberBlock;
    BlockPtr dib = ReadBlockNumberBlock(blockNumberBlock, inode->DoubleIndirectBlockNumber(), fs, process);
    int32_t sibn = blockNumberBlock.GetBlockNumber(n);
    if (sibn == -1)
    {
        return -1;
    }
    logicalBlockNumber = logicalBlockNumber % BlockNumberBlock::BlockNumberCount();
    BlockPtr sib = ReadBlockNumberBlock(blockNumberBlock, sibn, fs, process);
    return blockNumberBlock.GetBlockNumber(logicalBlockNumber);
}

void SetBlockNumber(int32_t logicalBlockNumber, int32_t blockNumber, INode* inode, Filesystem* fs, cmsx::machine::Process* process)
{
    if (logicalBlockNumber >= 0 && logicalBlockNumber < INode::NumDirectBlockNumbers())
    {
        inode->SetDirectBlockNumber(blockNumber, logicalBlockNumber);
    }
    else
    {
        logicalBlockNumber -= INode::NumDirectBlockNumbers();
        if (logicalBlockNumber >= 0 && logicalBlockNumber < BlockNumberBlock::BlockNumberCount())
        {
            BlockNumberBlock blockNumberBlock;
            BlockPtr sibn(nullptr);
            if (inode->SingleIndirectBlockNumber() == -1)
            {
                sibn = AllocateBlock(fs->Id(), process);
                inode->SetSingleIndirectBlockNumber(sibn.Get()->Key().blockNumber);
            }
            else
            {
                sibn = ReadBlockNumberBlock(blockNumberBlock, inode->SingleIndirectBlockNumber(), fs, process);
            }
            blockNumberBlock.SetBlockNumber(blockNumber, logicalBlockNumber);
            WriteBlockNumberBlock(blockNumberBlock, sibn, fs, process);
        }
        else
        {
            logicalBlockNumber -= BlockNumberBlock::BlockNumberCount();
            BlockNumberBlock blockNumberBlock;
            BlockPtr dib(nullptr);
            if (inode->DoubleIndirectBlockNumber() == -1)
            {
                dib = AllocateBlock(fs->Id(), process);
                inode->SetDoubleIndirectBlockNumber(dib.Get()->Key().blockNumber);
            }
            else
            {
                dib = ReadBlockNumberBlock(blockNumberBlock, inode->DoubleIndirectBlockNumber(), fs, process);
            }
            int32_t n = logicalBlockNumber / BlockNumberBlock::BlockNumberCount();
            int32_t sibn = blockNumberBlock.GetBlockNumber(n);
            BlockPtr sib(nullptr);
            if (sibn == -1)
            {
                sib = AllocateBlock(fs->Id(), process);
                sibn = sib.Get()->Key().blockNumber;
                blockNumberBlock.SetBlockNumber(sibn, n);
                WriteBlockNumberBlock(blockNumberBlock, dib, fs, process);
                blockNumberBlock = BlockNumberBlock();
            }
            else
            {
                sib = ReadBlockNumberBlock(blockNumberBlock, sibn, fs, process);
            }
            logicalBlockNumber = logicalBlockNumber % BlockNumberBlock::BlockNumberCount();
            blockNumberBlock.SetBlockNumber(blockNumber, logicalBlockNumber);
            WriteBlockNumberBlock(blockNumberBlock, sib, fs, process);
        }
    }
    WriteINode(inode, process);
}

INodePtr SearchDirectory(const std::string& name, INode* dirINode, const std::string& dirPath, Filesystem* fs, cmsx::machine::Process* process)
{
    if (dirINode->GetFileType() != FileType::directory)
    {
        throw SystemError(EFAIL, "not a directory inode");
    }
    CheckAccess(Access::execute, process->EUID(), process->EGID(), dirINode, "could not search directory '" + dirPath + "'");
    int32_t logicalBlockNumber = 0;
    int32_t blockNumber = MapBlockNumber(logicalBlockNumber, dirINode, fs, process);
    while (blockNumber != -1)
    {
        DirectoryBlock directoryBlock;
        BlockPtr blockPtr = ReadDirectoryBlock(directoryBlock, blockNumber, fs, process);
        int32_t n = directoryBlock.Entries().size();
        for (int32_t i = 0; i < n; ++i)
        {
            DirectoryEntry directoryEntry;
            directoryEntry.Read(blockPtr.Get(), i);
            int32_t inodeNumber = directoryEntry.INodeNumber();
            if (inodeNumber != -1)
            {
                if (directoryEntry.Name() == name)
                {
                    return ReadINode(MakeINodeKey(fs->Id(), inodeNumber), process);
                }
            }
        }
        ++logicalBlockNumber;
        blockNumber = MapBlockNumber(logicalBlockNumber, dirINode, fs, process);
    }
    return INodePtr(nullptr);
}

DirectoryEntry GetDirectoryEntry(INode* dirINode, int32_t inodeNumber, Filesystem* fs, cmsx::machine::Process* process)
{
    if (dirINode->GetFileType() != FileType::directory)
    {
        throw SystemError(EFAIL, "not a directory inode");
    }
    int32_t logicalBlockNumber = 0;
    int32_t blockNumber = MapBlockNumber(logicalBlockNumber, dirINode, fs, process);
    while (blockNumber != -1)
    {
        DirectoryBlock directoryBlock;
        BlockPtr blockPtr = ReadDirectoryBlock(directoryBlock, blockNumber, fs, process);
        int32_t n = directoryBlock.Entries().size();
        for (int32_t i = 0; i < n; ++i)
        {
            DirectoryEntry directoryEntry;
            directoryEntry.Read(blockPtr.Get(), i);
            if (inodeNumber == directoryEntry.INodeNumber())
            {
                return directoryEntry;
            }
        }
        ++logicalBlockNumber;
        blockNumber = MapBlockNumber(logicalBlockNumber, dirINode, fs, process);
    }
    return DirectoryEntry();
}

void SetDirectoryChanged(INodeKey dirINodeKey)
{
    std::vector<int32_t>* changeNotificationPIDs = GetDirChangeNotificationPIDs(dirINodeKey);
    if (changeNotificationPIDs)
    {
        for (int32_t pid : *changeNotificationPIDs)
        {
            Process* process = ProcessManager::Instance().GetProcess(pid);
            if (process)
            {
                process->SetDirectoriesChanged();
            }
        }
    }
}

void AddDirectoryEntry(const DirectoryEntry& entry, INode* dirINode, Filesystem* fs, cmsx::machine::Process* process)
{
    if (dirINode->GetFileType() != FileType::directory)
    {
        throw SystemError(EFAIL, "not a directory inode");
    }
    int32_t logicalBlockNumber = 0;
    int32_t blockNumber = MapBlockNumber(logicalBlockNumber, dirINode, fs, process);
    while (blockNumber != -1)
    {
        DirectoryBlock directoryBlock;
        BlockPtr blockPtr = ReadDirectoryBlock(directoryBlock, blockNumber, fs, process);
        if (!directoryBlock.IsFull())
        {
            directoryBlock.AddEntry(entry);
            WriteDirectoryBlock(directoryBlock, blockPtr, fs, process);
            dirINode->SetFileSize(dirINode->FileSize() + DirectoryEntry::Size());
            dirINode->SetMTime(GetCurrentDateTime());
            dirINode->SetCTime(GetCurrentDateTime());
            SetDirectoryChanged(dirINode->Key());
            WriteINode(dirINode, process);
            return;
        }
        ++logicalBlockNumber;
        blockNumber = MapBlockNumber(logicalBlockNumber, dirINode, fs, process);
    }
    BlockPtr blockPtr = AllocateBlock(fs->Id(), process);
    SetBlockNumber(logicalBlockNumber, blockPtr.Get()->Key().blockNumber, dirINode, fs, process);
    DirectoryBlock directoryBlock;
    directoryBlock.AddEntry(entry);
    WriteDirectoryBlock(directoryBlock, blockPtr, fs, process);
    dirINode->SetFileSize(dirINode->FileSize() + DirectoryEntry::Size());
    dirINode->SetMTime(GetCurrentDateTime());
    dirINode->SetCTime(GetCurrentDateTime());
    SetDirectoryChanged(dirINode->Key());
    WriteINode(dirINode, process);
}

void RemoveDirectoryEntry(const std::string& name, const std::string& filePath, INode* dirINode, Filesystem* fs, cmsx::machine::Process* process)
{
    RemoveDirectoryEntry(name, filePath, dirINode, fs, process, false);
}

void RemoveDirectoryEntry(const std::string& name, const std::string& filePath, INode* dirINode, Filesystem* fs, cmsx::machine::Process* process, bool unlink)
{
    if (dirINode->GetFileType() != FileType::directory)
    {
        throw SystemError(EFAIL, "not a directory");
    }
    int32_t logicalBlockNumber = 0;
    int32_t blockNumber = MapBlockNumber(logicalBlockNumber, dirINode, fs, process);
    while (blockNumber != -1)
    {
        DirectoryBlock directoryBlock;
        BlockPtr blockPtr = ReadDirectoryBlock(directoryBlock, blockNumber, fs, process);
        int32_t n = directoryBlock.Entries().size();
        for (int32_t i = 0; i < n; ++i)
        {
            DirectoryEntry directoryEntry;
            directoryEntry.Read(blockPtr.Get(), i);
            int32_t inodeNumber = directoryEntry.INodeNumber();
            if (inodeNumber != -1)
            {
                if (directoryEntry.Name() == name)
                {
                    if (unlink)
                    {
                        INodePtr inodePtr = ReadINode(MakeINodeKey(fs->Id(), inodeNumber), process);
                        INode* inode = inodePtr.Get();
                        CheckAccess(Access::write, process->EUID(), process->EGID(), inode, "could not unlink '" + filePath + "'");
                        int32_t nlinks = inode->NLinks();
                        --nlinks;
                        inode->SetNLinks(nlinks);
                        dirINode->SetFileSize(dirINode->FileSize() - DirectoryEntry::Size());
                        dirINode->SetCTime(GetCurrentDateTime());
                        dirINode->SetMTime(GetCurrentDateTime());
                        SetDirectoryChanged(dirINode->Key());
                        if (nlinks == 0 && inode->ReferenceCount() == 0)
                        {
                            FreeBlocks(inode, fs, process);
                        }
                        WriteINode(inode, process);
                    }
                    directoryBlock.RemoveEntry(i);
                    WriteDirectoryBlock(directoryBlock, blockPtr, fs, process);
                    return;
                }
            }
        }
        ++logicalBlockNumber;
        blockNumber = MapBlockNumber(logicalBlockNumber, dirINode, fs, process);
    }
    throw SystemError(EFAIL, "could not remove: name '" + name + "' not found");
}

void RenameDirectoryEntry(INode* dirINode, const std::string& oldName, const std::string& newName, Filesystem* fs, cmsx::machine::Process* process)
{
    if (dirINode->GetFileType() != FileType::directory)
    {
        throw SystemError(EFAIL, "not a directory");
    }
    int32_t logicalBlockNumber = 0;
    int32_t blockNumber = MapBlockNumber(logicalBlockNumber, dirINode, fs, process);
    while (blockNumber != -1)
    {
        DirectoryBlock directoryBlock;
        BlockPtr blockPtr = ReadDirectoryBlock(directoryBlock, blockNumber, fs, process);
        int32_t n = directoryBlock.Entries().size();
        for (int32_t i = 0; i < n; ++i)
        {
            DirectoryEntry directoryEntry;
            directoryEntry.Read(blockPtr.Get(), i);
            int32_t inodeNumber = directoryEntry.INodeNumber();
            if (inodeNumber != -1)
            {
                if (directoryEntry.Name() == oldName)
                {
                    directoryEntry.SetName(newName);
                    directoryBlock.RemoveEntry(i);
                    directoryBlock.AddEntry(directoryEntry);
                    WriteDirectoryBlock(directoryBlock, blockPtr, fs, process);
                    dirINode->SetMTime(GetCurrentDateTime());
                    dirINode->SetCTime(GetCurrentDateTime());
                    SetDirectoryChanged(dirINode->Key());
                    return;
                }
            }
        }
        ++logicalBlockNumber;
        blockNumber = MapBlockNumber(logicalBlockNumber, dirINode, fs, process);
    }
    throw SystemError(EFAIL, "could not remove: name '" + oldName + "' not found");
}

INodePtr MakeDirectory(const std::string& path, Filesystem* fs, cmsx::machine::Process* process, int32_t mode)
{
    std::string parentDirectoryPath = Path::GetDirectoryName(path);
    if (parentDirectoryPath.empty())
    {
        if (path.starts_with("/"))
        {
            parentDirectoryPath = "/";
        }
        else
        {
            parentDirectoryPath = ".";
        }
    }
    INodePtr parentDirINode = PathToINode(parentDirectoryPath, fs, process);
    if (!parentDirINode.Get())
    {
        throw SystemError(ENOTFOUND, "could not create directory: parent directory '" + parentDirectoryPath + "' does not exist");
    }
    INodePtr dirINodePtr = AllocateINode(fs->Id(), process);
    INode* dirINode = dirINodePtr.Get();
    dirINode->SetFileType(FileType::directory);
    dirINode->SetMode(AlterMode(mode, process->UMask(), true));
    BlockPtr dirBlockPtr = AllocateBlock(fs->Id(), process);
    dirINode->SetDirectBlockNumber(dirBlockPtr.Get()->Key().blockNumber, 0);
    DirectoryBlock dirBlock; 
    DirectoryEntry thisEntry;
    thisEntry.SetINodeNumber(dirINode->Key().inodeNumber);
    thisEntry.SetName(".");
    dirBlock.AddEntry(thisEntry);
    DirectoryEntry parentEntry;
    parentEntry.SetINodeNumber(parentDirINode.Get()->Key().inodeNumber);
    parentEntry.SetName("..");
    dirBlock.AddEntry(parentEntry);
    dirINode->SetFileSize(2 * DirectoryEntry::Size());
    dirINode->SetCTime(GetCurrentDateTime());
    dirINode->SetMTime(GetCurrentDateTime());
    WriteDirectoryBlock(dirBlock, dirBlockPtr, fs, process);
    WriteINode(dirINode, process);
    DirectoryEntry entry;
    entry.SetName(Path::GetFileName(path));
    entry.SetINodeNumber(dirINode->Key().inodeNumber);
    AddDirectoryEntry(entry, parentDirINode.Get(), fs, process);
    return dirINodePtr;
}

bool DirectoryExists(const std::string& path, Filesystem* fs, cmsx::machine::Process* process)
{
    INodePtr inode = PathToINode(path, fs, process);
    if (!inode.Get())
    {
        return false;
    }
    if (inode.Get()->GetFileType() == FileType::directory)
    {
        return true;
    }
    return false;
}

std::string GetHostFilePath(const INodeKey& key, cmsx::machine::Process* process)
{
    int32_t fsNumber = key.fsNumber;
    Filesystem* fs = GetFs(fsNumber);
    return fs->GetHostFilePath(key.inodeNumber, process);
}

INodePtr PathToINode(const std::string& path, Filesystem* fs, cmsx::machine::Process* process)
{
    return PathToINode(path, fs, process, PathToINodeFlags::none);
}

INodePtr PathToINode(const std::string& path, Filesystem* fs, cmsx::machine::Process* process, PathToINodeFlags flags)
{
    Filesystem* rootFs = GetFs(rootFSNumber);
    SuperBlock superBlock;
    ReadSuperBlock(superBlock, rootFs, process);
    if (path.empty() || path == "/")
    {
        INodeKey rootDirINodeKey = MakeINodeKey(rootFSNumber, superBlock.INodeNumberOfRootDirectory());
        INodePtr inode = rootFs->ReadINode(rootDirINodeKey, process);
        return inode;
    }
    std::string dirPath;
    INodeKey inodeKey;
    int start = 0;
    if (path.starts_with('/'))
    {
        inodeKey = MakeINodeKey(rootFSNumber, superBlock.INodeNumberOfRootDirectory());
        start = 1;
        dirPath.append(1, '/');
    }
    else
    {
        inodeKey = ToINodeKey(process->GetINodeKeyOfWorkingDir());
    }
    fs = GetFs(inodeKey.fsNumber);
    INodePtr inode = fs->ReadINode(inodeKey, process);
    std::vector<std::string> components = Split(path, '/');
    for (int i = start; i < components.size(); ++i)
    {
        const std::string& name = components[i];
        fs = GetFs(inode.Get()->Key().fsNumber);
        inode = fs->SearchDirectory(name, inode.Get(), dirPath, process);
        if (!inode.Get())
        {
            return INodePtr(nullptr);
        }
        if (inode.Get()->IsMountPoint() && ((flags & PathToINodeFlags::ignoreMountPoint) == PathToINodeFlags::none))
        {
            MountTable& mountTable = Kernel::Instance().GetMountTable();
            fs = mountTable.GetMountedFilesystem(inode.Get()->Key());
            if (fs)
            {
                inode = fs->SearchDirectory(".", inode.Get(), dirPath, process);
                if (!inode.Get())
                {
                    return INodePtr(nullptr);
                }
            }
            else
            {
                return INodePtr(nullptr);
            }
        }
        dirPath = Path::Combine(dirPath, name);
    }
    if (inode.Get() && (flags & PathToINodeFlags::stat) != PathToINodeFlags::none)
    {
        Filesystem* fs = GetFs(inode.Get()->Key().fsNumber);
        fs->Stat(inode.Get(), process);
    }
    return inode;
}

} // namespace cmsx::kernel
