// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/Statics.hpp>
#include <cmajor/rt/Error.hpp>
#include <cmajor/rt/Io.hpp>
#include <soulng/util/Error.hpp>
#include <boost/functional/hash.hpp>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <sstream>
#include <unordered_map>

namespace cmajor {namespace rt {

class StaticInitTable
{
public:
    static void Init();
    static void Done();
    static StaticInitTable& Instance() { return *instance; }
    void AllocateMutexes(const std::vector<boost::uuids::uuid>& staticClassIds);
    void BeginCriticalSection(const boost::uuids::uuid& classId);
    void EndCriticalSection(const boost::uuids::uuid& classId);
private:
    static std::unique_ptr<StaticInitTable> instance;
    std::vector<std::unique_ptr<std::recursive_mutex>> mutexes;
    std::unordered_map<boost::uuids::uuid, int, boost::hash<boost::uuids::uuid>> mutexMap;
};

std::unique_ptr<StaticInitTable> StaticInitTable::instance;

void StaticInitTable::Init()
{
    instance.reset(new StaticInitTable());
}

void StaticInitTable::Done()
{
    instance.reset();
}

void StaticInitTable::AllocateMutexes(const std::vector<boost::uuids::uuid>& staticClassIds)
{
    int n = staticClassIds.size();
    for (int i = 0; i < n; ++i)
    {
        const boost::uuids::uuid& classId = staticClassIds[i];
        mutexMap[classId] = mutexes.size();
        mutexes.push_back(std::unique_ptr<std::recursive_mutex>(new std::recursive_mutex()));
    }
}

void StaticInitTable::BeginCriticalSection(const boost::uuids::uuid& classId)
{
    auto it = mutexMap.find(classId);
    if (it != mutexMap.cend())
    {
        int mutexIndex = it->second;
        Assert(mutexIndex >= 0 && mutexIndex < mutexes.size(), "invalid mutex index");
        std::recursive_mutex* mutex = mutexes[mutexIndex].get();
        mutex->lock();
    }
    else
    {
        Assert(false, "invalid class id");
    }
}

void StaticInitTable::EndCriticalSection(const boost::uuids::uuid& classId)
{
    auto it = mutexMap.find(classId);
    if (it != mutexMap.cend())
    {
        int mutexIndex = it->second;
        Assert(mutexIndex >= 0 && mutexIndex < mutexes.size(), "invalid mutex index");
        std::recursive_mutex* mutex = mutexes[mutexIndex].get();
        mutex->unlock();
    }
    else
    {
        Assert(false, "invalid class id");
    }
}

void AllocateMutexes(const std::vector<boost::uuids::uuid>& staticClassIds)
{
    StaticInitTable::Instance().AllocateMutexes(staticClassIds);
}

typedef void(*destructor_ptr)(void* arg);

struct Destruction
{
    Destruction(destructor_ptr destructor_, void* arg_, Destruction* next_) : destructor(destructor_), arg(arg_), next(next_)
    {
    }
    destructor_ptr destructor;
    void* arg;
    Destruction* next;
};

Destruction* destructionList = nullptr;

void ExecuteDestructors()
{
    Destruction* destruction = destructionList;
    while (destruction)
    {
        destructionList = destructionList->next;
        destruction->destructor(destruction->arg);
        delete destruction;
        destruction = destructionList;
    }
}

void InitStatics()
{
    StaticInitTable::Init();
}

void DoneStatics()
{
    ExecuteDestructors();
    StaticInitTable::Done();
}

} }  // namespace cmajor::rt

extern "C" RT_API void RtBeginStaticInitCriticalSection(void* staticClassId)
{
    try
    {
        boost::uuids::uuid* classId = reinterpret_cast<boost::uuids::uuid*>(staticClassId);
        cmajor::rt::StaticInitTable::Instance().BeginCriticalSection(*classId);
    }
    catch (const std::exception& ex)
    {
        std::stringstream s;
        s << "internal error: " << ex.what() << "\n";
        std::string str = s.str();
        RtWrite(stdErrFileHandle, reinterpret_cast<const uint8_t*>(str.c_str()), str.length());
        exit(exitCodeInternalError);
    }
}

extern "C" RT_API void RtEndStaticInitCriticalSection(void* staticClassId)
{
    try
    {
        boost::uuids::uuid* classId = reinterpret_cast<boost::uuids::uuid*>(staticClassId);
        cmajor::rt::StaticInitTable::Instance().EndCriticalSection(*classId);
    }
    catch (const std::exception& ex)
    {
        std::stringstream s;
        s << "internal error: " << ex.what() << "\n";
        std::string str = s.str();
        RtWrite(stdErrFileHandle, reinterpret_cast<const uint8_t*>(str.c_str()), str.length());
        exit(exitCodeInternalError);
    }
}

std::mutex destructionListMutex;
 
extern "C" RT_API void RtEnqueueDestruction(void* destructor, void* arg)
{
    std::lock_guard<std::mutex> lock(destructionListMutex);
    cmajor::rt::destructionList = new cmajor::rt::Destruction(reinterpret_cast<cmajor::rt::destructor_ptr>(destructor), arg, cmajor::rt::destructionList);
}

