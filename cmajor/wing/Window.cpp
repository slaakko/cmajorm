// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/Window.hpp>
#include <wing/Application.hpp>
#include <wing/Button.hpp>
#include <wing/Menu.hpp>
#include <wing/Metrics.hpp>
#include <wing/Icon.hpp>
#include <wing/TextBox.hpp>
#include <wing/Theme.hpp>
#include <soulng/util/Unicode.hpp>
#include <windowsx.h>

#undef max
#undef min

namespace cmajor { namespace wing {

using namespace soulng::unicode;

std::string DefaultWindowFontFamilyName()
{
    return "Microsoft Sans Serif";
}

std::string DefaultWindowCaptionFontFamilyName()
{
    return "Segoe UI";
}

float DefaultWindowFontSize()
{
    return 8.25f;
}

FontStyle DefaultWindowFontStyle()
{
    return FontStyle::FontStyleRegular;
}

float DefaultWindowCaptionFontPercent()
{
    return 0.4f;
}

WindowCreateParams::WindowCreateParams()
{
    controlCreateParams.WindowClassName("wing.Window").WindowStyle(WS_OVERLAPPEDWINDOW).
        WindowClassBackgroundColor(DefaultWindowClassWindowBackgroundColor()).
        BackgroundColor(DefaultWindowBackgroundColor()).Location(DefaultLocation()).SetSize(DefaultSize());
    FontFamilyName(DefaultWindowFontFamilyName()).FontSize(DefaultWindowFontSize()).SetFontStyle(DefaultWindowFontStyle()).
        CaptionFontFamilyName(DefaultWindowCaptionFontFamilyName()).CaptionFontPercent(DefaultWindowCaptionFontPercent());
}

WindowCreateParams& WindowCreateParams::Defaults()
{
    return *this;
}

WindowCreateParams& WindowCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

WindowCreateParams& WindowCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

WindowCreateParams& WindowCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

WindowCreateParams& WindowCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

WindowCreateParams& WindowCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

WindowCreateParams& WindowCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

WindowCreateParams& WindowCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

WindowCreateParams& WindowCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

WindowCreateParams& WindowCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

WindowCreateParams& WindowCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

WindowCreateParams& WindowCreateParams::FontFamilyName(const std::string& fontFamilyName_)
{
    fontFamilyName = fontFamilyName_;
    return *this;
}

WindowCreateParams& WindowCreateParams::FontSize(float fontSize_)
{
    fontSize = fontSize_;
    return *this;
}

WindowCreateParams& WindowCreateParams::SetFontStyle(FontStyle fontStyle_)
{
    fontStyle = fontStyle_;
    return *this;
}

WindowCreateParams& WindowCreateParams::CaptionFontFamilyName(const std::string& captionFontFamilyName_)
{
    captionFontFamilyName = captionFontFamilyName_;
    return *this;
}

WindowCreateParams& WindowCreateParams::CaptionFontPercent(float captionFontPercent_)
{
    captionFontPercent = captionFontPercent_;
    return *this;
}

Window::Window(WindowCreateParams& createParams) : 
    ContainerControl(createParams.controlCreateParams),
    fontFamilyName(createParams.fontFamilyName), 
    fontSize(createParams.fontSize), 
    fontStyle(createParams.fontStyle),
    captionFontFamilyName(createParams.captionFontFamilyName),
    captionFontPercent(createParams.captionFontPercent),
    captionHeight(GetSystemMetrics(SM_CYCAPTION)),
    borderWidth(4),
    buttonWidth(GetSystemMetrics(SM_CXSIZE)),
    buttonHeight(31),
    windowActive(true),
    mainWindow(false),
    showingDialog(false),
    windowState(WindowState::normal),
    defaultButton(nullptr),
    cancelButton(nullptr),
    focusedControl(nullptr),
    menuBar(nullptr),
    contextMenu(nullptr),
    dialogResult(DialogResult::none)
{
    if (Handle())
    {
        if (!ScreenMetrics::Calculated())
        {
            ScreenMetrics::SetCalculated();
            Graphics graphics(Handle());
            ScreenMetrics::Calculate(graphics);
        }
    }
    KeyPreviewMethod defaultKeyPreview;
    defaultKeyPreview.SetHandlerFunction(this, &Window::DefaultKeyPreview);
    SetKeyPreviewMethod(defaultKeyPreview);
    KeyPreviewMethod defaultDialogKeyPreview;
    defaultDialogKeyPreview.SetHandlerFunction(this, &Window::DefaultKeyPreview);
    SetDialogKeyPreviewMethod(defaultDialogKeyPreview);
    if (!fontFamilyName.empty())
    {
        std::u16string familyName = ToUtf16(fontFamilyName);
        const WCHAR* familyStr = (const WCHAR*)familyName.c_str();
        SetFont(Font(FontFamily(familyStr), fontSize, fontStyle, Unit::UnitPoint));
    }
}

void Window::SetContextMenu(ContextMenu* contextMenu_)
{
    RemoveContextMenu();
    contextMenu = contextMenu_;
    AddChild(contextMenu);
}

void Window::ShowContextMenu(ContextMenu* contextMenu_, Point& ptInScreenCoords)
{
    SetContextMenu(contextMenu_);
    Point windowLoc = Location();
    Size windowSize = GetSize();
    Point windowEdgeLoc(windowLoc.X + windowSize.Width, windowLoc.Y + windowSize.Height);
    Point screenWindowEdgeLoc = ClientToScreen(windowEdgeLoc);
    contextMenu->CalculateSize();
    Size contextMenuSize = contextMenu->GetSize();
    if (ptInScreenCoords.X + contextMenuSize.Width >= screenWindowEdgeLoc.X)
    {
        ptInScreenCoords.X = ptInScreenCoords.X - contextMenuSize.Width;
    }
    if (ptInScreenCoords.Y + contextMenuSize.Height >= screenWindowEdgeLoc.Y)
    {
        ptInScreenCoords.Y = ptInScreenCoords.Y - contextMenuSize.Height;
    }
    Point loc = ScreenToClient(ptInScreenCoords);
    contextMenu->SetLocation(loc);
    contextMenu->BringToFront();
    contextMenu->Show();
    contextMenu->Invalidate();
    contextMenu->Update();
}

void Window::RemoveContextMenu()
{
    if (contextMenu)
    {
        HideContextMenu();
        RemoveChild(contextMenu);
        contextMenu = nullptr;
    }
}

void Window::HideContextMenu()
{
    if (contextMenu)
    {
        contextMenu->Hide();
    }
}

void Window::SetIcon(const Icon& icon)
{
    ::SetClassLongPtr(Handle(), GCLP_HICON, reinterpret_cast<int64_t>(icon.Handle()));
}

void Window::SetSmallIcon(const Icon& icon)
{
    ::SetClassLongPtr(Handle(), GCLP_HICONSM, reinterpret_cast<int64_t>(icon.Handle()));
}

KeyPreviewMethod Window::GetKeyPreviewMethod() const
{
    return keyPreviewMethod;
}

void Window::SetKeyPreviewMethod(KeyPreviewMethod& keyPreviewMethod_)
{
    keyPreviewMethod = keyPreviewMethod_;
}

KeyPreviewMethod Window::GetDialogKeyPreviewMethod() const
{
    return dialogKeyPreviewMethod;
}

void Window::SetDialogKeyPreviewMethod(KeyPreviewMethod& dialogKeyPreviewMethod_)
{
    dialogKeyPreviewMethod = dialogKeyPreviewMethod_;
}

void Window::DefaultKeyPreview(Keys key, bool& handled)
{
    switch (key)
    {
        case Keys::enter:
        {
            if (ShowingDialog())
            {
                if (focusedControl)
                {
                    if (focusedControl->IsTextBox())
                    {
                        TextBox* textBox = static_cast<TextBox*>(focusedControl);
                        if (textBox->IsMultiline()) 
                        {
                            break;
                        }
                    }
                }
                if (focusedControl)
                {
                    if (focusedControl->IsButton())
                    {
                        Button* button = static_cast<Button*>(focusedControl);
                        button->DoClick();
                        handled = true;
                    }
                }
                if (!handled)
                {
                    if (defaultButton)
                    {
                        if (defaultButton->IsEnabled())
                        {
                            defaultButton->DoClick();
                            handled = true;
                        }
                    }
                }
            }
            break;
        }
        case Keys::escape:
        {
            if (ShowingDialog())
            {
                if (cancelButton)
                {
                    cancelButton->DoClick();
                    handled = true;
                }
            }
            break;
        }
        case Keys::tab:
        {
            FocusNext();
            handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::tab:
        {
            FocusPrev();
            handled = true;
            break;
        }
    }
}

void Window::SetWindowState(WindowState newWindowState)
{
    if (windowState != newWindowState)
    {
        windowState = newWindowState;
        OnWindowStateChanged();
    }
}

void Window::ShowWindow(int showCommand)
{
    ::ShowWindow(Handle(), showCommand);
}

void Window::UpdateColors()
{
    ContainerControl::UpdateColors();
    captionBrush.reset();
    disabledCaptionBrush.reset();
    sysButtonPen.reset();
    sysButtonSelectedPen.reset();
    sysButtonHoverBackgroundBrush.reset();
    frame.reset();
    RedrawWindow(Handle(), nullptr, nullptr, RDW_INVALIDATE | RDW_FRAME);
}

void Window::HandleNCPaint(NCPaintEventArgs& args)
{
#ifndef DCX_USESTYLE
#define DCX_USESTYLE 0x00010000
#endif
    try
    {
        HDC dc = GetDCEx(args.hwnd, 0, DCX_WINDOW | DCX_USESTYLE);
        if (dc)
        {
            RECT clientRect;
            if (!GetClientRect(args.hwnd, &clientRect))
            {
                throw WindowsException(GetLastError());
            }
            RECT windowRect;
            if (!GetWindowRect(args.hwnd, &windowRect))
            {
                throw WindowsException(GetLastError());
            }
            POINT ptTopLeft;
            ptTopLeft.x = windowRect.left;
            ptTopLeft.y = windowRect.top;
            MapWindowPoints(0, args.hwnd, (LPPOINT)&windowRect, (sizeof(RECT) / sizeof(Point)));
            if (!OffsetRect(&clientRect, -windowRect.left, -windowRect.top))
            {
                throw std::runtime_error("OffsetRect");
            }
            if (!OffsetRect(&windowRect, -windowRect.left, -windowRect.top))
            {
                throw std::runtime_error("OffsetRect");
            }
            HRGN tempRgn = nullptr;
            if (args.hrgn == (HRGN)NULLREGION || args.hrgn == (HRGN)ERROR)
            {
                int result = ExcludeClipRect(dc, clientRect.left, clientRect.top, clientRect.right, clientRect.bottom);
            }
            else
            {
                tempRgn = CreateRectRgn(clientRect.left + ptTopLeft.x, clientRect.top + ptTopLeft.y, clientRect.right + ptTopLeft.x, clientRect.bottom + ptTopLeft.y);
                if (tempRgn)
                {
                    if (CombineRgn(tempRgn, args.hrgn, tempRgn, RGN_DIFF) == NULLREGION)
                    {
                        return;
                    }
                    else
                    {
                        int res0 = OffsetRgn(tempRgn, -ptTopLeft.x, -ptTopLeft.y);
                        int res1 = ExtSelectClipRgn(dc, tempRgn, RGN_AND);
                    }
                }
            }
            Graphics graphics(dc);
            PaintWindowRect(graphics);
            ReleaseDC(args.hwnd, dc);
            if (tempRgn)
            {
                DeleteObject(tempRgn);
            }
            args.handled = true;
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(nullptr, "HandleNCPaint failed: " + std::string(ex.what()));
    }
    //    RECT windowRect;
    //    if (GetWindowRect(args.hwnd, &windowRect))
    //    {
    //        int captionHeight = GetSystemMetrics(SM_CYCAPTION);
    //        int borderWidth = GetSystemMetrics(SM_CXBORDER);
    //        int borderHeight = GetSystemMetrics(SM_CYBORDER);
    //        int buttonWidth = GetSystemMetrics(SM_CXSIZE);
    //        int buttonHeight = GetSystemMetrics(SM_CYSIZE);
    //        int smallIconWidth = GetSystemMetrics(SM_CXSMICON);
    //        int smallIconHeight = GetSystemMetrics(SM_CYSMICON);
    //        int smallCaptionButtonWidth = GetSystemMetrics(SM_CXSMSIZE);
    //        int smallCaptionButtonHeight = GetSystemMetrics(SM_CYSMSIZE);

    //        HDC dc = GetDCEx(args.hwnd, args.hrgn, DCX_WINDOW | DCX_INTERSECTRGN);
    //        Graphics graphics(dc);
    //        ReleaseDC(args.hwnd, dc);
    //        args.handled = true;
    //    }

}

void Window::Close()
{
    PostMessage(Handle(), WM_CLOSE, 0, 0);
}

void Window::FocusNext()
{
    if (focusedControl == nullptr)
    {
        Component* child = Children().FirstChild();
        while (child)
        {
            if (child->IsControl())
            {
                Control* control = static_cast<Control*>(child);
                Control* tabStopChild = control->GetFirstEnabledTabStopControl();
                if (tabStopChild)
                {
                    tabStopChild->SetFocus();
                    focusedControl = tabStopChild;
                    return;
                }
            }
            child = child->NextSibling();
        }
    }
    else
    {
        Component* parent = nullptr;
        Component* child = focusedControl;
        Container* container = child->GetContainer();
        if (container)
        {
            parent = container->Parent();
        }
        while (child)
        {
            child = child->NextSibling();
            while (child)
            {
                if (child->IsControl())
                {
                    Control* control = static_cast<Control*>(child);
                    Control* tabStopChild = control->GetFirstEnabledTabStopControl();
                    if (tabStopChild)
                    {
                        tabStopChild->SetFocus();
                        focusedControl = tabStopChild;
                        return;
                    }
                }
                child = child->NextSibling();
            }
            child = parent;
            if (parent)
            {
                container = parent->GetContainer();
                if (container)
                {
                    parent = container->Parent();
                }
                else
                {
                    parent = nullptr;
                }
            }
        }
        focusedControl = nullptr;
        FocusNext();
    }
}

void Window::FocusPrev()
{
    if (focusedControl == nullptr)
    {
        Component* child = Children().LastChild();
        while (child)
        {
            if (child->IsControl())
            {
                Control* control = static_cast<Control*>(child);
                Control* tabStopChild = control->GetLastEnabledTabStopControl();
                if (tabStopChild)
                {
                    tabStopChild->SetFocus();
                    focusedControl = tabStopChild;
                    return;
                }
            }
            child = child->PrevSibling();
        }
    }
    else
    {
        Component* parent = nullptr;
        Component* child = focusedControl;
        Container* container = child->GetContainer();
        if (container)
        {
            parent = container->Parent();
        }
        while (child)
        {
            child = child->PrevSibling();
            while (child)
            {
                if (child->IsControl())
                {
                    Control* control = static_cast<Control*>(child);
                    Control* tabStopChild = control->GetLastEnabledTabStopControl();
                    if (tabStopChild)
                    {
                        tabStopChild->SetFocus();
                        focusedControl = tabStopChild;
                        return;
                    }
                }
                child = child->PrevSibling();
            }
            child = parent;
            if (parent)
            {
                container = parent->GetContainer();
                if (container)
                {
                    parent = container->Parent();
                }
                else
                {
                    parent = nullptr;
                }
            }
        }
        focusedControl = nullptr;
        FocusPrev();
    }
}

struct DialogGuard
{
    DialogGuard(Window* window_) : window(window_) 
    {
        window->SetShowingDialog();
    }
    ~DialogGuard()
    {
        window->ResetShowingDialog();
    }
    Window* window;
};

DialogResult Window::ShowDialog(Window& parentWindow)
{
    DialogGuard dialogGuard(this);
    Point parentLoc = parentWindow.Location();
    Size parentSize = parentWindow.GetSize();
    Size size = GetSize();
    SetLocation(Point(std::max(0, parentLoc.X + (parentSize.Width - size.Width) / 2), std::max(0, parentLoc.Y + (parentSize.Height - size.Height) / 2)));
    SetDialogResult(DialogResult::none);
    DialogResultFunction dialogResultFn = cmajor::wing::GetDialogResult;
    DialogWindowKeyPreviewFunction keyPreviewFn = cmajor::wing::DialogWindowKeyPreview;
    int result = DialogMessageLoop(Handle(), parentWindow.Handle(), dialogResultFn, keyPreviewFn, this);
    return static_cast<DialogResult>(result);
}

void Window::SetDefaultButton(Button* defaultButton_)
{
    if (defaultButton != defaultButton_)
    {
        if (defaultButton)
        {
            defaultButton->ResetDefault();
        }
        defaultButton = defaultButton_;
        if (defaultButton)
        {
            defaultButton->SetDefault();
        }
    }
}

void Window::MouseUpNotification(MouseEventArgs& args)
{
}

bool Window::ProcessMessage(Message& msg) 
{
    switch (msg.message)
    {
        case WM_CLOSE:
        {
            bool cancelClose = false;
            CancelArgs args(cancelClose);
            OnWindowClosing(args);
            if (args.cancel) 
            {
                msg.result = 0;
                return true;
            }
            break;
        }
        case WM_DESTROY:
        {
            bool processed = false;
            OnWindowClosed(processed);
            if (processed)
            {
                msg.result = 0;
                return true;
            }
            break;
        }
        case WM_SIZE:
        {
            DoWindowStateChanged(msg.wParam);
            return ContainerControl::ProcessMessage(msg);
        }
        case WM_NCPAINT:
        {
            NCPaintEventArgs args(msg.handle, (HRGN)msg.wParam);
            OnNCPaint(args);
            if (args.handled)
            {
                msg.result = 0;
                return true;
            }
            break;
        }
        case WM_NCACTIVATE:
        {
            if (frame)
            {
                windowActive = msg.wParam;
                if (!windowActive)
                {
                    frame->Invalidate();
                    msg.result = 1;
                    return true;
                }
            }
            break;
        }
        case WM_NCMOUSEMOVE:
        {
            if (frame)
            {
                int x = GET_X_LPARAM(msg.lParam);
                int y = GET_Y_LPARAM(msg.lParam);
                Point mousePos(x, y);
                if (frame->SetState(mousePos))
                {
                    frame->Invalidate();
                    msg.result = 0;
                    return true;
                }
            }
            break;
        }
        case WM_NCMOUSELEAVE:
        {
            if (frame)
            {
                frame->Invalidate();
                msg.result = 0;
                return true;
            }
            break;
        }
        case WM_NCHITTEST:
        {
            if (frame)
            {
                int x = GET_X_LPARAM(msg.lParam);
                int y = GET_Y_LPARAM(msg.lParam);
                Point pt(x, y);
                msg.result = frame->HitTest(pt);
                return true;
            }
            break;
        }
        case WM_NCLBUTTONDOWN: 
        {
            int hitTestValue = msg.wParam;
            if (OnNCLButtonDown(hitTestValue))
            {
                msg.result = 0;
                return true;
            }
            break;
        }
        case WM_NCLBUTTONUP:
        {
            int hitTestValue = msg.wParam;
            if (OnNCLButtonUp(hitTestValue))
            {
                msg.result = 0;
                return true;
            }
            break;
        }
        case WM_ACTIVATE:
        {
            if (frame)
            {
                frame->Invalidate();
                msg.result = 0;
                return true;
            }
            break;
        }
        case WM_COMMAND: case WM_MOVE: case WM_PAINT: 
        case WM_MOUSEMOVE: case WM_MOUSELEAVE: case WM_LBUTTONDOWN: case WM_LBUTTONUP: case WM_LBUTTONDBLCLK: case WM_RBUTTONDOWN: case WM_RBUTTONUP:
        case WM_CHAR: case WM_KEYDOWN: case WM_KEYUP: case WM_SYSCOMMAND: case WM_HSCROLL: case WM_VSCROLL: case WM_MOUSEWHEEL: case WM_TIMER: case WM_SETFOCUS: case WM_KILLFOCUS:
        case WM_SHOWWINDOW: case WM_CLIPBOARDUPDATE:
        {
            return ContainerControl::ProcessMessage(msg);
        }
    }
    return false;
}

void Window::OnKeyDown(KeyEventArgs& args)
{
    ContainerControl::OnKeyDown(args);
    if (!args.handled)
    {
        switch (args.key)
        {
            case Keys::enter:
            {
                if (ShowingDialog())
                {
                    if (focusedControl)
                    {
                        if (focusedControl->IsTextBox())
                        {
                            TextBox* textBox = static_cast<TextBox*>(focusedControl);
                            if (textBox->IsMultiline())
                            {
                                break;
                            }
                        }
                    }
                    if (focusedControl)
                    {
                        if (focusedControl->IsButton())
                        {
                            Button* button = static_cast<Button*>(focusedControl);
                            button->DoClick();
                            args.handled = true;
                        }
                    }
                    if (!args.handled)
                    {
                        if (defaultButton)
                        {
                            if (defaultButton->IsEnabled())
                            {
                                defaultButton->DoClick();
                                args.handled = true;
                            }
                        }
                    }
                }
                break;
            }
            case Keys::escape:
            {
                if (ShowingDialog())
                {
                    if (cancelButton)
                    {
                        cancelButton->DoClick();
                        args.handled = true;
                    }
                }
                break;
            }
            case Keys::tab:
            {
                FocusNext();
                args.handled = true;
                break;
            }
            case Keys::shiftModifier | Keys::tab:
            {
                FocusPrev();
                args.handled = true;
                break;
            }
        }
    }
}

void Window::OnPaint(PaintEventArgs& args)
{
    try
    {
        args.graphics.Clear(BackgroundColor());
        ContainerControl::OnPaint(args);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void Window::OnControlAdded(ControlEventArgs& args)
{
    ContainerControl::OnControlAdded(args);
    Control* control = args.control;
    if (control->IsMenuBar())
    { 
        menuBar = static_cast<MenuBar*>(control);
    }
    else if (control->IsButton())
    {
        Button* button = static_cast<Button*>(control);
        if (button->IsDefault())
        {
            SetDefaultButton(button);
        }
    }
}

void Window::OnControlRemoved(ControlEventArgs& args)
{
    ContainerControl::OnControlRemoved(args);
    Control* control = args.control;
    if (control == menuBar)
    {
        menuBar = nullptr;
    }
    else if (control == defaultButton)
    {
        defaultButton = nullptr;
    }
    else if (control == cancelButton)
    {
        cancelButton = nullptr;
    }
}

void Window::OnMouseDown(MouseEventArgs& args)
{
    ContainerControl::OnMouseDown(args);
    if (menuBar)
    {
        menuBar->MouseDownInternal(args);
    }
}

void Window::OnMouseUp(MouseEventArgs& args)
{
    ContainerControl::OnMouseUp(args);
    if (menuBar)
    {
        menuBar->MouseUpInternal(args);
    }
}

void Window::OnMouseMove(MouseEventArgs& args)
{
    ContainerControl::OnMouseMove(args);
    if (menuBar)
    {
        menuBar->MouseMoveInternal(args);
    }
}

void Window::OnGotFocus()
{
    ContainerControl::OnGotFocus();
    Application::SetActiveWindow(this);
}

void Window::OnLostFocus()
{
    ContainerControl::OnLostFocus();
    Application::SetActiveWindow(nullptr);
}

void Window::OnLocationChanged()
{
    ContainerControl::OnLocationChanged();
    if (GetTheme() == "dark")
    {
        if (frame)
        {
            frame->Invalidate();
            frame.reset();
        }
    }
}

void Window::OnWindowStateChanged()
{
    windowStateChanged.Fire();
}

void Window::OnWindowClosing(CancelArgs& args)
{
    windowClosing.Fire(args);
}

void Window::OnWindowClosed(bool& processed)
{
    if (windowClosed.Handlers().empty())
    {
        if (IsMainWindow())
        {
            Application::Exit();
            processed = true;
        }
    }
    else
    {
        windowClosed.Fire();
        processed = true;
    }
}

void Window::OnNCPaint(NCPaintEventArgs& args)
{
    ncPaint.Fire(args);
}

void Window::PaintWindowRect(Graphics& graphics)
{
    MakeWindowFrame();
    Color backgroundColor = GetColor("window.title.bar");
    graphics.Clear(backgroundColor);
    DrawWindowCaption(graphics);
    PaintFrame(graphics);
}

bool Window::OnNCLButtonDown(int hitTestValue)
{
    if (!frame) return false;
    ncLButtonDownHitTest = hitTestValue;
    switch (hitTestValue)
    {
        case HTCLOSE:
        {
            return true;
        }
        case HTMINBUTTON:
        {
            return true;
        }
        case HTMAXBUTTON:
        {
            WindowState windowState = GetWindowState();
            if (windowState == WindowState::maximized || windowState == WindowState::minimized)
            {
                return true;
            }
            else if (windowState == WindowState::normal)
            {
                return true;
            }
            break;
        }
    }
    return false;
}

bool Window::OnNCLButtonUp(int hitTestValue)
{
    if (!frame) return false;
    if (hitTestValue == ncLButtonDownHitTest)
    {
        switch (hitTestValue)
        {
            case HTCLOSE:
            {
                Close();
                return true;
            }
            case HTMINBUTTON:
            {
                ShowWindow(SW_SHOWMINIMIZED);
                return true;
            }
            case HTMAXBUTTON:
            {
                WindowState windowState = GetWindowState();
                if (windowState == WindowState::maximized || windowState == WindowState::minimized)
                {
                    ShowWindow(SW_RESTORE);
                    return true;
                }
                else if (windowState == WindowState::normal)
                {
                    ShowWindow(SW_MAXIMIZE);
                    return true;
                }
                break;
            }
        }
    }
    return false;
}

void Window::DrawWindowCaption(Graphics& graphics)
{
    MakeCaptionResources();
    PointF origin(16 + borderWidth, 8);
    Brush* brush = captionBrush.get();
    if (!windowActive)
    {
        brush = disabledCaptionBrush.get();
    }
    DrawString(graphics, Text(), *captionFont, origin, *brush);
}

void Window::MakeWindowFrame()
{
    RECT wr;
    GetWindowRect(Handle(), &wr);
    RECT cr;
    GetClientRect(Handle(), &cr);
    Rect windowRect = ToRect(wr);
    Rect clientRect = ToRect(cr);
    if (!frame || !frame->DataEqual(windowRect, borderWidth, buttonWidth, buttonHeight, static_cast<int>(windowState)))
    {
        frame.reset(new WindowFrame(this, windowRect, clientRect, borderWidth, buttonWidth, buttonHeight, static_cast<int>(windowState)));
    }
}

void Window::PaintFrame(Graphics& graphics)
{
    frame->Paint(graphics, sysButtonPen.get(), sysButtonSelectedPen.get(), sysButtonDisabledPen.get(), titleBarBackgroundBrush.get(), sysButtonHoverBackgroundBrush.get());
}

void Window::MakeCaptionResources()
{
    if (!captionFont)
    {
        std::u16string capFontFamily = ToUtf16(captionFontFamilyName);
        float captionFontSize = captionFontPercent * captionHeight;
        captionFont.reset(new Font(FontFamily((CONST WCHAR*)capFontFamily.c_str()), captionFontSize, FontStyle::FontStyleRegular, Unit::UnitPoint));
    }
    if (!captionBrush)
    {
        captionBrush.reset(new SolidBrush(GetColor("window.title.text")));
    }
    if (!disabledCaptionBrush)
    {
        disabledCaptionBrush.reset(new SolidBrush(GetColor("window.title.disabled.text")));
    }
    if (!sysButtonPen)
    {
        sysButtonPen.reset(new Pen(GetColor("window.sys.button")));
    }
    if (!sysButtonDisabledPen)
    {
        sysButtonDisabledPen.reset(new Pen(GetColor("window.title.disabled.text")));
    }
    if (!sysButtonSelectedPen)
    {
        sysButtonSelectedPen.reset(new Pen(GetColor("window.sys.button.selected")));
    }
    if (!titleBarBackgroundBrush)
    {
        titleBarBackgroundBrush.reset(new SolidBrush(GetColor("window.title.bar")));
    }
    if (!sysButtonHoverBackgroundBrush)
    {
        sysButtonHoverBackgroundBrush.reset(new SolidBrush(GetColor("window.sys.button.hover.background")));
    }
}

void Window::DoWindowStateChanged(int sizeType)
{
    switch (sizeType)
    {
        case SIZE_RESTORED:
        {
            SetWindowState(WindowState::normal);
            break;
        }
        case SIZE_MAXIMIZED:
        {
            SetWindowState(WindowState::maximized);
            break;
        }
        case SIZE_MINIMIZED:
        {
            SetWindowState(WindowState::minimized);
            break;
        }
    }
}

int GetDialogResult(void* dialogWindowPtr)
{
    if (dialogWindowPtr)
    {
        Window* dialogWindow = static_cast<Window*>(dialogWindowPtr);
        return static_cast<int>(dialogWindow->GetDialogResult());
    }
    return 0;
}

void DialogWindowKeyPreview(void* dialogWindowPtr, Keys key, bool& handled)
{
    if (dialogWindowPtr)
    {
        Window* dialogWindow = static_cast<Window*>(dialogWindowPtr);
        KeyPreviewMethod dialogKeyPreviewMethod = dialogWindow->GetDialogKeyPreviewMethod();
        dialogKeyPreviewMethod(key, handled);
    }
}

} } // cmajor::wing
