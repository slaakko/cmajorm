// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/ToolTip.hpp>
#include <wing/TextView.hpp>
#include <soulng/util/Unicode.hpp>

#undef max

namespace cmajor { namespace wing {

using namespace soulng::unicode;

ToolTipCreateParams::ToolTipCreateParams() : controlCreateParams()
{
    controlCreateParams.WindowClassName("wing.ToolTip");
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    controlCreateParams.BackgroundColor(Color::FloralWhite);
    controlCreateParams.WindowStyle(HiddenChildWindowStyle());
    textColor = Color::Black;
    frameColor = Color::Black;
    fontFamilyName = "Segoe UI";
    fontSize = 9.0f;
}

ToolTipCreateParams& ToolTipCreateParams::Defaults()
{
    return *this;
}

ToolTipCreateParams& ToolTipCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

ToolTipCreateParams& ToolTipCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

ToolTipCreateParams& ToolTipCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

ToolTipCreateParams& ToolTipCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

ToolTipCreateParams& ToolTipCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

ToolTipCreateParams& ToolTipCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

ToolTipCreateParams& ToolTipCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

ToolTipCreateParams& ToolTipCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

ToolTipCreateParams& ToolTipCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

ToolTipCreateParams& ToolTipCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

ToolTipCreateParams& ToolTipCreateParams::FontFamilyName(const std::string& fontFamilyName_)
{
    fontFamilyName = fontFamilyName_;
    return *this;
}

ToolTipCreateParams& ToolTipCreateParams::FontSize(float size)
{
    fontSize = size;
    return *this;
}

ToolTip::ToolTip(ToolTipCreateParams& createParams) : 
    Control(createParams.controlCreateParams), 
    flags(ToolTipFlags::none), 
    textColor(createParams.textColor), 
    frameColor(createParams.frameColor),
    framePen(frameColor),
    textBrush(textColor),
    textHeight(0)
{
    stringFormat.SetAlignment(StringAlignment::StringAlignmentNear);
    stringFormat.SetLineAlignment(StringAlignment::StringAlignmentNear);
    std::u16string familyName = ToUtf16(createParams.fontFamilyName);
    SetFont(Font(FontFamily((const WCHAR*)familyName.c_str()), createParams.fontSize, FontStyle::FontStyleRegular, Unit::UnitPoint));
    SetChanged();
}

void ToolTip::MeasureExtent()
{
    Graphics graphics(Handle());
    Measure(graphics);
}

void ToolTip::OnTextChanged()
{
    Control::OnTextChanged();
    lines.clear();
    std::vector<std::u32string> ulines = SplitTextIntoLines(ToUtf32(Text()));
    for (const std::u32string& line : ulines)
    {
        lines.push_back(ToUtf8(line));
    }
    SetChanged();
    Invalidate();
}

void ToolTip::OnPaint(PaintEventArgs& args)
{
    if (Changed())
    {
        ResetChanged();
        Measure(args.graphics);
    }
    Rect r(Point(), GetSize());
    r.Width = r.Width - 1;
    r.Height = r.Height - 1;
    args.graphics.Clear(BackgroundColor());
    CheckGraphicsStatus(args.graphics.DrawRectangle(&framePen, r));
    PointF pt(1, 1);
    for (const std::string& line : lines)
    {
        DrawString(args.graphics, line, GetFont(), pt, textBrush);
        pt.Y = pt.Y + textHeight;
    }
    Control::OnPaint(args);
}

void ToolTip::Measure(Graphics& graphics)
{
    Size size;
    textHeight = 0;
    for (const std::string& line : lines)
    {
        RectF textRect = MeasureString(graphics, line, GetFont(), PointF(0, 0), stringFormat);
        textHeight = std::max(textHeight, textRect.Height);
        size.Width = std::max(size.Width, static_cast<int>(textRect.Width));
    }
    size.Height = static_cast<int>(lines.size() * textHeight);
    size.Width = size.Width + 1;
    size.Height = size.Height + 1;
    SetSize(size);
}

} } // cmajor::wing
