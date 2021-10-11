#include <sngcm/cmlexer/ContainerFileLexer.hpp>
#include <sngcm/cmlexer/CmajorLexer.hpp>
#include <sngcm/cmparser/ProjectFile.hpp>
#include <sngcm/cmparser/SolutionFile.hpp>
#include <sngcm/cmparser/CompileUnit.hpp>
#include <sngcm/ast/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>

using namespace sngcm::ast;
using namespace soulng::util;
using namespace soulng::unicode;

void InitApplication()
{
    soulng::util::Init();
    sngcm::ast::Init();
}

void PrintHelp()
{
    std::cout << "Usage: cppcmparser [options] { file.cm | file.cmp | file.cms }" << std::endl;
    std::cout << "Parse Cmajor source file file.cm, project file.cmp or solution file.cms." << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "--help | -h:" << std::endl;
    std::cout << "  Print help and exit." << std::endl;
    std::cout << "--verbose| -v:" << std::endl;
    std::cout << "  Be verbose." << std::endl;
}

void ParseSourceFile(const std::string& fileName, int index, bool verbose)
{
    if (verbose)
    {
        std::cout << ">>> " << fileName << std::endl;
    }
    std::u32string content = ToUtf32(ReadFile(fileName));
    CmajorLexer lexer(content, fileName, index);
    boost::uuids::uuid moduleId = boost::uuids::nil_uuid();
    ParsingContext ctx;
    std::unique_ptr<sngcm::ast::CompileUnitNode> compileUnit = CompileUnitParser::Parse(lexer, &moduleId, &ctx);
}

void ParseProject(const std::string& projectFileName, int index, bool verbose)
{
    if (verbose)
    {
        std::cout << ">> " << projectFileName << std::endl;
    }
    std::u32string content = ToUtf32(ReadFile(projectFileName));
    ContainerFileLexer lexer(content, projectFileName, index);
    std::unique_ptr<sngcm::ast::Project> project = ProjectFileParser::Parse(lexer, "debug", BackEnd::llvm, "", SystemDirKind::regular);
    project->ResolveDeclarations();
    int n = project->SourceFilePaths().size();
    for (int i = 0; i < n; ++i)
    {
        ParseSourceFile(project->SourceFilePaths()[i], i, verbose);
    }
}

void ParseSolution(const std::string& solutionFileName, int index, bool verbose)
{
    if (verbose)
    {
        std::cout << "> " << solutionFileName << std::endl;
    }
    std::u32string content = ToUtf32(ReadFile(solutionFileName));
    ContainerFileLexer lexer(content, solutionFileName, index);
    std::unique_ptr<sngcm::ast::Solution> solution = SolutionFileParser::Parse(lexer);
    solution->ResolveDeclarations();
    int n = solution->ProjectFilePaths().size();
    for (int i = 0; i < n; ++i)
    {
        const std::string& projectFilePath = solution->ProjectFilePaths()[i];
        ParseProject(projectFilePath, i, verbose);
    }
}

int main(int argc, const char** argv)
{
    try
    {
        InitApplication();
        bool verbose = false;
        std::vector<std::string> files;
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
            else
            {
                files.push_back(arg);
            }
        }
        int n = files.size();
        if (n == 0)
        {
            throw std::runtime_error("no files given");
        }
        for (int i = 0; i < n; ++i)
        {
            const std::string& fileName = GetFullPath(files[i]);
            std::string extension = Path::GetExtension(fileName);
            if (extension == ".cm")
            {
                ParseSourceFile(fileName, i, verbose);
            }
            else if (extension == ".cmp")
            {
                ParseProject(fileName, i, verbose);
            }
            else if (extension == ".cms")
            {
                ParseSolution(fileName, i, verbose);
            }
            else
            {
                throw std::runtime_error("unknown extension '" + extension + "' of file '" + fileName);
            }
        }
    }
    catch (const std::exception & ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
