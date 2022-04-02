// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmlog/Logger.hpp>
#include <soulng/util/Socket.hpp>
#include <thread>
#include <memory>
#include <vector>
#include <iostream>

int defaultPortNumber = 55000;

class LogServer
{
public:
    static void Init();
    static void Done();
    static LogServer& Instance() { return *instance; }
    void SetPort(int port_) { port = port_; }
    void Start();
    void Stop();
    void Run();
private:
    static std::unique_ptr<LogServer> instance;
    LogServer();
    int port;
    bool exiting;
    std::thread logServerThread;
    TcpSocket socket;
    std::vector<std::unique_ptr<Logger>> loggers;
    std::vector<std::thread> loggerThreads;
};

using namespace soulng::util;

std::unique_ptr<LogServer> LogServer::instance;

LogServer::LogServer() : port(defaultPortNumber), exiting(false)
{
}

void RunLogServer()
{
    try
    {
        std::cout << "running log server..." << std::endl;
        LogServer::Instance().Run();
    }
    catch (const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
    }
    std::cout << "stopped running log server" << std::endl;
}

void LogServer::Start()
{
    logServerThread = std::thread(RunLogServer);
}

void LogServer::Stop()
{
    exiting = true;
    TcpSocket client("127.0.0.1", std::to_string(port));
    logServerThread.join();
}

void LogServer::Run()
{
    std::cout << "binding port " << port << "..." << std::endl;
    socket.Bind(port);
    std::cout << "listening port " << port << "..." << std::endl;
    socket.Listen(10);
    while (!exiting)
    {
        TcpSocket client = socket.Accept();
        if (exiting)
        {
            std::cout << "exiting..." << std::endl;
            for (auto& logger : loggers)
            {
                logger->Stop();
            }
            for (auto& loggerThread : loggerThreads)
            {
                loggerThread.join();
            }
            return;
        }
        std::cout << "client accepted to port " << port << "..." << std::endl;
        std::unique_ptr<Logger> logger(new Logger(std::move(client)));
        std::thread loggerThread = std::thread(RunLogger, logger.get());
        loggerThreads.push_back(std::move(loggerThread));
        loggers.push_back(std::move(logger));
    }
}

void LogServer::Init()
{
    instance.reset(new LogServer());
}

void LogServer::Done()
{
    instance.reset();
}

void SetLogServerPort(int port)
{
    LogServer::Instance().SetPort(port);
}

void StartLogServer()
{
    LogServer::Instance().Start();
}

void StopLogServer()
{
    LogServer::Instance().Stop();
}

void InitLogServer()
{
    LogServer::Init();
}

void DoneLogServer()
{
    LogServer::Done();
}
