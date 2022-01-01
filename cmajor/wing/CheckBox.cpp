// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/CheckBox.hpp>

#undef min
#undef max
 
namespace cmajor { namespace wing {

Color DefaultCheckBoxBackgroundColor()
{
    return Color::Red;
}

CheckBoxCreateParams::CheckBoxCreateParams() : controlCreateParams(), autoSize(false)
{
    controlCreateParams.WindowClassName("BUTTON");
    controlCreateParams.WindowStyle(DefaultChildWindowStyle() | WS_TABSTOP | BS_CHECKBOX | BS_NOTIFY);
    autoSize = true;
}

CheckBoxCreateParams& CheckBoxCreateParams::Defaults()
{
    return *this;
}

CheckBoxCreateParams& CheckBoxCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

CheckBoxCreateParams& CheckBoxCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

CheckBoxCreateParams& CheckBoxCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

CheckBoxCreateParams& CheckBoxCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

CheckBoxCreateParams& CheckBoxCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

CheckBoxCreateParams& CheckBoxCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

CheckBoxCreateParams& CheckBoxCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

CheckBoxCreateParams& CheckBoxCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

CheckBoxCreateParams& CheckBoxCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

CheckBoxCreateParams& CheckBoxCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

CheckBoxCreateParams& CheckBoxCreateParams::AutoSize(bool autoSize_)
{
    autoSize = autoSize_;
    return *this;
}

CheckBox::CheckBox(CheckBoxCreateParams& createParams) : ButtonBase(createParams.controlCreateParams), flags(CheckBoxFlags::none)
{
    if (createParams.autoSize)
    {
        flags = flags | CheckBoxFlags::autoSize;
    }
    else
    {
        flags = flags & ~CheckBoxFlags::autoSize;
    }
}

void CheckBox::SetChecked(bool checked)
{
    if (checked)
    {
        SetCheckedFlag();
        if (Handle())
        {
            SendMessage(Handle(), BM_SETCHECK, BST_CHECKED, 0);
        }
    }
    else
    {
        ResetCheckedFlag();
        if (Handle())
        {
            SendMessage(Handle(), BM_SETCHECK, BST_UNCHECKED, 0);
        }
    }
}

void CheckBox::OnCreated()
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
    if ((flags & CheckBoxFlags::autoSize) != CheckBoxFlags::none)
    {
        DoAutoSize();
    }
}

void CheckBox::OnClick()
{
    ButtonBase::OnClick();
    SetChecked(!Checked());
}

void CheckBox::OnTextChanged()
{
    ButtonBase::OnTextChanged();
    if ((flags & CheckBoxFlags::autoSize) != CheckBoxFlags::none)
    {
        flags = flags & ~CheckBoxFlags::autoSized;
        DoAutoSize();
    }
}

void CheckBox::OnCheckedChanged()
{
    checkedChanged.Fire();
}

void CheckBox::DoAutoSize()
{
    if ((flags & CheckBoxFlags::autoSized) != CheckBoxFlags::none) return;
    if (!Handle()) return;
    Graphics graphics(Handle());
    const Font& font = GetFont();
    StringFormat stringFormat;
    stringFormat.SetAlignment(StringAlignment::StringAlignmentNear);
    stringFormat.SetLineAlignment(StringAlignment::StringAlignmentNear);
    RectF r = MeasureString(graphics, Text(), font, PointF(0, 0), stringFormat);
    Size checkSize = GetCheckSize();
    Size borderSize = GetBorderSize();
    r.Width = r.Width + checkSize.Width + borderSize.Width;
    r.Height = std::max(r.Height, float(checkSize.Height + borderSize.Height));
    SetSize(Size(static_cast<int>(r.Width), static_cast<int>(r.Height)));
    flags = flags | CheckBoxFlags::autoSized;
}

Size CheckBox::GetCheckSize()
{
    int x = GetSystemMetrics(SM_CXMENUCHECK);
    int y = GetSystemMetrics(SM_CYMENUCHECK);
    return Size(x, y);
}

Size CheckBox::GetBorderSize()
{
    int x = GetSystemMetrics(SM_CXBORDER);
    int y = GetSystemMetrics(SM_CYBORDER);
    return Size(x, y);
}

void CheckBox::SetCheckedFlag()
{
    if (!Checked())
    {
        flags = flags | CheckBoxFlags::checked;
        OnCheckedChanged();
    }
}

void CheckBox::ResetCheckedFlag()
{
    if (Checked())
    {
        flags = flags & ~CheckBoxFlags::checked;
        OnCheckedChanged();
    }
}

} } // cmajor::wing
