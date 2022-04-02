// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmlog/Logger.hpp>
#include <iostream>

Logger::Logger(TcpSocket&& socket_) : socket(std::move(socket_)), exiting(false)
{
}

void Logger::Run()
{
    while (!exiting)
    {
        std::string str = ReadStr(socket);
        if (str.empty())
        {
            return;
        }
        std::cout << str << std::endl;
    }
}

void Logger::Stop()
{
    exiting = true;
    socket.Close();
}

void RunLogger(Logger* logger)
{
    try
    {
        std::cout << "running logger..." << std::endl;
        logger->Run();
    }
    catch (const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
    }
    std::cout << "stopped running logger" << std::endl;
}
