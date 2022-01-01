// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsvc/Request.hpp>
#include <list>
#include <mutex>
#include <condition_variable>

namespace cmajor { namespace service {

class RequestQueue
{
public:
    static void Init();
    static void Done();
    static RequestQueue& Instance() { return *instance; }
    bool IsEmpty();
    bool Exiting() const { return exiting; }
    void Put(Request* request);
    std::unique_ptr<Request> Get();
    bool RequestAvailableOrExiting() const { return !requestQueue.empty() || exiting; }
    void Exit();
private:
    static std::unique_ptr<RequestQueue> instance;
    RequestQueue();
    std::list<std::unique_ptr<Request>> requestQueue;
    std::mutex mtx;
    std::condition_variable requestAvailableOrExiting;
    bool exiting;
};

std::unique_ptr<RequestQueue> RequestQueue::instance;

void RequestQueue::Init()
{
    instance.reset(new RequestQueue());
}

void RequestQueue::Done()
{
    instance.reset();
}

RequestQueue::RequestQueue() : exiting(false)
{
}

void RequestQueue::Exit()
{
    exiting = true;
    requestAvailableOrExiting.notify_one();
}

bool RequestQueue::IsEmpty()
{
    std::lock_guard<std::mutex> lock(mtx);
    return requestQueue.empty();
}

void RequestQueue::Put(Request* request)
{
    if (exiting) return;
    std::lock_guard<std::mutex> lock(mtx);
    requestQueue.push_back(std::unique_ptr<Request>(request));
    requestAvailableOrExiting.notify_one();
}

std::unique_ptr<Request> RequestQueue::Get()
{
    std::unique_lock<std::mutex> lock(mtx);
    requestAvailableOrExiting.wait(lock, [this]{ return RequestAvailableOrExiting(); });
    if (exiting) return std::unique_ptr<Request>();
    if (!requestQueue.empty())
    {
        std::unique_ptr<Request> request = std::move(requestQueue.front());
        requestQueue.pop_front();
        return request;
    }
    return std::unique_ptr<Request>();
}

Request::~Request()
{
}

void PutRequest(Request* request)
{
    RequestQueue::Instance().Put(request);
}

std::unique_ptr<Request> GetRequest()
{
    return RequestQueue::Instance().Get();
}

void Exit()
{
    RequestQueue::Instance().Exit();
}

bool Exiting()
{
    return RequestQueue::Instance().Exiting();
}

void InitRequest() 
{
    RequestQueue::Init();
}

void DoneRequest()
{
    RequestQueue::Done();
}

} } // namespace cmajor::service
