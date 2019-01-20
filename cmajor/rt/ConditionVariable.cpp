// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/ConditionVariable.hpp>
#include <cmajor/rt/Mutex.hpp>
#include <cmajor/rt/Error.hpp>
#include <cmajor/util/Error.hpp>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <unordered_map>
#include <string>
#include <stdexcept>

namespace cmajor { namespace rt {

class ConditionVariableTable
{
public:
    static void Init();
    static void Done();
    static ConditionVariableTable& Instance() { Assert(instance, "condition variable table not initialized"); return *instance; }
    int32_t AllocateConditionVariable();
    void FreeConditionVariable(int32_t conditionVariableId);
    std::condition_variable_any& GetConditionVariable(int32_t conditionVariableId);
private:
    const int32_t maxNoLockConditionVariables = 256;
    static std::unique_ptr<ConditionVariableTable> instance;
    std::atomic<int32_t> nextConditionVariableId;
    std::vector<std::unique_ptr<std::condition_variable_any>> conditionVariables;
    std::unordered_map<int32_t, std::unique_ptr<std::condition_variable_any>> conditionVariableMap;
    std::mutex mtx;
    ConditionVariableTable();
};

std::unique_ptr<ConditionVariableTable> ConditionVariableTable::instance;

void ConditionVariableTable::Init()
{
    instance.reset(new ConditionVariableTable());
}

void ConditionVariableTable::Done()
{
    instance.reset();
}

ConditionVariableTable::ConditionVariableTable() : nextConditionVariableId(1)
{
    conditionVariables.resize(maxNoLockConditionVariables);
}

int32_t ConditionVariableTable::AllocateConditionVariable()
{
    int32_t conditionVariableId = nextConditionVariableId++;
    Assert(conditionVariableId > 0, "invalid condition variable id");
    if (conditionVariableId < maxNoLockConditionVariables)
    {
        conditionVariables[conditionVariableId].reset(new std::condition_variable_any());
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        conditionVariableMap[conditionVariableId].reset(new std::condition_variable_any());
    }
    return conditionVariableId;
}

void ConditionVariableTable::FreeConditionVariable(int32_t conditionVariableId)
{
    if (conditionVariableId <= 0)
    {
        throw std::runtime_error("invalid condition variable id " + std::to_string(conditionVariableId));
    }
    if (conditionVariableId < maxNoLockConditionVariables)
    {
        conditionVariables[conditionVariableId].reset();
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        conditionVariableMap.erase(conditionVariableId);
    }
}

std::condition_variable_any& ConditionVariableTable::GetConditionVariable(int32_t conditionVariableId)
{
    if (conditionVariableId <= 0)
    {
        throw std::runtime_error("invalid condition variable id " + std::to_string(conditionVariableId));
    }
    else if (conditionVariableId < maxNoLockConditionVariables)
    {
        std::condition_variable_any* c = conditionVariables[conditionVariableId].get();
        if (c)
        {
            return *c;
        }
        else
        {
            throw std::runtime_error("invalid condition variable id " + std::to_string(conditionVariableId));
        }
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = conditionVariableMap.find(conditionVariableId);
        if (it != conditionVariableMap.cend())
        {
            std::condition_variable_any* c = it->second.get();
            if (c)
            {
                return *c;
            }
            else
            {
                throw std::runtime_error("invalid condition variable id " + std::to_string(conditionVariableId));
            }
        }
        else
        {
            throw std::runtime_error("invalid condition variable id " + std::to_string(conditionVariableId));
        }
    }
}

void InitConditionVariable()
{
    ConditionVariableTable::Init();
}

void DoneConditionVariable()
{
    ConditionVariableTable::Done();
}

} } // namespace cmajor::rt

extern "C" RT_API int32_t RtAllocateConditionVariable()
{
    return cmajor::rt::ConditionVariableTable::Instance().AllocateConditionVariable();
}

extern "C" RT_API int32_t RtFreeConditionVariable(int32_t conditionVariableId)
{
    try
    {
        cmajor::rt::ConditionVariableTable::Instance().FreeConditionVariable(conditionVariableId);
        return 0;
    }
    catch (const std::exception& ex)
    {
        return cmajor::rt::InstallError(ex.what());
    }
}

extern "C" RT_API int32_t RtNotifyOne(int32_t conditionVariableId)
{
    try
    {
        std::condition_variable_any& conditionVariable = cmajor::rt::ConditionVariableTable::Instance().GetConditionVariable(conditionVariableId);
        conditionVariable.notify_one();
        return 0;
    }
    catch (const std::exception& ex)
    {
        return cmajor::rt::InstallError(ex.what());
    }
}

extern "C" RT_API int32_t RtNotifyAll(int32_t conditionVariableId)
{
    try
    {
        std::condition_variable_any& conditionVariable = cmajor::rt::ConditionVariableTable::Instance().GetConditionVariable(conditionVariableId);
        conditionVariable.notify_all();
        return 0;
    }
    catch (const std::exception& ex)
    {
        return cmajor::rt::InstallError(ex.what());
    }
}

struct RecursiveMutex
{
    int32_t id;
};

extern "C" RT_API int32_t RtWaitConditionVariable(int32_t conditionVariableId, void* recursiveMutex)
{
    try
    {
        RecursiveMutex* m = reinterpret_cast<RecursiveMutex*>(recursiveMutex);
        int32_t mutexId = m->id;
        std::recursive_mutex& mtx = cmajor::rt::GetRecursiveMutex(mutexId);
        std::condition_variable_any& conditionVariable = cmajor::rt::ConditionVariableTable::Instance().GetConditionVariable(conditionVariableId);
        conditionVariable.wait(mtx);
        return 0;
    }
    catch (const std::exception& ex)
    {
        return cmajor::rt::InstallError(ex.what());
    }
}

enum class CondVarStatus : int32_t
{
    timeout = 0, no_timeout = 1
};

extern "C" RT_API int32_t RtWaitConditionVariableDuration(int32_t conditionVariableId, void* recursiveMutex, int64_t nanoseconds)
{
    try
    {
        RecursiveMutex* m = reinterpret_cast<RecursiveMutex*>(recursiveMutex);
        int32_t mutexId = m->id;
        std::recursive_mutex& mtx = cmajor::rt::GetRecursiveMutex(mutexId);
        std::condition_variable_any& conditionVariable = cmajor::rt::ConditionVariableTable::Instance().GetConditionVariable(conditionVariableId);
        CondVarStatus status = CondVarStatus::timeout;
        std::chrono::nanoseconds duration(nanoseconds);
        if (conditionVariable.wait_for(mtx, duration) == std::cv_status::no_timeout)
        {
            status = CondVarStatus::no_timeout;
        }
        return static_cast<int32_t>(status);
    }
    catch (const std::exception& ex)
    {
        return cmajor::rt::InstallError(ex.what());
    }
}
