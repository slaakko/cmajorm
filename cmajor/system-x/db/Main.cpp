// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/db/MainWindow.hpp>
#include <system-x/guiterm/Api.hpp>
#include <wing/InitDone.hpp>
#include <system-x/kernel/InitDone.hpp>
#include <system-x/machine/InitDone.hpp>
#include <system-x/db/Themes.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <sngcm/cmparser/CommandLine.hpp>
#include <soulng/parser/Parser.hpp>
#include <soulng/lexer/TrivialLexer.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/Path.hpp>
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
    cmsx::kernel::Init(false);
    cmsx::db::InitThemes();
}

void DoneApplication()
{
    cmsx::kernel::Done();
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
        if (!commandLine.empty())
        {
            std::u32string content = ToUtf32(commandLine);
            content.append(1, '\n');
            TrivialLexer lexer(content, "", 0);
            args = CommandLineParser::Parse(lexer);
            if (!args.empty())
            {
                filePath = GetFullPath(args[0]);
            }
        }
        MainWindow mainWindow(filePath);
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
