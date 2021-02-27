// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/wing/InitDone.hpp>
#include <cmajor/wing/Metrics.hpp>
#include <cmajor/wing/Wing.hpp>
#include <cmajor/wing/Application.hpp>

namespace cmajor { namespace wing {

void Init(HINSTANCE instance)
{
    WingInit(instance);
    ApplicationInit();
    LoadMetrics();
}

void Done()
{
    ApplicationDone();
    WingDone();
}

} } // cmajor::wing