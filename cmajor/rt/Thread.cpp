// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/Thread.hpp>
#include <cmajor/rt/Io.hpp>
#include <cmajor/rt/CallStack.hpp>
#include <cmajor/rt/Error.hpp>
#include <cmajor/rt/InitDone.hpp>
#include <soulng/util/Error.hpp>
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>
#include <sstream>
#include <unordered_map>

namespace cmajor { namespace rt {

typedef void(*ThreadFunction)();
typedef void(*ThreadFunctionWithParam)(void*);
typedef void(*ThreadMethod)(void*);
typedef void(*ThreadMethodWithParam)(void*, void*);

void ExecuteThreadFunction(ThreadFunction threadFunction, int32_t threadId)
{
    try
    {
        threadFunction();
    }
    catch (...)
    {
        std::string str = "exception escaped from thread " + std::to_string(threadId) + "\n";
        RtWrite(stdErrFileHandle, reinterpret_cast<const uint8_t*>(str.c_str()), str.length());
        RtPrintCallStack(stdErrFileHandle);
        RtExit(exitCodeExceptionEscapedFromThread);
    }
}

void ExecuteThreadFunctionWithParam(ThreadFunctionWithParam threadFunction, void* param, int32_t threadId)
{
    try
    {
        threadFunction(param);
    }
    catch (...)
    {
        std::string str = "exception escaped from thread " + std::to_string(threadId) + "\n";
        RtWrite(stdErrFileHandle, reinterpret_cast<const uint8_t*>(str.c_str()), str.length());
        RtPrintCallStack(stdErrFileHandle);
        RtExit(exitCodeExceptionEscapedFromThread);
    }
}

void ExecuteThreadMethod(ThreadMethod threadMethod, void* object, int32_t threadId)
{
    try
    {
        threadMethod(object);
    }
    catch (...)
    {
        std::string str = "exception escaped from thread " + std::to_string(threadId) + "\n";
        RtWrite(stdErrFileHandle, reinterpret_cast<const uint8_t*>(str.c_str()), str.length());
        RtPrintCallStack(stdErrFileHandle);
        RtExit(exitCodeExceptionEscapedFromThread);
    }
}

void ExecuteThreadMethodWithParam(ThreadMethodWithParam threadMethod, void* object, void* param, int32_t threadId)
{
    try
    {
        threadMethod(object, param);
    }
    catch (...)
    {
        std::string str = "exception escaped from thread " + std::to_string(threadId) + "\n";
        RtWrite(stdErrFileHandle, reinterpret_cast<const uint8_t*>(str.c_str()), str.length());
        RtPrintCallStack(stdErrFileHandle);
        RtExit(exitCodeExceptionEscapedFromThread);
    }
}

class ThreadPool
{
public:
    static void Init();
    static void Done();
    void Exit();
    static ThreadPool& Instance() { Assert(instance, "thread pool not initialized"); return *instance; }
    int32_t StartThreadFunction(ThreadFunction fun);
    int32_t StartThreadFunction(ThreadFunctionWithParam fun, void* param);
    int32_t StartThreadMethod(ThreadMethod method, void* object);
    int32_t StartThreadMethod(ThreadMethodWithParam method, void* object, void* param);
    bool JoinThread(int32_t threadId);
private:
    static std::unique_ptr<ThreadPool> instance;
    const int32_t numNoLockThreads = 256;
    std::atomic<int32_t> nextThreadId;
    std::vector<std::unique_ptr<std::thread>> noLockThreads;
    std::unordered_map<int32_t, std::unique_ptr<std::thread>> threadMap;
    std::mutex mtx;
    ThreadPool();
};

void ThreadPool::Exit()
{
    for (std::unique_ptr<std::thread>& t : noLockThreads)
    {
        if (t.get())
        {
            if (t->joinable())
            {
                t->join();
            }
        }
    }
    for (auto& p : threadMap)
    {
        JoinThread(p.first);
    }
}

void ThreadPool::Init()
{
    instance.reset(new ThreadPool());
}

void ThreadPool::Done()
{
    if (instance)
    {
        instance->Exit();
    }
    instance.reset();
}

std::unique_ptr<ThreadPool> ThreadPool::instance;

ThreadPool::ThreadPool() : nextThreadId(1), noLockThreads()
{
    noLockThreads.resize(numNoLockThreads);
}

int32_t ThreadPool::StartThreadFunction(ThreadFunction fun)
{
    int32_t threadId = nextThreadId++;
    if (threadId < numNoLockThreads)
    {
        noLockThreads[threadId].reset(new std::thread(ExecuteThreadFunction, fun, threadId));
        return threadId;
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        threadMap[threadId].reset(new std::thread(ExecuteThreadFunction, fun, threadId));
        return threadId;
    }
}

int32_t ThreadPool::StartThreadFunction(ThreadFunctionWithParam fun, void* param)
{
    int32_t threadId = nextThreadId++;
    if (threadId < numNoLockThreads)
    {
        noLockThreads[threadId].reset(new std::thread(ExecuteThreadFunctionWithParam, fun, param, threadId));
        return threadId;
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        threadMap[threadId].reset(new std::thread(ExecuteThreadFunctionWithParam, fun, param, threadId));
        return threadId;
    }
}

int32_t ThreadPool::StartThreadMethod(ThreadMethod method, void* object)
{
    int32_t threadId = nextThreadId++;
    if (threadId < numNoLockThreads)
    {
        noLockThreads[threadId].reset(new std::thread(ExecuteThreadMethod, method, object, threadId));
        return threadId;
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        threadMap[threadId].reset(new std::thread(ExecuteThreadMethod, method, object, threadId));
        return threadId;
    }
}

int32_t ThreadPool::StartThreadMethod(ThreadMethodWithParam method, void* object, void* param)
{
    int32_t threadId = nextThreadId++;
    if (threadId < numNoLockThreads)
    {
        noLockThreads[threadId].reset(new std::thread(ExecuteThreadMethodWithParam, method, object, param, threadId));
        return threadId;
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        threadMap[threadId].reset(new std::thread(ExecuteThreadMethodWithParam, method, object, param, threadId));
        return threadId;
    }
}

bool ThreadPool::JoinThread(int32_t threadId)
{
    if (threadId < numNoLockThreads)
    {
        if (noLockThreads[threadId])
        {
            if (noLockThreads[threadId]->joinable())
            {
                noLockThreads[threadId]->join();
            }
            noLockThreads[threadId].reset();
            return true;
        }
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = threadMap.find(threadId);
        if (it != threadMap.cend())
        {
            std::thread* thread = it->second.get();
            if (thread)
            {
                if (thread->joinable())
                {
                    thread->join();
                }
                threadMap.erase(threadId);
                return true;
            }
        }
    }
    return false;
}

void InitThread()
{
    ThreadPool::Init();
}

void DoneThread() 
{
    ThreadPool::Done();
}

} } // namespace cmajor::rt

extern "C" RT_API int32_t RtGetHardwareConcurrency()
{
    return std::thread::hardware_concurrency();
}

extern "C" RT_API int32_t RtStartThreadFunction(void* function)
{
    cmajor::rt::ThreadFunction threadFun = reinterpret_cast<cmajor::rt::ThreadFunction>(function);
    return cmajor::rt::ThreadPool::Instance().StartThreadFunction(threadFun);
}

extern "C" RT_API int32_t RtStartThreadFunctionWithParam(void* function, void* param)
{
    cmajor::rt::ThreadFunctionWithParam threadFunWithParam = reinterpret_cast<cmajor::rt::ThreadFunctionWithParam>(function);
    return cmajor::rt::ThreadPool::Instance().StartThreadFunction(threadFunWithParam, param);
}

struct ClassDelegate
{
    void* object;
    void* method;
};

extern "C" RT_API int32_t RtStartThreadMethod(void* classDelegate)
{
    ClassDelegate* clsDlg = reinterpret_cast<ClassDelegate*>(classDelegate);
    cmajor::rt::ThreadMethod threadMethod = reinterpret_cast<cmajor::rt::ThreadMethod>(clsDlg->method);
    return cmajor::rt::ThreadPool::Instance().StartThreadMethod(threadMethod, clsDlg->object);
}

extern "C" RT_API int32_t RtStartThreadMethodWithParam(void* classDelegate, void* param)
{
    ClassDelegate* clsDlg = reinterpret_cast<ClassDelegate*>(classDelegate);
    cmajor::rt::ThreadMethodWithParam threadMethodWithParam = reinterpret_cast<cmajor::rt::ThreadMethodWithParam>(clsDlg->method);
    return cmajor::rt::ThreadPool::Instance().StartThreadMethod(threadMethodWithParam, clsDlg->object, param);
}

extern "C" RT_API bool RtJoinThread(int32_t threadId)
{
    return cmajor::rt::ThreadPool::Instance().JoinThread(threadId);
}

std::unordered_map<std::thread::id, int> threadIdMap;

int nextThreadId = 0;
std::mutex threadIdMapMutex;

extern "C" RT_API int32_t RtThisThreadId()
{
    std::lock_guard<std::mutex> lock(threadIdMapMutex);
    std::thread::id id = std::this_thread::get_id();
    auto it = threadIdMap.find(id);
    if (it != threadIdMap.cend())
    {
        return it->second;
    }
    int threadId = nextThreadId++;
    threadIdMap[id] = threadId;
    return threadId;
}
