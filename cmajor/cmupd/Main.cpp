// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmupd/ConversionVisitor.hpp>
#include <sngcm/cmlexer/CmajorLexer.hpp>
#include <sngcm/cmlexer/ContainerFileLexer.hpp>
#include <sngcm/cmparser/CompileUnit.hpp>
#include <sngcm/cmparser/ProjectFile.hpp>
#include <sngcm/cmparser/SolutionFile.hpp>
#include <sngcm/cmparser/ParsingContext.hpp>
#include <sngcm/ast/SourceWriter.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>

using namespace soulng::unicode;

std::unique_ptr<sngcm::ast::CompileUnitNode> ParseCompileUnit(const std::string& sourceFilePath, bool verbose)
{
    if (verbose)
    {
        std::cout << "> " << sourceFilePath << std::endl;
    }
    CmajorLexer lexer(ToUtf32(ReadFile(sourceFilePath)), sourceFilePath, 0);
    ParsingContext ctx;
    return CompileUnitParser::Parse(lexer, &ctx);
}

std::unique_ptr<sngcm::ast::Project> ParseProject(const std::string& projectFilePath, bool verbose)
{
    if (verbose)
    {
        std::cout << "> " << projectFilePath << std::endl;
    }
    ContainerFileLexer lexer(ToUtf32(ReadFile(projectFilePath)), projectFilePath, 0);
    return ProjectFileParser::Parse(lexer, "debug", sngcm::ast::BackEnd::llvm, "");
}

std::unique_ptr<sngcm::ast::Solution> ParseSolution(const std::string& solutionFilePath, bool verbose)
{
    if (verbose)
    {
        std::cout << "> " << solutionFilePath << std::endl;
    }
    ContainerFileLexer lexer(ToUtf32(ReadFile(solutionFilePath)), solutionFilePath, 0);
    return SolutionFileParser::Parse(lexer);
}

std::vector<std::unique_ptr<sngcm::ast::CompileUnitNode>> ParseSources(const std::vector<std::string>& filePaths, bool verbose)
{
    if (verbose)
    {
        std::cout << "parsing sources..." << std::endl;
    }
    std::vector<std::unique_ptr<sngcm::ast::CompileUnitNode>> compileUnits;
    for (const std::string& filePath : filePaths)
    {
        std::string ext = Path::GetExtension(filePath);
        if (ext == ".cms")
        {
            std::unique_ptr<sngcm::ast::Solution> solution = ParseSolution(filePath, verbose);
            solution->ResolveDeclarations();
            for (const std::string& projectFilePath : solution->ProjectFilePaths())
            {
                std::unique_ptr<sngcm::ast::Project> project = ParseProject(projectFilePath, verbose);
                project->ResolveDeclarations();
                for (const std::string& sourceFilePath : project->SourceFilePaths())
                {
                    std::unique_ptr<sngcm::ast::CompileUnitNode> compileUnit = ParseCompileUnit(sourceFilePath, verbose);
                    compileUnits.push_back(std::move(compileUnit));
                }
            }
        }
        else if (ext == ".cmp")
        {
            std::unique_ptr<sngcm::ast::Project> project = ParseProject(filePath, verbose);
            project->ResolveDeclarations();
            for (const std::string& sourceFilePath : project->SourceFilePaths())
            {
                std::unique_ptr<sngcm::ast::CompileUnitNode> compileUnit = ParseCompileUnit(sourceFilePath, verbose);
                compileUnits.push_back(std::move(compileUnit));
            }
        }
        else if (ext == ".cm")
        {
            std::unique_ptr<sngcm::ast::CompileUnitNode> compileUnit = ParseCompileUnit(filePath, verbose);
            compileUnits.push_back(std::move(compileUnit));
        }
        else
        {
            throw std::runtime_error("unknown extension '" + ext + "' in argument '" + filePath + "'");
        }
    }
    return compileUnits;
}

std::vector<std::string> SplitIntoLines(const std::string& s)
{
    int state = 0;
    std::string line;
    std::vector<std::string> lines;
    for (char c : s)
    {
        switch (state)
        {
            case 0:
            {
                switch (c)
                {
                    case '\r': state = 1; break;
                    case '\n': state = 2; break;
                    default:
                    {
                        line.append(1, c);
                    }
                }
                break;
            }
            case 1:
            {
                switch (c)
                {
                    case '\n': state = 2; break;
                }
                break;
            }
            case 2:
            {
                switch (c)
                {
                    case '\r':
                    {
                        lines.push_back(line);
                        line.clear();
                        state = 1;
                        break;
                    }
                    case '\n':
                    {
                        lines.push_back(line);
                        line.clear();
                        state = 2;
                        break;
                    }
                    default:
                    {
                        lines.push_back(line);
                        line.clear();
                        line.append(1, c);
                        state = 0;
                        break;
                    }
                }
                break;
            }
        }
    }
    switch (state)
    {
        case 0:
        {
            if (!line.empty())
            {
                lines.push_back(line);
            }
            break;
        }
        case 1:
        {
            if (!line.empty())
            {
                lines.push_back(line);
            }
            break;
        }
        case 2:
        {
            if (!line.empty())
            {
                lines.push_back(line);
            }
            break;
        }
    }
    return lines;
}

std::vector<std::string> GetLines(sngcm::ast::CompileUnitNode* compileUnit)
{
    std::vector<std::string> lines;
    std::stringstream stream; 
    CodeFormatter formatter(stream);
    SourceWriter writer(formatter);
    compileUnit->Accept(writer);
    std::string s = stream.str();
    return SplitIntoLines(s);
}

void PrintChangedLines(const std::string& sourceFilePath, const std::vector<std::string>& originalLines, const std::vector<std::string>& changedLines, bool commit)
{
    int numChangedLines = 0;
    int n = std::min(originalLines.size(), changedLines.size());
    int on = originalLines.size();
    int cn = changedLines.size();
    int numDigits = 1 + static_cast<int>(std::log10(n));
    for (int i = 0; i < n; ++i)
    {
        const std::string& originalLine = originalLines[i];
        const std::string& changedLine = changedLines[i];
        int lineNumber = i + 1;
        if (originalLine != changedLine)
        {
            std::string oline = "-";
            oline.append(soulng::util::Format(std::to_string(lineNumber), numDigits, soulng::util::FormatWidth::min, soulng::util::FormatJustify::right, '0'));
            oline.append(1, ' ');
            oline.append(originalLine);
            std::cout << oline << std::endl;
            std::string cline = "+";
            cline.append(soulng::util::Format(std::to_string(lineNumber), numDigits, soulng::util::FormatWidth::min, soulng::util::FormatJustify::right, '0'));
            cline.append(1, ' ');
            cline.append(changedLine);
            std::cout << cline << std::endl;
            ++numChangedLines;
        }
    }
    if (on > n)
    {
        for (int i = n; i < on; ++i)
        {
            int lineNumber = i + 1;
            const std::string& originalLine = originalLines[i];
            std::string oline = "-";
            oline.append(soulng::util::Format(std::to_string(lineNumber), numDigits, soulng::util::FormatWidth::min, soulng::util::FormatJustify::right, '0'));
            oline.append(1, ' ');
            oline.append(originalLine);
            std::cout << oline << std::endl;
            ++numChangedLines;
        }
    }
    if (cn > n)
    {
        for (int i = n; i < cn; ++i)
        {
            int lineNumber = i + 1;
            const std::string& changedLine = changedLines[i];
            std::string cline = "+";
            cline.append(soulng::util::Format(std::to_string(lineNumber), numDigits, soulng::util::FormatWidth::min, soulng::util::FormatJustify::right, '0'));
            cline.append(1, ' ');
            cline.append(changedLine);
            std::cout << cline << std::endl;
            ++numChangedLines;
        }
    }
    if (commit)
    {
        std::cout << sourceFilePath << ": " << numChangedLines << " changed lines." << std::endl;
    }
    else
    {
        std::cout << sourceFilePath << ": " << numChangedLines << " lines would be changed." << std::endl;
    }
}

void WriteChanges(sngcm::ast::CompileUnitNode* originalCompileUnit, sngcm::ast::CompileUnitNode* convertedCompileUnit)
{
    std::string convertedCompileUnitFilePath = originalCompileUnit->FilePath() + ".new";
    {
        std::ofstream file(convertedCompileUnitFilePath);
        CodeFormatter formatter(file);
        SourceWriter writer(formatter);
        convertedCompileUnit->Accept(writer);
    }
    int index = 0;
    std::string oldCompileUnitFilePath = originalCompileUnit->FilePath() + "." + Format(std::to_string(index), 3, FormatWidth::min, FormatJustify::right, '0');
    while (boost::filesystem::exists(oldCompileUnitFilePath))
    {
        if (index >= 10)
        {
            throw std::runtime_error("too many old files");
        }
        ++index;
        oldCompileUnitFilePath = originalCompileUnit->FilePath() + "." + Format(std::to_string(index), 3, FormatWidth::min, FormatJustify::right, '0');
    }
    boost::filesystem::rename(originalCompileUnit->FilePath(), oldCompileUnitFilePath);
    std::cout << originalCompileUnit->FilePath() << " -> " << oldCompileUnitFilePath << std::endl;
    boost::filesystem::rename(convertedCompileUnitFilePath, originalCompileUnit->FilePath());
    std::cout << convertedCompileUnitFilePath << " -> " << originalCompileUnit->FilePath() << std::endl;
}

bool Process(sngcm::ast::CompileUnitNode* compileUnit, bool verbose, bool commit)
{
    ConversionVisitor visitor;
    compileUnit->Accept(visitor);
    if (visitor.Changed())
    {
        std::unique_ptr<sngcm::ast::CompileUnitNode> convertedCompileUnit = visitor.GetConvertedCompileUnit();
        std::vector<std::string> originalLines = GetLines(compileUnit);
        std::vector<std::string> convertedLines = GetLines(convertedCompileUnit.get());
        PrintChangedLines(compileUnit->FilePath(), originalLines, convertedLines, commit);
        if (commit)
        {
            WriteChanges(compileUnit, convertedCompileUnit.get());
        }
        return true;
    }
    else
    {
        if (verbose)
        {
            std::cout << compileUnit->FilePath() << ": no changes" << std::endl;
        }
        return false;
    }
}

void Process(const std::vector<std::unique_ptr<sngcm::ast::CompileUnitNode>>& compileUnits, bool verbose, bool commit)
{
    int numFilesChanged = 0;
    for (const std::unique_ptr<sngcm::ast::CompileUnitNode>& compileUnit : compileUnits)
    {
        bool changed = Process(compileUnit.get(), verbose, commit);
        if (changed)
        {
            ++numFilesChanged;
        }
    }
    if (commit)
    {
        std::cout << numFilesChanged << " source files changed." << std::endl;
    }
    else
    {
        std::cout << numFilesChanged << " source files would be changed." << std::endl;
    }
}

void PrintHelp()
{
    std::cout << "Cmajor source update." << std::endl;
    std::cout << "Usage: cmupd [options] { FILE.cms | FILE.cmp | FILE.cm }" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "--help | -h" << std::endl;
    std::cout << "  Print help and exit." << std::endl;
    std::cout << "--verbose | -v" << std::endl;
    std::cout << "  Be verbose." << std::endl;
    std::cout << "--commit | -c" << std::endl;
    std::cout << "  Make changes to sources." << std::endl;
    std::cout << "  If run without the --commit flag, only shows what changes would be made without actually making the changes." << std::endl;
}

int main(int argc, const char** argv)
{
    try
    {
        bool verbose = false;
        bool commit = false;
        InitMaps();
        std::vector<std::string> filePaths;
        std::vector<std::unique_ptr<sngcm::ast::CompileUnitNode>> compileUnits;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (soulng::util::StartsWith(arg, "--"))
            {
                if (arg == "--verbose")
                {
                    verbose = true;
                }
                else if (arg == "--help")
                {
                    PrintHelp();
                    return 1;
                }
                else if (arg == "--commit")
                {
                    commit = true;
                }
                else
                {
                    throw std::runtime_error("unknonwn option '" + arg + "'");
                }
            }
            else if (soulng::util::StartsWith(arg, "-"))
            {
                std::string options = arg.substr(1);
                for (char o : options)
                {
                    switch (o)
                    {
                        case 'v': verbose = true; break;
                        case 'h': PrintHelp(); return 1;
                        case 'c': commit = true; break;
                        default:
                        {
                            throw std::runtime_error("unknown option '-" + std::string(1, o) + "'");
                        }
                    }
                }
            }
            else
            {
                std::string filePath = GetFullPath(arg);
                std::string ext = Path::GetExtension(filePath);
                if (ext != ".cms" && ext != ".cmp" && ext != ".cm")
                {
                    throw std::runtime_error("unknown extension '" + ext + "' in argument '" + arg + "'");
                }
                else
                {
                    filePaths.push_back(filePath);
                }
            }
        }
        compileUnits = ParseSources(filePaths, verbose);
        if (verbose)
        {
            std::cout << "processing..." << std::endl;
        }
        Process(compileUnits, verbose, commit);
        if (!commit)
        {
            std::cout << "No files actually changed. Run with --commit to make changes." << std::endl;
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
