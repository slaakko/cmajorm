// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/InitDone.hpp>
#include <wing/Metrics.hpp>
#include <wing/Wing.hpp>
#include <wing/Application.hpp>
#include <wing/Shell.hpp>
#include <wing/Theme.hpp>

namespace cmajor { namespace wing {

void Init(HINSTANCE instance)
{
    WingInit(instance);
    ApplicationInit();
    LoadMetrics();
    ShellInit();
    ThemeInit();
}

void Done()
{
    ThemeDone();
    ShellDone();
    ApplicationDone();
    WingDone();
}

} } // cmajor::wing