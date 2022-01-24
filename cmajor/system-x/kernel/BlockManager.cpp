// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/BlockManager.hpp>
#include <system-x/machine/Config.hpp>
#include <condition_variable>
#include <boost/pool/pool_alloc.hpp>
#include <list>

namespace cmsx::kernel {

CMSX_KERNEL_API void PutBlock(Block* block);
    
BlockPtr::~BlockPtr()
{ 
    if (block)
    {
        PutBlock(block);
    }
}

class BlockManager
{
public:
    static void Init();
    static void Done();
    static BlockManager& Instance() { return *instance; }
    ~BlockManager();
    void Start();
    void Stop();
    BlockPtr GetBlock();
    void PutBlock(Block* block);
private:
    BlockManager();
    static std::unique_ptr<BlockManager> instance;
    int maxBlocks;
    bool exiting;
    std::list<Block*, boost::fast_pool_allocator<Block*>> freeList;
    std::mutex mtx;
    std::condition_variable blockAvailableOrExitingVar;
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

BlockManager::BlockManager() : exiting(false), maxBlocks(cmsx::machine::MaxBlocks())
{
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
    for (int i = 0; i < maxBlocks; ++i)
    {
        freeList.push_back(new Block());
    }
}

void BlockManager::Stop()
{
    exiting = true;
    blockAvailableOrExitingVar.notify_all();
}

BlockPtr BlockManager::GetBlock()
{
    std::unique_lock<std::mutex> lock(mtx);
    blockAvailableOrExitingVar.wait(lock, [this] { return !freeList.empty() || exiting; });
    if (exiting)
    {
        return BlockPtr(nullptr);
    }
    Block* block = freeList.front();
    freeList.pop_front();
    return BlockPtr(block);
}

void BlockManager::PutBlock(Block* block)
{
    std::unique_lock<std::mutex> lock(mtx);
    freeList.push_back(block);
    blockAvailableOrExitingVar.notify_one();
}

BlockPtr GetBlock()
{
    return BlockManager::Instance().GetBlock();
}

void PutBlock(Block* block)
{
    BlockManager::Instance().PutBlock(block);
}

void InitBlockManager()
{
    BlockManager::Init();
}

void DoneBlockManager()
{
    BlockManager::Done();
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