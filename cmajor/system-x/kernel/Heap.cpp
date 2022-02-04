// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Heap.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/machine/Machine.hpp>
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

std::string HeapLogFilePath(uint64_t index)
{
    return Path::Combine(LogDir(), "heap" + std::to_string(index) + ".log");
}

void Print(uint64_t a, uint64_t x, const std::string& name, CodeFormatter& formatter)
{
    formatter.WriteLine("address: #" + ToHexString(a) + ": " + name + ": value: #" + ToHexString(x));
}

void DumpHeap(cmsx::machine::Processor& processor, uint64_t freeAddr, int32_t tag, uint64_t ptr, uint64_t size)
{
    static uint64_t index = 0;
    std::ofstream file(HeapLogFilePath(index++));
    CodeFormatter formatter(file);
    if (tag == 0)
    {
        formatter.WriteLine("before alloc size: #" + ToHexString(size));
    }
    else if (tag == 1)
    {
        formatter.WriteLine("after alloc ptr: #" + ToHexString(ptr));
    }
    else if (tag == 2)
    {
        formatter.WriteLine("before free ptr: #" + ToHexString(ptr));
    }
    else if (tag == 3)
    {
        formatter.WriteLine("after free:");
    }
    else if (tag == 4)
    {
        formatter.WriteLine("before resize");
    }
    else if (tag == 5)
    {
        formatter.WriteLine("after resize");
    }
    cmsx::machine::UserProcess* currentProcess = processor.CurrentProcess();
    if (currentProcess)
    {
        int64_t heapStart = currentProcess->HeapStartAddress();
        formatter.WriteLine("heap start:  #" + ToHexString(static_cast<uint64_t>(heapStart)));
        int64_t heapLength = currentProcess->HeapLength();
        formatter.WriteLine("heap length: #" + ToHexString(static_cast<uint64_t>(heapLength)));
    }
    uint64_t rv = processor.Regs().GetSpecial(cmsx::machine::rV);
    if (freeAddr != 0u)
    {
        uint64_t next = processor.GetMachine()->Mem().ReadOcta(rv, freeAddr, cmsx::machine::Protection::read);
        Print(freeAddr, next, "next", formatter);
        uint64_t blockSize = processor.GetMachine()->Mem().ReadOcta(rv, freeAddr + 8, cmsx::machine::Protection::read);
        Print(freeAddr + 8, 16 * blockSize, "size", formatter);
        while (next != freeAddr)
        {
            uint64_t n = processor.GetMachine()->Mem().ReadOcta(rv, next, cmsx::machine::Protection::read);
            uint64_t s = processor.GetMachine()->Mem().ReadOcta(rv, next + 8, cmsx::machine::Protection::read);
            Print(next, n, "next", formatter);
            Print(next + 8, 16 * s, "size", formatter);
            next = n;
        }
    }
}

void MCpy(cmsx::kernel::Process* process, uint64_t sourceBufferAddr, uint64_t targetBufferAddr, uint64_t count)
{
    process->GetProcessor()->GetMachine()->Mem().Copy(process->RV(), sourceBufferAddr, targetBufferAddr, count);
}

} // namespace cmsx::kernel
