// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/db/MainWindow.hpp>
#include <wing/InitDone.hpp>
#include <system-x/os/InitDone.hpp>
#include <system-x/machine/InitDone.hpp>
#include <system-x/db/Themes.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <sngcm/cmparser/CommandLine.hpp>
#include <soulng/parser/Parser.hpp>
#include <soulng/lexer/TrivialLexer.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/Unicode.hpp>
#include <string>
#include <iostream>
#include <vector>

using namespace soulng::util;

void InitApplication(HINSTANCE instance)
{
    soulng::util::Init();
    sngxml::xpath::Init();
    cmajor::wing::Init(instance);
    cmsx::machine::Init();
    cmsx::os::Init();
    cmsx::db::InitThemes();
}

void DoneApplication()
{
    cmsx::os::Done();
    cmsx::machine::Done();
    cmajor::wing::Done();
    sngxml::xpath::Done();
    soulng::util::Done();
}

using namespace cmsx::db;
using namespace soulng::unicode;

int WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
    try
    {
        InitApplication(instance);
        std::string filePath;
        std::string commandLine(cmdLine);
        std::vector<std::string> args;
        std::vector<std::string> env;
        if (!commandLine.empty())
        {
            std::u32string content = ToUtf32(commandLine);
            content.append(1, '\n');
            TrivialLexer lexer(content, "", 0);
            args = CommandLineParser::Parse(lexer);
            if (!args.empty())
            {
                filePath = args[0];
            }
        }
        MainWindow mainWindow(filePath, args, env);
        mainWindow.SetIcon(Application::GetResourceManager().GetIcon("bug.icon"));
        mainWindow.SetSmallIcon(Application::GetResourceManager().GetIcon("bug.icon"));
        Application::Run(mainWindow);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(nullptr, ex.what());
        return 1;
    }
    DoneApplication();
    return 0;

}