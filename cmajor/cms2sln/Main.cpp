#include <sngcm/ast/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <sngcm/cmlexer/ContainerFileLexer.hpp>
#include <sngcm/cmparser/ProjectFile.hpp>
#include <sngcm/cmparser/SolutionFile.hpp>
#include <cmajor/cmproj/Conversion.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/CharacterData.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <stdexcept>
#include <iostream>

struct InitDone
{
    InitDone()
    {
        sngcm::ast::Init();
        soulng::util::Init();
    }
    ~InitDone()
    {
        soulng::util::Done();
        sngcm::ast::Done();
    }
};

using namespace soulng::util;
using namespace soulng::unicode;
using namespace cmajor::cmproj;

void ConvertSolution(sngcm::ast::Solution* solution, const std::string& slnFilePath, const std::string& guidStr, bool verbose)
{
    std::ofstream slnFile(slnFilePath);
    CodeFormatter formatter(slnFile);
    formatter.SetIndentSize(4);
    formatter.WriteLine("Microsoft Visual Studio Solution File, Format Version 12.00");
    formatter.WriteLine("# Visual Studio 15");
    formatter.WriteLine("VisualStudioVersion = 15.0.27428.2015");
    formatter.WriteLine("MinimumVisualStudioVersion = 10.0.40219.1");
    std::vector<std::string> projectGuids;
    int n = solution->ProjectFilePaths().size();
    for (int i = 0; i < n; ++i)
    {
        std::string projectFilePath = solution->ProjectFilePaths()[i];
        MappedInputFile projectFile(projectFilePath);
        std::u32string p(ToUtf32(std::string(projectFile.Begin(), projectFile.End())));
        ContainerFileLexer containerFileLexer(p, projectFilePath, 0);
        std::unique_ptr<sngcm::ast::Project> project = ProjectFileParser::Parse(containerFileLexer, "debug", sngcm::ast::BackEnd::llvm);
        project->ResolveDeclarations();
        std::string cmprojFilePath = Path::ChangeExtension(projectFilePath, ".cmproj");
        boost::uuids::uuid guid = boost::uuids::random_generator()();
        std::string projectGuidStr = boost::uuids::to_string(guid);
        projectGuids.push_back(projectGuidStr);
        ConvertProjectToCmProject(project.get(), cmprojFilePath, projectGuidStr, verbose);
        std::string relativeProjectFilePath = Path::ChangeExtension(solution->RelativeProjectFilePaths()[i], ".cmproj");
        std::string projectLine = "Project(\"{A8A06387-5A08-470D-8F9C-CD5ECF2DDB09}\") = \"";
        projectLine.append(ToUtf8(project->Name())).append("\", \"").append(relativeProjectFilePath).append("\", \"{").append(projectGuidStr).append("}\"");
        formatter.WriteLine(projectLine);
        formatter.WriteLine("EndProject");
    }
    formatter.WriteLine("Global");
    formatter.IncIndent();
    formatter.WriteLine("GlobalSection(SolutionConfigurationPlatforms) = preSolution");
    formatter.IncIndent();
    formatter.WriteLine("Debug|Any CPU = Debug|Any CPU");
    formatter.WriteLine("Release|Any CPU = Release|Any CPU");
    formatter.DecIndent();
    formatter.WriteLine("EndGlobalSection");
    formatter.WriteLine("GlobalSection(ProjectConfigurationPlatforms) = postSolution");
    formatter.IncIndent();
    for (int i = 0; i < n; ++i)
    {
        formatter.WriteLine("{" + projectGuids[i] + "}.Debug|Any CPU.ActiveCfg = Debug|Any CPU");
        formatter.WriteLine("{" + projectGuids[i] + "}.Debug|Any CPU.Build.0 = Debug|Any CPU");
        formatter.WriteLine("{" + projectGuids[i] + "}.Release|Any CPU.ActiveCfg = Release|Any CPU");
        formatter.WriteLine("{" + projectGuids[i] + "}.Release|Any CPU.Build.0 = Release|Any CPU");
    }
    formatter.DecIndent();
    formatter.WriteLine("EndGlobalSection");
    formatter.WriteLine("GlobalSection(SolutionProperties) = preSolution");
    formatter.IncIndent();
    formatter.WriteLine("HideSolutionNode = FALSE");
    formatter.DecIndent();
    formatter.WriteLine("EndGlobalSection");
    boost::uuids::uuid guid = boost::uuids::random_generator()();
    std::string solutionGuidStr = boost::uuids::to_string(guid);
    formatter.WriteLine("GlobalSection(ExtensibilityGlobals) = postSolution");
    formatter.IncIndent();
    formatter.WriteLine("SolutionGuid = {" + solutionGuidStr + "}");
    formatter.DecIndent();
    formatter.WriteLine("EndGlobalSection");
    formatter.DecIndent();
    formatter.WriteLine("EndGlobal");
    if (verbose)
    {
        std::cout << "==> " << slnFilePath << std::endl;
    }
}

const char* version = "3.5.0";

void PrintHelp()
{
    std::cout << "Cmajor Solution File Converter version " << version << std::endl;
    std::cout << "Usage: cms2sln [options] { solution.cms }\n" <<
        "Convert a Cmajor solution file 'solution.cms' to MSBuild compatible solution file format 'solution.sln'.\n" <<
        "Converts also project files contained by the solution to MSBuild compatible project file format 'project.cmproj'.\n" <<
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
        std::vector<std::string> solutionFilePaths;
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
            else if (EndsWith(arg, ".cms"))
            {
                solutionFilePaths.push_back(arg);
            }
            else
            {
                throw std::runtime_error("invalid solution file name (not ending with .cms)");
            }
        }
        if (solutionFilePaths.empty())
        {
            throw std::runtime_error("no solution files given");
        }
        for (const std::string& solutionFilePath : solutionFilePaths)
        {
            MappedInputFile solutionFile(solutionFilePath);
            std::u32string s(ToUtf32(std::string(solutionFile.Begin(), solutionFile.End())));
            ContainerFileLexer containerFileLexer(s, solutionFilePath, 0);
            std::unique_ptr<sngcm::ast::Solution> solution = SolutionFileParser::Parse(containerFileLexer);
            solution->ResolveDeclarations();
            std::string slnFilePath = GetFullPath(Path::ChangeExtension(solutionFilePath, ".sln"));
            boost::uuids::uuid guid = boost::uuids::random_generator()();
            std::string guidStr = boost::uuids::to_string(guid);
            ConvertSolution(solution.get(), slnFilePath, guidStr, verbose);
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
