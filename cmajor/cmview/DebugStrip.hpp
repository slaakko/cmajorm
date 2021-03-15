// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_VIEW_DEBUG_STRIP_INCLUDED
#define CMAJOR_VIEW_DEBUG_STRIP_INCLUDED
#include <cmajor/cmview/ViewApi.hpp>
#include <cmajor/cmsvc/Breakpoint.hpp>
#include <cmajor/wing/SourceCodeView.hpp>

namespace cmajor { namespace view {

using SourceSpan = cmajor::wing::SourceSpan;
using namespace cmajor::service;
using namespace cmajor::wing;

struct CMVIEW_API AddBreakpointEventArgs
{
    AddBreakpointEventArgs();
    std::string filePath;
    Breakpoint* breakpoint;
};

struct CMVIEW_API RemoveBreakpointEventArgs
{
    std::string breakpointId;
};

using AddBreakpointEvent = EventWithArgs<AddBreakpointEventArgs>;
using RemoveBreakpointEvent = EventWithArgs<RemoveBreakpointEventArgs>;

CMVIEW_API Color DefaultDebugStripBackgroundColor();
CMVIEW_API Color DefaultBreakpointColor();
CMVIEW_API Color DefaultTransparentColor();
CMVIEW_API Point DefaultArrowOffset();
CMVIEW_API Point DefaultBreakpointOffset();
CMVIEW_API Size DefaultBreakpointInflate();
CMVIEW_API int DefaultDebugStripWidth();
CMVIEW_API Dock DefaultDebugStripDock();

struct CMVIEW_API DebugStripCreateParams
{
    DebugStripCreateParams(BreakpointList* breakpointList_);
    DebugStripCreateParams& Defaults();
    DebugStripCreateParams& WindowClassName(const std::string& windowClassName_);
    DebugStripCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    DebugStripCreateParams& WindowStyle(int windowStyle_);
    DebugStripCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    DebugStripCreateParams& BackgroundColor(const Color& backgroundColor_);
    DebugStripCreateParams& Text(const std::string& text_);
    DebugStripCreateParams& Location(Point location_);
    DebugStripCreateParams& SetSize(Size size_);
    DebugStripCreateParams& SetAnchors(Anchors anchors_);
    DebugStripCreateParams& SetDock(Dock dock_);
    DebugStripCreateParams& SetTextView(TextView* textView_);
    DebugStripCreateParams& BreakpointColor(const Color& color);
    DebugStripCreateParams& TransparentColor(const Color& color);
    DebugStripCreateParams& ArrowOffset(const Point& offset);
    DebugStripCreateParams& BreakpointOffset(const Point& offset);
    DebugStripCreateParams& BreakpointInflate(const Size& inflate);
    DebugStripCreateParams& Width(int width_);
    ControlCreateParams controlCreateParams;
    TextView* view;
    BreakpointList* breakpointList;
    Color breakpointColor;
    Color transparentColor;
    Point arrowOffset;
    Point breakpointOffset;
    Size breakpointInflate;
    int width;
};

class CMVIEW_API DebugStrip : public Control
{
public:
    DebugStrip(DebugStripCreateParams& createParams);
    Breakpoint* GetBreakpoint(int line) const;
    void AddBreakpoint(Breakpoint* breakpoint);
    void RemoveBreakpoint(Breakpoint* breakpoint);
    const SourceSpan& DebugLocation() const { return debugLocation; }
    void SetDebugLocation(const SourceSpan& debugLocation_);
    void ResetDebugLocation();
    AddBreakpointEvent& BreakpointAdded() { return breakpointAdded; }
    RemoveBreakpointEvent& BreakpointRemoved() { return breakpointRemoved; }
protected:
    void OnPaint(PaintEventArgs& args) override;
    void OnMouseDown(MouseEventArgs& args) override;
    virtual void OnBreakpointAdded(const std::string& filePath, Breakpoint* breakpoint);
    virtual void OnBreakpointRemoved(const std::string& breakpointId);
private:
    void DrawArrow(Graphics& graphics, const PointF& location);
    void ViewContentLocationChanged();
    void ViewLineInserted(LineEventArgs& args);
    void ViewLineDeleted(LineEventArgs& args);
    TextView* view;
    BreakpointList* breakpointList;
    SourceSpan debugLocation;
    int topLineNumber;
    Point origin;
    Color breakpointColor;
    Color transparentColor;
    Point arrowOffset;
    Point breakpointOffset;
    Size breakpointInflate;
    Bitmap* arrowBitmap;
    SolidBrush backgroundBrush;
    SolidBrush breakpointBrush;
    Pen breakpointPen;
    AddBreakpointEvent breakpointAdded;
    RemoveBreakpointEvent breakpointRemoved;
};

} } // namespace cmajor::view

#endif // CMAJOR_VIEW_DEBUG_STRIP_INCLUDED
