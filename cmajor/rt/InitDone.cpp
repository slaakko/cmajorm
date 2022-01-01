// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/InitDone.hpp>
#include <cmajor/rt/Error.hpp>
#include <soulng/util/InitDone.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <cmajor/rt/Io.hpp>
#include <cmajor/rt/Directory.hpp>
#include <cmajor/rt/CallStack.hpp>
#include <cmajor/rt/Classes.hpp>
#include <cmajor/rt/Statics.hpp>
#include <cmajor/rt/String.hpp>
#include <cmajor/rt/Mutex.hpp>
#include <cmajor/rt/ConditionVariable.hpp>
#include <cmajor/rt/Thread.hpp>
#include <cmajor/rt/Memory.hpp>
#include <cmajor/rt/Screen.hpp>
#ifdef _WIN32
#include <cmajor/rt/CommandLine.hpp>
#include <Windows.h>
#endif
#include <cmajor/rt/Socket.hpp>
#include <cmajor/rt/Environment.hpp>
#include <cmajor/rt/Unwind.hpp>
#include <cmajor/rt/Debug.hpp>
#include <mutex>

GlobalInitFunctionType initCompileUnitsFunction = nullptr;

void RtInit(int64_t numberOfPolymorphicClassIds, const uint64_t* polymorphicClassIdArray, int64_t numberOfStaticClassIds, const uint64_t* staticClassIdArray,
    GlobalInitFunctionType globalInitializationFunction)
{
    cmajor::rt::Init(numberOfPolymorphicClassIds, polymorphicClassIdArray, numberOfStaticClassIds, staticClassIdArray);
    initCompileUnitsFunction = globalInitializationFunction;
}

bool DynamicInitVmtsAndCompare(void* vmt1, void* vmt2)
{
    return cmajor::rt::DynamicInitVmtsAndCompare(vmt1, vmt2);
}

void RtDone()
{
    cmajor::rt::Done();
}

void RtExit(int32_t exitCode)
{
    exit(exitCode);
}

std::recursive_mutex initMutex;

void RtInitCompileUnits()
{
    std::lock_guard<std::recursive_mutex> initLock(initMutex);
    if (initCompileUnitsFunction)
    {
        GlobalInitFunctionType init = initCompileUnitsFunction;
        initCompileUnitsFunction = nullptr;
        init();
    }
}

void RtBeginUnwindInfoInit()
{
    initMutex.lock();
    RtInitCompileUnits();
}

void RtEndUnwindInfoInit()
{
    initMutex.unlock();
}

namespace cmajor { namespace rt {

void Init(int64_t numberOfPolymorphicClassIds, const uint64_t* polymorphicClassIdArray, int64_t numberOfStaticClassIds, const uint64_t* staticClassIdArray)
{
    soulng::util::Init();
    sngxml::xpath::Init();
    InitIo();
    InitScreen();
    InitDirectory();
    InitError();
    InitString();
    InitMemory();
    InitThread();
    InitSocket();
    InitEnvironment();
    InitStatics();
    InitClasses(numberOfPolymorphicClassIds, polymorphicClassIdArray, numberOfStaticClassIds, staticClassIdArray);
#ifdef _WIN32
    InitCommandLine();
#endif
    InitUnwind();
    InitCmdbSession();
    StartCmdbSession();
}

void Done()
{
    DoneCmdbSession();
    DoneUnwind();
#ifdef _WIN32
    DoneCommandLine();
#endif
    DoneClasses();
    DoneStatics();
    DoneEnvironment();
    DoneSocket();
    DoneThread();
    DoneMemory();
    DoneString();
    DoneError();
    DoneDirectory();
    DoneScreen();
    DoneIo();
    sngxml::xpath::Done();
    soulng::util::Done();
}

} } // namespace cmajor::rt
