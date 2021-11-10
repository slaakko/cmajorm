// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_PADDED_CONTROL_INCLUDED
#define CMAJOR_WING_PADDED_CONTROL_INCLUDED
#include <wing/Control.hpp>
#include <wing/Container.hpp>

namespace cmajor { namespace wing {

const int defaultControlPadding = 4;

WING_API Padding DefaultPadding();

WING_API Size PaddedSize(const Size& size, const Padding& padding);

struct WING_API PaddedControlCreateParams
{
    PaddedControlCreateParams(Control* child_);
    PaddedControlCreateParams& Defaults();
    PaddedControlCreateParams& WindowClassName(const std::string& windowClassName_);
    PaddedControlCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    PaddedControlCreateParams& WindowStyle(int windowStyle_);
    PaddedControlCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    PaddedControlCreateParams& BackgroundColor(const Color& backgroundColor_);
    PaddedControlCreateParams& Text(const std::string& text_);
    PaddedControlCreateParams& Location(const Point& location_);
    PaddedControlCreateParams& SetSize(Size size_);
    PaddedControlCreateParams& SetAnchors(Anchors anchors_);
    PaddedControlCreateParams& SetDock(Dock dock_);
    PaddedControlCreateParams& SetPadding(const Padding& padding_);
    ControlCreateParams controlCreateParams;
    Control* child;
    Padding padding;
};

class WING_API PaddedControl : public Control
{
public:
    PaddedControl(PaddedControlCreateParams& createParams);
    Control* Child() const { return child; }
    bool IsDecoratorControl() const override { return true; }
    const Padding& GetPadding() const { return padding; }
    Control* GetFirstEnabledTabStopControl() const override;
    Control* GetLastEnabledTabStopControl() const override;
    std::string BackgroundItemName() const override;
protected:
    void OnPaint(PaintEventArgs& args) override;
    void OnLocationChanged() override;
    void OnSizeChanged() override;
    void OnChildSizeChanged(ControlEventArgs& args) override;
    void OnChildContentChanged(ControlEventArgs& args) override;
    void OnChildContentLocationChanged(ControlEventArgs& args) override;
    void OnChildContentSizeChanged(ControlEventArgs& args) override;
    void OnChildGotFocus(ControlEventArgs& args) override;
    void OnChildLostFocus(ControlEventArgs& args) override;
private:
    void SetChildPos();
    Control* child;
    Container container;
    Padding padding;
};

} } // cmajor::wing

#endif // CMAJOR_WING_PADDED_CONTROL_INCLUDED
