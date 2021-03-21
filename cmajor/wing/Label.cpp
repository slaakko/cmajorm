// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/wing/Label.hpp>

namespace cmajor { namespace wing {

LabelCreateParams::LabelCreateParams() : controlCreateParams()
{
    controlCreateParams.WindowClassName("STATIC");
    controlCreateParams.WindowStyle(DefaultChildWindowStyle() | SS_LEFT);
    autoSize = true;
}

LabelCreateParams& LabelCreateParams::Defaults()
{
    return *this;
}

LabelCreateParams& LabelCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

LabelCreateParams& LabelCreateParams::WindowClassStyle(uint32_t windowClassStyle_) 
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

LabelCreateParams& LabelCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

LabelCreateParams& LabelCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

LabelCreateParams& LabelCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

LabelCreateParams& LabelCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

LabelCreateParams& LabelCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

LabelCreateParams& LabelCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

LabelCreateParams& LabelCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

LabelCreateParams& LabelCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

LabelCreateParams& LabelCreateParams::AutoSize(bool autoSize_)
{
    autoSize = autoSize_;
    return *this;
}

Label::Label(LabelCreateParams& createParams) : Control(createParams.controlCreateParams), flags()
{
    if (createParams.autoSize)
    {
        flags = flags | LabelFlags::autoSize;
    }
}

void Label::OnCreated()
{
    Control::OnCreated();
    Graphics graphics(Handle());
    const FontHandle& fontHandle = GetFontHandle(graphics);
    if (!fontHandle.IsNull())
    {
        SendMessage(Handle(), WM_SETFONT, reinterpret_cast<WPARAM>(fontHandle.Handle()), 0);
    }
    if ((flags & LabelFlags::autoSize) != LabelFlags::none)
    {
        DoAutoSize();
    }
}

void Label::OnTextChanged()
{
    Control::OnTextChanged();
    if ((flags & LabelFlags::autoSize) != LabelFlags::none)
    {
        flags = flags & ~LabelFlags::autoSized;
        DoAutoSize();
    }
}

void Label::DoAutoSize()
{
    if ((flags & LabelFlags::autoSized) != LabelFlags::none) return;
    if (!Handle()) return;
    Graphics graphics(Handle());
    const Font& font = GetFont();
    StringFormat stringFormat;
    stringFormat.SetAlignment(StringAlignment::StringAlignmentNear);
    stringFormat.SetLineAlignment(StringAlignment::StringAlignmentNear);
    RectF r = MeasureString(graphics, Text(), font, PointF(0, 0), stringFormat);
    SetSize(Size(static_cast<int>(r.Width), static_cast<int>(r.Height)));
    flags = flags | LabelFlags::autoSized;
}

} } // cmajor::wing
