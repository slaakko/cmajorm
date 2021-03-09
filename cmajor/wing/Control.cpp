// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/wing/Control.hpp>
#include <cmajor/wing/ContainerControl.hpp>
#include <cmajor/wing/Menu.hpp>
#include <cmajor/wing/Window.hpp>
#include <cmajor/wing/Application.hpp>
#include <cmajor/wing/Container.hpp>
#include <cmajor/wing/Buffer.hpp>
#include <soulng/util/Unicode.hpp>
#include <windowsx.h>

namespace cmajor { namespace wing {

using namespace soulng::unicode;

ControlCreateParams::ControlCreateParams() :
    windowClassName(),
    windowClassStyle(0),
    windowStyle(DefaultChildWindowStyle()),
    windowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()),
    backgroundColor(DefaultControlBackgroundColor()),
    text(), 
    location(), 
    size(), 
    anchors(Anchors::none), 
    dock(Dock::none)
{
}

ControlCreateParams& ControlCreateParams::Defaults()
{
    return *this;
}

ControlCreateParams& ControlCreateParams::WindowClassName(const std::string& windowClassName_)
{
    windowClassName = windowClassName_;
    return *this;
}

ControlCreateParams& ControlCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    windowClassStyle = windowClassStyle_;
    return *this;
}

ControlCreateParams& ControlCreateParams::WindowStyle(int windowStyle_)
{
    windowStyle = windowStyle_;
    return *this;
}

ControlCreateParams& ControlCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    windowClassBackgroundColor = windowClassBackgroundColor_;
    return *this;
}

ControlCreateParams& ControlCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    backgroundColor = backgroundColor_;
    return *this;
}

ControlCreateParams& ControlCreateParams::Text(const std::string& text_)
{
    text = text_;
    return *this;
}

ControlCreateParams& ControlCreateParams::Location(Point location_)
{
    location = location_;
    return *this;
}

ControlCreateParams& ControlCreateParams::SetSize(Size size_)
{
    size = size_;
    return *this;
}

ControlCreateParams& ControlCreateParams::SetAnchors(Anchors anchors_)
{
    anchors = anchors_;
    return *this;
}

ControlCreateParams& ControlCreateParams::SetDock(Dock dock_)
{
    dock = dock_;
    return *this;
}

Control::Control(ControlCreateParams& createParams) :
    Component(),
    windowClassName(createParams.windowClassName),
    windowClassStyle(createParams.windowClassStyle),
    windowClassBackgroundColor(createParams.windowClassBackgroundColor),
    windowStyle(createParams.windowStyle),
    backgroundColor(createParams.backgroundColor),
    location(createParams.location),
    size(createParams.size),
    anchors(createParams.anchors),
    dock(createParams.dock),
    text(createParams.text),
    handle(nullptr),
    font(),
    fontHandle(nullptr),
    flags(ControlFlags::none),
    caretShowCount(0),
    arrowCursor(LoadStandardCursor(StandardCursorId::arrow)),
    mouseHoverMs(0),
    mouseHoverLocation(),
    mouseHoverRectSize(DefaultMouseHoverSize())
{
    if ((windowStyle & WS_DISABLED) != 0)
    {
        SetDisabled();
    }
    if ((windowStyle & WS_VISIBLE) == 0)
    {
        SetHidden();
    }
    if ((windowStyle & WS_TABSTOP) != 0)
    {
        SetTabStop();
    }
    if ((windowStyle & WS_CHILD) == 0)
    {
        Create();
    }
}

Control::~Control()
{
    Application::GetWindowManager().RemoveWindow(this);
}

ContainerControl* Control::GetContainerControl() const
{
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        return parentControl->GetContainerControl();
    }
    return nullptr;
}

Window* Control::GetWindow() const
{
    if (IsWindow())
    {
        return const_cast<Window*>(static_cast<const Window*>(this));
    }
    Container* container = GetContainer();
    if (container)
    {
        Component* parent = container->Parent();
        if (parent)
        {
            if (parent->IsControl())
            {
                Control* control = static_cast<Control*>(parent);
                return control->GetWindow();
            }
        }
    }
    return nullptr;
}

void Control::AddChildVisual(Control* child)
{
    if (handle)
    {
        if (!child->handle)
        {
            child->Create();
        }
        else
        {
            child->SetParentWindow(this);
        }
        if ((child->WindowStyle() & WS_VISIBLE) != 0)
        {
            child->Show();
            child->Update();
        }
    }
    else
    {
        createList.push_back(child);
    }
}

void Control::SetParentWindow(Control* parent)
{
    SetParent(handle, parent->handle);
}

Control* Control::ParentControl() const
{
    Control* parentControl = nullptr;
    Container* container = GetContainer();
    if (container)
    {
        Component* parent = container->Parent();
        if (parent && parent->IsControl())
        {
            parentControl = static_cast<Control*>(parent);
        }
    }
    return parentControl;
}

void Control::Create()
{
    if (handle) return;
    Control* parentControl = ParentControl();
    HWND parentHandle = nullptr;
    if (parentControl)
    {
        parentHandle = parentControl->handle;
    }
    std::u16string className = ToUtf16(windowClassName);
    std::u16string windowName = ToUtf16(text);
    WindowManager& windowManager = Application::GetWindowManager();
    if (windowManager.IsSystemClassName(windowClassName))
    {
        handle = CreateWindowExW(0, LPCWSTR(className.c_str()), LPCWSTR(windowName.c_str()), windowStyle, location.X, location.Y, size.Width, size.Height, parentHandle, nullptr, Instance(), nullptr);
    }
    else
    {
        uint16_t windowClass = windowManager.Register(windowClassName, windowClassStyle, windowClassBackgroundColor);
        uint64_t wc = windowClass;
        LPCWSTR wcs = *reinterpret_cast<LPCWSTR*>(&wc);
        handle = CreateWindowExW(0, wcs, LPCWSTR(windowName.c_str()), windowStyle, location.X, location.Y, size.Width, size.Height, parentHandle, nullptr, Instance(), nullptr);
    }
    if (!handle)
    {
        throw WindowsException(GetLastError());
    }
    for (Control* child : createList)
    {
        child->Create();
    }
    createList.clear();
    windowManager.AddWindow(this);
    OnCreated();
    RECT clientRect;
    bool succeeded = GetClientRect(handle, &clientRect);
    if (succeeded)
    {
        size.Width = clientRect.right - clientRect.left;
        size.Height = clientRect.bottom - clientRect.top;
    }
    else
    {
        throw WindowsException(GetLastError());
    }
    if ((windowStyle & WS_CHILD) == 0)
    {
        RECT windowRect;
        bool succeeded = GetWindowRect(handle, &windowRect);
        if (succeeded)
        {
            location.X = windowRect.left;
            location.Y = windowRect.top;
        }
        else
        {
            throw WindowsException(GetLastError());
        }
    }
    if (parentControl != nullptr && parentControl->createList.empty())
    {
        DockWindow();
    }
}

void Control::Show()
{
    if (Hidden())
    {
        ResetHidden();
        windowStyle = windowStyle | WS_VISIBLE;
        OnVisibleChanged();
    }
    if (handle)
    {
        ShowWindow(handle, SW_SHOW);
    }
}

void Control::Hide()
{
    if (handle)
    {
        ShowWindow(handle, SW_HIDE);
    }
    if (!Hidden())
    {
        SetHidden();
        windowStyle = (windowStyle & ~WS_VISIBLE);
        OnVisibleChanged();
    }
}

void Control::Enable()
{
    if (Disabled())
    {
        ResetDisabled();
        windowStyle = windowStyle & ~WS_DISABLED;
        if (handle)
        {
            EnableWindow(handle, true);
        }
        OnEnabledChanged();
    }
}

void Control::Disable()
{
    if (!Disabled())
    {
        SetDisabled();
        windowStyle = windowStyle | WS_DISABLED;
        if (handle)
        {
            EnableWindow(handle, false);
        }
        OnEnabledChanged();
    }
}

void Control::Update()
{
    UpdateWindow(handle);
}

void Control::DockWindow()
{
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        if (parentControl->IsContainerControl())
        {
            ContainerControl* containerControl = static_cast<ContainerControl*>(parentControl);
            containerControl->DockChildren();
        }
    }
}

void Control::DockWindow(Rect& parentRect)
{
    if (dock == Dock::none) return;
    if (!handle) return;
    Point parentLoc;
    parentRect.GetLocation(&parentLoc);
    Size parentSize;
    parentRect.GetSize(&parentSize);
    Point newLocation = location;
    Size newSize = size;
    switch (dock)
    {
        case Dock::left:
        {
            newLocation = parentLoc;
            newSize.Height = parentSize.Height;
            parentRect.X = parentLoc.X + size.Width;
            parentRect.Y = parentLoc.Y;
            parentRect.Width = parentSize.Width - size.Width;
            parentRect.Height = parentSize.Height;
            break;
        }
        case Dock::top:
        {
            newLocation = parentLoc;
            newSize.Width = parentSize.Width;
            parentRect.X = parentLoc.X;
            parentRect.Y = parentLoc.Y + size.Height;
            parentRect.Width = parentSize.Width;
            parentRect.Height = parentSize.Height - size.Height;
            break;
        }
        case Dock::right:
        {
            newLocation.X = parentLoc.X + parentSize.Width - size.Width;
            newLocation.Y = parentLoc.Y;
            newSize.Height = parentSize.Height;
            parentRect.Width = parentSize.Width - size.Width;
            parentRect.Height = parentSize.Height;
            break;
        }
        case Dock::bottom:
        {
            newLocation.X = parentLoc.X;
            newLocation.Y = parentLoc.Y + parentSize.Height - size.Height;
            newSize.Width = parentSize.Width;
            parentRect.Width = parentSize.Width;
            parentRect.Height = parentSize.Height - size.Height;
            break;
        }
        case Dock::fill:
        {
            newLocation = parentLoc;
            newSize = parentSize;
            parentRect.X = 0;
            parentRect.Y = 0;
            parentRect.Width = 0;
            parentRect.Height = 0;
            break;
        }
    }
    if (location != newLocation || size != newSize)
    {
        SetLocation(newLocation);
        SetSize(newSize);
        MoveWindow(location, size, true);
    }
}

void Control::MoveWindow(int dx, int dy)
{
    Point newLocation = location;
    Size newSize = size;
    bool leftAnchored = (anchors & Anchors::left) != Anchors::none;
    bool rightAnchored = (anchors & Anchors::right) != Anchors::none;
    if (!leftAnchored)
    {
        newLocation.X = newLocation.X + dx;
    }
    else if (rightAnchored)
    {
        newSize.Width = newSize.Width + dx;
    }
    bool topAnchored = (anchors & Anchors::top) != Anchors::none;
    bool bottomAnchored = (anchors & Anchors::bottom) != Anchors::none;
    if (!topAnchored)
    {
        newLocation.Y = newLocation.Y + dy;
    }
    else if (bottomAnchored)
    {
        newSize.Height = newSize.Height + dy;
    }
    if (location != newLocation || size != newSize)
    {
        SetLocation(newLocation);
        SetSize(newSize);
    }
}

void Control::MoveWindow(const Point& loc, const Size& sz, bool repaint)
{
    bool succeeded = ::MoveWindow(handle, loc.X, loc.Y, sz.Width, sz.Height, repaint);
    if (!succeeded)
    {
        throw WindowsException(GetLastError());
    }
}

void Control::PaintAll(PaintEventArgs& args, bool skipMenuBar)
{
    OnPaint(args);
}

void Control::SetLocation(const Point& newLocation)
{
    if (location != newLocation)
    {
        location = newLocation;
        if (handle)
        {
            MoveWindow(location, size, true);
        }
        OnLocationChanged();
    }
}

void Control::SetLocationInternal(const Point& newLocation)
{
    if (location != newLocation)
    {
        location = newLocation;
        OnLocationChanged();
    }
}

void Control::SetSize(const Size& newSize)
{
    if (size != newSize)
    {
        SetSizeInternal(newSize);
        if (handle)
        {
            MoveWindow(location, size, true);
        }
    }
}

void Control::SetSizeInternal(const Size& newSize)
{
    if (size != newSize)
    {
        size = newSize;
        OnSizeChanged();
    }
}

void Control::SetContentLocationInternal(const Point& newContentLocation)
{
    if (contentLocation != newContentLocation)
    {
        contentLocation = newContentLocation;
        OnContentLocationChanged();
    }
}

void Control::SetContentLocation(const Point& newContentLocation)
{
    if (contentLocation != newContentLocation)
    {
        SetContentLocationInternal(newContentLocation);
        Control* parentControl = ParentControl();
        if (parentControl)
        {
            ControlEventArgs args(this);
            parentControl->OnChildContentLocationChanged(args);
        }
    }
}

void Control::SetContentSizeInternal(const Size& newContentSize)
{
    if (contentSize != newContentSize)
    {
        contentSize = newContentSize;
        OnContentSizeChanged();
    }
}

void Control::SetContentSize(const Size& newContentSize)
{
    if (contentSize != newContentSize)
    {
        SetContentSizeInternal(newContentSize);
        Control* parentControl = ParentControl();
        if (parentControl)
        {
            ControlEventArgs args(this);
            parentControl->OnChildContentSizeChanged(args);
        }
    }
}

const Font& Control::GetFont() const
{
    if (!font.IsNull())
    {
        return font;
    }
    Control* parent = ParentControl();
    if (parent)
    {
        return parent->GetFont();
    }
    else
    {
        return font;
    }
}

void Control::SetFont(Font& font_)
{
    font = font_;
}

void Control::SetFont(Font&& font_)
{
    font = std::move(font_);
}

const FontHandle& Control::GetFontHandle(Graphics& graphics)
{
    if (!fontHandle.IsNull())
    {
        return fontHandle;
    }
    if (HasFont())
    {
        fontHandle = ToFontHandle(graphics, font);
        return fontHandle;
    }
    Control* parent = ParentControl();
    if (parent != nullptr)
    {
        return parent->GetFontHandle(graphics);
    }
    else
    {
        return fontHandle;
    }
}

void Control::ShowCaret()
{
    ::ShowCaret(handle);
    ++caretShowCount;
}

void Control::HideCaret()
{
    ::HideCaret(handle);
    --caretShowCount;
}

void Control::Invalidate()
{
    InvalidateRect(handle, nullptr, false);
}

void Control::Invalidate(bool eraseBackground)
{
    InvalidateRect(handle, nullptr, eraseBackground);
}

void Control::Invalidate(const Rect& rect)
{
    RECT winRect = ToWinRect(rect);
    InvalidateRect(handle, &winRect, false);
}

void Control::Invalidate(const Rect& rect, bool eraseBackground)
{
    RECT winRect = ToWinRect(rect);
    InvalidateRect(handle, &winRect, eraseBackground);
}

void Control::SetFocus()
{
    ::SetFocus(handle);
    Window* window = GetWindow();
    if (window)
    {
        window->SetFocusedControl(this);
    }
}

void Control::SetText(const std::string& text_)
{
    if (text != text_)
    {
        text = text_;
        std::u16string txt = ToUtf16(text);
        ::SetWindowText(handle, (LPCWSTR)txt.c_str());
        OnTextChanged();
    }
}

void Control::SetTextInternal(const std::string& text_)
{
    if (text != text_)
    {
        text = text_;
        OnTextChanged();
    }
}

void Control::SetTimer(int timerId, int durationMs)
{
    int retval = ::SetTimer(handle, timerId, durationMs, nullptr);
    if (!retval)
    {
        throw WindowsException(GetLastError());
    }
}

void Control::KillTimer(int timerId)
{
    if (!::KillTimer(handle, timerId))
    {
        throw WindowsException(GetLastError());
    }
}

void Control::SetContentChanged()
{
    OnContentChanged();
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        ControlEventArgs args(this);
        parentControl->OnChildContentChanged(args);
    }
}

void Control::ScrollLineDown()
{
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        parentControl->ScrollLineDown();
    }
}

void Control::ScrollLineUp()
{
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        parentControl->ScrollLineUp();
    }
}

void Control::TranslateChildGraphics(Graphics& graphics)
{
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        parentControl->TranslateChildGraphics(graphics);
    }
}

void Control::TranslateMousePos(Point& location)
{
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        parentControl->TranslateMousePos(location);
    }
}

void Control::TranslateContentLocation(Point& location)
{
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        parentControl->TranslateContentLocation(location);
    }
}

void Control::CreateCaret()
{
    ::CreateCaret(handle, nullptr, 1, 15);
}

bool Control::ProcessMessage(Message& msg)
{
    // todo originalWndProc
    switch (msg.message)
    {
        case WM_PAINT:
        {
            DoPaint();
            msg.result = 0;
            return true;
        }
        case WM_MOUSEMOVE:
        {
            MouseEventArgs args(Point(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam)), static_cast<MouseButtons>(msg.wParam), 0);
            DoMouseMove(args);
            msg.result = 0;
            return true;
        }
        case WM_MOUSELEAVE:
        {
            DoMouseLeave();
            msg.result = 0;
            return true;
        }
        case WM_LBUTTONDOWN:
        {
            MouseEventArgs args(Point(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam)), static_cast<MouseButtons>(static_cast<MouseButtons>(msg.wParam) | MouseButtons::lbutton), 1);
            DoMouseDown(args);
            msg.result = 0;
            return true;
        }
        case WM_LBUTTONUP:
        {
            MouseEventArgs args(Point(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam)), static_cast<MouseButtons>(static_cast<MouseButtons>(msg.wParam) | MouseButtons::lbutton), 1);
            DoMouseUp(args);
            msg.result = 0;
            return true;
        }
        case WM_LBUTTONDBLCLK:
        {
            MouseEventArgs args(Point(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam)), static_cast<MouseButtons>(static_cast<MouseButtons>(msg.wParam) | MouseButtons::lbutton), 2);
            DoMouseDoubleClick(args);
            msg.result = 0;
            return true;
        }
        case WM_RBUTTONDOWN:
        {
            MouseEventArgs args(Point(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam)), static_cast<MouseButtons>(static_cast<MouseButtons>(msg.wParam) | MouseButtons::rbutton), 1);
            DoMouseDown(args);
            msg.result = 0;
            return true;
        }
        case WM_RBUTTONUP:
        {
            MouseEventArgs args(Point(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam)), static_cast<MouseButtons>(static_cast<MouseButtons>(msg.wParam) | MouseButtons::rbutton), 1);
            DoMouseUp(args);
            msg.result = 0;
            return true;
        }
        case WM_HSCROLL:
        {
            int value = LOWORD(msg.wParam);
            DoHScroll(value);
            msg.result = 0;
            return true;
        }
        case WM_VSCROLL:
        {
            int value = LOWORD(msg.wParam);
            DoVScroll(value);
            msg.result = 0;
            return true;
        }
        case WM_MOUSEWHEEL:
        {
            int value = GET_WHEEL_DELTA_WPARAM(msg.wParam);
            MouseWheelEventArgs args(value);
            DoMouseWheel(args);
            if (args.handled)
            {
                msg.result = 0;
                return true;
            }
            break;
        }
        case WM_SETFOCUS:
        {
            DoSetFocus();
            msg.result = 0;
            return true;
        }
        case WM_KILLFOCUS:
        {
            DoKillFocus();
            msg.result = 0;
            return true;
        }
        case WM_TIMER:
        {
            DoTimer(msg.wParam);
            msg.result = 0;
            return true;
        }
        case WM_SHOWWINDOW:
        {
            if (msg.wParam == 1)
            {
                OnShown();
            }
            msg.result = 0;
            return true;
        }
        case WM_MOVE:
        {
            Point newLocation = LParamLocation(msg);
            Point oldLocation = Location();
            if (newLocation != oldLocation)
            {
                SetLocationInternal(newLocation);
            }
            msg.result = 0;
            return true;
        }
        case WM_SIZE:
        {
            Size newSize = LParamSize(msg);
            Size oldSize = GetSize();
            if (newSize != oldSize)
            {
                SetSizeInternal(newSize);
            }
            msg.result = 0;
            return true;
        }
        case WM_COMMAND:
        {
            HWND childWindowHandle = LParamHandle(msg);
            uint16_t notificationCode = msg.WParamHiWord();
            uint16_t controlId = msg.WParamLoWord();
            Control* child = Application::GetWindowManager().GetWindow(childWindowHandle);
            if (child != nullptr)
            {
                switch (notificationCode)
                {
                    case BN_CLICKED:
                    {
                        child->OnClick();
                        msg.result = 0;
                        return true;
                    }
                    case BN_SETFOCUS:
                    {
                        child->DoGotFocus();
                        msg.result = 0;
                        return true;
                    }
                    case BN_KILLFOCUS:
                    {
                        child->DoLostFocus();
                        msg.result = 0;
                        return true;
                    }
                }
            }
            break;
        }
        case WM_SYSCOMMAND:
        {
            if (DoSysCommand(msg.wParam, msg.lParam))
            {
                msg.result = 0;
                return true;
            }
            break;
        }
        case WM_KEYDOWN:
        {
            int virtualKeyCode = static_cast<int>(msg.wParam);
            if (DoKeyDown(virtualKeyCode))
            {
                msg.result = 0;
                return true;
            }
            break;
        }
        case WM_KEYUP:
        {
            int virtualKeyCode = static_cast<int>(msg.wParam);
            if (DoKeyUp(virtualKeyCode))
            {
                msg.result = 0;
                return true;
            }
            break;
        }
        case WM_CHAR:
        {
            char16_t ch = static_cast<char16_t>(msg.wParam);
            KeyPressEventArgs args(ch);
            DoKeyPress(args);
            if (args.handled)
            {
                msg.result = 0;
                return true;
            }
            break;
        }
    }
    return false;
}

struct PaintGuard
{
    PaintGuard(HWND handle_, const PAINTSTRUCT* ps_) : handle(handle_), ps(ps_) {}
    ~PaintGuard() { EndPaint(handle, ps); }
    HWND handle;
    const PAINTSTRUCT* ps;
};

void Control::DoPaint()
{
    try
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(handle, &ps);
        Rect clipRect = ToRect(ps.rcPaint);
        PaintGuard paintGuard(handle, &ps);
        {        
            Graphics graphics(hdc);
            if (!IsDecoratorControl())
            {
                TranslateChildGraphics(graphics);
            }
            if (Focused())
            {
                --caretShowCount;
            }
            if (IsDoubleBuffered())
            {
                if (!buffer || buffer->GetSize() != GetSize())
                {
                    buffer.reset(new Buffer(GetSize(), graphics));
                }
                buffer->BitmapGraphics().Clear(BackgroundColor());
                Matrix matrix;
                CheckGraphicsStatus(graphics.GetTransform(&matrix));
                CheckGraphicsStatus(buffer->BitmapGraphics().SetTransform(&matrix));
                PaintEventArgs args(buffer->BitmapGraphics(), clipRect);
                OnPaint(args);
                Graphics windowGraphics(hdc);
                buffer->Draw(windowGraphics);
            }
            else
            { 
                PaintEventArgs args(graphics, clipRect);
                OnPaint(args);
            }
        }
        if (Focused() && CaretCreated())
        {
            ShowCaret();
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void Control::DoMouseMove(MouseEventArgs& args)
{
    try
    {
        if (!MouseInClient())
        {
            SetMouseInClient();
            SetCursor();
            OnMouseEnter();
            TRACKMOUSEEVENT trackMouseEvent;
            trackMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
            trackMouseEvent.hwndTrack = handle;
            trackMouseEvent.dwFlags = TME_LEAVE;
            trackMouseEvent.dwHoverTime = HOVER_DEFAULT;
            TrackMouseEvent(&trackMouseEvent);
        }
        else
        {
            if (!IsDecoratorControl())
            {
                TranslateMousePos(args.location);
            }
            OnMouseMove(args);
        }
        if (mouseHoverMs != 0u && !MouseHoverTimerStarted())
        {
            Point pt = args.location;
            int dx = std::abs(mouseHoverLocation.X - pt.X);
            int dy = std::abs(mouseHoverLocation.Y - pt.Y);
            SetMouseHoverTimerStarted();
            mouseHoverLocation = pt;
            SetTimer(mouseHoverTimerId, mouseHoverMs);
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(handle, ex.what());
    }
}

void Control::DoMouseLeave()
{
    ResetMouseInClient();
    ResetLButtonPressed();
    OnMouseLeave();
}

void Control::DoSetFocus()
{
    if (IsWindow())
    {
        Window* thisWindow = static_cast<Window*>(this);
        if (thisWindow->IsMainWindow())
        {
            OnGotFocus();
            return;
        }
    }
    SetFocused();
    DoGotFocus();
    DoCreateAndShowCaret();
}

void Control::DoGotFocus()
{
    OnGotFocus();
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        ControlEventArgs args(this);
        parentControl->OnChildGotFocus(args);
    }
}

void Control::DoKillFocus()
{
    ResetFocused();
    DoLostFocus();
    DoDestroyCaret();
}

void Control::DoLostFocus()
{
    OnLostFocus();
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        ControlEventArgs args(this);
        parentControl->OnChildLostFocus(args);
    }
}

void Control::DoTimer(int timerId)
{
    TimerEventArgs timerEventArgs(timerId);
    OnTimer(timerEventArgs);
}

void Control::DoMouseDown(MouseEventArgs& args)
{
    if (!IsDecoratorControl())
    {
        TranslateMousePos(args.location);
    }
    OnMouseDown(args);
    if ((args.buttons & MouseButtons::lbutton) != MouseButtons::none)
    {
        SetLButtonPressed();
    }
}

void Control::DoMouseUp(MouseEventArgs& args)
{
    if (!IsDecoratorControl())
    {
        TranslateMousePos(args.location);
    }
    OnMouseUp(args);
    if ((args.buttons & MouseButtons::lbutton) != MouseButtons::none)
    {
        if (LButtonPressed())
        {
            ResetLButtonPressed();
            OnClick();
        }
    }
    Window* mainWindow = Application::MainWindow();
    if (mainWindow)
    {
        mainWindow->MouseUpNotificationInternal(args);
    }
}

void Control::DoMouseDoubleClick(MouseEventArgs& args)
{
    if (!IsDecoratorControl())
    {
        TranslateMousePos(args.location);
    }
    OnMouseDoubleClick(args);
}

void Control::DoMouseHover()
{
    ResetMouseHoverTimerStarted();
    KillTimer(mouseHoverTimerId);
    Point pt = GetCursorPos();
    pt = ScreenToClient(pt);
    if (!IsDecoratorControl())
    {
        TranslateMousePos(pt);
    }
    int dx = std::abs(mouseHoverLocation.X - pt.X);
    int dy = std::abs(mouseHoverLocation.Y - pt.Y);
    if (dx <= mouseHoverRectSize.Width && dy <= mouseHoverRectSize.Height)
    {
        MouseEventArgs args(pt, MouseButtons::none, 0);
        args.location = pt;
        OnMouseHover(args);
    }
}

void Control::DoVScroll(int value)
{
    IntArgs args(value);
    OnVScroll(args);
}

void Control::DoHScroll(int value)
{
    IntArgs args(value);
    OnHScroll(args);
}

void Control::DoMouseWheel(MouseWheelEventArgs& args)
{
    OnMouseWheel(args);
}

Point Control::GetCursorPos()
{
    POINT pt;
    bool succeeded = ::GetCursorPos(&pt);
    if (!succeeded)
    {
        throw WindowsException(GetLastError());
    }
    return Point(pt.x, pt.y);
}

Point Control::ScreenToClient(const Point& pt)
{
    POINT p;
    p.x = pt.X;
    p.y = pt.Y;
    bool succeeded = ::ScreenToClient(handle, &p);
    if (!succeeded)
    {
        throw WindowsException(GetLastError());
    }
    return Point(p.x, p.y);
}

Point Control::ClientToScreen(const Point& pt)
{
    POINT p;
    p.x = pt.X;
    p.y = pt.Y;
    bool succeeded = ::ClientToScreen(handle, &p);
    if (!succeeded)
    {
        throw WindowsException(GetLastError());
    }
    return Point(p.x, p.y);
}

Point Control::GetCaretPos() const
{
    POINT cp;
    bool succeeded = ::GetCaretPos(&cp);
    if (!succeeded)
    {
        throw WindowsException(GetLastError());
    }
    return Point(cp.x, cp.y);
}

void Control::SetCaretPos(const Point& caretPos)
{
    if (CaretCreated())
    {
        bool succeeded = ::SetCaretPos(caretPos.X, caretPos.Y);
        if (!succeeded)
        {
            throw WindowsException(GetLastError());
        }
    }
}

void Control::BringToFront()
{
    BringWindowToTop(handle);
}

Control* Control::TopControl() const
{
    HWND topHandle = GetTopWindow(handle);
    if (topHandle)
    {
        return Application::GetWindowManager().GetWindow(topHandle);
    }
    else
    {
        return nullptr;
    }
}

void Control::DoCreateAndShowCaret()
{
    if (Focused())
    {
        CreateCaret();
        SetCaretCreated();
        SetCaretLocation();
        ShowCaret();
    }
}

void Control::DoDestroyCaret()
{
    if (CaretCreated())
    {
        HideCaret();
        ResetCaretCreated();
        DestroyCaret();
    }
}

void Control::DestroyCaret()
{
    ::DestroyCaret();
    caretShowCount = 0;
}

bool Control::DoSysCommand(WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
        case SC_KEYMENU:
        {
            char16_t accessKey = static_cast<char16_t>(ToUpper(static_cast<char32_t>(static_cast<char16_t>(lParam))));
            if (DoMenu(accessKey, Keys::none))
            {
                return true;
            }
            else
            {
                Keys modifiers = Application::GetKeyboardModifiers();
                modifiers = modifiers | Keys::altModifier;
                Application::SetKeyboardModifiers(modifiers);
                if (DoKeyDown(static_cast<int>(accessKey)))
                {
                    Keys modifiers = Application::GetKeyboardModifiers();
                    modifiers = modifiers & ~Keys::altModifier;
                    Application::SetKeyboardModifiers(modifiers);
                    return true;
                }
                else
                {
                    Keys modifiers = Application::GetKeyboardModifiers();
                    modifiers = modifiers & ~Keys::altModifier;
                    Application::SetKeyboardModifiers(modifiers);
                }
            }
            break;
        }
        case SC_CLOSE:
        {
            KeyEventArgs args(Keys::f4, Keys::altModifier);
            DoMenu(args);
            if (args.handled)
            {
                return true;
            }
            break;
        }
    }
    return false;
}

bool Control::DoMenu(char16_t accessKey, Keys keyCode)
{
    ResetKeyDownHandled();
    Window* window = GetWindow();
    if (window)
    {
        MenuBar* menuBar = window->GetMenuBar();
        if (menuBar)
        {
            bool menuWantsKeys = false;
            bool handled = menuBar->HandleAccessKey(accessKey, keyCode, menuWantsKeys);
            if (handled)
            {
                SetKeyDownHandled();
            }
            if (menuWantsKeys)
            {
                SetMenuWantsKeys();
            }
            else
            {
                ResetMenuWantsKeys();
            }
            return handled;
        }
    }
    return false;
}

void Control::DoMenu(KeyEventArgs& args)
{
    Window* window = GetWindow();
    if (window)
    {
        MenuBar* menuBar = window->GetMenuBar();
        if (menuBar)
        {
            menuBar->DoKeyDown(args);
        }
    }
}

bool Control::DoKeyDown(int virtualKeyCode)
{
    ResetKeyDownHandled();
    Keys keyCode = static_cast<Keys>(virtualKeyCode);
    if (MenuWantsKeys())
    {
        if (DoMenu(char16_t(), keyCode))
        {
            SetKeyDownHandled();
            return true;
        }
    }
    switch (keyCode)
    {
        case Keys::controlKey:
        {
            Keys modifiers = Application::GetKeyboardModifiers();
            modifiers = modifiers | Keys::controlModifier;
            Application::SetKeyboardModifiers(modifiers);
            break;
        }
        case Keys::shiftKey:
        {
            Keys modifiers = Application::GetKeyboardModifiers();
            modifiers = modifiers | Keys::shiftModifier;
            Application::SetKeyboardModifiers(modifiers);
            break;
        }
        case Keys::menu:
        {
            Keys modifiers = Application::GetKeyboardModifiers();
            modifiers = modifiers | Keys::altModifier;
            Application::SetKeyboardModifiers(modifiers);
            break;
        }
    }
    KeyEventArgs args(keyCode, Application::GetKeyboardModifiers());
    DoMenu(args);
    if (args.handled)
    {
        SetKeyDownHandled();
        return true;
    }
    DispatchKeyDown(args);
    if (args.handled)
    {
        SetKeyDownHandled();
        return true;
    }
    return false;
}

bool Control::DoKeyUp(int virtualKeyCode)
{
    Keys keyCode = static_cast<Keys>(virtualKeyCode);
    switch (keyCode)
    {
        case Keys::controlKey:
        {
            Keys modifiers = Application::GetKeyboardModifiers();
            modifiers = modifiers & ~Keys::controlModifier;
            Application::SetKeyboardModifiers(modifiers);
            break;
        }
        case Keys::shiftKey:
        {
            Keys modifiers = Application::GetKeyboardModifiers();
            modifiers = modifiers & ~Keys::shiftModifier;
            Application::SetKeyboardModifiers(modifiers);
            break;
        }
        case Keys::menu:
        {
            Keys modifiers = Application::GetKeyboardModifiers();
            modifiers = modifiers & ~Keys::altModifier;
            Application::SetKeyboardModifiers(modifiers);
            break;
        }
    }
    if (KeyDownHandled() || MenuWantsKeys())
    {
        return KeyDownHandled();
    }
    Keys modifiers = Keys::none;
    switch (keyCode)
    {
        case Keys::controlKey:
        {
            modifiers = modifiers | Keys::controlModifier;
            break;
        }
        case Keys::shiftKey:
        {
            modifiers = modifiers | Keys::shiftModifier;
            break;
        }
        case Keys::menu:
        {
            modifiers = modifiers | Keys::altModifier;
            break;
        }
    }
    KeyEventArgs args(keyCode, modifiers);
    DispatchKeyUp(args);
    if (args.handled)
    {
        return true;
    }
    return false;
}

void Control::DoKeyPress(KeyPressEventArgs& args)
{
    if (KeyDownHandled() || MenuWantsKeys())
    {
        args.handled = KeyDownHandled();
        return;
    }
    DispatchKeyPress(args);
}

void Control::DispatchKeyDown(KeyEventArgs& args)
{
    OnKeyDown(args);
    if (!args.handled)
    {
        Control* parent = ParentControl();
        if (parent)
        {
            parent->DispatchKeyDown(args);
        }
    }
}

void Control::DispatchKeyUp(KeyEventArgs& args)
{
    OnKeyUp(args);
    if (!args.handled)
    {
        Control* parent = ParentControl();
        if (parent)
        {
            parent->DispatchKeyUp(args);
        }
    }
}

void Control::DispatchKeyPress(KeyPressEventArgs& args)
{
    OnKeyPress(args);
    if (!args.handled)
    {
        Control* parent = ParentControl();
        if (parent)
        {
            parent->DispatchKeyPress(args);
        }
    }
}

void Control::OnPaint(PaintEventArgs& args)
{
    paint.Fire(args);
}

void Control::OnClick()
{
    click.Fire();
}

void Control::OnCreated()
{
    created.Fire();
}

void Control::OnShown()
{
    shown.Fire();
}

void Control::OnGotFocus()
{
    gotFocus.Fire();
}

void Control::OnChildGotFocus(ControlEventArgs& args)
{
    childGotFocus.Fire(args);
}

void Control::OnLostFocus()
{
    lostFocus.Fire();
}

void Control::OnChildLostFocus(ControlEventArgs& args)
{
    childLostFocus.Fire(args);
}

void Control::OnTextChanged()
{
    textChanged.Fire();
}

void Control::OnTimer(TimerEventArgs& args)
{
    timer.Fire(args);
}

void Control::OnVisibleChanged()
{
    visibleChanged.Fire();
}

void Control::OnEnabledChanged()
{
    enabledChanged.Fire();
}

void Control::OnLocationChanged()
{
    locationChanged.Fire();
}

void Control::OnSizeChanged()
{
    sizeChanged.Fire();
}

void Control::OnContentChanged()
{
    contentChanged.Fire();
}

void Control::OnChildContentChanged(ControlEventArgs& args)
{
    childContentChanged.Fire(args);
}

void Control::OnContentLocationChanged()
{
    contentLocationChanged.Fire();
}

void Control::OnChildContentLocationChanged(ControlEventArgs& args)
{
    childContentLocationChanged.Fire(args);
}

void Control::OnContentSizeChanged()
{
    contentSizeChanged.Fire();
}

void Control::OnChildContentSizeChanged(ControlEventArgs& args)
{
    childContentSizeChanged.Fire(args);
}

void Control::OnMouseEnter()
{
    mouseEnter.Fire();
}

void Control::OnMouseLeave()
{
    mouseLeave.Fire();
}

void Control::OnMouseDown(MouseEventArgs& args)
{
    mouseDown.Fire(args);
    if (!IsMenuItem() && !IsMenuControl())
    {
        Window* window = GetWindow();
        if (window)
        {
            MenuBar* menuBar = window->GetMenuBar();
            if (menuBar)
            {
                menuBar->CloseMenu();
            }
        }
    }
}

void Control::OnMouseUp(MouseEventArgs& args)
{
    mouseUp.Fire(args);
}

void Control::OnMouseMove(MouseEventArgs& args)
{
    mouseMove.Fire(args);
}

void Control::OnMouseHover(MouseEventArgs& args)
{
    mouseHover.Fire(args);
}

void Control::OnMouseDoubleClick(MouseEventArgs& args)
{
    mouseDoubleClick.Fire(args);
}

void Control::OnRightClick(RightClickEventArgs& args)
{
    rightClick.Fire(args);
}

void Control::OnHScroll(IntArgs& args)
{
    hscroll.Fire(args);
}

void Control::OnVScroll(IntArgs& args)
{
    vscroll.Fire(args);
}

void Control::OnMouseWheel(MouseWheelEventArgs& args)
{
    mouseWheel.Fire(args);
}

void Control::OnKeyDown(KeyEventArgs& args)
{
    keyDown.Fire(args);
}

void Control::OnKeyUp(KeyEventArgs& args)
{
    keyUp.Fire(args);
}

void Control::OnKeyPress(KeyPressEventArgs& args)
{
    keyPress.Fire(args);
}

void Control::SetCaretLocation()
{
    SetCaretPos(Point(0, 0));
}

void Control::SetCursor()
{
    cmajor::wing::SetCursor(arrowCursor);
}

HWND LParamHandle(Message& msg)
{
    return reinterpret_cast<HWND>(msg.lParam);
}

} } // cmajor::wing
