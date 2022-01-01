// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/ToolBar.hpp>
#include <cmajor/wing/Theme.hpp>

namespace cmcode {

Color DefaultToolBarBorderColor()
{
    return Color(204, 206, 219);
}

Color DefaultToolBarTextButtonColor()
{
    return Color(0, 78, 170);
}

Color DefaultToolBarToolTipColor()
{
    return Color(241, 242, 247);
}

int DefaultToolBarToolButtonHeight()
{
    return 30;
}

Padding DefaultToolBarPadding()
{
    return Padding(4, 4, 4, 4);
}

Padding DefaultToolBarToolButtonPadding()
{
    return Padding(2, 0, 2, 0);
}

ToolBar* MakeToolBar()
{
    ToolBar* toolBar = new ToolBar(ToolBarCreateParams().BackgroundColor(GetColor("tool.bar.background")).
        DarkColor(GetColor("tool.bar.tool.button.dark")).
        LightColor(GetColor("tool.bar.tool.button.light")).
        MediumLightColor(GetColor("tool.bar.tool.button.medium.light")).
        MediumDarkColor(GetColor("tool.bar.tool.button.medium.dark")).
        TextColor(GetColor("tool.bar.tool.button.text")).
        DisabledColor(GetColor("tool.bar.tool.button.disabled")).
        SeparatorColor1(GetColor("tool.bar.tool.button.separator1")).
        SeparatorColor2(GetColor("tool.bar.tool.button.separator2")).
        ToolTipWindowColor(GetColor("tool.bar.tool.tip")).
        SetDock(Dock::none).ToolButtonHeight(DefaultToolBarToolButtonHeight()).
        ToolBarPadding(DefaultToolBarPadding()).ToolBarToolButtonPadding(DefaultToolBarToolButtonPadding()).ToolTipWindowColor(DefaultToolBarToolTipColor()));
    toolBar->SetBackgroundItemName("tool.bar.background");
    return toolBar;
}

} // namespace cmcode
