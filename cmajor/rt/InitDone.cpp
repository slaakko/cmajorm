// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/InitDone.hpp>
#include <cmajor/rt/Error.hpp>
#include <soulng/util/InitDone.hpp>
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
#include <csignal>


extern "C" RT_API void RtInit(int64_t numberOfPolymorphicClassIds, const uint64_t* polymorphicClassIdArray, int64_t numberOfStaticClassIds, const uint64_t* staticClassIdArray)
{
    cmajor::rt::Init(numberOfPolymorphicClassIds, polymorphicClassIdArray, numberOfStaticClassIds, staticClassIdArray);
}

extern "C" RT_API uint64_t RtDynamicInitVmt(void* vmt)
{
    return cmajor::rt::DynamicInitVmt(vmt);
}

extern "C" RT_API void RtDone()
{
    cmajor::rt::Done();
}

extern "C" RT_API void RtExit(int32_t exitCode)
{
    exit(exitCode);
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
}

void Done()
{
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
