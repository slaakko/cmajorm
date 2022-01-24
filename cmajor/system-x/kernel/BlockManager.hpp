// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_BLOCK_MANAGER_INCLUDED
#define CMSX_KERNEL_BLOCK_MANAGER_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <stdint.h>
#include <algorithm>

namespace cmsx::kernel {

struct CMSX_KERNEL_API BlockKey
{
    BlockKey(uint32_t fsNumber_, uint32_t blockNumber_) : fsNumber(fsNumber_), blockNumber(blockNumber_) {}
    uint32_t fsNumber;
    uint32_t blockNumber;
};

const int blockSize = 4096;

class CMSX_KERNEL_API Block
{
public:
    static constexpr int Size() { return blockSize; }
    uint8_t* Data() { return &data[0]; }
private:
    uint8_t data[blockSize];
};

class CMSX_KERNEL_API BlockPtr
{
public:
    BlockPtr(Block* block_) noexcept : block(block_) {}
    ~BlockPtr();
    BlockPtr(const BlockPtr&) = delete;
    BlockPtr& operator=(const BlockPtr&) = delete;
    BlockPtr(BlockPtr&& that) noexcept : block(that.block) { that.block = nullptr; }
    BlockPtr& operator=(BlockPtr&& that) noexcept
    {
        std::swap(block, that.block);
        return *this;
    }
    Block* Get() { return block; }
private:
    Block* block;
};

CMSX_KERNEL_API BlockPtr GetBlock();

CMSX_KERNEL_API void InitBlockManager();
CMSX_KERNEL_API void DoneBlockManager();
CMSX_KERNEL_API void StartBlockManager();
CMSX_KERNEL_API void StopBlockManager();

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_BLOCK_INCLUDED
