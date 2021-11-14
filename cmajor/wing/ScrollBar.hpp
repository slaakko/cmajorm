// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_SCROLL_BAR_INCLUDED
#define CMAJOR_WING_SCROLL_BAR_INCLUDED
#include <wing/Control.hpp>

namespace cmajor { namespace wing {

enum class ScrollBarOrientation
{
    horizontal, vertical
};

struct WING_API ScrollBarCreateParams
{
    ScrollBarCreateParams(ScrollBarOrientation orientation);
    ScrollBarCreateParams& Defaults();
    ScrollBarCreateParams& WindowClassName(const std::string& windowClassName_);
    ScrollBarCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    ScrollBarCreateParams& WindowStyle(int windowStyle_);
    ScrollBarCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    ScrollBarCreateParams& BackgroundColor(const Color& backgroundColor_);
    ScrollBarCreateParams& Text(const std::string& text_);
    ScrollBarCreateParams& Location(Point location_);
    ScrollBarCreateParams& SetSize(Size size_);
    ScrollBarCreateParams& SetAnchors(Anchors anchors_);
    ScrollBarCreateParams& SetDock(Dock dock_);
    ControlCreateParams controlCreateParams;
};

class WING_API ScrollBar : public Control
{
public:
    ScrollBar(ScrollBarCreateParams& createParams);
};

} } // cmajor::wing

#endif // CMAJOR_WING_SCROLL_BAR_INCLUDED
