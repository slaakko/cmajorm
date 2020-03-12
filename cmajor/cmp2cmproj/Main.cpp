#include <sngcm/ast/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <sngcm/cmlexer/ContainerFileLexer.hpp>
#include <sngcm/cmparser/ProjectFile.hpp>
#include <cmajor/cmproj/Conversion.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>
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
using namespace soulng::unicode;
using namespace cmajor::cmproj;

const char* version = "3.4.0";

void PrintHelp()
{
    std::cout << "Cmajor Project File Converter version " << version << std::endl;
    std::cout << "Usage: cmp2cmproj [options] { project.cmp }\n" <<
        "Convert a Cmajor project file 'project.cmp' to MSBuild compatible project file format 'project.cmproj'.\n" <<
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
        std::vector<std::string> projectFilePaths;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (StartsWith(arg, "-"))
            {
                if (arg == "--help" || arg == "-h")
                {
                    PrintHelp();
                    return 0;
                }
                else if (arg == "--verbose" || arg == "-v")
                {
                    verbose = true;
                }
                else
                {
                    throw std::runtime_error("unknown option '" + arg + "'");
                }
            }
            else if (EndsWith(arg, ".cmp"))
            {
                projectFilePaths.push_back(arg);
            }
            else
            {
                throw std::runtime_error("invalid project file name (not ending with .cmp)");
            }
        }
        if (projectFilePaths.empty())
        {
            throw std::runtime_error("no project files given");
        }
        for (const std::string& projectFilePath : projectFilePaths)
        {
            MappedInputFile projectFile(projectFilePath);
            std::u32string p(ToUtf32(std::string(projectFile.Begin(), projectFile.End())));
            ContainerFileLexer containerFileLexer(p, projectFilePath, 0);
            std::unique_ptr<sngcm::ast::Project> project = ProjectFileParser::Parse(containerFileLexer, "debug", sngcm::ast::BackEnd::llvm);
            project->ResolveDeclarations();
            std::string cmprojFilePath = GetFullPath(Path::ChangeExtension(projectFilePath, ".cmproj"));
            boost::uuids::uuid guid = boost::uuids::random_generator()();
            std::string guidStr = boost::uuids::to_string(guid);
            ConvertProjectToCmProject(project.get(), cmprojFilePath, guidStr, verbose);
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
