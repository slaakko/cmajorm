// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmpmv/MainWindow.hpp>
#include <cmajor/cmpmv/Themes.hpp>
#include <cmajor/wing/Application.hpp>
#include <cmajor/wing/InitDone.hpp>
#include <cmajor/wing/Graphics.hpp>
#include <cmajor/cmsvc/InitDone.hpp>
#include <cmajor/cmview/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/Unicode.hpp>
#include <sngxml/serialization/InitDone.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <cmpm/Register.hpp>
#include <stdexcept>

void InitApplication(HINSTANCE instance)
{
    soulng::util::Init();
    sngxml::xpath::Init();
    sngxml::xmlser::Init();
    cmajor::wing::Init(instance);
    cmajor::service::Init();
    cmajor::view::Init();
    cmpm::Register();
    cmajor::pmv::InitThemes();
}

void DoneApplication()
{
    cmajor::view::Done();
    cmajor::service::Done();
    cmajor::wing::Done();
    sngxml::xmlser::Done();
    sngxml::xpath::Done();
    soulng::util::Done();
}

using namespace cmajor::pmv;
using namespace cmajor::wing;


int WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
    try
    {
        InitApplication(instance);
        MainWindow mainWindow;
        mainWindow.SetIcon(Application::GetResourceManager().GetIcon("cmpmv.icon"));
        mainWindow.SetSmallIcon(Application::GetResourceManager().GetIcon("cmpmv.icon"));
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
