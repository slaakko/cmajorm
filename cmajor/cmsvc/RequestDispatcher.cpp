// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsvc/RequestDispatcher.hpp>
#include <cmajor/cmsvc/Request.hpp>
#include <cmajor/cmsvc/Message.hpp>
#include <memory>
#include <thread>

namespace cmajor { namespace service {

class RequestDispatcher
{
public:
    static void Init();
    static void Done();
    static RequestDispatcher& Instance() { return *instance; }
    void Start();
    void Stop();
    void Run();
    void HandleRequest(Request* request);
private:
    static std::unique_ptr<RequestDispatcher> instance;
    std::thread thread;
};

std::unique_ptr<RequestDispatcher> RequestDispatcher::instance;

void RequestDispatcher::Init()
{
    instance.reset(new RequestDispatcher());
}

void RequestDispatcher::Done()
{
    instance.reset();
}

void RunDispatcher()
{
    RequestDispatcher::Instance().Run();
}

void RequestDispatcher::Start()
{
    thread = std::thread{ RunDispatcher };
}

void RequestDispatcher::Stop()
{
    try
    {
        Exit();
        thread.join();
    }
    catch (...)
    {
    }
}

void RequestDispatcher::Run()
{
    try
    {
        std::unique_ptr<Request> request = GetRequest();
        while (request)
        {
            try
            {
                request->Execute();
            }
            catch (const std::exception& ex)
            {
                PutOutputServiceMessage("request dispatcher: error executing " + request->Name() + ": " + std::string(ex.what()));
            }
            request = GetRequest();
        }
    }
    catch (...)
    {
    }
}

void StartRequestDispatcher()
{
    RequestDispatcher::Instance().Start();
}

void StopRequestDispatcher()
{
    RequestDispatcher::Instance().Stop();
}

void InitRequestDispatcher()
{
    RequestDispatcher::Init();
}

void DoneRequestDispatcher()
{
    RequestDispatcher::Done();
}

} } // namespace cmajor::service
