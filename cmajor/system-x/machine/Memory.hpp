// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_MACHINE_MEMORY_INCLUDED
#define CMSX_MACHINE_MEMORY_INCLUDED
#include <system-x/machine/Api.hpp>
#include <mutex>
#include <memory>
#include <map>
#include <vector>
#include <stdint.h>
#include <assert.h>

namespace cmsx::machine {

class Machine;
    
enum class Protection : int
{
    notPresent = 0, 
    execute = 1 << 0,
    read = 1 << 1,
    write = 1 << 2
};

inline CMSX_MACHINE_API Protection operator|(Protection left, Protection right)
{
    return Protection(int(left) | int(right));
}

inline CMSX_MACHINE_API Protection operator&(Protection left, Protection right)
{
    return Protection(int(left) & int(right));
}

inline CMSX_MACHINE_API Protection operator~(Protection prot)
{
    return Protection(~int(prot));
}

const uint64_t pageSize = 0x1000; // 4K bytes

const uint64_t textSegmentBaseAddress = 0x0000000000000000;
const uint64_t dataSegmentBaseAddress = 0x2000000000000000;
const uint64_t poolSegmentBaseAddress = 0x4000000000000000;
const uint64_t stackSegmentBaseAddress = 0x6000000000000000;
const uint64_t kernelBaseAddress = 0x8000000000000000;

const uint64_t m2mask = ~static_cast<uint64_t>(1);
const uint64_t m4mask = ~static_cast<uint64_t>(3);
const uint64_t m8mask = ~static_cast<uint64_t>(7);

class CMSX_MACHINE_API Memory
{
public:
    Memory(Machine& machine_);
    ~Memory();
    Memory(const Memory&) = delete;
    Memory& operator=(const Memory&) = delete;
    uint8_t ReadByte(uint64_t virtualAddress, Protection protection);
    void WriteByte(uint64_t virtualAddress, uint8_t value, Protection protection);
    uint16_t ReadWyde(uint64_t virtualAddress, Protection protection);
    void WriteWyde(uint64_t virtualAddress, uint16_t value, Protection protection);
    uint32_t ReadTetra(uint64_t virtualAddress, Protection protection);
    void WriteTetra(uint64_t virtualAddress, uint32_t value, Protection protection);
    uint64_t ReadOcta(uint64_t virtualAddress, Protection protection);
    void WriteOcta(uint64_t virtualAddress, uint64_t value, Protection protection);
    uint64_t AllocateTranslationMap();
    void FreeMemory(uint64_t rv);
private:
    uint64_t AllocatePage();
    void FreeMemoryUnlocked(uint64_t rv);
    void FreePage(uint64_t pageAddress);
    uint8_t ReadByte(uint64_t address);
    void WriteByte(uint64_t address, uint8_t value);
    uint16_t ReadWyde(uint64_t address);
    void WriteWyde(uint64_t address, uint16_t value);
    uint32_t ReadTetra(uint64_t address);
    void WriteTetra(uint64_t address, uint32_t value);
    uint64_t ReadOcta(uint64_t address);
    void WriteOcta(uint64_t address, uint64_t value);
    uint64_t TranslateAddress(uint64_t virtualAddress, Protection access);
    Machine& machine;
    int maxProcs;
    uint64_t nextRV;
    std::vector<std::unique_ptr<std::map<uint64_t, uint64_t>>> translationMaps;
};

} // cmsx::machine

#endif // CMSX_MACHINE_MEMORY_INCLUDED