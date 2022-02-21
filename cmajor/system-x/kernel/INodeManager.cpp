// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/INodeManager.hpp>
#include <system-x/kernel/EventManager.hpp>
#include <system-x/kernel/BlockManager.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/kernel/DebugHelp.hpp>
#include <system-x/machine/Config.hpp>
#include <system-x/machine/Event.hpp>
#include <soulng/util/MemoryReader.hpp>
#include <soulng/util/MemoryWriter.hpp>

namespace cmsx::kernel {

void PutINode(INode* inode);

int32_t EncodeMode(FileType fileType, INodeFlags flags, Access ownerAccess, Access groupAccess, Access otherAccess)
{
    int32_t mode = 0;
    int32_t mntPointBit = GetFlag(flags, INodeFlags::mountPoint) ? 1 : 0;
    mode = mode | (int32_t(fileType) & 7) | (mntPointBit << 3) | (int32_t(ownerAccess) & 7) << 4 | (int32_t(groupAccess) & 7) << 7 | (int32_t(otherAccess) & 7) << 10;
    return mode;
}

void DecodeMode(int32_t mode, FileType& fileType, INodeFlags& flags, Access& ownerAccess, Access& groupAccess, Access& otherAccess)
{
    fileType = FileType(mode & 7);
    int32_t mntPointBit = (mode >> 3) & 1;
    if (mntPointBit)
    {
        SetFlag(flags, INodeFlags::mountPoint);
    }
    else
    {
        ResetFlag(flags, INodeFlags::mountPoint);
    }
    ownerAccess = Access((mode >> 4) & 7);
    groupAccess = Access((mode >> 7) & 7);
    otherAccess = Access((mode >> 10) & 7);
}

INode::INode() :
    key(),
    fileType(FileType::free),
    flags(INodeFlags::none),
    ownerAccess(Access::none),
    groupAccess(Access::none),
    otherAccess(Access::none),
    nlinks(1),
    uid(0),
    gid(0),
    fileSize(0),
    ctime(),
    mtime(),
    atime(),
    directBlockNumbers(NumDirectBlockNumbers(), -1),
    singleIndirectBlockNumber(-1),
    doubleIndirectBlockNumber(-1),
    referenceCount(0),
    lockCount(0),
    lockerProcessId(-1)
{
}

INode::INode(INodeKey key_) :
    key(key_),
    fileType(FileType::free),
    flags(INodeFlags::none),
    ownerAccess(Access::none),
    groupAccess(Access::none),
    otherAccess(Access::none),
    nlinks(1),
    uid(0),
    gid(0),
    fileSize(0),
    ctime(),
    mtime(),
    atime(),
    directBlockNumbers(NumDirectBlockNumbers(), -1),
    singleIndirectBlockNumber(-1),
    doubleIndirectBlockNumber(-1),
    referenceCount(0),
    lockCount(0),
    lockerProcessId(-1)
{
}

int32_t INode::EncodeMode() const
{
    return cmsx::kernel::EncodeMode(fileType, flags, ownerAccess, groupAccess, otherAccess);
}

void INode::DecodeMode(int32_t mode)
{
    cmsx::kernel::DecodeMode(mode, fileType, flags, ownerAccess, groupAccess, otherAccess);
}

int32_t INode::GetDirectBlockNumber(int32_t index) const
{
    if (index >= 0 && index < NumDirectBlockNumbers())
    {
        return directBlockNumbers[index];
    }
    else
    {
        throw SystemError(EFAIL, "invalid direct block number index ");
    }
}

void INode::SetDirectBlockNumber(int32_t blockNumber, int32_t index)
{
    if (index >= 0 && index < NumDirectBlockNumbers())
    {
        directBlockNumbers[index] = blockNumber;
    }
    else
    {
        throw SystemError(EFAIL, "invalid direct block number index ");
    }
}

void INode::Read(Block* block, int index)
{
    MemoryReader reader(block->Data() + Size() * index, Size());
    int32_t mode = reader.ReadInt();
    DecodeMode(mode);
    nlinks = reader.ReadInt();
    uid = reader.ReadInt();
    gid = reader.ReadInt();
    fileSize = reader.ReadLong();
    ctime = reader.ReadDateTime();
    mtime = reader.ReadDateTime();
    atime = reader.ReadDateTime();
    for (int32_t i = 0; i < NumDirectBlockNumbers(); ++i)
    {
        int32_t blockNumber = reader.ReadInt();
        SetDirectBlockNumber(blockNumber, i);
    }
    singleIndirectBlockNumber = reader.ReadInt();
    doubleIndirectBlockNumber = reader.ReadInt();
}

void INode::Write(Block* block, int index)
{
    MemoryWriter writer(block->Data() + Size() * index, Size());
    int32_t mode = EncodeMode();
    writer.Write(mode);
    writer.Write(nlinks);
    writer.Write(uid);
    writer.Write(gid);
    writer.Write(fileSize);
    writer.Write(ctime);
    writer.Write(mtime);
    writer.Write(atime);
    for (int32_t i = 0; i < NumDirectBlockNumbers(); ++i)
    {
        writer.Write(GetDirectBlockNumber(i));
    }
    writer.Write(singleIndirectBlockNumber);
    writer.Write(doubleIndirectBlockNumber);
}

int32_t INode::NumberOfBlocks() const
{
    if (fileSize == 0)
    {
        return 0;
    }
    return static_cast<int32_t>(fileSize / Block::Size());
}

void INode::WriteStat(MemoryWriter& writer)
{
    writer.Write(key.fsNumber);
    writer.Write(key.inodeNumber);
    writer.Write(static_cast<int32_t>(fileType));
    writer.Write(static_cast<int32_t>(ownerAccess));
    writer.Write(static_cast<int32_t>(groupAccess));
    writer.Write(static_cast<int32_t>(otherAccess));
    writer.Write(nlinks);
    writer.Write(uid);
    writer.Write(gid);
    writer.Write(fileSize);
    writer.Write(ctime);
    writer.Write(mtime);
    writer.Write(atime);
}

INodePtr::~INodePtr()
{
    if (inode)
    {
        PutINode(inode);
    }
}

void INodePtr::Release()
{
    if (inode)
    {
        PutINode(inode);
        inode = nullptr;
    }
}

void INodePtr::SetINode(const INode& inode_)
{
    int32_t lockCount = inode->LockCount();
    int32_t lockerProcessId = inode->LockerProcessId();
    *inode = inode_;
    inode->SetLockCount(lockCount);
    inode->SetLockerProcessId(lockerProcessId);
}

uint64_t INodeKeyHash(INodeKey inodeKey)
{
    return static_cast<uint64_t>(1099511628211) * inodeKey.fsNumber + inodeKey.inodeNumber;
}

using INodeFreeList = std::list<INode*, boost::fast_pool_allocator<INode*>>;

struct INodeHashQueueEntry
{
    INodeHashQueueEntry() : inode(nullptr), it() {}
    INodeHashQueueEntry(INode* inode_, INodeFreeList::iterator it_) : inode(inode_), it(it_) {}
    INode* inode;
    INodeFreeList::iterator it;
};

class INodeManager
{
public:
    static void Init();
    static void Done();
    static INodeManager& Instance() { return *instance; }
    ~INodeManager();
    void SetMachine(cmsx::machine::Machine* machine_);
    cmsx::machine::Machine* GetMachine() const { return machine; }
    void Start();
    void Stop();
    int NumberOfHashQueues() const { return numberOfHashQueues; }
    int GetHashQueueNumber(const INodeKey& key) const;
    INodeHashQueueEntry* GetINodeFromHashQueue(const INodeKey& inodeKey);
    void RemoveFromHashQueue(INode* inode);
    void InsertIntoHashQueue(INode* inode);
    void RemoveFromFreeList(INodeHashQueueEntry* entry);
    void PutINodeToFreeList(INode* inode);
    cmsx::machine::Event GetINodeKeyEvent(const INodeKey& inodeKey);
    void RemoveINodeKeyEvent(const INodeKey& inodeKey);
    bool IsFreeListEmpty() const { return freeList.empty(); }
    INode* GetINodeFromFreeList();
private:
    INodeManager();
    static std::unique_ptr<INodeManager> instance;
    cmsx::machine::Machine* machine;
    int numCachedINodes;
    int numberOfHashQueues;
    INodeFreeList freeList;
    std::vector<std::list<INodeHashQueueEntry, boost::fast_pool_allocator<INodeHashQueueEntry>>> hashQueues;
    std::map<INodeKey, cmsx::machine::Event> inodeKeyMapEventMap;
    int nextINodeKeyEventId;
};

std::unique_ptr<INodeManager> INodeManager::instance;

void INodeManager::Init()
{
    instance.reset(new INodeManager());
}

void INodeManager::Done()
{
    instance.reset();
}

INodeManager::INodeManager() : machine(nullptr), numCachedINodes(cmsx::machine::NumCachedINodes()), numberOfHashQueues(cmsx::machine::NumINodeHashQueues()), nextINodeKeyEventId(0)
{
    hashQueues.resize(numberOfHashQueues);
}

INodeManager::~INodeManager()
{
    for (auto& inode : freeList)
    {
        delete inode;
    }
}

void INodeManager::Start()
{
    for (int i = 0; i < numCachedINodes; ++i)
    {
        freeList.push_back(new INode());
    }
}

void INodeManager::Stop()
{
}

void INodeManager::SetMachine(cmsx::machine::Machine* machine_)
{
    machine = machine_;
}

int INodeManager::GetHashQueueNumber(const INodeKey& key) const
{
    uint64_t inodeKeyHash = INodeKeyHash(key);
    uint64_t n = numberOfHashQueues;
    return static_cast<int>(inodeKeyHash % n);
}

INodeHashQueueEntry* INodeManager::GetINodeFromHashQueue(const INodeKey& inodeKey)
{
    int hashQueueIndex = GetHashQueueNumber(inodeKey);
    auto& hashQueue = hashQueues[hashQueueIndex];
    for (auto it = hashQueue.begin(); it != hashQueue.end(); ++it)
    {
        INode* inode = it->inode;
        if (inode->Key() == inodeKey)
        {
            return &(*it);
        }
    }
    return nullptr;
}

void INodeManager::RemoveFromHashQueue(INode* inode)
{
    if (inode->Key() != INodeKey())
    {
        int hashQueueIndex = GetHashQueueNumber(inode->Key());
        auto& hashQueue = hashQueues[hashQueueIndex];
        auto it = hashQueue.begin();
        while (it != hashQueue.end())
        {
            if (inode == it->inode)
            {
                auto next = it;
                ++next;
                hashQueue.erase(it);
                it = next;
            }
            else
            {
                ++it;
            }
        }
    }
}

void INodeManager::InsertIntoHashQueue(INode* inode)
{
    int hashQueueIndex = GetHashQueueNumber(inode->Key());
    auto& hashQueue = hashQueues[hashQueueIndex];
    hashQueue.push_back(INodeHashQueueEntry(inode, INodeFreeList::iterator()));
}

void INodeManager::RemoveFromFreeList(INodeHashQueueEntry* entry)
{
    if (entry->it != freeList.end())
    {
        freeList.erase(entry->it);
        entry->it = freeList.end();
    }
}

void INodeManager::PutINodeToFreeList(INode* inode)
{
    freeList.push_back(inode);
    INodeHashQueueEntry* entry = GetINodeFromHashQueue(inode->Key());
    if (entry)
    {
        INodeFreeList::iterator it = freeList.end();
        --it;
        entry->it = it;
    }
}

cmsx::machine::Event INodeManager::GetINodeKeyEvent(const INodeKey& inodeKey)
{
    auto it = inodeKeyMapEventMap.find(inodeKey);
    if (it != inodeKeyMapEventMap.cend())
    {
        return it->second;
    }
    cmsx::machine::Event evnt = cmsx::machine::Event(cmsx::machine::EventKind::inodeFreeEvent, nextINodeKeyEventId++);
    inodeKeyMapEventMap[inodeKey] = evnt;
    return evnt;
}

void INodeManager::RemoveINodeKeyEvent(const INodeKey& inodeKey)
{
    inodeKeyMapEventMap.erase(inodeKey);
}

INode* INodeManager::GetINodeFromFreeList()
{
    INode* inode = freeList.front();
    freeList.pop_front();
    return inode;
}

INodePtr GetINode(INodeKey inodeKey, cmsx::machine::Process* process)
{
    while (true)
    {
        INodeManager& inodeManager = INodeManager::Instance();
#if (LOCK_DEBUG)
        DebugLock startDebugLock(&inodeManager.GetMachine()->Lock(), INODE_MANAGER, process->Id(), NO_LOCK | GET_INODE);
#endif 
        std::unique_lock<std::recursive_mutex> lock(inodeManager.GetMachine()->Lock());
#if (LOCK_DEBUG)
        DebugLock hasDebugLock(&inodeManager.GetMachine()->Lock(), INODE_MANAGER, process->Id(), HAS_LOCK | GET_INODE);
#endif 
        INodeHashQueueEntry* entry = inodeManager.GetINodeFromHashQueue(inodeKey);
        if (entry)
        {
            if (entry->inode->IsLocked())
            {
                if (entry->inode->LockerProcessId() == process->Id())
                {
                    entry->inode->Lock(process->Id());
                    return INodePtr(entry->inode);
                }
                cmsx::machine::Event inodeBecomesFreeEvent = inodeManager.GetINodeKeyEvent(inodeKey);
                Sleep(inodeBecomesFreeEvent, process, lock);
                lock.lock();
                continue;
            }
            else
            {
                entry->inode->Lock(process->Id());
                inodeManager.RemoveFromFreeList(entry);
                return INodePtr(entry->inode);
            }
        }
        else
        {
            if (inodeManager.IsFreeListEmpty())
            {
                throw SystemError(ELIMITEXCEEDED, "no cached inodes available");
            }
            else
            {
                INode* inode = inodeManager.GetINodeFromFreeList();
                inodeManager.RemoveFromHashQueue(inode);
                *inode = INode();
                inode->SetKey(inodeKey);
                inodeManager.InsertIntoHashQueue(inode);
                inode->ResetValid();
                inode->Lock(process->Id());
                return INodePtr(inode);
            }
        }
    }
}

void PutINode(INode* inode)
{
    INodeManager& inodeManager = INodeManager::Instance();
#if (LOCK_DEBUG)
    DebugLock startDebugLock(&inodeManager.GetMachine()->Lock(), INODE_MANAGER, 0, NO_LOCK | PUT_INODE);
#endif 
    std::unique_lock<std::recursive_mutex> lock(inodeManager.GetMachine()->Lock());
#if (LOCK_DEBUG)
    DebugLock hasDebugLock(&inodeManager.GetMachine()->Lock(), INODE_MANAGER, 0, HAS_LOCK | PUT_INODE);
#endif 
    inode->Unlock();
    if (inode->LockCount() == 0)
    {
        cmsx::machine::Event inodeFreeEvent = inodeManager.GetINodeKeyEvent(inode->Key());
        Wakeup(inodeFreeEvent);
        inodeManager.RemoveINodeKeyEvent(inode->Key());
        inodeManager.PutINodeToFreeList(inode);
    }
}

void InitINodeManager()
{
    INodeManager::Init();
}

void DoneINodeManager()
{
    INodeManager::Done();
}

void SetINodeManagerMachine(cmsx::machine::Machine* machine)
{
    INodeManager::Instance().SetMachine(machine);
}

void StartINodeManager()
{
    INodeManager::Instance().Start();
}

void StopINodeManager()
{
    INodeManager::Instance().Stop();
}

} // namespace cmsx::kernel
