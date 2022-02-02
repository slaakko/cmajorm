// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_INODE_MANAGER_INCLUDED
#define CMSX_KERNEL_INODE_MANAGER_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <system-x/machine/Machine.hpp>
#include <soulng/util/MemoryWriter.hpp>
#include <soulng/util/Time.hpp>
#include <stdint.h>
#include <algorithm>

namespace cmsx::kernel {

using namespace soulng::util;

class Block;
class Process;
class BlockFile;

enum class FileType : int32_t
{
    free = 0, regular = 1, directory = 2, fifo = 3
};
    
enum class Access : int32_t
{
    none = 0, read = 1 << 0, write = 1 << 1, execute = 1 << 2
};

CMSX_KERNEL_API inline Access operator|(Access left, Access right)
{
    return Access(int32_t(left) | int32_t(right));
}

CMSX_KERNEL_API inline Access operator&(Access left, Access right)
{
    return Access(int32_t(left) & int32_t(right));
}

CMSX_KERNEL_API inline Access operator~(Access access)
{
    return Access(~int32_t(access));
}

enum class INodeFlags : int32_t
{
    none = 0, mountPoint = 1 << 0, locked = 1 << 1, valid = 1 << 2
};

CMSX_KERNEL_API inline INodeFlags operator|(INodeFlags left, INodeFlags right)
{
    return INodeFlags(int32_t(left) | int32_t(right));
}

CMSX_KERNEL_API inline INodeFlags operator&(INodeFlags left, INodeFlags right)
{
    return INodeFlags(int32_t(left) & int32_t(right));
}

CMSX_KERNEL_API inline INodeFlags operator~(INodeFlags flags)
{
    return INodeFlags(~int32_t(flags));
}

CMSX_KERNEL_API inline bool GetFlag(INodeFlags flags, INodeFlags flag) { return (flags & flag) != INodeFlags::none; }
CMSX_KERNEL_API inline void SetFlag(INodeFlags& flags, INodeFlags flag) { flags = flags | flag; }
CMSX_KERNEL_API inline void ResetFlag(INodeFlags& flags, INodeFlags flag) { flags = flags & ~flag; }

CMSX_KERNEL_API int32_t EncodeMode(FileType fileType, INodeFlags flags, Access ownerAccess, Access groupAccess, Access otherAccess);
CMSX_KERNEL_API void DecodeMode(int32_t mode, FileType& fileType, INodeFlags& flags, Access& ownerAccess, Access& groupAccess, Access& otherAccess);

struct CMSX_KERNEL_API INodeKey
{
    INodeKey() : fsNumber(-1), inodeNumber(-1) {}
    INodeKey(int32_t fsNumber_, int32_t inodeNumber_) : fsNumber(fsNumber_), inodeNumber(inodeNumber_) {}
    bool IsDefault() const { return fsNumber == -1 && inodeNumber == -1; }
    int32_t fsNumber;
    int32_t inodeNumber;
};

CMSX_KERNEL_API inline bool operator==(const INodeKey& left, const INodeKey& right)
{
    return left.fsNumber == right.fsNumber && left.inodeNumber == right.inodeNumber;
}

CMSX_KERNEL_API inline bool operator!=(const INodeKey& left, const INodeKey& right)
{
    return !(left == right);
}

CMSX_KERNEL_API inline bool operator<(INodeKey left, INodeKey right)
{
    if (left.fsNumber < right.fsNumber) return true;
    if (left.fsNumber > right.fsNumber) return false;
    return left.inodeNumber < right.inodeNumber;
}

CMSX_KERNEL_API inline INodeKey RootDirectoryINodeKey(int32_t fsNumber)
{
    return INodeKey(fsNumber, 0);
}

CMSX_KERNEL_API inline INodeKey MakeINodeKey(int32_t fsNumber, int32_t inodeNumber)
{
    return INodeKey(fsNumber, inodeNumber);
}

class CMSX_KERNEL_API INode
{
public:
    static constexpr int Size() { return 128; }
    INode();
    INode(INodeKey key_);
    const INodeKey& Key() const { return key; }
    void SetKey(const INodeKey& key_) { key = key_; }
    int32_t EncodeMode() const;
    void DecodeMode(int32_t mode);
    void Read(Block* block, int index);
    void Write(Block* block, int index);
    bool IsLocked() const { return GetFlag(flags, INodeFlags::locked); }
    void Lock(int32_t processId) { SetFlag(flags, INodeFlags::locked); lockerProcessId = processId; ++lockCount; }
    void Unlock() { --lockCount; if (lockCount == 0) { ResetFlag(flags, INodeFlags::locked); lockerProcessId = -1; } }
    bool IsValid() const { return GetFlag(flags, INodeFlags::valid); }
    void SetValid() { SetFlag(flags, INodeFlags::valid); }
    void ResetValid() { ResetFlag(flags, INodeFlags::valid); }
    bool IsMountPoint() const { return GetFlag(flags, INodeFlags::mountPoint); }
    void SetMountPoint() { SetFlag(flags, INodeFlags::mountPoint); }
    void ResetMountPoint() { ResetFlag(flags, INodeFlags::mountPoint); }
    bool IsFree() const { return fileType == FileType::free; }
    void SetFree() { fileType = FileType::free; }
    FileType GetFileType() const { return fileType; }
    void SetFileType(FileType fileType_) { fileType = fileType_; }
    Access OwnerAccess() const { return ownerAccess; }
    void SetOwnerAccess(Access ownerAccess_) { ownerAccess = ownerAccess_; }
    Access GroupAccess() const { return groupAccess; }
    void SetGroupAccess(Access groupAccess_) { groupAccess = groupAccess_; }
    Access OtherAccess() const { return otherAccess; }
    void SetOtherAccess(Access otherAccess_) { otherAccess = otherAccess_; }
    int32_t NLinks() const { return nlinks; }
    void SetNLinks(int32_t nlinks_) { nlinks = nlinks_; }
    int32_t Uid() const { return uid; }
    void SetUid(int32_t uid_) { uid = uid_; }
    int32_t Gid() const { return gid; }
    void SetGid(int32_t gid_) { gid = gid_; }
    int64_t FileSize() const { return fileSize; }
    void SetFileSize(int64_t fileSize_) { fileSize = fileSize_; }
    const DateTime& CTime() const { return ctime; }
    void SetCTime(const DateTime& ctime_) { ctime = ctime_; }
    const DateTime& MTime() const { return mtime; }
    void SetMTime(const DateTime& mtime_) { mtime = mtime_; }
    const DateTime& ATime() const { return atime; }
    void SetATime(const DateTime& atime_) { atime = atime_; }
    static constexpr int32_t NumDirectBlockNumbers() { return 16; }
    int32_t GetDirectBlockNumber(int32_t index) const;
    void SetDirectBlockNumber(int32_t blockNumber, int32_t index);
    int32_t SingleIndirectBlockNumber() const { return singleIndirectBlockNumber; }
    void SetSingleIndirectBlockNumber(int32_t singleIndirectBlockNumber_) { singleIndirectBlockNumber = singleIndirectBlockNumber_; }
    int32_t DoubleIndirectBlockNumber() const { return doubleIndirectBlockNumber; }
    void SetDoubleIndirectBlockNumber(int32_t doubleIndirectBlockNumber_) { doubleIndirectBlockNumber = doubleIndirectBlockNumber_; }
    int32_t NumberOfBlocks() const;
    int32_t LockCount() const { return lockCount; }
    void SetLockCount(int32_t lockCount_) { lockCount = lockCount_; }
    int32_t LockerProcessId() const { return lockerProcessId; }
    void SetLockerProcessId(int32_t lockerProcessId_) { lockerProcessId = lockerProcessId_; }
    int32_t ReferenceCount() const { return referenceCount; }
    void IncrementReferenceCount() { ++referenceCount; }
    void DecrementReferenceCount() { --referenceCount; }
    static constexpr int32_t StatBufSize() { return 68; }
    void WriteStat(MemoryWriter& writer);
private:
    INodeKey key;
    FileType fileType;
    INodeFlags flags;
    Access ownerAccess;
    Access groupAccess;
    Access otherAccess;
    int32_t nlinks;
    int32_t uid;
    int32_t gid;
    int64_t fileSize;
    DateTime ctime;
    DateTime mtime;
    DateTime atime;
    std::vector<int32_t> directBlockNumbers;
    int32_t singleIndirectBlockNumber;
    int32_t doubleIndirectBlockNumber;
    int32_t referenceCount;
    int32_t lockCount;
    int32_t lockerProcessId;
};

class CMSX_KERNEL_API INodePtr
{
public:
    INodePtr(INode* inode_) noexcept : inode(inode_) {}
    ~INodePtr();
    INodePtr(const INodePtr&) = delete;
    INodePtr& operator=(const INodePtr&) = delete;
    INodePtr(INodePtr&& that) noexcept : inode(that.inode) { that.inode = nullptr; }
    INodePtr& operator=(INodePtr&& that) noexcept
    {
        std::swap(inode, that.inode);
        return *this;
    }
    INode* Get() { return inode; }
    void SetINode(const INode& inode_);
    void Release();
private:
    INode* inode;
};

CMSX_KERNEL_API INodePtr GetINode(INodeKey inodeKey, cmsx::machine::Process* process);

CMSX_KERNEL_API void InitINodeManager();
CMSX_KERNEL_API void DoneINodeManager();
CMSX_KERNEL_API void SetINodeManagerMachine(cmsx::machine::Machine* machine);
CMSX_KERNEL_API void StartINodeManager();
CMSX_KERNEL_API void StopINodeManager();

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_INODE_MANAGER_INCLUDED
