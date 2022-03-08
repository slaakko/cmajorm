// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/BlockManager.hpp>
#include <system-x/kernel/EventManager.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/kernel/File.hpp>
#include <system-x/kernel/DebugHelp.hpp>
#include <system-x/kernel/Fs.hpp>
#include <system-x/kernel/Mount.hpp>
#include <system-x/machine/Config.hpp>
#include <system-x/machine/Event.hpp>
#include <condition_variable>
#include <boost/pool/pool_alloc.hpp>
#include <list>

namespace cmsx::kernel {

CMSX_KERNEL_API void PutBlock(Block* block);

Block::Block() : flags(BlockFlags::none), key(), data()
{
}

Block::Block(BlockKey key_) : flags(BlockFlags::none), key(key_), data()
{
}

void Block::Clear()
{
    std::memset(data, 0, Size());
}


bool Block::IsValid() const
{
    if (key.fsNumber == rootFSNumber)
    {
        return GetFlag(BlockFlags::valid);
    }
    else
    {
        return false;
    }
}

BlockPtr::~BlockPtr()
{ 
    if (block)
    {
        PutBlock(block);
    }
}

void BlockPtr::Release()
{
    if (block)
    {
        PutBlock(block);
        block = nullptr;
    }
}

uint64_t BlockKeyHash(BlockKey blockKey)
{
    return static_cast<uint64_t>(1099511628211) * blockKey.fsNumber + blockKey.blockNumber;
}

using BlockFreeList = std::list<Block*, boost::fast_pool_allocator<Block*>>;

struct BlockHashQueueEntry
{
    BlockHashQueueEntry() : block(nullptr), it() {}
    BlockHashQueueEntry(Block* block_, BlockFreeList::iterator it_) : block(block_), it(it_) {}
    Block* block;
    BlockFreeList::iterator it;
};

class BlockManager
{
public:
    static void Init();
    static void Done();
    static BlockManager& Instance() { return *instance; }
    ~BlockManager();
    void SetMachine(cmsx::machine::Machine* machine_);
    cmsx::machine::Machine* GetMachine() const { return machine; }
    void Start();
    void Stop();
    int NumberOfHashQueues() const { return numberOfHashQueues; }
    int GetHashQueueNumber(const BlockKey& key) const;
    BlockHashQueueEntry* GetBlockFromHashQueue(const BlockKey& blockKey);
    void RemoveFromHashQueue(Block* block);
    void InsertIntoHashQueue(Block* block);
    void RemoveFromFreeList(BlockHashQueueEntry* entry);
    void PutBlockToFreeList(Block* block);
    const cmsx::machine::Event* GetBlockKeyEvent(const BlockKey& blockKey) const;
    cmsx::machine::Event MakeBlockKeyEvent(const BlockKey& blockKey);
    void RemoveBlockKeyEvent(const BlockKey& blockKey);
    cmsx::machine::Event GetAnyBlockBecomesFreeEvent() const { return anyBlockBecomesFreeEvent; }
    bool IsFreeListEmpty() const { return freeList.empty(); }
    int32_t FreeListSize() const { return freeList.size(); }
    Block* GetBlockFromFreeList();
private:
    BlockManager();
    static std::unique_ptr<BlockManager> instance;
    cmsx::machine::Machine* machine;
    int numCachedBlocks;
    int numberOfHashQueues;
    BlockFreeList freeList;
    std::vector<std::list<BlockHashQueueEntry, boost::fast_pool_allocator<BlockHashQueueEntry>>> hashQueues;
    std::map<BlockKey, cmsx::machine::Event> blockKeyMapEventMap;
    cmsx::machine::Event anyBlockBecomesFreeEvent;
    int nextBlockKeyEventId;
};

std::unique_ptr<BlockManager> BlockManager::instance;

void BlockManager::Init()
{
    instance.reset(new BlockManager());
}

void BlockManager::Done()
{
    instance.reset();
}

BlockManager::BlockManager() : machine(nullptr), numCachedBlocks(cmsx::machine::NumCachedBlocks()), numberOfHashQueues(cmsx::machine::NumBlockHashQueues()),
    anyBlockBecomesFreeEvent(cmsx::machine::EventKind::blockFreeEvent, 0), nextBlockKeyEventId(1)
{
    hashQueues.resize(numberOfHashQueues);
}

BlockManager::~BlockManager()
{
    for (auto& block : freeList)
    {
        delete block;
    }
}

void BlockManager::Start()
{
    for (int i = 0; i < numCachedBlocks; ++i)
    {
        freeList.push_back(new Block());
    }
}

void BlockManager::Stop()
{
}

void BlockManager::SetMachine(cmsx::machine::Machine* machine_)
{
    machine = machine_;
}

int BlockManager::GetHashQueueNumber(const BlockKey& key) const
{
    uint64_t blockKeyHash = BlockKeyHash(key);
    uint64_t n = numberOfHashQueues;
    return static_cast<int>(blockKeyHash % n);
}

BlockHashQueueEntry* BlockManager::GetBlockFromHashQueue(const BlockKey& blockKey) 
{
    int hashQueueIndex = GetHashQueueNumber(blockKey);
    auto& hashQueue = hashQueues[hashQueueIndex];
    for (auto it = hashQueue.begin(); it != hashQueue.end(); ++it)
    {
        Block* block = it->block;
        if (block->Key() == blockKey)
        {
            return &(*it);
        }
    }
    return nullptr;
}

void BlockManager::RemoveFromHashQueue(Block* block)
{
    if (block->Key() != BlockKey())
    {
        int hashQueueIndex = GetHashQueueNumber(block->Key());
        auto& hashQueue = hashQueues[hashQueueIndex];
        auto it = hashQueue.begin();
        while (it != hashQueue.end())
        {
            if (block == it->block)
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

void BlockManager::InsertIntoHashQueue(Block* block)
{
    int hashQueueIndex = GetHashQueueNumber(block->Key());
    auto& hashQueue = hashQueues[hashQueueIndex];
    hashQueue.push_back(BlockHashQueueEntry(block, BlockFreeList::iterator()));
}

void BlockManager::RemoveFromFreeList(BlockHashQueueEntry* entry)
{
    if (entry->it != freeList.end())
    {
        freeList.erase(entry->it);
        entry->it = freeList.end();
    }
}

void BlockManager::PutBlockToFreeList(Block* block)
{
    freeList.push_back(block);
    BlockHashQueueEntry* entry = GetBlockFromHashQueue(block->Key());
    if (entry)
    {
        BlockFreeList::iterator it = freeList.end();
        --it;
        entry->it = it;
    }
}

const cmsx::machine::Event* BlockManager::GetBlockKeyEvent(const BlockKey& blockKey) const
{
    auto it = blockKeyMapEventMap.find(blockKey);
    if (it != blockKeyMapEventMap.cend())
    {
        return &it->second;
    }
    else
    {
        return nullptr;
    }
}

cmsx::machine::Event BlockManager::MakeBlockKeyEvent(const BlockKey& blockKey)
{
    cmsx::machine::Event evnt = cmsx::machine::Event(cmsx::machine::EventKind::blockFreeEvent, nextBlockKeyEventId++);
    blockKeyMapEventMap[blockKey] = evnt;
    return evnt;
}

void BlockManager::RemoveBlockKeyEvent(const BlockKey& blockKey)
{
    blockKeyMapEventMap.erase(blockKey);
}

Block* BlockManager::GetBlockFromFreeList()
{
    Block* block = freeList.front();
    freeList.pop_front();
    return block;
}

BlockPtr GetBlock(BlockKey blockKey, cmsx::machine::Process* process)
{
    while (true)
    {
        BlockManager& blockManager = BlockManager::Instance();
#if (LOCK_DEBUG)
        DebugLock startDebugLock(&blockManager.GetMachine()->Lock(), BLOCK_MANAGER, process->Id(), NO_LOCK | GET_BLOCK);
#endif 
        std::unique_lock<std::recursive_mutex> lock(blockManager.GetMachine()->Lock());
#if (LOCK_DEBUG)
        DebugLock hasDebugLock(&blockManager.GetMachine()->Lock(), BLOCK_MANAGER, process->Id(), HAS_LOCK | GET_BLOCK);
#endif 
        BlockHashQueueEntry* entry = blockManager.GetBlockFromHashQueue(blockKey);
        if (entry)
        {
            if (entry->block->IsLocked())
            {
                const cmsx::machine::Event* blockBecomesFreeEvent = blockManager.GetBlockKeyEvent(blockKey);
                if (blockBecomesFreeEvent)
                {
                    Sleep(*blockBecomesFreeEvent, process, lock);
                }
                else
                {
                    cmsx::machine::Event blockBecomesFreeEvent = blockManager.MakeBlockKeyEvent(blockKey);
                    Sleep(blockBecomesFreeEvent, process, lock);
                }
                lock.lock();
                continue;
            }
            else
            {
                entry->block->SetLocked();
                blockManager.RemoveFromFreeList(entry);
                return BlockPtr(entry->block);
            }
        }
        else
        {
            if (blockManager.IsFreeListEmpty())
            {
                cmsx::machine::Event anyBlockBecomesFreeEvent = blockManager.GetAnyBlockBecomesFreeEvent();
                Sleep(anyBlockBecomesFreeEvent, process, lock);
                lock.lock();
                continue;
            }
            else
            {
                Block* block = blockManager.GetBlockFromFreeList();
                blockManager.RemoveFromHashQueue(block);
                *block = Block();
                block->SetKey(blockKey);
                blockManager.InsertIntoHashQueue(block);
                block->ResetValid();
                block->SetLocked();
                return BlockPtr(block);
            }
        }
    }
}

void PutBlock(Block* block)
{
    BlockManager& blockManager = BlockManager::Instance();
#if (LOCK_DEBUG)
    DebugLock startDebugLock(&blockManager.GetMachine()->Lock(), BLOCK_MANAGER, 0, NO_LOCK | PUT_BLOCK);
#endif 
    std::unique_lock<std::recursive_mutex> lock(blockManager.GetMachine()->Lock());
#if (LOCK_DEBUG)
    DebugLock hasDebugLock(&blockManager.GetMachine()->Lock(), BLOCK_MANAGER, 0, HAS_LOCK | PUT_BLOCK);
#endif 
    Wakeup(blockManager.GetAnyBlockBecomesFreeEvent());
    const cmsx::machine::Event* blockFreeEvent = blockManager.GetBlockKeyEvent(block->Key());
    if (blockFreeEvent)
    {
        Wakeup(*blockFreeEvent);
        blockManager.RemoveBlockKeyEvent(block->Key());
    }
    blockManager.PutBlockToFreeList(block);
    block->ResetLocked();
}

void InitBlockManager()
{
    BlockManager::Init();
}

void DoneBlockManager()
{
    BlockManager::Done();
}

void SetBlockManagerMachine(cmsx::machine::Machine* machine)
{
    BlockManager::Instance().SetMachine(machine);
}

void StartBlockManager()
{
    BlockManager::Instance().Start();
}

void StopBlockManager()
{
    BlockManager::Instance().Stop();
}

} // namespace cmsx::kernel
