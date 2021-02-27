// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/wing/ContainerControl.hpp>
#include <cmajor/wing/Menu.hpp>

namespace cmajor { namespace wing {

ContainerControl::ContainerControl(ControlCreateParams& createParams) : Control(createParams), children(this)
{
}

ContainerControl* ContainerControl::GetContainerControl() const
{
    return const_cast<ContainerControl*>(this);
}

void ContainerControl::PaintAll(PaintEventArgs& args, bool skipMenuBar)
{
    Control::PaintAll(args, skipMenuBar);
    Control* topControl = TopControl();
    Component* child = children.FirstChild();
    while (child)
    {
        if (child->IsControl())
        {
            Control* childControl = static_cast<Control*>(child);
            if (childControl != topControl)
            {
                bool skip = false;
                if (skipMenuBar)
                {
                    if (childControl->IsMenuBar())
                    {
                        skip = true;
                    }
                    else if (childControl->IsMenuBox())
                    {
                        MenuBox* menuBox = static_cast<MenuBox*>(childControl); 
                        if (!menuBox->PaintThisMenuBox())
                        {
                            skip = true;
                        }
                    }
                }
                if (!skip)
                {
                    GraphicsState state = args.graphics.Save();
                    Point loc = childControl->Location();
                    CheckGraphicsStatus(args.graphics.SetClip(Rect(loc, childControl->GetSize())));
                    CheckGraphicsStatus(args.graphics.TranslateTransform(loc.X, loc.Y));
                    childControl->PaintAll(args, skipMenuBar);
                    CheckGraphicsStatus(args.graphics.Restore(state));
                }
            }
        }
        child = child->NextSibling();
    }
    if (topControl)
    {
        GraphicsState state = args.graphics.Save();
        Point loc = topControl->Location();
        CheckGraphicsStatus(args.graphics.SetClip(Rect(loc, topControl->GetSize())));
        CheckGraphicsStatus(args.graphics.TranslateTransform(loc.X, loc.Y));
        topControl->PaintAll(args, skipMenuBar);
        CheckGraphicsStatus(args.graphics.Restore(state));
    }
}

void ContainerControl::AddChild(Control* child)
{
    children.AddChild(child);
    ControlEventArgs args(child);
    OnControlAdded(args);
}

std::unique_ptr<Control> ContainerControl::RemoveChild(Control* child)
{
    std::unique_ptr<Component> childComponent = children.RemoveChild(child);
    ControlEventArgs args(child);
    OnControlRemoved(args);
    return std::unique_ptr<Control>(static_cast<Control*>(childComponent.release()));
}

void ContainerControl::InsertChildBefore(Control* child, Control* before)
{
    children.InsertBefore(child, before);
    ControlEventArgs args(child); 
    OnControlAdded(args);
}

void ContainerControl::InsertChildAfter(Control* child, Control* after)
{
    children.InsertAfter(child, after);
    ControlEventArgs args(child);
    OnControlAdded(args);
}

void ContainerControl::DockChildren()
{
    Rect parentRect(Point(), GetSize());
    DockChildren(parentRect);
}

void ContainerControl::DockChildren(Rect& parentRect)
{
    std::vector<Control*> filledControls;
    std::vector<ContainerControl*> containers;
    Component* child = children.FirstChild();
    while (child)
    {
        if (child->IsControl())
        {
            Control* childControl = static_cast<Control*>(child);
            switch (childControl->GetDock())
            {
                case Dock::none: break;
                case Dock::fill: filledControls.push_back(childControl); break;
                default:
                {
                    childControl->DockWindow(parentRect);
                    break;
                }
            }
            if (childControl->IsContainerControl())
            {
                ContainerControl* containerChild = static_cast<ContainerControl*>(childControl);
                containers.push_back(containerChild);
            }
        }
        child = child->NextSibling();
    }
    for (Control* filledControl : filledControls)
    {
        filledControl->DockWindow(parentRect);
    }
    for (ContainerControl* container : containers)
    {
        container->DockChildren();
    }
}

bool ContainerControl::ProcessMessage(Message& msg)
{
    switch (msg.message)
    {
        case WM_SIZE:
        {
            Size newSize = LParamSize(msg);
            Size oldSize = GetSize();
            if (newSize != oldSize)
            {
                Rect parentRect(Point(), newSize);
                int dx = newSize.Width - oldSize.Width;
                int dy = newSize.Height - oldSize.Height;
                Component* child = children.FirstChild();
                while (child)
                {
                    if (child->IsControl())
                    {
                        Control* childControl = static_cast<Control*>(child);
                        if (childControl->GetDock() == Dock::none)
                        {
                            childControl->MoveWindow(dx, dy);
                        }
                    }
                    child = child->NextSibling();
                }
                SetSizeInternal(newSize);
                DockChildren();
                OnSizeChanged();
            }
            msg.result = 0;
            return true;
        }
        case WM_PAINT: case WM_MOVE: case WM_MOUSEMOVE: case WM_MOUSELEAVE: case WM_LBUTTONDOWN: case WM_LBUTTONUP: case WM_LBUTTONDBLCLK: case WM_RBUTTONDOWN: case WM_RBUTTONUP:
        case WM_CHAR: case WM_KEYDOWN: case WM_KEYUP: case WM_SYSCOMMAND: case WM_HSCROLL: case WM_VSCROLL: case WM_MOUSEWHEEL:
        case WM_SETFOCUS: case WM_KILLFOCUS: case WM_TIMER: case WM_COMMAND: case WM_SHOWWINDOW: case WM_CLIPBOARDUPDATE:
        {
            return Control::ProcessMessage(msg);
        }
    }
    return false;
}

void ContainerControl::OnControlAdded(ControlEventArgs& args)
{
    controlAdded.Fire(args);
}

void ContainerControl::OnControlRemoved(ControlEventArgs& args)
{
    controlRemoved.Fire(args);
}

} } // cmajor::wing
