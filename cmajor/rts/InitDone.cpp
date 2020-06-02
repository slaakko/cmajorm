// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rts/InitDone.hpp>
#include <cmajor/rts/Error.hpp>
#include <soulng/util/InitDone.hpp>
#include <cmajor/rts/Io.hpp>
#include <cmajor/rts/Directory.hpp>
#include <cmajor/rts/CallStack.hpp>
#include <cmajor/rts/Classes.hpp>
#include <cmajor/rts/Statics.hpp>
#include <cmajor/rts/String.hpp>
#include <cmajor/rts/Mutex.hpp>
#include <cmajor/rts/ConditionVariable.hpp>
#include <cmajor/rts/Thread.hpp>
#include <cmajor/rts/Memory.hpp>
#include <cmajor/rts/Screen.hpp>
#ifdef _WIN32
#include <cmajor/rts/CommandLine.hpp>
#include <Windows.h>
#endif
#include <cmajor/rts/Socket.hpp>
#include <cmajor/rts/Environment.hpp>
#include <cmajor/rts/Unwind.hpp>
#include <csignal>

GlobalInitFunctionType initCompileUnitsFunction = nullptr;

extern "C" RT_API void RtInit(int64_t numberOfPolymorphicClassIds, const uint64_t* polymorphicClassIdArray, int64_t numberOfStaticClassIds, const uint64_t* staticClassIdArray,
    GlobalInitFunctionType globalInitializationFunction)
{
    cmajor::rt::Init(numberOfPolymorphicClassIds, polymorphicClassIdArray, numberOfStaticClassIds, staticClassIdArray);
    initCompileUnitsFunction = globalInitializationFunction;
}

extern "C" RT_API bool DynamicInitVmtsAndCompare(void* vmt1, void* vmt2)
{
    return cmajor::rt::DynamicInitVmtsAndCompare(vmt1, vmt2);
}

extern "C" RT_API void RtDone()
{
    cmajor::rt::Done();
}

extern "C" RT_API void RtExit(int32_t exitCode)
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

extern "C" RT_API void RtBeginUnwindInfoInit()
{
    initMutex.lock();
    RtInitCompileUnits();
}

extern "C" RT_API void RtEndUnwindInfoInit()
{
    initMutex.unlock();
}

namespace cmajor { namespace rt {

void Init(int64_t numberOfPolymorphicClassIds, const uint64_t* polymorphicClassIdArray, int64_t numberOfStaticClassIds, const uint64_t* staticClassIdArray)
{
    InitMutex();
    soulng::util::Init();
    InitIo();
    InitScreen();
    InitDirectory();
    InitError();
    InitString();
    InitMemory();
    InitConditionVariable();
    InitThread();
    InitSocket();
    InitEnvironment();
    InitStatics();
    InitClasses(numberOfPolymorphicClassIds, polymorphicClassIdArray, numberOfStaticClassIds, staticClassIdArray);
#ifdef _WIN32
    InitCommandLine();
#endif
    InitUnwind();
}

void Done()
{
    DoneUnwind();
#ifdef _WIN32
    DoneCommandLine();
#endif
    DoneClasses();
    DoneStatics();
    DoneEnvironment();
    DoneSocket();
    DoneThread();
    DoneConditionVariable();
    DoneMemory();
    DoneString();
    DoneError();
    DoneDirectory();
    DoneScreen();
    DoneIo();
    soulng::util::Done();
    DoneMutex();
}

} } // namespace cmajor::rt
