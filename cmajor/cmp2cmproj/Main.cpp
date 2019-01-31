#include <cmajor/ast/InitDone.hpp>
#include <cmajor/parsing/InitDone.hpp>
#include <cmajor/util/InitDone.hpp>
#include <cmajor/parser/Project.hpp>
#include <cmajor/parsing/Exception.hpp>
#include <cmajor/cmproj/Conversion.hpp>
#include <cmajor/util/TextUtils.hpp>
#include <cmajor/util/MappedInputFile.hpp>
#include <cmajor/util/Path.hpp>
#include <cmajor/util/Unicode.hpp>
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
        cmajor::ast::Init();
        cmajor::parsing::Init();
        cmajor::util::Init();
    }
    ~InitDone()
    {
        cmajor::util::Done();
        cmajor::parsing::Done();
        cmajor::ast::Done();
    }
};

using namespace cmajor::parser;
using namespace cmajor::parsing;
using namespace cmajor::util;
using namespace cmajor::unicode;
using namespace cmajor::cmproj;

const char* version = "3.3.0";

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

cmajor::parser::Project* projectGrammar = nullptr;

int main(int argc, const char** argv)
{
    try
    {
        InitDone initDone;
        if (!projectGrammar)
        {
            projectGrammar = cmajor::parser::Project::Create();
        }
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
            std::unique_ptr<cmajor::ast::Project> project(projectGrammar->Parse(&p[0], &p[0] + p.length(), 0, projectFilePath, "debug", cmajor::ast::BackEnd::llvm));
            project->ResolveDeclarations();
            std::string cmprojFilePath = GetFullPath(Path::ChangeExtension(projectFilePath, ".cmproj"));
            boost::uuids::uuid guid = boost::uuids::random_generator()();
            std::string guidStr = boost::uuids::to_string(guid);
            ConvertProject(project.get(), cmprojFilePath, guidStr, verbose);
        }
    }
    catch (const ParsingException& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
