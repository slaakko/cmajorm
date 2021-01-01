// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/Memory.hpp>
#include <cmajor/rt/Error.hpp>
#include <cmajor/rt/Io.hpp>
#include <cmajor/rt/CallStack.hpp>
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <memory>
#include <malloc.h>
#include <fstream>
#include <vector>
#include <cstring>

namespace cmajor { namespace rt {

class Allocation
{
public:
    Allocation();
    Allocation(int serial_, int64_t size_, const char* info_);
    void Print(const std::string& title) const;
    int Serial() const { return serial; }
    void SetSerial(int serial_) { serial = serial_; }
    int64_t Size() const { return size; }
    void SetSize(int64_t size_) { size = size_; }
    bool Disposed() const { return disposed; }
    void SetDisposed(bool disposed_) { disposed = disposed_; }
    const char* Info() const { return info; }
    void SetInfo(const char* info_) { info = info_; }
private:
    int serial;
    int64_t size;
    bool disposed;
    const char* info;
};

Allocation::Allocation() : serial(0), size(0), disposed(false), info(nullptr)
{
}

Allocation::Allocation(int serial_, int64_t size_, const char* info_) : serial(serial_), size(size_), disposed(false), info(info_)
{
}

void Allocation::Print(const std::string& title) const
{
    std::string s = title + " allocation #" + std::to_string(serial) + " : size=" + std::to_string(size) + " : disposed=" + (disposed ? "true" : "false");
    if (info != nullptr)
    {
        s.append(" : info '").append(info).append("'");
    }
    s.append("\n");
    RtWrite(2, (const uint8_t*)s.c_str(), s.length());
}

struct SerialLess
{
    bool operator()(const Allocation* left, const Allocation* right) const
    {
        return left->Serial() < right->Serial();
    }
};

class DebugHeap
{
public:
    static void Init();
    static void Done();
    static DebugHeap& Instance() { return *instance; }
    void SetDebugHeap() { debugHeap = true; }
    bool GetDebugHeap() const { return debugHeap; }
    void SetDebugSerial(int debugSerial_) { debugSerial = debugSerial_; }
    int GetDebugSerial() const { return debugSerial; }
    void Allocate(void* ptr, int64_t size, const char* info);
    void Dispose(void* ptr);
    int NextSerial() { return ++serial; }
    void PrintLeaks();
private:
    static std::unique_ptr<DebugHeap> instance;
    DebugHeap();
    bool debugHeap;
    int debugSerial;
    int serial;
    std::unordered_map<void*, Allocation> allocationMap;
};

std::unique_ptr<DebugHeap> DebugHeap::instance;

DebugHeap::DebugHeap() : debugHeap(false), debugSerial(0), serial(0)
{
}

void DebugHeap::Allocate(void* ptr, int64_t size, const char* info)
{
    auto it = allocationMap.find(ptr);
    if (it == allocationMap.cend())
    {
        allocationMap[ptr] = Allocation(serial, size, info);
        if (serial == debugSerial)
        {
            allocationMap[ptr].Print("allocating");
            RtPrintCallStack(2);
        }
    }
    else
    {
        Allocation& alloc = it->second;
        alloc.SetSerial(serial);
        alloc.SetSize(size);
        alloc.SetDisposed(false);
        alloc.SetInfo(info);
    }
}

void DebugHeap::Dispose(void* ptr)
{
    if (!ptr) return;
    auto it = allocationMap.find(ptr);
    if (it != allocationMap.cend())
    {
        Allocation& alloc = it->second;
        if (alloc.Disposed())
        {
            alloc.Print("dangling");
            if (debugSerial == alloc.Serial())
            {
                RtPrintCallStack(2);
            }
        }
        else
        {
            if (debugSerial == alloc.Serial())
            {
                alloc.Print("disposing");
                RtPrintCallStack(2);
            }
            alloc.SetDisposed(true);
        }
    }
    else if (debugHeap)
    {
        std::string s = "disposing : allocation not found\n";
        RtWrite(2, (const uint8_t*)s.c_str(), s.length());
        RtPrintCallStack(2);
    }
}

void DebugHeap::PrintLeaks()
{
    std::vector<const Allocation*> leaks;
    for (const auto& p : allocationMap)
    {
        const Allocation& alloc = p.second;
        if (!alloc.Disposed())
        {
            leaks.push_back(&alloc);
        }
    }
    if (!leaks.empty())
    {
        std::string title = std::to_string(leaks.size()) + " memory leaks:\n";
        RtWrite(2, (const uint8_t*)title.c_str(), title.size());
        std::sort(leaks.begin(), leaks.end(), SerialLess());
        int i = 0;
        for (const Allocation* leak : leaks)
        {
            leak->Print(std::to_string(i) + ": leaked");
            ++i;
        }
    }
}

void DebugHeap::Init()
{
    instance.reset(new DebugHeap());
}

void DebugHeap::Done()
{
    if (instance->GetDebugHeap())
    {
        instance->PrintLeaks();
    }
    instance.reset();
}

void SetDebugHeap()
{
    DebugHeap::Instance().SetDebugHeap();
}

void SetDebugAllocation(int debugSerial)
{
    DebugHeap::Instance().SetDebugSerial(debugSerial);
}

void InitMemory()
{
    DebugHeap::Init();
}

void DoneMemory()
{
    DebugHeap::Done();
}

} }  // namespace cmajor::rt

extern "C" RT_API void* RtMemAllocInfo(int64_t size, const char* info)
{
    void* ptr = malloc(size);
    if (!ptr)
    {
        std::stringstream s;
        s << "program out of memory\n";
        std::string str = s.str();
        RtWrite(stdErrFileHandle, reinterpret_cast<const uint8_t*>(str.c_str()), str.length());
        RtPrintCallStack(stdErrFileHandle);
        exit(exitCodeOutOfMemory);
    }
    int serial = cmajor::rt::DebugHeap::Instance().NextSerial();
    if (cmajor::rt::DebugHeap::Instance().GetDebugHeap() || serial == cmajor::rt::DebugHeap::Instance().GetDebugSerial())
    {
        cmajor::rt::DebugHeap::Instance().Allocate(ptr, size, info);
    }
    return ptr;
}

extern "C" RT_API void* RtMemAlloc(int64_t size)
{
    return RtMemAllocInfo(size, nullptr);
}

extern "C" RT_API void RtDispose(void* ptr)
{
    if (cmajor::rt::DebugHeap::Instance().GetDebugHeap() || cmajor::rt::DebugHeap::Instance().GetDebugSerial() != 0)
    {
        cmajor::rt::DebugHeap::Instance().Dispose(ptr);
    }
}

extern "C" RT_API void RtMemFree(void* ptr)
{
    free(ptr);
}

extern "C" RT_API void RtMemZero(void* ptr, int64_t size)
{
    std::memset(ptr, 0, size);
}
