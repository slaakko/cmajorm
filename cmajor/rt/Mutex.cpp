// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/Mutex.hpp>
#include <soulng/util/Error.hpp>
#include <atomic>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

namespace cmajor { namespace rt {

class MutexTable
{
public:
    static void Init();
    static void Done();
    static MutexTable& Instance() { Assert(instance, "mutex table not initialized"); return *instance; }
    int32_t AllocateMutex();
    void FreeMutex(int32_t mutexId);
    void LockMutex(int32_t mutexId);
    void UnlockMutex(int32_t mutexId);
private:
    MutexTable();
    static std::unique_ptr<MutexTable> instance;
    const int32_t numNoLockMutexes = 256;
    std::atomic<int32_t> nextMutexId;
    std::vector<std::unique_ptr<std::mutex>> noLockMutexes;
    std::unordered_map<int32_t, std::unique_ptr<std::mutex>> mutexMap;
    std::mutex mtx;
};

std::unique_ptr<MutexTable> MutexTable::instance;

void MutexTable::Init()
{
    instance.reset(new MutexTable());
}

void MutexTable::Done()
{
    instance.reset();
}

MutexTable::MutexTable() : nextMutexId(1), noLockMutexes()
{
    noLockMutexes.resize(256);
}

int32_t MutexTable::AllocateMutex()
{
    int32_t mutexId = nextMutexId++;
    if (mutexId < numNoLockMutexes)
    {
        noLockMutexes[mutexId].reset(new std::mutex());
        return mutexId;
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        mutexMap[mutexId].reset(new std::mutex());
        return mutexId;
    }
}

void MutexTable::FreeMutex(int32_t mutexId)
{
    if (mutexId < numNoLockMutexes)
    {
        noLockMutexes[mutexId].reset();
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        mutexMap[mutexId].reset();
    }
}

void MutexTable::LockMutex(int32_t mutexId)
{
    if (mutexId < numNoLockMutexes)
    {
        noLockMutexes[mutexId]->lock();
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        mutexMap[mutexId]->lock();
    }
}

void MutexTable::UnlockMutex(int32_t mutexId)
{
    if (mutexId < numNoLockMutexes)
    {
        noLockMutexes[mutexId]->unlock();
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        mutexMap[mutexId]->unlock();;
    }
}

class RecursiveMutexTable
{
public:
    static void Init();
    static void Done();
    static RecursiveMutexTable& Instance() { Assert(instance, "recursive mutex table not initialized"); return *instance; }
    int32_t AllocateRecursiveMutex();
    void FreeRecursiveMutex(int32_t mutexId);
    void LockRecursiveMutex(int32_t mutexId);
    void UnlockRecursiveMutex(int32_t mutexId);
    std::recursive_mutex& GetRecursiveMutex(int32_t mutexId);
private:
    RecursiveMutexTable();
    static std::unique_ptr<RecursiveMutexTable> instance;
    const int32_t numNoLockMutexes = 256;
    std::atomic<int32_t> nextMutexId;
    std::vector<std::unique_ptr<std::recursive_mutex>> noLockMutexes;
    std::unordered_map<int32_t, std::unique_ptr<std::recursive_mutex>> mutexMap;
    std::mutex mtx;
};

std::unique_ptr<RecursiveMutexTable> RecursiveMutexTable::instance;

void RecursiveMutexTable::Init()
{
    instance.reset(new RecursiveMutexTable());
}

void RecursiveMutexTable::Done()
{
    instance.reset();
}

RecursiveMutexTable::RecursiveMutexTable() : nextMutexId(1), noLockMutexes()
{
    noLockMutexes.resize(256);
}

int32_t RecursiveMutexTable::AllocateRecursiveMutex()
{
    int32_t mutexId = nextMutexId++;
    if (mutexId < numNoLockMutexes)
    {
        noLockMutexes[mutexId].reset(new std::recursive_mutex());
        return mutexId;
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        mutexMap[mutexId].reset(new std::recursive_mutex());
        return mutexId;
    }
}

void RecursiveMutexTable::FreeRecursiveMutex(int32_t mutexId)
{
    if (mutexId < numNoLockMutexes)
    {
        noLockMutexes[mutexId].reset();
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        mutexMap[mutexId].reset();
    }
}

void RecursiveMutexTable::LockRecursiveMutex(int32_t mutexId)
{
    if (mutexId < numNoLockMutexes)
    {
        noLockMutexes[mutexId]->lock();
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        mutexMap[mutexId]->lock();
    }
}

void RecursiveMutexTable::UnlockRecursiveMutex(int32_t mutexId)
{
    if (mutexId < numNoLockMutexes)
    {
        noLockMutexes[mutexId]->unlock();
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        mutexMap[mutexId]->unlock();;
    }
}

std::recursive_mutex& RecursiveMutexTable::GetRecursiveMutex(int32_t mutexId)
{
    if (mutexId < numNoLockMutexes)
    {
        std::recursive_mutex* mtx = noLockMutexes[mutexId].get();
        if (!mtx)
        {
            throw std::runtime_error("invalid recursive mutex id " + std::to_string(mutexId));
        }
        return *mtx;
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = mutexMap.find(mutexId);
        if (it != mutexMap.cend())
        {
            std::recursive_mutex* mtx = it->second.get();
            if (!mtx)
            {
                throw std::runtime_error("invalid recursive mutex id " + std::to_string(mutexId));
            }
            return *mtx;
        }
        else
        {
            throw std::runtime_error("invalid recursive mutex id " + std::to_string(mutexId));
        }
    }
}

std::recursive_mutex& GetRecursiveMutex(int32_t mutexId)
{
    return RecursiveMutexTable::Instance().GetRecursiveMutex(mutexId);
}

void InitMutex()
{
    MutexTable::Init();
    RecursiveMutexTable::Init();
}

void DoneMutex()
{
    RecursiveMutexTable::Done();
    MutexTable::Done();
}

} } // namespace cmajor::rt

extern "C" RT_API int32_t RtAllocateMutex()
{
    return cmajor::rt::MutexTable::Instance().AllocateMutex();
}

extern "C" RT_API void RtFreeMutex(int32_t mutexId)
{
    cmajor::rt::MutexTable::Instance().FreeMutex(mutexId);
}

extern "C" RT_API void RtLockMutex(int32_t mutexId)
{
    cmajor::rt::MutexTable::Instance().LockMutex(mutexId);
}

extern "C" RT_API void RtUnlockMutex(int32_t mutexId)
{
    cmajor::rt::MutexTable::Instance().UnlockMutex(mutexId);
}

extern "C" RT_API int32_t RtAllocateRecursiveMutex()
{
    return cmajor::rt::RecursiveMutexTable::Instance().AllocateRecursiveMutex();
}

extern "C" RT_API void RtFreeRecursiveMutex(int32_t mutexId)
{
    cmajor::rt::RecursiveMutexTable::Instance().FreeRecursiveMutex(mutexId);
}

extern "C" RT_API void RtLockRecursiveMutex(int32_t mutexId)
{
    cmajor::rt::RecursiveMutexTable::Instance().LockRecursiveMutex(mutexId);
}

extern "C" RT_API void RtUnlockRecursiveMutex(int32_t mutexId)
{
    cmajor::rt::RecursiveMutexTable::Instance().UnlockRecursiveMutex(mutexId);
}
