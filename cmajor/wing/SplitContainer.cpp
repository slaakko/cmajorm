// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/SplitContainer.hpp>
#include <wing/Application.hpp>
#include <wing/Panel.hpp>
#include <wing/Theme.hpp>

namespace cmajor { namespace wing {

Dock SplitDock(SplitterOrientation orientation)
{
    switch (orientation)
    {
        case SplitterOrientation::horizontal:
        {
            return Dock::left;
        }
        case SplitterOrientation::vertical:
        {
            return Dock::top;
        }
    }
    return Dock::none;
}

Size SplitSize(SplitterOrientation orientation, int width)
{
    switch (orientation)
    {
        case SplitterOrientation::horizontal:
        {
            return Size(width, 0);
        }
        case SplitterOrientation::vertical:
        {
            return Size(0, width);
        }
    }
    return Size();
}

Color DefaultSplitterEdgeColor()
{
    return Color(204, 206, 219);
}

SplitterCreateParams::SplitterCreateParams(SplitterOrientation orientation_) : controlCreateParams(), orientation(orientation_)
{
    controlCreateParams.WindowClassName("wing.Splitter");
    controlCreateParams.BackgroundColor(DefaultControlBackgroundColor());
    controlCreateParams.SetSize(SplitSize(orientation, defaultSplitterWidth));
    edgeColor = DefaultSplitterEdgeColor();
}

SplitterCreateParams& SplitterCreateParams::Defaults()
{
    return *this;
}

SplitterCreateParams& SplitterCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

SplitterCreateParams& SplitterCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

SplitterCreateParams& SplitterCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

SplitterCreateParams& SplitterCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

SplitterCreateParams& SplitterCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

SplitterCreateParams& SplitterCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

SplitterCreateParams& SplitterCreateParams::Location(const Point& location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

SplitterCreateParams& SplitterCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

SplitterCreateParams& SplitterCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

SplitterCreateParams& SplitterCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

SplitterCreateParams& SplitterCreateParams::EdgeColor(const Color& edgeColor_)
{
    edgeColor = edgeColor_;
    return *this;
}

Splitter::Splitter(SplitterCreateParams& createParams) : 
    Control(createParams.controlCreateParams), 
    horizontalSplitterCursor(&Application::GetResourceManager().GetCursor(GetCursorName("horizontal.splitter.cursor"))),
    verticalSplitterCursor(&Application::GetResourceManager().GetCursor(GetCursorName("vertical.splitter.cursor"))),
    flags(SplitterFlags::none), 
    orientation(createParams.orientation), edgeColor(createParams.edgeColor), edgePen(new Pen(edgeColor)), x(0), y(0), container(nullptr)
{
}

void Splitter::SetEdgeColor(const Color& edgeColor_)
{
    if (edgeColor != edgeColor_)
    {
        edgeColor = edgeColor_;
        edgePen.reset(new Pen(edgeColor));
        Invalidate();
    }
}

void Splitter::UpdateColors()
{
    Control::UpdateColors();
    SetBackgroundColor(GetColor("splitter.background"));
    SetEdgeColor(GetColor("splitter.edge"));
    horizontalSplitterCursor = &Application::GetResourceManager().GetCursor(GetCursorName("horizontal.splitter.cursor"));
    verticalSplitterCursor = &Application::GetResourceManager().GetCursor(GetCursorName("vertical.splitter.cursor"));
}

void Splitter::SetCursor()
{
    switch (orientation)
    {
        case SplitterOrientation::horizontal:
        {
            cmajor::wing::SetCursor(*horizontalSplitterCursor);
            break;
        }
        case SplitterOrientation::vertical:
        {
            cmajor::wing::SetCursor(*verticalSplitterCursor);
            break;
        }
    }
}

void Splitter::OnPaint(PaintEventArgs& args)
{
    args.graphics.Clear(BackgroundColor());
    Size size = GetSize();
    switch (orientation)
    {
        case SplitterOrientation::horizontal:
        {
            CheckGraphicsStatus(args.graphics.DrawLine(edgePen.get(), Point(0, 0), Point(0, size.Height - 1)));
            CheckGraphicsStatus(args.graphics.DrawLine(edgePen.get(), Point(size.Width - 1, 0), Point(size.Width - 1, size.Height - 1)));
            break;
        }
        case SplitterOrientation::vertical:
        {
            CheckGraphicsStatus(args.graphics.DrawLine(edgePen.get(), Point(0, 0), Point(size.Width - 1, 0)));
            CheckGraphicsStatus(args.graphics.DrawLine(edgePen.get(), Point(0, size.Height - 1), Point(size.Width - 1, size.Height - 1)));
            break;
        }
    }
    Control::OnPaint(args);
}

void Splitter::OnMouseDown(MouseEventArgs& args)
{
    Control::OnMouseDown(args);
    if ((args.buttons & MouseButtons::lbutton) != MouseButtons::none)
    {
        flags = flags | SplitterFlags::moveSplitter;
        switch (orientation)
        {
            case SplitterOrientation::horizontal:
            {
                x = args.location.X;
                break;
            }
            case SplitterOrientation::vertical:
            {
                y = args.location.Y;
                break;
            }
        }
        SetCapture(Handle());
    }
}

void Splitter::OnMouseUp(MouseEventArgs& args)
{
    try
    {
        Control::OnMouseUp(args);
        if ((flags & SplitterFlags::moveSplitter) != SplitterFlags::none)
        {
            flags = flags & ~SplitterFlags::moveSplitter;
            bool succeded = ReleaseCapture();
            if (!succeded)
            {
                throw WindowsException(GetLastError());
            }
            switch (orientation)
            {
                case SplitterOrientation::horizontal:
                {
                    int dx = args.location.X - x;
                    if (container)
                    {
                        container->SetSplitterDistance(container->SplitterDistance() + dx);
                    }
                    break;
                }
                case SplitterOrientation::vertical:
                {
                    int dy = args.location.Y - y;
                    if (container)
                    {
                        container->SetSplitterDistance(container->SplitterDistance() + dy);
                    }
                    break;
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void Splitter::OnMouseMove(MouseEventArgs& args)
{
    Control::OnMouseMove(args);
    if ((flags & SplitterFlags::moveSplitter) != SplitterFlags::none)
    {
        switch (orientation)
        {
            case SplitterOrientation::horizontal:
            {
                int dx = args.location.X - x;
                if (container)
                {
                    container->SetSplitterDistance(container->SplitterDistance() + dx);
                }
                break;
            }
            case SplitterOrientation::vertical:
            {
                int dy = args.location.Y - y;
                if (container)
                {
                    container->SetSplitterDistance(container->SplitterDistance() + dy);
                }
                break;
            }
        }
    }
}

SplitContainerCreateParams& MakeSplitContainerCreateParams(SplitContainerCreateParams& createParams)
{
    createParams.controlCreateParams.WindowClassName("wing.SplitContainer");
    createParams.SplitterDock(SplitDock(createParams.splitterCreateParams.orientation));
    if (!createParams.splitter)
    {
        createParams.splitter.reset(new Splitter(createParams.splitterCreateParams));
    }
    createParams.pane1CreateParams.SetDock(SplitDock(createParams.splitter->Orientation()));
    createParams.pane1CreateParams.SetSize(SplitSize(createParams.splitter->Orientation(), createParams.splitterDistance));
    if (!createParams.pane1)
    {
        createParams.pane1.reset(new Panel(createParams.pane1CreateParams));
    }
    createParams.pane2CreateParams.Location(createParams.controlCreateParams.location);
    createParams.pane2CreateParams.SetDock(Dock::fill);
    if (!createParams.pane2)
    {
        createParams.pane2.reset(new Panel(createParams.pane2CreateParams));
    }
    return createParams;
}

SplitContainerCreateParams::SplitContainerCreateParams(SplitterOrientation orientation) : 
    controlCreateParams(), splitterCreateParams(orientation), pane1CreateParams(), pane2CreateParams(), splitterDistance(defaultSplitterDistance)
{
}

SplitContainerCreateParams& SplitContainerCreateParams::SplitterDistance(int splitterDistance_)
{
    splitterDistance = splitterDistance_;
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::Defaults()
{
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::Location(const Point& location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::SplitterWindowClassName(const std::string& windowClassName_)
{
    splitterCreateParams.WindowClassName(windowClassName_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::SplitterWindowClassStyle(uint32_t windowClassStyle_)
{
    splitterCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::SplitterWindowStyle(int windowStyle_)
{
    splitterCreateParams.WindowStyle(windowStyle_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::SplitterWindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    splitterCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::SplitterBackgroundColor(const Color& backgroundColor_)
{
    splitterCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::SplitterText(const std::string& text_)
{
    splitterCreateParams.Text(text_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::SplitterLocation(const Point& location_)
{
    splitterCreateParams.Location(location_);
    return* this;
}

SplitContainerCreateParams& SplitContainerCreateParams::SplitterSize(const Size& size_)
{
    splitterCreateParams.SetSize(size_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::SplitterAnchors(Anchors anchors_)
{
    splitterCreateParams.SetAnchors(anchors_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::SplitterDock(Dock dock_)
{
    splitterCreateParams.SetDock(dock_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::SplitterEdgeColor(const Color& edgeColor_)
{
    splitterCreateParams.EdgeColor(edgeColor_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::SetSplitter(Splitter* splitter_)
{
    splitter.reset(splitter_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::Pane1WindowClassName(const std::string& windowClassName_)
{
    pane1CreateParams.WindowClassName(windowClassName_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::Pane1WindowClassStyle(uint32_t windowClassStyle_)
{
    pane1CreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::Pane1WindowStyle(int windowStyle_)
{
    pane1CreateParams.WindowStyle(windowStyle_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::Pane1WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    pane1CreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::Pane1BackgroundColor(const Color& backgroundColor_)
{
    pane1CreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::Pane1Text(const std::string& text_)
{
    pane1CreateParams.Text(text_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::Pane1Location(const Point& location_)
{
    pane1CreateParams.Location(location_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::Pane1Size(const Size& size_)
{
    pane1CreateParams.SetSize(size_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::Pane1Anchors(Anchors anchors_)
{
    pane1CreateParams.SetAnchors(anchors_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::Pane1Dock(Dock dock_)
{
    pane1CreateParams.SetDock(dock_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::SetPane1(Control* pane1_)
{
    pane1.reset(pane1_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::Pane2WindowClassName(const std::string& windowClassName_)
{
    pane2CreateParams.WindowClassName(windowClassName_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::Pane2WindowClassStyle(uint32_t windowClassStyle_)
{
    pane2CreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::Pane2WindowStyle(int windowStyle_)
{
    pane2CreateParams.WindowStyle(windowStyle_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::Pane2WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    pane2CreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::Pane2BackgroundColor(const Color& backgroundColor_)
{
    pane2CreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::Pane2Text(const std::string& text_)
{
    pane2CreateParams.Text(text_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::Pane2Location(const Point& location_)
{
    pane2CreateParams.Location(location_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::Pane2Size(const Size& size_)
{
    pane2CreateParams.SetSize(size_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::Pane2Anchors(Anchors anchors_)
{
    pane2CreateParams.SetAnchors(anchors_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::Pane2Dock(Dock dock_)
{
    pane2CreateParams.SetDock(dock_);
    return *this;
}

SplitContainerCreateParams& SplitContainerCreateParams::SetPane2(Control* pane2_)
{
    pane2.reset(pane2_);
    return *this;
}

SplitContainer::SplitContainer(SplitContainerCreateParams& createParams) : ContainerControl(MakeSplitContainerCreateParams(createParams).controlCreateParams), 
    splitterDistance(createParams.splitterDistance)
{
    pane1 = createParams.pane1.get();
    AddChild(createParams.pane1.release());
    splitter = createParams.splitter.get();
    splitter->SetContainer(this);
    AddChild(createParams.splitter.release());
    pane2 = createParams.pane2.get();
    AddChild(createParams.pane2.release());
}

ContainerControl* SplitContainer::Pane1Container() const
{
    return pane1->GetContainerControl();
}

ContainerControl* SplitContainer::Pane2Container() const
{
    return pane2->GetContainerControl();
}

void SplitContainer::SetSplitterDistance(int splitterDistance_)
{
    Size size = GetSize();
    int maxDistance = 1;
    if (splitterDistance_ < 1) return;
    int prevSplitterDistance = splitterDistance;
    splitterDistance = splitterDistance_;
    switch (splitter->Orientation())
    {
        case SplitterOrientation::horizontal:
        {
            Size pane1Size = pane1->GetSize();
            pane1->SetSize(Size(splitterDistance, pane1Size.Height));
            break;
        }
        case SplitterOrientation::vertical:
        {
            Size pane1Size = pane1->GetSize();
            pane1->SetSize(Size(pane1Size.Width, splitterDistance));
            break;
        }
    }
    DockChildren();
    Invalidate();
    if (prevSplitterDistance != splitterDistance)
    {
        OnSplitterDistanceChanged();
    }
}

void SplitContainer::OnPaint(PaintEventArgs& args)
{
    if (splitterDistance == defaultSplitterDistance)
    {
        Size size = GetSize();
        int s = 0;
        switch (splitter->Orientation())
        {
            case SplitterOrientation::horizontal:
            {
                s = size.Width;
                break;
            }
            case SplitterOrientation::vertical:
            {
                s = size.Height;
                break;
            }
        }
        int d = static_cast<int>((2.0 / 3.0) * s);
        SetSplitterDistance(d);
    }
    ContainerControl::OnPaint(args);
}

void SplitContainer::OnSplitterDistanceChanged()
{
    splitterDistanceChanged.Fire();
}

} } // cmajor::wing
