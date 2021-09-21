// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/PaddedControl.hpp>
#include <wing/ContainerControl.hpp>

namespace cmajor { namespace wing {

Padding DefaultPadding()
{
    return Padding(defaultControlPadding, defaultControlPadding, defaultControlPadding, defaultControlPadding);
}

Size PaddedSize(const Size& size, const Padding& padding)
{
    return Size(size.Width + padding.Horizontal(), size.Height + padding.Vertical());
}

PaddedControlCreateParams::PaddedControlCreateParams(Control* child_) : 
    controlCreateParams(), 
    child(child_), 
    padding(defaultControlPadding, defaultControlPadding, defaultControlPadding, defaultControlPadding)
{
    controlCreateParams.WindowClassName("wing.PaddedControl");
    controlCreateParams.WindowStyle(DefaultChildWindowStyle());
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    controlCreateParams.BackgroundColor(child->BackgroundColor());
}

PaddedControlCreateParams& PaddedControlCreateParams::Defaults()
{
    return *this;
}

PaddedControlCreateParams& PaddedControlCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

PaddedControlCreateParams& PaddedControlCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

PaddedControlCreateParams& PaddedControlCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

PaddedControlCreateParams& PaddedControlCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

PaddedControlCreateParams& PaddedControlCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

PaddedControlCreateParams& PaddedControlCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

PaddedControlCreateParams& PaddedControlCreateParams::Location(const Point& location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

PaddedControlCreateParams& PaddedControlCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

PaddedControlCreateParams& PaddedControlCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

PaddedControlCreateParams& PaddedControlCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

PaddedControlCreateParams& PaddedControlCreateParams::SetPadding(const Padding& padding_)
{
    padding = padding_;
    return *this;
}

PaddedControl::PaddedControl(PaddedControlCreateParams& createParams) : Control(createParams.controlCreateParams), padding(createParams.padding), child(createParams.child), container(this)
{
    container.AddChild(child);
    SetChildPos();
}

Control* PaddedControl::GetFirstEnabledTabStopControl() const
{
    return child->GetFirstEnabledTabStopControl();
}

Control* PaddedControl::GetLastEnabledTabStopControl() const
{
    return child->GetLastEnabledTabStopControl();
}

void PaddedControl::OnPaint(PaintEventArgs& args)
{
    args.graphics.Clear(BackgroundColor());
    Control::OnPaint(args);
}

void PaddedControl::OnLocationChanged()
{
    Control::OnLocationChanged();
    SetChildPos();
}

void PaddedControl::OnSizeChanged()
{
    Control::OnSizeChanged();
    SetChildPos();
}

void PaddedControl::OnChildSizeChanged(ControlEventArgs& args)
{
    Control::OnChildSizeChanged(args);
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        parentControl->FireChildSizeChanged(args);
    }
}

void PaddedControl::OnChildContentChanged(ControlEventArgs& args)
{
    Control::OnChildContentChanged(args);
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        parentControl->FireChildContentChanged(args);
    }
}

void PaddedControl::OnChildContentLocationChanged(ControlEventArgs& args)
{
    Control::OnChildContentLocationChanged(args);
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        parentControl->FireChildContentLocationChanged(args);
    }
}

void PaddedControl::OnChildContentSizeChanged(ControlEventArgs& args)
{
    Control::OnChildContentSizeChanged(args);
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        parentControl->FireChildContentSizeChanged(args);
    }
}

void PaddedControl::OnChildGotFocus(ControlEventArgs& args)
{
    Control::OnChildGotFocus(args);
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        parentControl->FireChildGotFocus(args);
    }
}

void PaddedControl::OnChildLostFocus(ControlEventArgs& args)
{
    Control::OnChildLostFocus(args);
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        parentControl->FireChildLostFocus(args);
    }
}

void PaddedControl::SetChildPos()
{
    Point loc;
    Size size = GetSize();
    Rect childRect(loc, size);
    childRect.X = childRect.X + padding.left;
    childRect.Y = childRect.Y + padding.top;
    childRect.Width = childRect.Width - padding.Horizontal();
    childRect.Height = childRect.Height - padding.Vertical();
    child->SetLocation(Point(childRect.X, childRect.Y));
    child->SetSize(Size(childRect.Width, childRect.Height));
}

} } // cmajor::wing
