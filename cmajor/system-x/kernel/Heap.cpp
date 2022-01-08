// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Heap.hpp>
#include <system-x/kernel/ProcessManager.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <fstream>

namespace cmsx::kernel {

using namespace soulng::unicode;
using namespace soulng::util;

std::string LogDir()
{
    std::string root = CmajorRoot();
    return GetFullPath(Path::Combine(root, "log"));
}

std::string HeapLogFilePath()
{
    return Path::Combine(LogDir(), "heap.log");
}

void Print(uint64_t a, uint64_t x, const std::string& name, CodeFormatter& formatter)
{
    formatter.WriteLine("#" + ToHexString(a) + ": " + name + ": #" + ToHexString(x));
}

void DumpHeap(cmsx::machine::Memory& memory, uint64_t freeAddr)
{
    std::ofstream file(HeapLogFilePath());
    CodeFormatter formatter(file);
    Process* currentProcess = ProcessManager::Instance().CurrentProcess();
    if (currentProcess)
    {
        int64_t heapStart = currentProcess->HeapStartAddress();
        formatter.WriteLine("heap start:  #" + ToHexString(static_cast<uint64_t>(heapStart)));
        int64_t heapLength = currentProcess->HeapLength();
        formatter.WriteLine("heap length: #" + ToHexString(static_cast<uint64_t>(heapLength)));
    }
    uint64_t next = memory.ReadOcta(freeAddr, cmsx::machine::Protection::read);
    Print(freeAddr, next, "next", formatter);
    uint64_t size = memory.ReadOcta(freeAddr + 8, cmsx::machine::Protection::read);
    Print(freeAddr + 8, size, "size", formatter);
    while (next != freeAddr)
    {
        uint64_t n = memory.ReadOcta(next, cmsx::machine::Protection::read);
        uint64_t s = memory.ReadOcta(next + 8, cmsx::machine::Protection::read);
        Print(next, n, "next", formatter);
        Print(next + 8, s, "size", formatter);
        next = n;
    }
}

} // namespace cmsx::kernel
