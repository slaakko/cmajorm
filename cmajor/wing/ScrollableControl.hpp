// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_SCROLLABLE_CONTROL_INCLUDED
#define CMAJOR_WING_SCROLLABLE_CONTROL_INCLUDED
#include <wing/Control.hpp>
#include <wing/Container.hpp>

namespace cmajor { namespace wing {

struct WING_API ScrollableControlCreateParams
{
    ScrollableControlCreateParams(Control* child_);
    ScrollableControlCreateParams& WindowClassName(const std::string& windowClassName_);
    ScrollableControlCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    ScrollableControlCreateParams& WindowStyle(int windowStyle_);
    ScrollableControlCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    ScrollableControlCreateParams& BackgroundColor(const Color& backgroundColor_);
    ScrollableControlCreateParams& Text(const std::string& text_);
    ScrollableControlCreateParams& Location(const Point& location_);
    ScrollableControlCreateParams& SetSize(Size size_);
    ScrollableControlCreateParams& SetAnchors(Anchors anchors_);
    ScrollableControlCreateParams& SetDock(Dock dock_);
    ControlCreateParams controlCreateParams;
    Control* child;
};

class WING_API ScrollableControl : public Control
{
public:
    ScrollableControl(ScrollableControlCreateParams& createParams);
    bool IsDecoratorControl() const override { return true; }
    void ScrollLineDown() override;
    void ScrollLineUp() override;
    Control* Child() const { return child; }
    Control* GetFirstEnabledTabStopControl() const override;
    Control* GetLastEnabledTabStopControl() const override;
protected:
    void TranslateChildGraphics(Graphics& graphics) override;
    void TranslateMousePos(Point& location) override;
    void TranslateContentLocation(Point& contentLocation) override;
    void OnLocationChanged() override;
    void OnSizeChanged() override;
    void OnChildSizeChanged(ControlEventArgs& args) override;
    void OnChildContentChanged(ControlEventArgs& args) override;
    void OnChildContentLocationChanged(ControlEventArgs& args) override;
    void OnChildContentSizeChanged(ControlEventArgs& args) override;
    void OnChildGotFocus(ControlEventArgs& args) override;
    void OnChildLostFocus(ControlEventArgs& args) override;
    void OnHScroll(IntArgs& args) override;
    void OnVScroll(IntArgs& args) override;
    void OnMouseWheel(MouseWheelEventArgs& args) override;
private:
    void SetScrollPos(int bar, int pos);
    void SetScrollBarData(int bar, int page, int pos, int min, int max);
    void GetScrollBarData(int bar, int& page, int& pos, int& min, int& max, int& trackpos);
    void ScrollChild(int dx, int dy);
    Rect MakeUpdateRect(int xAmount, int yAmount);
    void ChildSizeOrContentSizeChanged(ControlEventArgs& args);
    Control* child;
    Control* scrolledChild;
    Container container;
    ScrollUnits scrollUnits;
    int hpos;
    int hmin;
    int hmax;
    int hpage;
    int htrackpos;
    int vpos;
    int vmin;
    int vmax;
    int vpage;
    int vtrackpos;
    bool verticalScrollBarShown;
    bool horizontalScrollBarShown;
};

} } // cmajor::wing

#endif // CMAJOR_WING_SCROLLABLE_CONTROL_INCLUDED
