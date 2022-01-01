// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/System.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/InitDone.hpp>
#include <iostream>
#include <stdexcept>

void InitApplication()
{
    soulng::util::Init();
}

int main(int argc, const char** argv)
{
    try
    {
        InitApplication();
        if (argc < 4)
        {
            std::cout << "cmfileredirector { -<handle_to_redirect> <filename_to_redirect_to> }... <executable_to_launch> { <argument_to_executable> }..." << std::endl;
        }
        else
        {
            std::vector<std::pair<int, std::string>> redirections;
            int handle = 1;
            bool prevWasHandle = false;
            bool first = true;
            bool args = false;
            std::string command;
            for (int i = 1; i < argc; ++i)
            {
                std::string arg = argv[i];
                if (!args && arg[0] == '-')
                {
                    std::string handleStr = arg.substr(1);
                    handle = std::atoi(handleStr.c_str());
                    prevWasHandle = true;
                }
                else if (prevWasHandle)
                {
                    redirections.push_back(std::make_pair(handle, arg));
                    prevWasHandle = false;
                }
                else if (first)
                {
                    command.append(soulng::util::QuotedPath(arg));
                    first = false;
                    args = true;
                }
                else
                {
                    command.append(" ").append(soulng::util::QuotedPath(arg));
                }
            }
            soulng::util::System(command, redirections);
        }
    }
    catch (...)
    {
        return 1;
    }
    return 0;
}
