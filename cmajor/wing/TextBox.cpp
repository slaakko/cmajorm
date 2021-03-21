// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/wing/TextBox.hpp>

namespace cmajor { namespace wing {

TextBoxCreateParams::TextBoxCreateParams() : controlCreateParams(), selectAll(false), multiline(false)
{
    controlCreateParams.WindowClassName("EDIT");
    controlCreateParams.WindowStyle(DefaultChildWindowStyle() | ES_LEFT | ES_AUTOHSCROLL | WS_TABSTOP);
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    controlCreateParams.BackgroundColor(Color::White);
    selectAll = true;
}

TextBoxCreateParams& TextBoxCreateParams::Defaults()
{
    return *this;
}

TextBoxCreateParams& TextBoxCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

TextBoxCreateParams& TextBoxCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

TextBoxCreateParams& TextBoxCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

TextBoxCreateParams& TextBoxCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

TextBoxCreateParams& TextBoxCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

TextBoxCreateParams& TextBoxCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

TextBoxCreateParams& TextBoxCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

TextBoxCreateParams& TextBoxCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

TextBoxCreateParams& TextBoxCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

TextBoxCreateParams& TextBoxCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

TextBoxCreateParams& TextBoxCreateParams::SelectAll(bool selectAll_)
{
    selectAll = selectAll_;
    return *this;
}

TextBoxCreateParams& TextBoxCreateParams::Multiline(bool multiline_)
{
    multiline = multiline_;
    return *this;
}

ControlCreateParams& MakeTextBoxControlCreateParams(TextBoxCreateParams& createParams)
{
    if (createParams.multiline)
    {
        createParams.controlCreateParams.WindowStyle(createParams.controlCreateParams.windowStyle | ES_MULTILINE);
    }
    return createParams.controlCreateParams;
}

TextBox::TextBox(TextBoxCreateParams& createParams) : Control(MakeTextBoxControlCreateParams(createParams)), flags(TextBoxFlags::none)
{
    if (createParams.selectAll)
    {
        flags = flags | TextBoxFlags::selectAll;
    }
    if (createParams.multiline)
    {
        SetMultiline();
    }
}

void TextBox::Clear()
{
    SetText(std::string());
}

void TextBox::SelectAll()
{
    if (Handle())
    {
        SendMessage(Handle(), EM_SETSEL, 0, -1);
    }
    else
    {
        flags = flags | TextBoxFlags::selectAll;
    }
}

void TextBox::OnCreated()
{
    Control::OnCreated();
    Graphics graphics(Handle());
    const FontHandle& fontHandle = GetFontHandle(graphics);
    if (!fontHandle.IsNull())
    {
        SendMessage(Handle(), WM_SETFONT, reinterpret_cast<WPARAM>(fontHandle.Handle()), 0);
    }
    if ((flags & TextBoxFlags::selectAll) != TextBoxFlags::none)
    {
        flags = flags & ~TextBoxFlags::selectAll;
        SelectAll();
    }
}

void TextBox::OnGotFocus()
{
    Control::OnGotFocus();
    SelectAll();
}

} } // cmajor::wing
