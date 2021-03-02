// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/wing/Panel.hpp>

namespace cmajor { namespace wing {

ControlCreateParams& MakePanelCreateParams(ControlCreateParams& createParams)
{
    if (createParams.windowClassName.empty())
    { 
        createParams.WindowClassName("wing.Panel");
    }
    createParams.WindowStyle(DefaultChildWindowStyle());
    return createParams;
}

Panel::Panel(ControlCreateParams& createParams) : ContainerControl(MakePanelCreateParams(createParams))
{
}

void Panel::OnPaint(PaintEventArgs& args)
{
    args.graphics.Clear(BackgroundColor());
    ContainerControl::OnPaint(args);
}

} } // cmajor::wing
