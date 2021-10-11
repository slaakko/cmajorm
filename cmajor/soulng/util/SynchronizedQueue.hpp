// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_SYNCHRONIZED_QUEUE_INCLUDED
#define SOULNG_UTIL_SYNCHRONIZED_QUEUE_INCLUDED
#include <mutex>
#include <list>
#include <condition_variable>

namespace soulng { namespace util {

template<class T>
class SynchronizedQueue
{
public:
    SynchronizedQueue();
    bool IsEmpty() const;
    void Put(const T& item);
    T Get();
    void Exit();
private:
    bool ItemAvailableOrExiting() const { return !queue.empty() || exiting; }
    std::mutex mtx;
    std::list<T> queue;
    std::condition_variable itemAvailableOrExiting;
    bool exiting;
};

template<class T>
SynchronizedQueue<T>::SynchronizedQueue() : exiting(false)
{
}

template<class T>
bool SynchronizedQueue<T>::IsEmpty() const
{
    std::lock_guard<std::mutex> lock(mtx);
    return queue.empty();
}

template<class T>
void SynchronizedQueue<T>::Put(const T& item)
{
    if (exiting) return;
    std::lock_guard<std::mutex> lock(mtx);
    queue.push_back(item);
    itemAvailableOrExiting.notify_one();
}

template<class T>
T SynchronizedQueue<T>::Get()
{
    std::unique_lock<std::mutex> lock(mtx);
    itemAvailableOrExiting.wait(lock, [this]{ return ItemAvailableOrExiting(); });
    if (exiting) return T();
    if (!queue.empty())
    {
        T item = std::move(queue.front());
        queue.pop_front();
        return item;
    }
    return T();
}

template<class T>
void SynchronizedQueue<T>::Exit()
{
    exiting = true;
    itemAvailableOrExiting.notify_one();
}

} } // namespace soulng::util

#endif // SOULNG_UTIL_SYNCHRONIZED_QUEUE_INCLUDED
