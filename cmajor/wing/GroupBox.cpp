// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/GroupBox.hpp>
#include <wing/RadioButton.hpp>

namespace cmajor { namespace wing {

GroupBoxCreateParams::GroupBoxCreateParams() : controlCreateParams()
{
    controlCreateParams.WindowClassName("BUTTON");
    controlCreateParams.WindowStyle(DefaultChildWindowStyle() | WS_TABSTOP | BS_GROUPBOX | BS_NOTIFY);
}

GroupBoxCreateParams& GroupBoxCreateParams::Defaults()
{
    return *this;
}

GroupBoxCreateParams& GroupBoxCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

GroupBoxCreateParams& GroupBoxCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

GroupBoxCreateParams& GroupBoxCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

GroupBoxCreateParams& GroupBoxCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

GroupBoxCreateParams& GroupBoxCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

GroupBoxCreateParams& GroupBoxCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

GroupBoxCreateParams& GroupBoxCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

GroupBoxCreateParams& GroupBoxCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

GroupBoxCreateParams& GroupBoxCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

GroupBoxCreateParams& GroupBoxCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

GroupBox::GroupBox(GroupBoxCreateParams& createParams) : ButtonBase(createParams.controlCreateParams), children(this), checkedRadioButton(nullptr)
{
}

void GroupBox::AddChild(Control* child)
{
    children.AddChild(child);
    ControlEventArgs args(child);
    OnControlAdded(args);
}

std::unique_ptr<Control> GroupBox::RemoveChild(Control* child)
{
    std::unique_ptr<Component> childComponent = children.RemoveChild(child);
    ControlEventArgs args(child);
    OnControlRemoved(args);
    return std::unique_ptr<Control>(static_cast<Control*>(childComponent.release()));
}

void GroupBox::InsertChildBefore(Control* child, Control* before)
{
    children.InsertBefore(child, before);
    ControlEventArgs args(child);
    OnControlAdded(args);
}

void GroupBox::InsertChildAfter(Control* child, Control* after)
{
    children.InsertAfter(child, after);
    ControlEventArgs args(child);
    OnControlAdded(args);
}

void GroupBox::SetCheckedRadioButton(RadioButton* radioButton)
{
    if (radioButton != checkedRadioButton)
    {
        if (checkedRadioButton)
        {
            checkedRadioButton->SetChecked(false);
        }
        checkedRadioButton = radioButton;
        if (checkedRadioButton)
        {
            checkedRadioButton->SetChecked(true);
        }
    }
}

void GroupBox::ResetCheckedRadioButton()
{
    checkedRadioButton = nullptr;
}

void GroupBox::OnCreated()
{
    ButtonBase::OnCreated();
    Graphics graphics(Handle());
    const FontHandle& fontHandle = GetFontHandle(graphics);
    if (!fontHandle.IsNull())
    {
        SendMessage(Handle(), WM_SETFONT, reinterpret_cast<WPARAM>(fontHandle.Handle()), 0);
    }
    SubClassCommandWndProc();
}

void GroupBox::OnControlAdded(ControlEventArgs& args)
{
    ButtonBase::OnControlAdded(args);
    if (args.control->IsRadioButton())
    {
        if (!checkedRadioButton)
        {
            SetCheckedRadioButton(static_cast<RadioButton*>(args.control));
        }
    }
}

void GroupBox::OnControlRemoved(ControlEventArgs& args)
{
    ButtonBase::OnControlRemoved(args);
    if (args.control->IsRadioButton())
    {
        if (checkedRadioButton == args.control)
        {
            ResetCheckedRadioButton();
        }
    }
}

} } // cmajor::wing
