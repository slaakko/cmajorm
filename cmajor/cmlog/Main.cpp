// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/InitDone.hpp>
#include <cmlog/LogServer.hpp>
#include <iostream>
#include <stdexcept>

void InitApplication()
{
    soulng::util::Init();
    InitLogServer();
}

void DoneApplication()
{
    DoneLogServer();
    soulng::util::Done();
}

int main(int argc, const char** argv)
{
    try
    {
        InitApplication();
        StartLogServer();
        std::cout << "type 'exit' to quit..." << std::endl;
        std::string line;
        std::cin >> line;
        while (line != "exit")
        {
            std::cin >> line;
        }
        StopLogServer();
        std::cout << "exited" << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    DoneApplication();
    return 0;
}