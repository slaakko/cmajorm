// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_MUTEX_INCLUDED
#define SOULNG_UTIL_MUTEX_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <mutex>
#include <string>

namespace soulng {namespace util {

#ifndef __MINGW32__

class UTIL_API MutexOwner
{
public:
    MutexOwner(char id_);
    void Push(char mutexId);
    void Pop();
private:
    char id;
    std::string ownedMutexes;
};

class Mutex;

class UTIL_API OwnerGuard
{
public:
    OwnerGuard(Mutex& mtx_, MutexOwner& owner_);
    ~OwnerGuard();
private:
    Mutex& mtx;
    MutexOwner& owner;
};

class UTIL_API LockGuard
{
public:
    LockGuard(Mutex& mtx_, MutexOwner& owner_);
    ~LockGuard();
    void Lock();
    void Unlock();
private:
    Mutex& mtx;
    MutexOwner& owner;
    bool locked;
};

class UTIL_API Mutex
{
public:
    Mutex(char id_);
    char Id() const { return id; }
    void Lock() { mtx.lock(); }
    void Unlock() { mtx.unlock(); }
    std::mutex& Mtx() { return mtx; }
private:
    char id;
    std::mutex mtx;
};

#endif

} } // namespace soulng::util

#endif // SOULNG_UTIL_MUTEX_INCLUDED
