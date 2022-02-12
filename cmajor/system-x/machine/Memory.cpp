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
#undef min
#undef max

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

void MemoryPage::DecUse()
{
    --useCount;
    if (useCount == 0)
    {
        bool retval = VirtualFree(reinterpret_cast<void*>(address), 0, MEM_RELEASE);
        if (!retval)
        {
            throw std::runtime_error("free page failed");
        }
        address = 0;
    }
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
    for (uint64_t rv = 0; rv < maxProcs; ++rv)
    {
        FreeMemory(rv);
    }
}

uint8_t Memory::ReadByte(uint64_t rv, uint64_t virtualAddress, Protection protection)
{
    uint64_t address = TranslateAddress(rv, virtualAddress, protection);
    return ReadByte(address);
}

void Memory::WriteByte(uint64_t rv, uint64_t virtualAddress, uint8_t value, Protection protection)
{
    uint64_t address = TranslateAddress(rv, virtualAddress, protection);
    WriteByte(address, value);
}

uint16_t Memory::ReadWyde(uint64_t rv, uint64_t virtualAddress, Protection protection)
{
    uint64_t address = TranslateAddress(rv, virtualAddress, protection);
    return ReadWyde(address);
}

void Memory::WriteWyde(uint64_t rv, uint64_t virtualAddress, uint16_t value, Protection protection)
{
    uint64_t address = TranslateAddress(rv, virtualAddress, protection);
    return WriteWyde(address, value);
}

uint32_t Memory::ReadTetra(uint64_t rv, uint64_t virtualAddress, Protection protection)
{
    uint64_t address = TranslateAddress(rv, virtualAddress, protection);
    return ReadTetra(address);
}

void Memory::WriteTetra(uint64_t rv, uint64_t virtualAddress, uint32_t value, Protection protection)
{
    uint64_t address = TranslateAddress(rv, virtualAddress, protection);
    WriteTetra(address, value);
}

uint64_t Memory::ReadOcta(uint64_t rv, uint64_t virtualAddress, Protection protection)
{
    uint64_t address = TranslateAddress(rv, virtualAddress, protection);
    return ReadOcta(address);
}

void Memory::WriteOcta(uint64_t rv, uint64_t virtualAddress, uint64_t value, Protection protection)
{
    uint64_t address = TranslateAddress(rv, virtualAddress, protection);
    return WriteOcta(address, value);
}

MemoryPage* Memory::AllocatePage()
{
    uint64_t pageAddr = reinterpret_cast<uint64_t>(VirtualAlloc(nullptr, pageSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    if (pageAddr == 0)
    {
        throw std::runtime_error("out of memory");
    }
    return new MemoryPage(pageAddr);
}

void Memory::FreePage(MemoryPage* page)
{
    if (page)
    {
        page->DecUse();
        if (page->UseCount() == 0)
        {
            delete page;
        }
    }
}

MemoryPage* Memory::CopyPage(MemoryPage* from)
{
    MemoryPage* copy = AllocatePage();
    std::memcpy(reinterpret_cast<void*>(copy->Address()), reinterpret_cast<void*>(from->Address()), static_cast<size_t>(pageSize));
    return copy;
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
    translationMaps[rv].reset(new std::map<uint64_t, MemoryPage*>());
    return rv;
}

void Memory::AllocateTranslationMap(uint64_t rv)
{
    std::lock_guard<std::recursive_mutex> lock(machine.Lock());
    translationMaps[rv].reset(new std::map<uint64_t, MemoryPage*>());
}

void Memory::FreeMemory(uint64_t rv)
{
    std::lock_guard<std::recursive_mutex> lock(machine.Lock());
    if (rv >= maxProcs)
    {
        throw std::runtime_error("invalid virtual translation register (rV) value #" + ToHexString(rv) + ": value greater than or equal to maxProcs (#" +
            ToHexString(static_cast<uint32_t>(maxProcs)) + ")");
    }
    auto& translationMapPtr = translationMaps[rv];
    if (translationMapPtr)
    {
        auto& translationMap = *translationMapPtr;
        for (auto& p : translationMap)
        {
            MemoryPage* page = p.second;
            FreePage(page);
            p.second = nullptr;
        }
        translationMapPtr.reset();
    }
}

void Memory::AllocateRange(uint64_t rv, uint64_t start, uint64_t length)
{
    if (length == 0) return;
    std::lock_guard<std::recursive_mutex> lock(machine.Lock());
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
    int64_t e = 1 + MakePageNumber(start + length - 1);
    int64_t n = e - s;
    for (int64_t i = 0; i < n; ++i)
    {
        int64_t pageNumber = s + i;
        auto it = translationMap.find(pageNumber);
        if (it == translationMap.cend())
        {
            MemoryPage* page = AllocatePage();
            translationMap[pageNumber] = page;
        }
    }
}

void Memory::FreeRange(uint64_t rv, uint64_t start, uint64_t length)
{
    if (length == 0) return;
    std::lock_guard<std::recursive_mutex> lock(machine.Lock());
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
    int64_t e = 1 + MakePageNumber(start + length - 1);
    int64_t n = e - s;
    for (int64_t i = 0; i < n; ++i)
    {
        int64_t pageNumber = s + i;
        auto it = translationMap.find(pageNumber);
        if (it != translationMap.cend())
        {
            MemoryPage* page = it->second;
            FreePage(page);
            translationMap[pageNumber] = nullptr;
        }
    }
}

void Memory::CopyRange(uint64_t fromRV, uint64_t toRV, uint64_t start, uint64_t length)
{
    if (length == 0) return;
    std::lock_guard<std::recursive_mutex> lock(machine.Lock());
    auto& fromTranslationMapPtr = translationMaps[fromRV];
    if (!fromTranslationMapPtr)
    {
        throw std::runtime_error("translation map for virtual translation register (rV) value #" + ToHexString(fromRV) + " not allocated");
    }
    auto& fromTranslationMap = *fromTranslationMapPtr;
    auto& toTranslationMapPtr = translationMaps[toRV];
    if (!toTranslationMapPtr)
    {
        throw std::runtime_error("translation map for virtual translation register (rV) value #" + ToHexString(toRV) + " not allocated");
    }
    auto& toTranslationMap = *toTranslationMapPtr;
    int64_t s = MakePageNumber(start);
    int64_t e = 1 + MakePageNumber(start + length - 1);
    int64_t n = e - s;
    for (int64_t i = 0; i < n; ++i)
    {
        int64_t pageNumber = s + i;
        auto it = fromTranslationMap.find(pageNumber);
        if (it != fromTranslationMap.cend())
        {
            MemoryPage* from = it->second;
            MemoryPage* page = CopyPage(from);
            toTranslationMap[pageNumber] = page;
        }
        else
        {
            throw std::runtime_error("page #" + ToHexString(static_cast<uint64_t>(i)) + " not found from memory translation map #" + ToHexString(fromRV));
        }
    }
}

void Memory::ShareRange(uint64_t fromRV, uint64_t toRV, uint64_t start, uint64_t length)
{
    if (length == 0) return;
    std::lock_guard<std::recursive_mutex> lock(machine.Lock());
    auto& fromTranslationMapPtr = translationMaps[fromRV];
    if (!fromTranslationMapPtr)
    {
        throw std::runtime_error("translation map for virtual translation register (rV) value #" + ToHexString(fromRV) + " not allocated");
    }
    auto& fromTranslationMap = *fromTranslationMapPtr;
    auto& toTranslationMapPtr = translationMaps[toRV];
    if (!toTranslationMapPtr)
    {
        throw std::runtime_error("translation map for virtual translation register (rV) value #" + ToHexString(toRV) + " not allocated");
    }
    auto& toTranslationMap = *toTranslationMapPtr;
    int64_t s = MakePageNumber(start);
    int64_t e = 1 + MakePageNumber(start + length - 1);
    int64_t n = e - s;
    for (int64_t i = 0; i < n; ++i)
    {
        int64_t pageNumber = s + i;
        auto it = fromTranslationMap.find(pageNumber);
        if (it != fromTranslationMap.cend())
        {
            MemoryPage* page = it->second;
            page->AddUse();
            toTranslationMap[pageNumber] = page;
        }
        else
        {
            throw std::runtime_error("page #" + ToHexString(static_cast<uint64_t>(i)) + " not found from memory translation map #" + ToHexString(fromRV));
        }
    }
}

void Memory::Copy(uint64_t rv, uint64_t sourceVirtualAddress, uint64_t targetVirtualAddress, uint64_t count)
{
    if (count == 0) return;
    int64_t fromPageOffset = 0;
    uint64_t from = TranslateAddress(rv, sourceVirtualAddress, cmsx::machine::Protection::read, fromPageOffset);
    int64_t toPageOffset = 0;
    uint64_t to = TranslateAddress(rv, targetVirtualAddress, cmsx::machine::Protection::write, toPageOffset);
    while (count > 0)
    {
        int64_t n = std::min(count, pageSize - fromPageOffset);
        n = std::min(n, static_cast<int64_t>(pageSize) - toPageOffset);
        std::memcpy(reinterpret_cast<void*>(to), reinterpret_cast<void*>(from), n);
        count -= n;
        if (count == 0) break;
        sourceVirtualAddress += n;
        targetVirtualAddress += n;
        fromPageOffset = 0;
        from = TranslateAddress(rv, sourceVirtualAddress, cmsx::machine::Protection::read, fromPageOffset);
        toPageOffset = 0;
        to = TranslateAddress(rv, targetVirtualAddress, cmsx::machine::Protection::write, toPageOffset);
    }
}

void Memory::NCopy(const uint8_t* source, uint64_t rv, uint64_t targetVirtualAddress, uint64_t count)
{
    if (count == 0) return;
    int64_t pageOffset = 0;
    uint64_t start = TranslateAddress(rv, targetVirtualAddress, Protection::write, pageOffset);
    uint64_t end = start + std::min(count, pageSize - pageOffset);
    int64_t n = end - start;
    std::memcpy(reinterpret_cast<void*>(start), source, n);
    count -= n;
    source += n;
    targetVirtualAddress += n;
    while (count > 0)
    {
        start = TranslateAddress(rv, targetVirtualAddress, Protection::write);
        end = start + std::min(count, pageSize);
        n = end - start;
        std::memcpy(reinterpret_cast<void*>(start), source, n);
        count -= n;
        source += n;
        targetVirtualAddress += n;
    }
}

uint64_t Memory::TranslateAddress(uint64_t rv, uint64_t virtualAddress, Protection access)
{
    int64_t pageOffset = 0;
    return TranslateAddress(rv, virtualAddress, access, pageOffset);
}

uint64_t Memory::TranslateAddress(uint64_t rv, uint64_t virtualAddress, Protection access, int64_t& pageOffset)
{
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
    pageOffset = MakePageOffset(virtualAddress);
    auto it = translationMap.find(pageNumber);
    if (it != translationMap.cend())
    {
        MemoryPage* page = it->second;
        return page->Address() + pageOffset;
    }
    if ((access & Protection::write) != Protection::notPresent)
    {
        MemoryPage* page = AllocatePage();
        translationMap[pageNumber] = page;
        return page->Address() + pageOffset;
    }
    else
    {
        throw std::runtime_error("virtual address #" + ToHexString(virtualAddress) + " not found from memory translation map #" + ToHexString(rv));
    }
}

} // cmsx::machine
