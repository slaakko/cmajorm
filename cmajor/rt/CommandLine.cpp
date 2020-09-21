// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/CommandLine.hpp>
#include <cmajor/rt/Memory.hpp>
#include <sngcm/cmparser/CommandLine.hpp>
#include <soulng/lexer/TrivialLexer.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Error.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <boost/lexical_cast.hpp>
#include <memory>
#include <string>
#ifdef _WIN32
#include <Windows.h>
#endif

namespace cmajor { namespace rt {

using namespace soulng::util;
using namespace soulng::unicode;

class CommandLineProcessor
{
public:
    static void Init();
    static void Done();
    static CommandLineProcessor& Instance() { Assert(instance, "command line processor not initialized"); return *instance; }
    int32_t Argc() const { return argc; }
    const char** Argv() const { return argv.get();  }
private:
    static std::unique_ptr<CommandLineProcessor> instance;
    std::u32string commandLine;
    std::vector<std::string> args;
    int32_t argc;
    std::unique_ptr<const char*[]> argv;
    CommandLineProcessor();
    std::vector<std::string> ExpandArguments();
    std::vector<std::string> ExpandArgument(const std::string& arg);
};

std::unique_ptr<CommandLineProcessor> CommandLineProcessor::instance;

void CommandLineProcessor::Init()
{
    instance.reset(new CommandLineProcessor());
}

void CommandLineProcessor::Done()
{
    instance.reset();
}

bool ContainsWildCard(const std::string& filePath)
{
    return filePath.find('*') != std::string::npos || filePath.find('?') != std::string::npos;
}

std::u32string GetCommandLineString()
{
    return ToUtf32(std::u16string((char16_t*)GetCommandLineW()));
}

CommandLineProcessor::CommandLineProcessor() : commandLine(GetCommandLineString()), argc(0), argv(nullptr)
{
    commandLine = Trim(commandLine);
    commandLine.append(1, '\n');
    TrivialLexer lexer(commandLine, "", 0);
    args = CommandLineParser::Parse(lexer);
    std::vector<std::string> newArgs;
    int n = args.size();
    for (int i = 0; i < n; ++i)
    {
        bool processed = false;
        if (args[i] == "--cmajor-debug-heap")
        {
            SetDebugHeap(); 
            processed = true;
        }
        else
        {
            if (args[i].find('=') != std::string::npos)
            {
                std::vector<std::string> components = Split(args[i], '=');
                if (components.size() == 2 && components[0] == "--cmajor-debug-allocation")
                {
                    int allocation = boost::lexical_cast<int>(components[1]);
                    SetDebugAllocation(allocation);
                    processed = true;
                }
            }
        }
        if (!processed)
        {
            newArgs.push_back(args[i]);
        }
    }
    std::swap(args, newArgs);
    argc = args.size();
    bool wildCards = false;
    for (int i = 0; i < argc; ++i)
    {
        if (i > 0 && ContainsWildCard(args[i]))
        {
            wildCards = true;
            break;
        }
    }
    if (wildCards)
    {
        args = ExpandArguments();
        argc = args.size();
    }
    argv.reset(new const char*[argc + 1]);
    for (int i = 0; i < argc; ++i)
    {
        argv[i] = args[i].c_str();
    }
    argv[argc] = "";
}

std::vector<std::string> CommandLineProcessor::ExpandArguments()
{
    std::vector<std::string> expandedArgs;
    for (int i = 0; i < argc; ++i)
    {
        if (i > 0 && ContainsWildCard(args[i]))
        {
            std::vector<std::string> expandedArg = ExpandArgument(args[i]);
            expandedArgs.insert(expandedArgs.end(), expandedArg.cbegin(), expandedArg.cend());
        }
        else
        {
            expandedArgs.push_back(args[i]);
        }
    }
    return expandedArgs;
}

std::vector<std::string> CommandLineProcessor::ExpandArgument(const std::string& arg)
{
    std::vector<std::string> expandedArg;
    WIN32_FIND_DATA findData;
    std::u16string filePath = ToUtf16(GetFullPath(arg));
    HANDLE findHandle = FindFirstFile((LPCWSTR)filePath.c_str(), &findData);
    if (findHandle == INVALID_HANDLE_VALUE)
    {
        expandedArg.push_back(arg);
    }
    else
    {
        std::u16string directory = ToUtf16(Path::GetDirectoryName(ToUtf8(filePath)));
        expandedArg.push_back(Path::Combine(ToUtf8(directory), ToUtf8(std::u16string((const char16_t*)(&findData.cFileName[0])))));
        while (FindNextFile(findHandle, &findData))
        {
            expandedArg.push_back(Path::Combine(ToUtf8(directory), ToUtf8(std::u16string((const char16_t*)(&findData.cFileName[0])))));
        }
        FindClose(findHandle);
    }
    return expandedArg;
}

void InitCommandLine()
{
    CommandLineProcessor::Init();
}

void DoneCommandLine()
{
    CommandLineProcessor::Done();
}

} } // namespace cmajor::rt

int32_t RtArgc()
{
    return cmajor::rt::CommandLineProcessor::Instance().Argc();
}

const char** RtArgv()
{
    return cmajor::rt::CommandLineProcessor::Instance().Argv();
}
