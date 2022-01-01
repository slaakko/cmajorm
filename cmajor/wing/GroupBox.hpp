// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_GROUP_BOX_INCLUDED
#define CMAJOR_WING_GROUP_BOX_INCLUDED
#include <wing/Button.hpp>

namespace cmajor { namespace wing {

class RadioButton;

struct WING_API GroupBoxCreateParams
{
    GroupBoxCreateParams();
    GroupBoxCreateParams& Defaults();
    GroupBoxCreateParams& WindowClassName(const std::string& windowClassName_);
    GroupBoxCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    GroupBoxCreateParams& WindowStyle(int windowStyle_);
    GroupBoxCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    GroupBoxCreateParams& BackgroundColor(const Color& backgroundColor_);
    GroupBoxCreateParams& Text(const std::string& text_);
    GroupBoxCreateParams& Location(Point location_);
    GroupBoxCreateParams& SetSize(Size size_);
    GroupBoxCreateParams& SetAnchors(Anchors anchors_);
    GroupBoxCreateParams& SetDock(Dock dock_);
    ControlCreateParams controlCreateParams;
};

class WING_API GroupBox : public ButtonBase
{
public:
    GroupBox(GroupBoxCreateParams& createParams);
    bool IsGroupBox() const override { return true; }
    void AddChild(Control* child);
    std::unique_ptr<Control> RemoveChild(Control* child);
    void InsertChildBefore(Control* child, Control* before);
    void InsertChildAfter(Control* child, Control* after);
    const Container& Children() const { return children; }
    RadioButton* CheckedRadioButton() const { return checkedRadioButton; }
    void SetCheckedRadioButton(RadioButton* radioButton);
    void ResetCheckedRadioButton();
protected:
    void OnCreated() override;
    void OnControlAdded(ControlEventArgs& args) override;
    void OnControlRemoved(ControlEventArgs& args) override;
private:
    Container children;
    RadioButton* checkedRadioButton;
};

} } // cmajor::wing

#endif // CMAJOR_WING_GROUP_BOX_INCLUDED
