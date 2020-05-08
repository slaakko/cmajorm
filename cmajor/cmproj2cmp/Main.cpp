#include <sngxml/xpath/InitDone.hpp>
#include <sngcm/ast/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <cmajor/cmproj/Conversion.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

struct InitDone
{
    InitDone()
    {
        soulng::util::Init();
        sngcm::ast::Init();
        sngxml::xpath::Init();
    }
    ~InitDone()
    {
        sngxml::xpath::Done();
        soulng::util::Done();
        sngcm::ast::Done();
    }
};

using namespace soulng::util;
using namespace cmajor::cmproj;

const char* version = "3.5.0";

void PrintHelp()
{
    std::cout << "Cmajor MSBuild Project File Converter version " << version << std::endl;
    std::cout << "Usage: cmproj2cmp [options] { project.cmproj }\n" <<
        "Convert a Cmajor MSBuild project file 'project.cmproj' to Cmajor project file format 'project.cmp'.\n" <<
        "Options:\n" <<
        "--help (-h)\n" <<
        "   Print this help.\n" <<
        "--verbose (-v)\n" <<
        "   Be verbose.\n" <<
        std::endl;
}

int main(int argc, const char** argv)
{
    try
    {
        InitDone initDone;
        bool verbose = false;
        std::vector<std::string> cmprojFilePaths;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (StartsWith(arg, "--"))
            {
                if (arg == "--help")
                {
                    PrintHelp();
                    return 1;
                }
                else if (arg == "--verbose")
                {
                    verbose = true;
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
                        case 'h': PrintHelp(); return 1;
                        case 'v': verbose = true; break;
                        default:
                        {
                            throw std::runtime_error("unknown option '-" + std::string(1, o) + "'");
                        }
                    }
                }
            }
            else if (EndsWith(arg, ".cmproj"))
            {
                cmprojFilePaths.push_back(GetFullPath(arg));
            }
            else
            {
                throw std::runtime_error("invalid MSBuIld project file name (not ending with .cmproj)");
            }
        }
        if (cmprojFilePaths.empty())
        {
            throw std::runtime_error("no project files given");
        }
        for (const std::string& cmprojFilePath : cmprojFilePaths)
        {
            ConvertCmProjectToProject(cmprojFilePath, Path::ChangeExtension(cmprojFilePath, ".cmp"), verbose);
        }
    }
    catch (const std::exception & ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
