// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/Mutex.hpp>

namespace soulng { namespace util {

MutexOwner::MutexOwner(char id_): id(id_)
{
}

void MutexOwner::Push(char mutexId)
{
    ownedMutexes.push_back(mutexId);
}

void MutexOwner::Pop()
{
    ownedMutexes.pop_back();
}

OwnerGuard::OwnerGuard(Mutex& mtx_, MutexOwner& owner_) : mtx(mtx_), owner(owner_)
{
    owner.Push(mtx.Id());
}

OwnerGuard::~OwnerGuard()
{
    owner.Pop();
}

LockGuard::LockGuard(Mutex& mtx_, MutexOwner& owner_) : mtx(mtx_), owner(owner_), locked(false)
{
    Lock();
}

LockGuard::~LockGuard()
{
    if (locked)
    {
        Unlock();
    }
}

void LockGuard::Lock()
{
    owner.Push(mtx.Id());
    mtx.Lock();
    locked = true;
}

void LockGuard::Unlock()
{
    mtx.Unlock();
    owner.Pop();
    locked = false;
}

Mutex::Mutex(char id_) : id(id_)
{
}

} } // namespace soulng::util
