// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/ScrollBar.hpp>
#include <wing/Theme.hpp>

namespace cmajor { namespace wing {

ScrollBarCreateParams::ScrollBarCreateParams(ScrollBarOrientation orientation) : controlCreateParams()
{
    controlCreateParams.WindowClassName("SCROLLBAR");
    controlCreateParams.BackgroundColor(GetColor("scroll.bar.background"));
    int style = WS_CHILD | WS_VISIBLE;
    if (orientation == ScrollBarOrientation::horizontal)
    {
        style = style | SBS_HORZ;
    }
    else if (orientation == ScrollBarOrientation::vertical)
    {
        style = style | SBS_VERT;
    }
    controlCreateParams.WindowStyle(style);
}

ScrollBarCreateParams& ScrollBarCreateParams::Defaults()
{
    return *this;
}

ScrollBarCreateParams& ScrollBarCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

ScrollBarCreateParams& ScrollBarCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

ScrollBarCreateParams& ScrollBarCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

ScrollBarCreateParams& ScrollBarCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

ScrollBarCreateParams& ScrollBarCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

ScrollBarCreateParams& ScrollBarCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

ScrollBarCreateParams& ScrollBarCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

ScrollBarCreateParams& ScrollBarCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

ScrollBarCreateParams& ScrollBarCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

ScrollBarCreateParams& ScrollBarCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

ScrollBar::ScrollBar(ScrollBarCreateParams& createParams) : Control(createParams.controlCreateParams)
{
    SetBackgroundItemName("scroll.bar.background");
}

} } // cmajor::wing
