#include <cmajor/cmtoolchain/ToolChains.hpp>
#include <soulng/util/TextUtils.hpp>
#include <iostream>
#include <stdexcept>

using namespace soulng::util;

void PrintHelp()
{
    std::cout << "Usage: cmtoolchainconfig [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "--help (-h):" << std::endl;
    std::cout << "  Print help and exit." << std::endl;
    std::cout << "--verbose (-v):" << std::endl;
    std::cout << "  Be verbose." << std::endl;
    std::cout << "--create (-c):" << std::endl;
    std::cout << "  Create tool chain configuration." << std::endl;
    std::cout << "--show (-s):" << std::endl;
    std::cout << "  Show tool chain configuration." << std::endl;
}

int main(int argc, const char** argv)
{
    try
    {
        bool create = false;
        bool show = false;
        bool verbose = false;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (StartsWith(arg, "--"))
            {
                if (arg == "--verbose")
                {
                    verbose = true;
                }
                else if (arg == "--create")
                {
                    create = true;
                }
                else if (arg == "--show")
                {
                    show = true;
                }
                else if (arg == "--help")
                {
                    PrintHelp();
                    return 1;
                }
                else
                {
                    throw std::runtime_error("unknown option '" + arg + "'");
                }
            }
            else if (StartsWith(arg, "-"))
            {
                std::string options = arg.substr(1);
                for (char o : options)
                {
                    switch (o)
                    {
                        case 'v':
                        {
                            verbose = true;
                            break;
                        }
                        case 'h':
                        {
                            PrintHelp();
                            return 1;
                        }
                        case 'c':
                        {
                            create = true;
                            break;
                        }
                        case 's':
                        {
                            show = true;
                            break;
                        }
                        default:
                        {
                            throw std::runtime_error("unknown option '-" + std::string(1, o) + "'");
                        }
                    }
                }
            }
        }
        if (create)
        {
            if (verbose)
            {
                std::cout << "creating tool chains" << std::endl;
            }
            CreateToolChains(verbose);
            if (verbose)
            {
                std::cout << "writing tool chains" << std::endl;
            }
            WriteToolChains(verbose);
        }
        if (show)
        {
            ReadToolChains(verbose);
            ShowToolChains();
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        PrintHelp();
        return 1;
    }
    return 0;
}
