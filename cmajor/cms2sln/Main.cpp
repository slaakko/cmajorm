#include <cmajor/ast/InitDone.hpp>
#include <cmajor/parsing/InitDone.hpp>
#include <cmajor/util/InitDone.hpp>
#include <cmajor/parser/Project.hpp>
#include <cmajor/parser/Solution.hpp>
#include <cmajor/cmproj/Conversion.hpp>
#include <cmajor/dom/Document.hpp>
#include <cmajor/dom/Element.hpp>
#include <cmajor/dom/CharacterData.hpp>
#include <cmajor/util/TextUtils.hpp>
#include <cmajor/util/MappedInputFile.hpp>
#include <cmajor/util/Path.hpp>
#include <cmajor/util/Unicode.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <stdexcept>
#include <iostream>

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

cmajor::parser::Project* projectGrammar = nullptr;

void ConvertSolution(cmajor::ast::Solution* solution, const std::string& slnFilePath, const std::string& guidStr, bool verbose)
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
        std::unique_ptr<cmajor::ast::Project> project(projectGrammar->Parse(&p[0], &p[0] + p.length(), 0, projectFilePath, "debug", cmajor::ast::BackEnd::llvm));
        project->ResolveDeclarations();
        std::string cmprojFilePath = Path::ChangeExtension(projectFilePath, ".cmproj");
        boost::uuids::uuid guid = boost::uuids::random_generator()();
        std::string projectGuidStr = boost::uuids::to_string(guid);
        projectGuids.push_back(projectGuidStr);
        ConvertProject(project.get(), cmprojFilePath, projectGuidStr, verbose);
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

const char* version = "3.3.0";

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

cmajor::parser::Solution* solutionGrammar = nullptr;

int main(int argc, const char** argv)
{
    try
    {
        InitDone initDone;
        if (!projectGrammar)
        {
            projectGrammar = cmajor::parser::Project::Create();
        }
        if (!solutionGrammar)
        {
            solutionGrammar = cmajor::parser::Solution::Create();
        }
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
            std::unique_ptr<cmajor::ast::Solution> solution(solutionGrammar->Parse(&s[0], &s[0] + s.length(), 0, solutionFilePath));
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
