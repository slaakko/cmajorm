#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/CharacterData.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace soulng::util;
using namespace soulng::unicode;

struct InitDone
{
    InitDone()
    {
        soulng::util::Init();
        sngxml::xpath::Init();
    }
    ~InitDone()
    {
        sngxml::xpath::Done();
        soulng::util::Done();
    }
};

void GetProjectData(const std::string& projectFilePath, std::string& projectName, std::string& projectGuid)
{
    std::unique_ptr<sngxml::dom::Document> projectFileDoc = sngxml::dom::ReadDocument(projectFilePath);
    std::unique_ptr<sngxml::xpath::XPathObject> projectNameResult = sngxml::xpath::Evaluate(U"/Project/PropertyGroup[@Label=\"Globals\"]/RootNamespace/text()", projectFileDoc.get());
    if (projectNameResult)
    {
        if (projectNameResult->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(projectNameResult.get());
            int n = nodeSet->Length();
            if (n == 1)
            {
                sngxml::dom::Node* node = (*nodeSet)[0];
                if (node->GetNodeType() == sngxml::dom::NodeType::textNode)
                {
                    sngxml::dom::Text* text = static_cast<sngxml::dom::Text*>(node);
                    projectName = ToUtf8(text->Data());
                }
            }
        }
    }
    std::unique_ptr<sngxml::xpath::XPathObject> projectGuidResult = sngxml::xpath::Evaluate(U"/Project/PropertyGroup[@Label=\"Globals\"]/ProjectGuid/text()", projectFileDoc.get());
    if (projectGuidResult)
    {
        if (projectGuidResult->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(projectGuidResult.get());
            int n = nodeSet->Length();
            if (n == 1)
            {
                sngxml::dom::Node* node = (*nodeSet)[0];
                if (node->GetNodeType() == sngxml::dom::NodeType::textNode)
                {
                    sngxml::dom::Text* text = static_cast<sngxml::dom::Text*>(node);
                    projectGuid = ToUtf8(text->Data());
                }
            }
        }
    }
}

const char* version = "3.5.0";

void PrintHelp()
{
    std::cout << "Cmajor Visual Studio solution .sln file generator version " << version << std::endl;
    std::cout << "Usage: cmslnfilegen [options] { PROJECT_FILE.vcxproj }" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "--help (-h):" << std::endl;
    std::cout << "  Print help and exit." << std::endl;
    std::cout << "--verbose (-v):" << std::endl;
    std::cout << "  Be verbose." << std::endl;
    std::cout << "--name (-n) $SOLUTION_NAME$:" << std::endl;
    std::cout << "  Set solution name to $SOLUTION_NAME$." << std::endl;
    std::cout << "--file (-f) $SOLUTION_FILE_PATH$:" << std::endl;
    std::cout << "  Generate solution file $SOLUTION_FILE_PATH$." << std::endl;
}

int main(int argc, const char** argv)
{
    InitDone initDone;
    try
    {
        bool verbose = false;
        bool prevWasName = false;
        bool prevWasFile = false;
        std::string solutionName;
        std::string solutionFilePath;
        std::vector<std::string> relativeProjectFilePaths;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (StartsWith(arg, "--"))
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
                else if (arg == "--name")
                {
                    prevWasName = true;
                }
                else if (arg == "--file")
                {
                    prevWasFile = true;
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
                        case 'n':
                        {
                            prevWasName = true;
                            break;
                        }
                        case 'f':
                        {
                            prevWasFile = true;
                            break;
                        }
                        default:
                        {
                            throw std::runtime_error("unknown option '-" + std::string(1, o) + "'");
                        }
                    }
                }
            }
            else
            {
                if (prevWasName)
                {
                    solutionName = arg;
                    prevWasName = false;
                }
                else if (prevWasFile)
                {
                    solutionFilePath = arg;
                    prevWasFile = false;
                }
                else
                {
                    relativeProjectFilePaths.push_back(arg);
                }
            }
        }
        if (verbose)
        {
            std::cout << "Cmajor Visual Studio solution .sln file generator 'cmslnfilegen' version " << version << std::endl;
        }
        boost::filesystem::create_directories(Path::GetDirectoryName(solutionFilePath));
        {
            std::ofstream solutionFile(solutionFilePath);
            CodeFormatter formatter(solutionFile);
            formatter.WriteLine("Microsoft Visual Studio Solution File, Format Version 12.00");
            formatter.WriteLine("VisualStudioVersion = 16.0.29806.167");
            formatter.WriteLine("MinimumVisualStudioVersion = 10.0.40219.1");
            for (const std::string& relativeProjectFilePath : relativeProjectFilePaths)
            {
                std::string projectFilePath = GetFullPath(Path::Combine(Path::GetDirectoryName(solutionFilePath), relativeProjectFilePath));
                std::string projectName;
                std::string projectGuid;
                GetProjectData(projectFilePath, projectName, projectGuid);
                formatter.WriteLine("Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"" + projectName + "\", \"" + relativeProjectFilePath + "\", \"" + projectGuid + "\"");
                formatter.WriteLine("EndProject");
                if (verbose)
                {
                    std::cout << "> " << projectName << std::endl;
                }
            }
        }
        if (verbose)
        {
            std::cout << "==> " << solutionFilePath << std::endl;
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
