// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/Panel.hpp>
#include <wing/Theme.hpp>

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
