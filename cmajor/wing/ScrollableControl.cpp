// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/wing/ScrollableControl.hpp>

namespace cmajor { namespace wing {

ScrollableControlCreateParams::ScrollableControlCreateParams(Control* child_) : controlCreateParams(), child(child_)
{
    controlCreateParams.WindowClassName("wing.ScrollableControl");
    controlCreateParams.WindowStyle(DefaultChildWindowStyle());
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    controlCreateParams.BackgroundColor(child->BackgroundColor());
}

ScrollableControlCreateParams& ScrollableControlCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

ScrollableControlCreateParams& ScrollableControlCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

ScrollableControlCreateParams& ScrollableControlCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

ScrollableControlCreateParams& ScrollableControlCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

ScrollableControlCreateParams& ScrollableControlCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

ScrollableControlCreateParams& ScrollableControlCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

ScrollableControlCreateParams& ScrollableControlCreateParams::Location(const Point& location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

ScrollableControlCreateParams& ScrollableControlCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

ScrollableControlCreateParams& ScrollableControlCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

ScrollableControlCreateParams& ScrollableControlCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

ScrollableControl::ScrollableControl(ScrollableControlCreateParams& createParams) : 
    Control(createParams.controlCreateParams), child(createParams.child), scrolledChild(nullptr), container(this), 
    hpos(0), hmin(0), hmax(0), hpage(0), htrackpos(0), vpos(0), vmin(0), vmax(0), vpage(0), vtrackpos(0),
    verticalScrollBarShown(false), horizontalScrollBarShown(false)
{
    container.AddChild(child);
}

void ScrollableControl::ScrollLineDown()
{
    GetScrollBarData(SB_VERT, vpage, vpos, vmin, vmax, vtrackpos);
    int prevVPos = vpos;
    if (vpos != vmax)
    {
        ++vpos;
        SetScrollPos(SB_VERT, vpos);
        GetScrollBarData(SB_VERT, vpage, vpos, vmin, vmax, vtrackpos);
        if (prevVPos != vpos)
        {
            ScrollChild(0, scrollUnits.vertical * (prevVPos - vpos));
        }
    }
}

void ScrollableControl::ScrollLineUp()
{
    GetScrollBarData(SB_VERT, vpage, vpos, vmin, vmax, vtrackpos);
    int prevVPos = vpos;
    if (vpos != vmax)
    {
        --vpos;
        SetScrollPos(SB_VERT, vpos);
        GetScrollBarData(SB_VERT, vpage, vpos, vmin, vmax, vtrackpos);
        if (prevVPos != vpos)
        {
            ScrollChild(0, scrollUnits.vertical * (prevVPos - vpos));
        }
    }
}

void ScrollableControl::TranslateChildGraphics(Graphics& graphics)
{
    int dx = -hpos * scrollUnits.horizontal;
    int dy = -vpos * scrollUnits.vertical;
    if (dx != 0 || dy != 0)
    {
        CheckGraphicsStatus(graphics.TranslateTransform(dx, dy));
    }
}

void ScrollableControl::TranslateMousePos(Point& location)
{
    int dx = hpos * scrollUnits.horizontal;
    int dy = vpos * scrollUnits.vertical;
    location.X = location.X + dx;
    location.Y = location.Y + dy;
}

void ScrollableControl::TranslateContentLocation(Point& contentLocation)
{
    int dx = hpos * scrollUnits.horizontal;
    int dy = vpos * scrollUnits.vertical;
    contentLocation.X = contentLocation.X - dx;
    contentLocation.Y = contentLocation.Y - dy;
}

void ScrollableControl::OnLocationChanged()
{
    Control::OnLocationChanged();
    child->SetLocation(Point());
    child->SetSize(GetSize());
}

void ScrollableControl::OnSizeChanged()
{
    Control::OnSizeChanged();
    child->SetLocation(Point());
    child->SetSize(GetSize());
}

void ScrollableControl::OnChildContentChanged(ControlEventArgs& args)
{
    Control::OnChildContentChanged(args);
    hpos = 0;
    vpos = 0;
}

void ScrollableControl::OnChildContentLocationChanged(ControlEventArgs& args)
{
    Control::OnChildContentLocationChanged(args);
    scrolledChild = args.control;
    scrollUnits = scrolledChild->GetScrollUnits();
    Point childContentLocation = scrolledChild->ContentLocation();
    vpos = childContentLocation.Y / scrollUnits.vertical;
    SetScrollPos(SB_VERT, vpos);
    hpos = childContentLocation.X / scrollUnits.horizontal;
    SetScrollPos(SB_HORZ, hpos);
}

void ScrollableControl::OnChildContentSizeChanged(ControlEventArgs& args)
{
    Control::OnChildContentChanged(args);
    scrolledChild = args.control;
    scrollUnits = scrolledChild->GetScrollUnits();
    Size clientSize = scrolledChild->GetSize();
    Size contentSize = scrolledChild->ContentSize();
    if (contentSize.Height > clientSize.Height)
    {
        vmin = 0;
        vmax = contentSize.Height / scrollUnits.vertical;
        vpage = clientSize.Height / scrollUnits.vertical;
        SetScrollBarData(SB_VERT, vpage, vpos, vmin, vmax);
        verticalScrollBarShown = true;
    }
    else
    {
        ShowScrollBar(Handle(), SB_VERT, false);        
        verticalScrollBarShown = false;
    }
    if (contentSize.Width > clientSize.Width)
    {
        hmin = 0;
        hmax = contentSize.Width / scrollUnits.horizontal;
        hpage = clientSize.Width / scrollUnits.horizontal;
        SetScrollBarData(SB_HORZ, hpage, hpos, hmin, hmax);
        horizontalScrollBarShown = true;
    }
    else
    {
        ShowScrollBar(Handle(), SB_HORZ, false);
        horizontalScrollBarShown = false;
    }
}

void ScrollableControl::OnChildGotFocus(ControlEventArgs& args)
{
    Control::OnChildGotFocus(args);
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        parentControl->FireChildGotFocus(args);
    }
}

void ScrollableControl::OnChildLostFocus(ControlEventArgs& args)
{
    Control::OnChildLostFocus(args);
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        parentControl->FireChildLostFocus(args);
    }
}

void ScrollableControl::OnHScroll(IntArgs& args)
{
    Control::OnHScroll(args);
    GetScrollBarData(SB_HORZ, hpage, hpos, hmin, hmax, htrackpos);
    int prevHPos = hpos;
    switch (args.value)
    {
        case SB_LINELEFT:
        {
            --hpos;
            break;
        }
        case SB_LINERIGHT:
        {
            ++hpos;
            break;
        }
        case SB_PAGELEFT:
        {
            hpos = hpos - hpage;
            break;
        }
        case SB_PAGERIGHT:
        {
            hpos = hpos + hpage;
            break;
        }
        case SB_THUMBTRACK:
        {
            hpos = htrackpos;
            break;
        }
    }
    SetScrollPos(SB_HORZ, hpos);
    GetScrollBarData(SB_HORZ, hpage, hpos, hmin, hmax, htrackpos);
    if (prevHPos != hpos)
    {
        ScrollChild(scrollUnits.horizontal * (prevHPos - hpos), 0);
    }
}

void ScrollableControl::OnVScroll(IntArgs& args)
{
    Control::OnVScroll(args);
    GetScrollBarData(SB_VERT, vpage, vpos, vmin, vmax, vtrackpos);
    int prevVPos = vpos;
    switch (args.value)
    {
        case SB_TOP:
        {
            vpos = vmin;
            break;
        }
        case SB_BOTTOM:
        {
            vpos = vmax;
            break;
        }
        case SB_LINEUP:
        {
            --vpos;
            break;
        }
        case SB_LINEDOWN:
        {
            ++vpos;
            break;
        }
        case SB_PAGEUP:
        {
            vpos = vpos - vpage;
            break;
        }
        case SB_PAGEDOWN:
        {
            vpos = vpos + vpage;
            break;
        }
        case SB_THUMBTRACK:
        {
            vpos = vtrackpos;
            break;
        }
    }
    SetScrollPos(SB_VERT, vpos);
    GetScrollBarData(SB_VERT, vpage, vpos, vmin, vmax, vtrackpos);
    if (prevVPos != vpos)
    {
        ScrollChild(0, scrollUnits.vertical * (prevVPos - vpos));
    }
}

void ScrollableControl::OnMouseWheel(MouseWheelEventArgs& args)
{
    Control::OnMouseWheel(args);
    if (!args.handled)
    {
        if (!verticalScrollBarShown) return;
        GetScrollBarData(SB_VERT, vpage, vpos, vmin, vmax, vtrackpos);
        int prevVPos = vpos;
        vpos = vpos - (args.value / (2.0 * scrollUnits.vertical));
        SetScrollPos(SB_VERT, vpos);
        GetScrollBarData(SB_VERT, vpage, vpos, vmin, vmax, vtrackpos);
        if (prevVPos != vpos)
        {
            ScrollChild(0, scrollUnits.vertical * (prevVPos - vpos));
        }
        args.handled = true;
    }
}

void ScrollableControl::SetScrollPos(int bar, int pos)
{
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_POS;
    si.nPos = pos;
    SetScrollInfo(Handle(), bar, &si, true);
}

void ScrollableControl::SetScrollBarData(int bar, int page, int pos, int min, int max)
{
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_POS | SIF_PAGE | SIF_RANGE;
    si.nPage = page;
    si.nPos = pos;
    si.nMin = min;
    si.nMax = max;
    SetScrollInfo(Handle(), bar, &si, true);
    ShowScrollBar(Handle(), bar, true);
}

void ScrollableControl::GetScrollBarData(int bar, int& page, int& pos, int& min, int& max, int& trackpos)
{
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
    if (!GetScrollInfo(Handle(), bar, &si))
    {
        throw WindowsException(GetLastError());
    }
    page = si.nPage;
    pos = si.nPos;
    min = si.nMin;
    max = si.nMax;
    trackpos = si.nTrackPos;
}

void ScrollableControl::ScrollChild(int xAmount, int yAmount)
{
    scrolledChild->SetContentLocationInternal(Point(scrollUnits.horizontal * hpos, scrollUnits.vertical * vpos));
    Rect updateRect = MakeUpdateRect(xAmount, yAmount);
    HRGN hrgnUpdate = CreateRectRgn(updateRect.X, updateRect.Y, updateRect.X + updateRect.Width, updateRect.Y + updateRect.Height);
    int result = ScrollWindowEx(scrolledChild->Handle(), xAmount, yAmount, nullptr, nullptr, hrgnUpdate, nullptr, SW_INVALIDATE);
    DeleteObject(hrgnUpdate);
    if (scrolledChild->IsDoubleBuffered())
    {
        scrolledChild->Invalidate();
    }
}

Rect ScrollableControl::MakeUpdateRect(int xAmount, int yAmount)
{
    Point loc(0, 0);
    Size size = scrolledChild->GetSize();
    if (xAmount < 0)
    {
        loc.X = size.Width + xAmount;
    }
    if (xAmount != 0)
    {
        size.Width = std::abs(xAmount);
    }
    if (yAmount < 0)
    {
        loc.Y = size.Height + yAmount;
    }
    if (yAmount != 0)
    {
        size.Height = std::abs(yAmount);
    }
    Rect updateRect(loc, size);
    updateRect.Inflate(scrollUnits.horizontal, scrollUnits.vertical);
    return updateRect;
}

} } // cmajor::wing

