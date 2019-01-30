// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_UTIL_MUTEX_INCLUDED
#define CMAJOR_UTIL_MUTEX_INCLUDED
#include <cmajor/util/UtilApi.hpp>
#include <mutex>

namespace cmajor {namespace util {

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

} } // namespace cmajor::util

#endif // CMAJOR_UTIL_MUTEX_INCLUDED
