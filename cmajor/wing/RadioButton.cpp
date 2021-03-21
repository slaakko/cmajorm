// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/wing/RadioButton.hpp>
#include <cmajor/wing/GroupBox.hpp>

#undef max
#undef min

namespace cmajor { namespace wing {

RadioButtonCreateParams::RadioButtonCreateParams() : controlCreateParams(), autoSize(false)
{
    controlCreateParams.WindowClassName("BUTTON");
    controlCreateParams.WindowStyle(DefaultChildWindowStyle() | WS_TABSTOP | BS_RADIOBUTTON | BS_NOTIFY);
    autoSize = true;
}

RadioButtonCreateParams& RadioButtonCreateParams::Defaults()
{
    return *this;
}

RadioButtonCreateParams& RadioButtonCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

RadioButtonCreateParams& RadioButtonCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

RadioButtonCreateParams& RadioButtonCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

RadioButtonCreateParams& RadioButtonCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

RadioButtonCreateParams& RadioButtonCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

RadioButtonCreateParams& RadioButtonCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

RadioButtonCreateParams& RadioButtonCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

RadioButtonCreateParams& RadioButtonCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

RadioButtonCreateParams& RadioButtonCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

RadioButtonCreateParams& RadioButtonCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

RadioButtonCreateParams& RadioButtonCreateParams::AutoSize(bool autoSize_)
{
    autoSize = autoSize_;
    return *this;
}

RadioButton::RadioButton(RadioButtonCreateParams& createParams) : ButtonBase(createParams.controlCreateParams), flags(RadioButtonFlags::none)
{
    if (createParams.autoSize)
    {
        flags = flags | RadioButtonFlags::autoSize;
    }
    else
    {
        flags = flags & ~RadioButtonFlags::autoSize;
    }
}

void RadioButton::SetChecked(bool checked)
{
    if (checked)
    {
        Control* parent = ParentControl();
        if (parent)
        {
            if (parent->IsGroupBox())
            {
                GroupBox* groupBox = static_cast<GroupBox*>(parent);
                if (groupBox->CheckedRadioButton() != this)
                {
                    groupBox->SetCheckedRadioButton(this);
                }
            }
        }
        SetCheckedFlag();
        if (Handle())
        {
            SendMessage(Handle(), BM_SETCHECK, BST_CHECKED, 0);
        }
        SetFocus();
    }
    else
    {
        Control* parent = ParentControl();
        if (parent)
        {
            if (parent->IsGroupBox())
            {
                GroupBox* groupBox = static_cast<GroupBox*>(parent);
                if (groupBox->CheckedRadioButton() == this)
                {
                    groupBox->ResetCheckedRadioButton();
                }
            }
        }
        ResetCheckedFlag();
        if (Handle())
        {
            SendMessage(Handle(), BM_SETCHECK, BST_UNCHECKED, 0);
        }
    }
}

void RadioButton::OnCreated()
{
    ButtonBase::OnCreated();
    Graphics graphics(Handle());
    const FontHandle& fontHandle = GetFontHandle(graphics);
    if (!fontHandle.IsNull())
    {
        SendMessage(Handle(), WM_SETFONT, reinterpret_cast<WPARAM>(fontHandle.Handle()), 0);
    }
    if (Checked())
    {
        SetChecked(true);
    }
    else
    {
        SetChecked(false);
    }
    if ((flags & RadioButtonFlags::autoSize) != RadioButtonFlags::none)
    {
        DoAutoSize();
    }
}

void RadioButton::OnClick()
{
    ButtonBase::OnClick();
    SetChecked(true);
}

void RadioButton::OnTextChanged()
{
    ButtonBase::OnTextChanged();
    if ((flags & RadioButtonFlags::autoSize) != RadioButtonFlags::none)
    {
        flags = flags & ~RadioButtonFlags::autoSized;
        DoAutoSize();
    }
}

void RadioButton::DoAutoSize()
{
    if ((flags & RadioButtonFlags::autoSized) != RadioButtonFlags::none) return;
    if (!Handle()) return;
    Graphics graphics(Handle());
    const Font& font = GetFont();
    StringFormat stringFormat;
    stringFormat.SetAlignment(StringAlignment::StringAlignmentNear);
    stringFormat.SetLineAlignment(StringAlignment::StringAlignmentNear);
    RectF r = MeasureString(graphics, Text(), font, PointF(0, 0), stringFormat);
    Size buttonSize = GetButtonSize();
    Size borderSize = GetBorderSize();
    r.Width = r.Width + buttonSize.Width + borderSize.Width;
    r.Height = std::max(r.Height, float(buttonSize.Height + borderSize.Height));
    SetSize(Size(static_cast<int>(r.Width), static_cast<int>(r.Height)));
    flags = flags | RadioButtonFlags::autoSized;

}

Size RadioButton::GetButtonSize()
{
    int x = GetSystemMetrics(SM_CXMENUCHECK);
    int y = GetSystemMetrics(SM_CYMENUCHECK);
    return Size(x, y);
}

Size RadioButton::GetBorderSize()
{
    int x = GetSystemMetrics(SM_CXBORDER);
    int y = GetSystemMetrics(SM_CYBORDER);
    return Size(x, y);
}

void RadioButton::OnCheckedChanged()
{
    checkedChanged.Fire();
}

void RadioButton::SetCheckedFlag()
{
    if (!Checked())
    {
        flags = flags | RadioButtonFlags::checked;
        OnCheckedChanged();
    }
}

void RadioButton::ResetCheckedFlag()
{
    if (Checked())
    {
        flags = flags & ~RadioButtonFlags::checked;
        OnCheckedChanged();
    }
}

} } // cmajor::wing