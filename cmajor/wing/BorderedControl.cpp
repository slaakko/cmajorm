// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/BorderedControl.hpp>
#include <wing/ContainerControl.hpp>

namespace cmajor { namespace wing {

int BorderWidth(BorderStyle borderStyle)
{
    switch (borderStyle)
    {
        case BorderStyle::single:
        {
            return 1;
        }
        case BorderStyle::threeD:
        {
            return 2;
        }
    }
    return 0;
}

Size BorderedSize(const Size& size, BorderStyle borderStyle)
{
    switch (borderStyle)
    {
        case BorderStyle::single:
        {
            return Size(size.Width + 2, size.Height + 2);
        }
        case BorderStyle::threeD:
        {
            return Size(size.Width + 4, size.Height + 4);
        }
    }
    return size;
}

Color DefaultNormalSingleBorderColor()
{
    return Color::Black;
}

Color DefaultFocusedSingleBorderColor()
{
    return Color::Blue;
}

Color Default3DOuterLeftTopEdgeColor()
{
    return Color(160, 160, 160);
}

Color Default3DOuterRightBottomEdgeColor()
{
    return Color::White;
}

Color Default3DInnerLeftTopEdgeColor()
{
    return Color(105, 105, 105);
}

Color Default3DInnerRightBottomEdgeColor()
{
    return Color(227, 227, 227);
}

BorderedControlCreateParams::BorderedControlCreateParams(Control* child_) : controlCreateParams(), child(child_), borderStyle(BorderStyle::single)
{
    controlCreateParams.WindowClassName("wing.BorderedControl");
    controlCreateParams.WindowStyle(DefaultChildWindowStyle());
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    controlCreateParams.BackgroundColor(child->BackgroundColor());
    normalSingleBorderColor = DefaultNormalSingleBorderColor();
    focusedSingleBorderColor = DefaultFocusedSingleBorderColor();
    threeDOuterLeftTopEdgeColor = Default3DOuterLeftTopEdgeColor();
    threeDOuterRightBottomEdgeColor = Default3DOuterRightBottomEdgeColor();
    threeDInnerLeftTopEdgeColor = Default3DInnerLeftTopEdgeColor();
    threeDInnerRightBottomEdgeColor = Default3DInnerRightBottomEdgeColor();
}

BorderedControlCreateParams& BorderedControlCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

BorderedControlCreateParams& BorderedControlCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

BorderedControlCreateParams& BorderedControlCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

BorderedControlCreateParams& BorderedControlCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

BorderedControlCreateParams& BorderedControlCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

BorderedControlCreateParams& BorderedControlCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

BorderedControlCreateParams& BorderedControlCreateParams::Location(const Point& location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

BorderedControlCreateParams& BorderedControlCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

BorderedControlCreateParams& BorderedControlCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

BorderedControlCreateParams& BorderedControlCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

BorderedControlCreateParams& BorderedControlCreateParams::SetBorderStyle(BorderStyle borderStyle_)
{
    borderStyle = borderStyle_;
    return *this;
}

BorderedControlCreateParams& BorderedControlCreateParams::NormalSingleBorderColor(const Color& color)
{
    normalSingleBorderColor = color;
    return *this;
}

BorderedControlCreateParams& BorderedControlCreateParams::FocusedSingleBorderColor(const Color& color)
{
    focusedSingleBorderColor = color;
    return *this;
}

BorderedControlCreateParams& BorderedControlCreateParams::ThreeDOuterLeftTopEdgeColor(const Color& color)
{
    threeDOuterLeftTopEdgeColor = color;
    return *this;
}

BorderedControlCreateParams& BorderedControlCreateParams::ThreeDOuterBottomRightEdgeColor(const Color& color)
{
    threeDOuterRightBottomEdgeColor = color;
    return *this;
}

BorderedControlCreateParams& BorderedControlCreateParams::ThreeDInnerLeftTopEdgeColor(const Color& color)
{
    threeDInnerLeftTopEdgeColor = color;
    return *this;
}

BorderedControlCreateParams& BorderedControlCreateParams::ThreeDInnerRightBottomEdgeColor(const Color& color)
{
    threeDInnerRightBottomEdgeColor = color;
    return *this;
}

BorderedControl::BorderedControl(BorderedControlCreateParams& createParams) : 
    Control(createParams.controlCreateParams), 
    flags(BorderedControlFlags::none),
    child(createParams.child), container(this), borderStyle(createParams.borderStyle),
    normalSingleBorderColor(createParams.normalSingleBorderColor),
    focusedSingleBorderColor(createParams.focusedSingleBorderColor),
    threeDOuterLeftTopEdgeColor(createParams.threeDOuterLeftTopEdgeColor),
    threeDOuterRightBottomEdgeColor(createParams.threeDOuterRightBottomEdgeColor),
    threeDInnerLeftTopEdgeColor(createParams.threeDInnerLeftTopEdgeColor),
    threeDInnerRightBottomEdgeColor(createParams.threeDInnerRightBottomEdgeColor)
{
    container.AddChild(child);
    SetChildPos();
}

Control* BorderedControl::GetFirstEnabledTabStopControl() const
{
    return child->GetFirstEnabledTabStopControl();
}

Control* BorderedControl::GetLastEnabledTabStopControl() const
{
    return child->GetLastEnabledTabStopControl();
}

ContainerControl* BorderedControl::GetContainerControl() const
{
    if (child->IsContainerControl())
    {
        return static_cast<ContainerControl*>(child);
    }
    return Control::GetContainerControl();
}

void BorderedControl::OnPaint(PaintEventArgs& args)
{
    switch (borderStyle)
    {
        case BorderStyle::single: 
        {
            DrawSingleBorder(args.graphics);
            break;
        }
        case BorderStyle::threeD:
        {
            Draw3DBorder(args.graphics);
            break;
        }
    }
    Control::OnPaint(args);
}

void BorderedControl::OnLocationChanged()
{
    Control::OnLocationChanged();
    SetChildPos();
}

void BorderedControl::OnSizeChanged()
{
    Control::OnSizeChanged();
    SetChildPos();
}

void BorderedControl::OnChildSizeChanged(ControlEventArgs& args)
{
    Control::OnChildSizeChanged(args);
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        parentControl->FireChildSizeChanged(args);
    }
}

void BorderedControl::OnChildContentChanged(ControlEventArgs& args)
{
    Control::OnChildContentChanged(args);
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        parentControl->FireChildContentChanged(args);
    }
}

void BorderedControl::OnChildContentLocationChanged(ControlEventArgs& args)
{
    Control::OnChildContentLocationChanged(args);
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        parentControl->FireChildContentLocationChanged(args);
    }
}

void BorderedControl::OnChildContentSizeChanged(ControlEventArgs& args)
{
    Control::OnChildContentSizeChanged(args);
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        parentControl->FireChildContentSizeChanged(args);
    }
}

void BorderedControl::OnChildGotFocus(ControlEventArgs& args)
{
    Control::OnChildGotFocus(args);
    flags = flags | BorderedControlFlags::childFocused;
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        parentControl->FireChildGotFocus(args);
    }
    Invalidate();
}

void BorderedControl::OnChildLostFocus(ControlEventArgs& args)
{
    Control::OnChildLostFocus(args);
    flags = flags & ~BorderedControlFlags::childFocused;
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        parentControl->FireChildLostFocus(args);
    }
    Invalidate();
}

void BorderedControl::SetChildPos()
{
    Point loc;
    Size size = GetSize();
    Rect childRect(loc, size);
    int borderWidth = BorderWidth(borderStyle);
    childRect.Inflate(-borderWidth, -borderWidth);
    child->SetLocation(Point(childRect.X, childRect.Y));
    child->SetSize(Size(childRect.Width, childRect.Height));
}

void BorderedControl::DrawSingleBorder(Graphics& graphics)
{
    Rect r(Point(), GetSize());
    r.Width = r.Width - 1;
    r.Height = r.Height - 1;
    if ((flags & BorderedControlFlags::childFocused) != BorderedControlFlags::none)
    {
        Pen pen(focusedSingleBorderColor);
        CheckGraphicsStatus(graphics.DrawRectangle(&pen, r));
    }
    else
    {
        Pen pen(normalSingleBorderColor);
        CheckGraphicsStatus(graphics.DrawRectangle(&pen, r));
    }
}

void BorderedControl::Draw3DBorder(Graphics& graphics)
{
    Rect r(Point(), GetSize());
    r.Width = r.Width - 1;
    r.Height = r.Height - 1;
    Pen outerLeftTopEdgePen(threeDOuterLeftTopEdgeColor);
    CheckGraphicsStatus(graphics.DrawLine(&outerLeftTopEdgePen, Point(r.X, r.Y), Point(r.X + r.Width - 1, r.Y)));
    CheckGraphicsStatus(graphics.DrawLine(&outerLeftTopEdgePen, Point(r.X, r.Y), Point(r.X, r.Y + r.Height - 1)));
    Pen outerRightBottomEdgePen(threeDOuterRightBottomEdgeColor);
    CheckGraphicsStatus(graphics.DrawLine(&outerRightBottomEdgePen, Point(r.X + r.Width, r.Y), Point(r.X + r.Width, r.Y + r.Height)));
    CheckGraphicsStatus(graphics.DrawLine(&outerRightBottomEdgePen, Point(r.X, r.Y + r.Height), Point(r.X + r.Width, r.Y + r.Height)));
    r.Inflate(-1, -1);
    Pen innerLeftTopEdgePen(threeDInnerLeftTopEdgeColor);
    CheckGraphicsStatus(graphics.DrawLine(&outerLeftTopEdgePen, Point(r.X, r.Y), Point(r.X + r.Width - 1, r.Y)));
    CheckGraphicsStatus(graphics.DrawLine(&outerLeftTopEdgePen, Point(r.X, r.Y), Point(r.X, r.Y + r.Height - 1)));
    Pen innerRightBottomEdgenPen(threeDInnerRightBottomEdgeColor);
    CheckGraphicsStatus(graphics.DrawLine(&outerRightBottomEdgePen, Point(r.X + r.Width, r.Y), Point(r.X + r.Width, r.Y + r.Height)));
    CheckGraphicsStatus(graphics.DrawLine(&outerRightBottomEdgePen, Point(r.X, r.Y + r.Height), Point(r.X + r.Width, r.Y + r.Height)));
}

} } // cmajor::wing
