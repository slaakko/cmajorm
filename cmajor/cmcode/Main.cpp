// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/MainWindow.hpp>
#include <cmajor/cmsvc/InitDone.hpp>
#include <sngcm/cmparser/CommandLine.hpp>
#include <soulng/lexer/TrivialLexer.hpp>
#include <cmajor/wing/InitDone.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/Unicode.hpp>
#include <stdexcept>
#include <string>

struct InitDone
{
    InitDone(HINSTANCE instance)
    {
        soulng::util::Init();
        sngxml::xpath::Init();
        cmajor::wing::Init(instance);
        cmajor::service::Init();
    }
    ~InitDone()
    {
        cmajor::service::Done();
        cmajor::wing::Done();
        sngxml::xpath::Done();
        soulng::util::Done();
    }
};

using namespace cmcode;
using namespace soulng::unicode;

int WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
    InitDone initDone(instance);
    try
    {
        std::u32string content = ToUtf32(cmdLine);
        content.append(1, '\n');
        TrivialLexer lexer(content, "", 0);
        std::vector<std::string> args = CommandLineParser::Parse(lexer);
        std::string filePath;
        if (!args.empty())
        {
            filePath = args[0];
        }
        MainWindow mainWindow(filePath);
        mainWindow.SetSmallIcon(Application::GetResourceManager().GetIcon("cmcode.small.icon"));
        Application::Run(mainWindow);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(nullptr, ex.what());
        return 1;
    }
    return 0;
}
