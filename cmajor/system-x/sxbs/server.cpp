// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sxbs/server.hpp>
#include <sxbs/server_thread.hpp>
#include <soulng/util/Socket.hpp>
#include <memory>
#include <thread>
#include <vector>
#include <iostream>
#include <condition_variable>

namespace sxbs {

class Server
{
public:
    static void Init();
    static void Done();
    static Server& Instance() { return *instance; }
    void Start();
    void Stop();
    void Run();
    void NotifyStopServer();
private:
    Server();
    static std::unique_ptr<Server> instance;
    int portNumber;
    bool exiting;
    std::thread thread;
    std::exception_ptr ex;
    soulng::util::TcpSocket socket;
    std::vector<std::thread> threads;
    std::vector<std::unique_ptr<ServerThread>> serverThreads;
    std::mutex mtx;
    std::condition_variable stopServerVar;
};

std::unique_ptr<Server> Server::instance;

void Server::Init()
{
    instance.reset(new Server());
}

void Server::Done()
{
    instance.reset();
}

void DoRunServer()
{
    Server::Instance().Run();
}

Server::Server() : portNumber(defaultPortNumber), exiting(false)
{
}

void Server::Start()
{
    thread = std::thread(DoRunServer);
}

void Server::Stop()
{
    std::unique_lock<std::mutex> lock(mtx);
    stopServerVar.wait(lock, [this] { return exiting; });
    soulng::util::TcpSocket dummy("127.0.0.1", std::to_string(portNumber));
    thread.join();
    for (auto& serverThread : serverThreads)
    {
        serverThread->Stop();
    }
    for (auto& thread : threads)
    {
        thread.join();
    }
    if (ex)
    {
        std::rethrow_exception(ex);
    }
}

void Server::Run()
{
    try
    {
        socket.Bind(portNumber);
        socket.Listen(10);
        std::cout << "sx.bs.ready" << std::endl;
        while (!exiting)
        {
            soulng::util::TcpSocket clientSocket = socket.Accept();
            if (exiting)
            {
                return;
            }
            std::unique_ptr<ServerThread> serverThread(new ServerThread(std::move(clientSocket)));
            RunServerThread(serverThread.get());
            serverThreads.push_back(std::move(serverThread));
        }
    }
    catch (...)
    {
        ex = std::current_exception();
    }
}

void Server::NotifyStopServer()
{
    exiting = true;
    stopServerVar.notify_one();
}

void RunServer()
{
    try
    {
        Server::Instance().Start();
        Server::Instance().Stop();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "unknown exception occurred" << std::endl;
    }
}

void NotifyStopServer()
{
    Server::Instance().NotifyStopServer();
}

void InitServer()
{
    Server::Init();
}

void DoneServer()
{
    Server::Done();
}

} // sxbs
