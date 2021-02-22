// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/wing/Control.hpp>
#include <cmajor/wing/ContainerControl.hpp>
#include <cmajor/wing/Window.hpp>
#include <cmajor/wing/Application.hpp>
#include <cmajor/wing/Container.hpp>
#include <cmajor/wing/Buffer.hpp>
#include <soulng/util/Unicode.hpp>

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

ControlCreateParams& ControlCreateParams::WindowClassBackgroundColor(int64_t windowClassBackgroundColor_)
{
    windowClassBackgroundColor = windowClassBackgroundColor_;
    return *this;
}

ControlCreateParams& ControlCreateParams::BackgroundColor(Color backgroundColor_)
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
    caretShowCount(0)
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

void Control::SetSize(const Size& newSize)
{
    if (size != newSize)
    {
        if (handle)
        {
            MoveWindow(location, size, true);
        }
        SetSizeInternal(newSize);
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
    if (!font.IsNull())
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

void Control::TranslateChildGraphics(Graphics& graphics)
{
    Control* parentControl = ParentControl();
    if (parentControl)
    {
        parentControl->TranslateChildGraphics(graphics);
    }
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
        case WM_SHOWWINDOW:
        {
            if (msg.wParam == 1)
            {
                OnShown();
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

void Control::CreateCaret()
{
    ::CreateCaret(Handle(), nullptr, 1, 15);
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

void Control::SetCaretLocation()
{
    SetCaretPos(0, 0);
}

HWND LParamHandle(Message& msg)
{
    return reinterpret_cast<HWND>(msg.lParam);
}

} } // cmajor::wing
