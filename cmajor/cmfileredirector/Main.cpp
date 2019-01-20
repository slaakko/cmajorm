// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/util/System.hpp>
#include <cmajor/util/TextUtils.hpp>
#include <cmajor/util/InitDone.hpp>
#include <iostream>
#include <stdexcept>

struct InitDone
{
    InitDone()
    {
        cmajor::util::Init();
    }
    ~InitDone()
    {
        cmajor::util::Done();
    }
};

int main(int argc, const char** argv)
{
    try
    {
        InitDone initDone;
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
                    command.append(cmajor::util::QuotedPath(arg));
                    first = false;
                    args = true;
                }
                else
                {
                    command.append(" ").append(cmajor::util::QuotedPath(arg));
                }
            }
            cmajor::util::System(command, redirections);
        }
    }
    catch (...)
    {
        return 1;
    }
    return 0;
}
