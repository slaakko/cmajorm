// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/machine/Memory.hpp>
#include <system-x/machine/Machine.hpp>
#include <system-x/machine/Config.hpp>
#include <soulng/util/TextUtils.hpp>
#include <Windows.h>
#include <stdexcept>

namespace cmsx::machine {

using namespace soulng::util;

inline uint64_t MakePageNumber(uint64_t virtualAddress)
{
    return virtualAddress / pageSize;
}

inline uint64_t MakePageOffset(uint64_t virtualAddress)
{
    return virtualAddress & (pageSize - 1);
}

Memory::Memory(Machine& machine_) : machine(machine_), maxProcs(MaxProcs()), nextRV(0)
{
    if (maxProcs <= 0 || maxProcs > 64 * 1024)
    {
        throw std::runtime_error("invalid 'maxProcs' (" + std::to_string(maxProcs) + ") in '" + ConfigFilePath() + "': value should be in range 1..." + std::to_string(64 * 1024));
    }
    translationMaps.resize(maxProcs);
}

Memory::~Memory()
{
    std::lock_guard<std::recursive_mutex> lock(machine.Lock());
    for (uint64_t rv = 0; rv < maxProcs; ++rv)
    {
        FreeMemoryUnlocked(rv);
    }
}

uint8_t Memory::ReadByte(uint64_t virtualAddress, Protection protection)
{
    uint64_t address = TranslateAddress(virtualAddress, protection);
    return ReadByte(address);
}

void Memory::WriteByte(uint64_t virtualAddress, uint8_t value, Protection protection)
{
    uint64_t address = TranslateAddress(virtualAddress, protection);
    WriteByte(address, value);
}

uint16_t Memory::ReadWyde(uint64_t virtualAddress, Protection protection)
{
    uint64_t address = TranslateAddress(virtualAddress, protection);
    return ReadWyde(address);
}

void Memory::WriteWyde(uint64_t virtualAddress, uint16_t value, Protection protection)
{
    uint64_t address = TranslateAddress(virtualAddress, protection);
    return WriteWyde(address, value);
}

uint32_t Memory::ReadTetra(uint64_t virtualAddress, Protection protection)
{
    uint64_t address = TranslateAddress(virtualAddress, protection);
    return ReadTetra(address);
}

void Memory::WriteTetra(uint64_t virtualAddress, uint32_t value, Protection protection)
{
    uint64_t address = TranslateAddress(virtualAddress, protection);
    WriteTetra(address, value);
}

uint64_t Memory::ReadOcta(uint64_t virtualAddress, Protection protection)
{
    uint64_t address = TranslateAddress(virtualAddress, protection);
    return ReadOcta(address);
}

void Memory::WriteOcta(uint64_t virtualAddress, uint64_t value, Protection protection)
{
    uint64_t address = TranslateAddress(virtualAddress, protection);
    return WriteOcta(address, value);
}

uint64_t Memory::AllocatePage()
{
    uint64_t pageAddr = reinterpret_cast<uint64_t>(VirtualAlloc(nullptr, pageSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    if (pageAddr == 0)
    {
        throw std::runtime_error("out of memory");
    }
    return pageAddr;
}

void Memory::FreePage(uint64_t pageAddress)
{
    bool retval = VirtualFree(reinterpret_cast<void*>(pageAddress), 0, MEM_RELEASE);
    if (!retval)
    {
        throw std::runtime_error("free page failed");
    }
}

uint8_t Memory::ReadByte(uint64_t address)
{
    void* ptr = reinterpret_cast<void*>(address);
    uint8_t* bytePtr = reinterpret_cast<uint8_t*>(ptr);
    return *bytePtr;
}

void Memory::WriteByte(uint64_t address, uint8_t value)
{
    void* ptr = reinterpret_cast<void*>(address);
    uint8_t* bytePtr = reinterpret_cast<uint8_t*>(ptr);
    *bytePtr = value;
}

uint16_t Memory::ReadWyde(uint64_t address)
{
    uint64_t a = address & m2mask;
    uint8_t m0 = ReadByte(address);
    uint8_t m1 = ReadByte(address + 1);
    uint16_t value = (static_cast<uint16_t>(m0) << 8) | static_cast<uint16_t>(m1);
    return value;
}

void Memory::WriteWyde(uint64_t address, uint16_t value)
{
    uint64_t a = address & m2mask;
    uint8_t m0 = static_cast<uint8_t>(value >> 8);
    uint8_t m1 = static_cast<uint8_t>(value);
    WriteByte(a, m0);
    WriteByte(a + 1, m1);
}

uint32_t Memory::ReadTetra(uint64_t address)
{
    uint64_t a = address & m4mask;
    uint8_t m0 = ReadByte(a);
    uint8_t m1 = ReadByte(a + 1);
    uint8_t m2 = ReadByte(a + 2);
    uint8_t m3 = ReadByte(a + 3);
    uint32_t value = (static_cast<uint32_t>(m0) << 24) | (static_cast<uint32_t>(m1) << 16) | (static_cast<uint32_t>(m2) << 8) | static_cast<uint32_t>(m3);
    return value;
}

void Memory::WriteTetra(uint64_t address, uint32_t value)
{
    uint64_t a = address & m4mask;
    uint8_t m0 = static_cast<uint8_t>(value >> 24);
    uint8_t m1 = static_cast<uint8_t>(value >> 16);
    uint8_t m2 = static_cast<uint8_t>(value >> 8);
    uint8_t m3 = static_cast<uint8_t>(value);
    WriteByte(a, m0);
    WriteByte(a + 1, m1);
    WriteByte(a + 2, m2);
    WriteByte(a + 3, m3);
}

uint64_t Memory::ReadOcta(uint64_t address)
{
    uint64_t a = address & m8mask;
    uint8_t m0 = ReadByte(a);
    uint8_t m1 = ReadByte(a + 1);
    uint8_t m2 = ReadByte(a + 2);
    uint8_t m3 = ReadByte(a + 3);
    uint8_t m4 = ReadByte(a + 4);
    uint8_t m5 = ReadByte(a + 5);
    uint8_t m6 = ReadByte(a + 6);
    uint8_t m7 = ReadByte(a + 7);
    uint64_t value = (static_cast<uint64_t>(m0) << 56) | (static_cast<uint64_t>(m1) << 48) | (static_cast<uint64_t>(m2) << 40) | (static_cast<uint64_t>(m3) << 32) |
        (static_cast<uint64_t>(m4) << 24) | (static_cast<uint64_t>(m5) << 16) | (static_cast<uint64_t>(m6) << 8) | static_cast<uint64_t>(m7);
    return value;
}

void Memory::WriteOcta(uint64_t address, uint64_t value)
{
    uint64_t a = address & m8mask;
    uint8_t m0 = static_cast<uint8_t>(value >> 56);
    uint8_t m1 = static_cast<uint8_t>(value >> 48);
    uint8_t m2 = static_cast<uint8_t>(value >> 40);
    uint8_t m3 = static_cast<uint8_t>(value >> 32);
    uint8_t m4 = static_cast<uint8_t>(value >> 24);
    uint8_t m5 = static_cast<uint8_t>(value >> 16);
    uint8_t m6 = static_cast<uint8_t>(value >> 8);
    uint8_t m7 = static_cast<uint8_t>(value);
    WriteByte(a, m0);
    WriteByte(a + 1, m1);
    WriteByte(a + 2, m2);
    WriteByte(a + 3, m3);
    WriteByte(a + 4, m4);
    WriteByte(a + 5, m5);
    WriteByte(a + 6, m6);
    WriteByte(a + 7, m7);
}

uint64_t Memory::AllocateTranslationMap()
{
    std::lock_guard<std::recursive_mutex> lock(machine.Lock());
    if (nextRV >= maxProcs)
    {
        nextRV = 0;
    }
    while (nextRV < maxProcs && translationMaps[nextRV])
    {
        ++nextRV;
    }
    if (nextRV >= maxProcs)
    {
        throw std::runtime_error("all memory translation maps in use");
    }
    uint64_t rv = nextRV++;
    translationMaps[rv].reset(new std::map<uint64_t, uint64_t>());
    return rv;
}

void Memory::FreeMemory(uint64_t rv)
{
    std::lock_guard<std::recursive_mutex> lock(machine.Lock());
    FreeMemoryUnlocked(rv);
}

void Memory::AllocateRange(uint64_t start, uint64_t length)
{
    uint64_t rv = machine.Regs().GetSpecial(rV);
    if (rv >= maxProcs)
    {
        throw std::runtime_error("invalid virtual translation register (rV) value #" + ToHexString(rv) + ": value greater than or equal to maxProcs (#" +
            ToHexString(static_cast<uint32_t>(maxProcs)) + ")");
    }
    auto& translationMapPtr = translationMaps[rv];
    if (!translationMapPtr)
    {
        throw std::runtime_error("translation map for virtual translation register (rV) value #" + ToHexString(rv) + " not allocated");
    }
    auto& translationMap = *translationMapPtr;
    int64_t s = MakePageNumber(start);
    int64_t e = MakePageNumber(start + length);
    int64_t n = e - s + 1;
    for (int64_t i = 0; i < n; ++i)
    {
        int64_t pageNumber = s + i;
        auto it = translationMap.find(pageNumber);
        if (it == translationMap.cend())
        {
            uint64_t pageAddress = AllocatePage();
            translationMap[pageNumber] = pageAddress;
        }
    }
}

void Memory::FreeMemoryUnlocked(uint64_t rv)
{
    if (rv >= maxProcs)
    {
        throw std::runtime_error("invalid virtual translation register (rV) value #" + ToHexString(rv) + ": value greater than or equal to maxProcs (#" +
            ToHexString(static_cast<uint32_t>(maxProcs)) + ")");
    }
    auto& translationMapPtr = translationMaps[rv];
    if (translationMapPtr)
    {
        auto& translationMap = *translationMapPtr;
        for (const auto& p : translationMap)
        {
            uint64_t pageAddress = p.second;
            FreePage(pageAddress);
        }
        translationMapPtr.reset();
    }
}

uint64_t Memory::TranslateAddress(uint64_t virtualAddress, Protection access)
{
    uint64_t rv = machine.Regs().GetSpecial(rV);
    if (rv >= maxProcs)
    {
        throw std::runtime_error("invalid virtual translation register (rV) value #" + ToHexString(rv) + ": value greater than or equal to maxProcs (#" + 
            ToHexString(static_cast<uint32_t>(maxProcs)) + ")");
    }
    auto& translationMapPtr = translationMaps[rv];
    if (!translationMapPtr)
    {
        throw std::runtime_error("translation map for virtual translation register (rV) value #" + ToHexString(rv) + " not allocated");
    }
    auto& translationMap = *translationMapPtr;
    int64_t pageNumber = MakePageNumber(virtualAddress);
    int64_t pageOffset = MakePageOffset(virtualAddress);
    auto it = translationMap.find(pageNumber);
    if (it != translationMap.cend())
    {
        uint64_t pageAddress = it->second;
        return pageAddress + pageOffset;
    }
    if ((access & Protection::write) != Protection::notPresent)
    {
        uint64_t pageAddress = AllocatePage();
        translationMap[pageNumber] = pageAddress;
        return pageAddress + pageOffset;
    }
    else
    {
        throw std::runtime_error("virtual address #" + ToHexString(virtualAddress) + " not found from memory translation map #" + ToHexString(rv));
    }
}

} // cmsx::machine
