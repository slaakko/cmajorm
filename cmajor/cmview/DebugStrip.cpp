// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmview/DebugStrip.hpp>
#include <cmajor/cmview/Bitmaps.hpp>

namespace cmajor { namespace view {

AddBreakpointEventArgs::AddBreakpointEventArgs() : filePath(), breakpoint(nullptr)
{
}

Color DefaultDebugStripBackgroundColor()
{
    return Color(230, 231, 232);
}

Color DefaultBreakpointColor()
{
    return Color(228, 20, 0);
}

Color DefaultTransparentColor()
{
    return Color(0, 128, 128);
}

Point DefaultArrowOffset()
{
    return Point(1, 1);
}

Point DefaultBreakpointOffset()
{
    return Point(0, -2);
}

Size DefaultBreakpointInflate()
{
    return Size(-4, -4);
}

int DefaultDebugStripWidth()
{
    return 20;
}

Dock DefaultDebugStripDock()
{
    return Dock::left;
}

DebugStripCreateParams::DebugStripCreateParams(BreakpointList* breakpointList_) : controlCreateParams(), view(nullptr), breakpointList(breakpointList_)
{
    controlCreateParams.WindowClassName("cmajor.view.DebugStrip");
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    controlCreateParams.BackgroundColor(DefaultDebugStripBackgroundColor());
    breakpointColor = DefaultBreakpointColor();
    transparentColor = DefaultTransparentColor();
    arrowOffset = DefaultArrowOffset();
    breakpointOffset = DefaultBreakpointOffset();
    breakpointInflate = DefaultBreakpointInflate();
    width = DefaultDebugStripWidth();
    controlCreateParams.SetSize(Size(width, 0));
    controlCreateParams.SetDock(DefaultDebugStripDock());
}

DebugStripCreateParams& DebugStripCreateParams::Defaults()
{
    return *this;
}

DebugStripCreateParams& DebugStripCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

DebugStripCreateParams& DebugStripCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

DebugStripCreateParams& DebugStripCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

DebugStripCreateParams& DebugStripCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

DebugStripCreateParams& DebugStripCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

DebugStripCreateParams& DebugStripCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

DebugStripCreateParams& DebugStripCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

DebugStripCreateParams& DebugStripCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

DebugStripCreateParams& DebugStripCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

DebugStripCreateParams& DebugStripCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

DebugStripCreateParams& DebugStripCreateParams::SetTextView(TextView* textView_)
{
    view = textView_;
    return *this;
}

DebugStripCreateParams& DebugStripCreateParams::BreakpointColor(const Color& color)
{
    breakpointColor = color;
    return *this;
}

DebugStripCreateParams& DebugStripCreateParams::TransparentColor(const Color& color)
{
    transparentColor = color;
    return *this;
}

DebugStripCreateParams& DebugStripCreateParams::ArrowOffset(const Point& offset)
{
    arrowOffset = offset;
    return *this;
}

DebugStripCreateParams& DebugStripCreateParams::BreakpointOffset(const Point& offset)
{
    breakpointOffset = offset;
    return *this;
}

DebugStripCreateParams& DebugStripCreateParams::BreakpointInflate(const Size& inflate)
{
    breakpointInflate = inflate;
    return *this;
}

DebugStripCreateParams& DebugStripCreateParams::Width(int width_)
{
    width = width_;
    controlCreateParams.SetSize(Size(width, 0));
    return *this;
}

DebugStrip::DebugStrip(DebugStripCreateParams& createParams) :
    Control(createParams.controlCreateParams),
    view(createParams.view),
    topLineNumber(1),
    origin(),
    breakpointList(createParams.breakpointList),
    breakpointColor(createParams.breakpointColor), 
    transparentColor(createParams.transparentColor),
    arrowOffset(createParams.arrowOffset),
    breakpointOffset(createParams.breakpointOffset),
    breakpointInflate(createParams.breakpointInflate),
    arrowBitmap(GetBitmap("cm.views.arrow.bitmap")),
    backgroundBrush(BackgroundColor()),
    breakpointBrush(breakpointColor),
    breakpointPen(breakpointColor)
{
    view->ContentLocationChanged().AddHandler(this, &DebugStrip::ViewContentLocationChanged);
    view->LineInserted().AddHandler(this, &DebugStrip::ViewLineInserted);
    view->LineDeleted().AddHandler(this, &DebugStrip::ViewLineDeleted);
}

Breakpoint* DebugStrip::GetBreakpoint(int line) const
{
    return breakpointList->GetBreakpoint(line);
}

void DebugStrip::AddBreakpoint(Breakpoint* breakpoint)
{
    breakpointList->AddBreakpoint(breakpoint);
    OnBreakpointAdded(breakpointList->FilePath(), breakpoint);
}

void DebugStrip::RemoveBreakpoint(Breakpoint* breakpoint)
{
    std::string breakpointId = breakpoint->info.breakpointId;
    breakpointList->RemoveBreakpoint(breakpoint);
    OnBreakpointRemoved(breakpointId);
}

void DebugStrip::SetDebugLocation(const SourceSpan& debugLocation_)
{
    debugLocation = debugLocation_;
}

void DebugStrip::ResetDebugLocation()
{
    debugLocation = SourceSpan();
}

void DebugStrip::OnPaint(PaintEventArgs& args)
{
    try
    {
        args.graphics.Clear(BackgroundColor());
        if (breakpointList->IsEmpty() && debugLocation == SourceSpan()) return;
        if (view->CharHeight() == 0 || view->CharWidth() == 0) return;
        Gdiplus::SmoothingMode prevSmoothingMode = args.graphics.GetSmoothingMode();
        args.graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
        Point myOrigin = origin;
        view->TranslateContentLocationInternal(myOrigin);
        int visibleLineCount = view->GetVisibleLineCount();
        float topLineDiff = view->TopLineDiff();
        PointF o(myOrigin.X, myOrigin.Y);
        float charHeight = view->CharHeight();
        Size s = GetSize();
        SizeF size(s.Width, s.Width);
        for (int line = topLineNumber; line < topLineNumber + visibleLineCount; ++line)
        {
            Breakpoint* bp = GetBreakpoint(line);
            RectF r(o, size);
            r.Inflate(breakpointInflate.Width, breakpointInflate.Height);
            r.Offset(breakpointOffset.X, breakpointOffset.Y);
            r.Offset(0.0f, topLineDiff);
            if (bp)
            {
                if (bp->disabled)
                {
                    CheckGraphicsStatus(args.graphics.FillEllipse(&backgroundBrush, r));
                    CheckGraphicsStatus(args.graphics.DrawEllipse(&breakpointPen, r));
                }
                else
                {
                    CheckGraphicsStatus(args.graphics.FillEllipse(&breakpointBrush, r));
                }
            }
            if (debugLocation.line == line)
            {
                DrawArrow(args.graphics, PointF(r.X, r.Y));
            }
            o.Y = o.Y + charHeight;
        }
        Control::OnPaint(args);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void DebugStrip::OnMouseDown(MouseEventArgs& args)
{
    try
    {
        Point location = args.location;
        view->TranslateMousePosInternal(location);
        int line = static_cast<int>(location.Y / view->CharHeight() + 1);
        if (args.buttons == MouseButtons::lbutton && args.clicks == 1)
        {
            Breakpoint* bp = GetBreakpoint(line);
            if (bp)
            {
                RemoveBreakpoint(bp);
                Invalidate();
            }
            else
            {
                Breakpoint* bp = new Breakpoint(line);
                AddBreakpoint(bp);
                Invalidate();
            }
        }
        else if (args.buttons == MouseButtons::rbutton && args.clicks == 1)
        {
            // todo
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void DebugStrip::OnBreakpointAdded(const std::string& filePath, Breakpoint* breakpoint)
{
    AddBreakpointEventArgs args;
    args.filePath = filePath;
    args.breakpoint = breakpoint;
    breakpointAdded.Fire(args);
    breakpoint->disabled = !breakpoint->info.success;
    Invalidate();
}

void DebugStrip::OnBreakpointRemoved(const std::string& breakpointId)
{
    RemoveBreakpointEventArgs args;
    args.breakpointId = breakpointId;
    breakpointRemoved.Fire(args);
    Invalidate();
}

void DebugStrip::DrawArrow(Graphics& graphics, const PointF& location)
{
    PointF bitmapLocation(location);
    bitmapLocation.X = bitmapLocation.X + arrowOffset.X;
    bitmapLocation.Y = bitmapLocation.Y + arrowOffset.Y;
    RectF r(bitmapLocation, SizeF(arrowBitmap->GetWidth(), arrowBitmap->GetHeight()));
    Gdiplus::ImageAttributes attributes;
    attributes.SetColorKey(transparentColor, transparentColor, Gdiplus::ColorAdjustType::ColorAdjustTypeDefault);
    Rect s(Point(), Size(arrowBitmap->GetWidth(), arrowBitmap->GetHeight()));
    CheckGraphicsStatus(graphics.DrawImage(arrowBitmap, r, s.X, s.Y, s.Width, s.Height, Unit::UnitPixel, &attributes));
}

void DebugStrip::ViewContentLocationChanged()
{
    topLineNumber = view->TopLine();
    origin = view->ContentLocation();
    Invalidate();
}

void DebugStrip::ViewLineInserted(LineEventArgs& args)
{
    bool changed = false;
    int line = args.lineIndex + 1 - 1;
    for (Breakpoint* breakpoint : breakpointList->Breakpoints())
    {
        if (breakpoint->line >= line)
        {
            ++breakpoint->line;
            changed = true;
        }
    }
    if (changed)
    {
        Invalidate();
    }
}

void DebugStrip::ViewLineDeleted(LineEventArgs& args)
{
    bool changed = false;
    std::vector<Breakpoint*> breakpointsToDelete;
    int line = args.lineIndex + 1;
    for (Breakpoint* breakpoint : breakpointList->Breakpoints())
    {
        if (breakpoint->line == line)
        {
            breakpointsToDelete.push_back(breakpoint);
        }
        else if (breakpoint->line > line)
        {
            --breakpoint->line;
            changed = true;
        }
    }
    for (Breakpoint* breakpointToDelete : breakpointsToDelete)
    {
        breakpointList->RemoveBreakpoint(breakpointToDelete);
        changed = true;
    }
    if (changed)
    {
        Invalidate();
    }
}

} } // namespace cmajor::view
