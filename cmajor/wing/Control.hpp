// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_CONTROL_INCLUDED
#define CMAJOR_WING_CONTROL_INCLUDED
#include <cmajor/wing/Component.hpp>
#include <cmajor/wing/Cursor.hpp>
#include <cmajor/wing/Event.hpp>
#include <cmajor/wing/Graphics.hpp>
#include <cmajor/wing/Keys.hpp>
#include <cmajor/wing/Wing.hpp>
#include <memory>
#include <string>
#include <vector>

namespace cmajor { namespace wing {

class Control;
class Buffer;
class Window;

struct WING_API ControlEventArgs
{
    ControlEventArgs(Control* control_) : control(control_) {}
    Control* control;
};

using ClickEvent = Event;
using CreatedEvent = Event;
using DestroyedEvent = Event;
using ClipboardUpdateEvent = Event;
using ShownEvent = Event;
using GotFocusEvent = Event;
using ChildGotFocusEvent = EventWithArgs<ControlEventArgs>;
using LostFocusEvent = Event;
using ChildLostFocusEvent = EventWithArgs<ControlEventArgs>;
using TextChangedEvent = Event;
using VisibleChangedEvent = Event;
using EnabledChangedEvent = Event;
using LocationChangedEvent = Event;
using SizeChangedEvent = Event;
using ContentChangedEvent = Event;
using ChildContentChangedEvent = EventWithArgs<ControlEventArgs>;
using ContentLocationChangedEvent = Event;
using ChildContentLocationChangedEvent = EventWithArgs<ControlEventArgs>;
using ContentSizeChangedEvent = Event;
using ChildContentSizeChangedEvent = EventWithArgs<ControlEventArgs>; 

WING_API inline int DoubleClickWindowClassStyle()
{
    return CS_DBLCLKS;
}

WING_API inline int DefaultChildWindowStyle()
{
    return WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
}

WING_API inline int HiddenChildWindowStyle()
{
    return WS_CHILD | WS_CLIPSIBLINGS;
}

WING_API inline int DialogWindowStyle()
{
    return WS_CAPTION | WS_DLGFRAME | WS_CLIPSIBLINGS;
}

WING_API inline int OverlappedWindowStyle()
{
    return WS_OVERLAPPEDWINDOW;
}

WING_API inline Point DefaultLocation()
{
    return Point(CW_USEDEFAULT, CW_USEDEFAULT);
}

WING_API inline Size DefaultSize()
{
    return Size(CW_USEDEFAULT, CW_USEDEFAULT);
}

WING_API inline int DefaultMouseHoverMs()
{
    return 300;
}

WING_API inline Size DefaultMouseHoverSize()
{
    return Size(8, 8);
}

const int mouseHoverTimerId = 2;

struct WING_API PaintEventArgs
{
    PaintEventArgs(Graphics& graphics_, Rect& clipRect_) : graphics(graphics_), clipRect(clipRect_) {}
    Graphics& graphics;
    Rect clipRect;
};

using PaintEvent = EventWithArgs<PaintEventArgs>;

WING_API inline int64_t DefaultControlWindowClassBackgroundColor()
{
    return COLOR_MENU;
}

WING_API inline Color DefaultControlBackgroundColor()
{
    return GetSystemColor(DefaultControlWindowClassBackgroundColor());
}

WING_API Color DefaultBitmapTransparentColor();

enum class Anchors : int
{
    none = 0, top = 1 << 0, bottom = 1 << 1, left = 1 << 2, right = 1 << 3
};

WING_API inline Anchors operator|(Anchors left, Anchors right)
{
    return Anchors(int(left) | int(right));
}

WING_API inline Anchors operator&(Anchors left, Anchors right)
{
    return Anchors(int(left) & int(right));
}

WING_API inline Anchors operator~(Anchors anchors)
{
    return Anchors(~int(anchors));
}

enum class Dock : int
{
    none = 0, top = 1, bottom = 2, left = 3, right = 4, fill = 5
};

struct WING_API ScrollUnits
{
    ScrollUnits() : vertical(0), horizontal(0) {}
    ScrollUnits(int vertical_, int horizontal_) : vertical(vertical_), horizontal(horizontal_) {}
    int vertical;
    int horizontal;
};

enum class MouseButtons : int
{
    none = 0, lbutton = 1, rbutton = 2, shift = 4, control = 8, mbutton = 16, xbutton1 = 32, xbutton2 = 64
};

WING_API inline MouseButtons operator|(MouseButtons left, MouseButtons right)
{
    return MouseButtons(int(left) | int(right));
}

WING_API inline MouseButtons operator&(MouseButtons left, MouseButtons right)
{
    return MouseButtons(int(left) & int(right));
}

WING_API inline MouseButtons operator~(MouseButtons buttons)
{
    return MouseButtons(~int(buttons));
}

struct WING_API MouseEventArgs
{
    MouseEventArgs(const Point& location_, MouseButtons buttons_, int clicks_) : location(location_), buttons(buttons_), clicks(clicks_) {}
    Point location;
    MouseButtons buttons;
    int clicks;
};

struct WING_API RightClickEventArgs
{
    RightClickEventArgs(Control* control_, const Point location_) : control(control_), location(location_) {}
    Control* control;
    Point location;
};

using MouseEnterEvent = Event;
using MouseLeaveEvent = Event;
using MouseDownEvent = EventWithArgs<MouseEventArgs>;
using MouseUpEvent = EventWithArgs<MouseEventArgs>;
using MouseMoveEvent = EventWithArgs<MouseEventArgs>;
using MouseHoverEvent = EventWithArgs<MouseEventArgs>;
using MouseDoubleClickEvent = EventWithArgs<MouseEventArgs>;
using RightClickEvent = EventWithArgs<RightClickEventArgs>;
using ControlAddedEvent = EventWithArgs<ControlEventArgs>;
using ControlRemovedEvent = EventWithArgs<ControlEventArgs>;

struct WING_API TimerEventArgs
{
    TimerEventArgs(int timerId_) : timerId(timerId_) {}
    int timerId;
};

using TimerEvent = EventWithArgs<TimerEventArgs>;

struct WING_API KeyEventArgs
{
    KeyEventArgs(Keys key_) : key(key_), handled(false), suppressKeyPress(false) {}
    Keys key;
    bool handled;
    bool suppressKeyPress;
};

using KeyDownEvent = EventWithArgs<KeyEventArgs>;
using KeyUpEvent = EventWithArgs<KeyEventArgs>;

struct WING_API KeyPressEventArgs
{
    KeyPressEventArgs(char16_t keyChar_) : keyChar(keyChar_), handled(false) {}
    char16_t keyChar;
    bool handled;
};

using KeyPressEvent = EventWithArgs<KeyPressEventArgs>;

struct WING_API IntArgs
{
    IntArgs(int value_) : value(value_) {}
    int value;
};

using HScrollEvent = EventWithArgs<IntArgs>;
using VScrollEvent = EventWithArgs<IntArgs>;

struct WING_API MouseWheelEventArgs
{
    MouseWheelEventArgs(int value_) : value(value_), handled(false) {}
    int value;
    bool handled;
};

using MouseWheelEvent = EventWithArgs<MouseWheelEventArgs>;

struct WING_API ControlCreateParams
{
    ControlCreateParams();
    ControlCreateParams& Defaults();
    ControlCreateParams& WindowClassName(const std::string& windowClassName_);
    ControlCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    ControlCreateParams& WindowStyle(int windowStyle_);
    ControlCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    ControlCreateParams& BackgroundColor(const Color& backgroundColor_);
    ControlCreateParams& Text(const std::string& text_);
    ControlCreateParams& Location(Point location_);
    ControlCreateParams& SetSize(Size size_);
    ControlCreateParams& SetAnchors(Anchors anchors_);
    ControlCreateParams& SetDock(Dock dock_);
    std::string windowClassName;
    uint32_t windowClassStyle;
    int windowStyle;
    int64_t windowClassBackgroundColor;
    Color backgroundColor;
    std::string text;
    Point location;
    Size size;
    Anchors anchors;
    Dock dock;
};

class ContainerControl;

enum class ControlFlags : int
{
    none = 0,
    hidden = 1 << 0,
    focused = 1 << 1,
    caretCreated = 1 << 2,
    caretShown = 1 << 3,
    caretDisabled = 1 << 4,
    disabled = 1 << 5,
    tabStop = 1 << 6,
    doubleBuffered = 1 << 7,
    mouseInClient = 1 << 8,
    lbuttonPressed = 1 << 9,
    mouseHoverTimerStarted = 1 << 10,
    keyDownHandled = 1 << 11,
    menuWantsKeys = 1 << 12
};

WING_API inline ControlFlags operator&(ControlFlags left, ControlFlags right)
{
    return ControlFlags(int(left) & int(right));
}

WING_API inline ControlFlags operator|(ControlFlags left, ControlFlags right)
{
    return ControlFlags(int(left) | int(right));
}

WING_API inline ControlFlags operator~(ControlFlags flags)
{
    return ControlFlags(~int(flags));
}

class WING_API Control : public Component
{
public:
    Control(ControlCreateParams& createParams_);
    ~Control();
    bool IsControl() const override { return true; }
    virtual bool IsContainerControl() const { return false; }
    virtual bool IsWindow() const { return false; }
    virtual bool IsButton() const { return false; }
    virtual bool IsMenuBar() const { return false; }
    virtual bool IsMenuBox() const { return false; }
    virtual bool IsTabControl() const { return false; }
    virtual bool IsGroupBox() const { return false; }
    virtual bool IsRadioButton() const { return false; }
    virtual bool IsTextBox() const { return false; }
    virtual bool IsListBox() const { return false; }
    virtual ContainerControl* GetContainerControl() const;
    Window* GetWindow() const;
    void AddChildVisual(Control* child);
    int WindowStyle() const { return windowStyle; }
    void SetWindowStyle(int windowStyle_) { windowStyle = windowStyle_; }
    void SetParentWindow(Control* parent);
    Control* ParentControl() const;
    void Create();
    void Show();
    void Hide();
    void Enable();
    void Disable();
    void Update();
    void DockWindow();
    void DockWindow(Rect& parentRect);
    void MoveWindow(int dx, int dy);
    void MoveWindow(const Point& loc, const Size& sz, bool repaint);
    virtual void PaintAll(PaintEventArgs& args, bool skipMenuBar);
    PaintEvent& Paint() { return paint; }
    ClickEvent& Click() { return click; }
    CreatedEvent& Created() { return created; }
    DestroyedEvent& Destroyed() { return destroyed; }
    ShownEvent& Shown() { return shown; }
    GotFocusEvent& GotFocus() { return gotFocus; }
    LostFocusEvent& LostFocus() { return lostFocus; }
    VisibleChangedEvent& VisibleChanged() { return visibleChanged; }
    EnabledChangedEvent& EnabledChanged() { return enabledChanged; }
    LocationChangedEvent& LocationChanged() { return locationChanged; }
    SizeChangedEvent& SizeChanged() { return sizeChanged; }
    ContentChangedEvent& ContentChanged() { return contentChanged; }
    ChildContentChangedEvent& ChildContentChanged() { return childContentChanged; }
    ContentLocationChangedEvent& ContentLocationChanged() { return contentLocationChanged; }
    ChildContentLocationChangedEvent& ChildContentLocationChanged() { return childContentLocationChanged; }
    ContentSizeChangedEvent& ContentSizeChanged() { return contentSizeChanged; }
    ChildContentSizeChangedEvent& ChildContentSizeChanged() { return childContentSizeChanged; }
    TextChangedEvent& TextChanged() { return textChanged; }
    MouseEnterEvent& MouseEnter() { return mouseEnter; }
    MouseLeaveEvent& MouseLeave() { return mouseLeave; }
    MouseDownEvent& MouseDown() { return mouseDown; }
    MouseUpEvent& MouseUp() { return mouseUp; }
    MouseMoveEvent& MouseMove() { return mouseMove; }
    MouseHoverEvent& MouseHover() { return mouseHover; }
    MouseDoubleClickEvent& MouseDoubleClick() { return mouseDoubleClick; }
    RightClickEvent& RightClick() { return rightClick; }
    HScrollEvent& HScroll() { return hscroll; }
    VScrollEvent& VScroll() { return vscroll; }
    MouseWheelEvent& MouseWheel() { return mouseWheel; }
    ControlAddedEvent& ControlAdded() { return controlAdded; }
    ControlRemovedEvent& ControlRemoved() { return controlRemoved; }
    KeyDownEvent& KeyDown() { return keyDown; }
    KeyUpEvent& KeyUp() { return keyUp; }
    KeyPressEvent& KeyPress() { return keyPress; }
    const Color& BackgroundColor() const { return backgroundColor; }
    const Point& Location() const { return location; }
    void SetLocation(const Point& newLocation);
    void SetLocationInternal(const Point& newLocation);
    const Size& GetSize() const { return size; }
    void SetSize(const Size& newSize);
    void SetSizeInternal(const Size& newSize);
    const Point& ContentLocation() const { return contentLocation; }
    virtual void SetContentLocationInternal(const Point& newContentLocation);
    void SetContentLocation(const Point& newContentLocation);
    const Size& ContentSize() const { return contentSize; }
    void SetContentSizeInternal(const Size& newContentSize);
    void SetContentSize(const Size& newContentSize);
    Anchors GetAnchors() const { return anchors; }
    Dock GetDock() const { return dock; }
    const ScrollUnits& GetScrollUnits() const { return scrollUnits; }
    void SetScrollUnits(const ScrollUnits& scrollUnits_) { scrollUnits = scrollUnits_; }
    Point GetCursorPos();
    Point ScreenToClient(const Point& pt);
    Point ClientToScreen(const Point& pt);
    Point GetCaretPos() const;
    void SetCaretPos(const Point& caretPos);
    HWND Handle() const { return handle; }
    void BringToFront();
    Control* TopControl() const;
    const Font& GetFont() const;
    bool HasFont() const { return !font.IsNull(); }
    void SetFont(Font& font_);
    void SetFont(Font&& font_);
    const FontHandle& GetFontHandle(Graphics& graphics);
    void ShowCaret();
    void HideCaret();
    void Invalidate();
    void Invalidate(bool eraseBackground);
    void Invalidate(const Rect& rect);
    void Invalidate(const Rect& rect, bool eraseBackground);
    void SetFocus();
    const std::string& Text() const { return text; }
    void SetText(const std::string& text_);
    void SetTextInternal(const std::string& text_);
    std::string DoGetWindowText();
    bool GetFlag(ControlFlags flag) const { return (flags & flag) != ControlFlags::none; }
    void SetFlag(ControlFlags flag) { flags = flags | flag; }
    void ResetFlag(ControlFlags flag) { flags = flags & ~flag; }
    bool Hidden() const { return GetFlag(ControlFlags::hidden); }
    void SetHidden() { SetFlag(ControlFlags::hidden); }
    void ResetHidden() { ResetFlag(ControlFlags::hidden); }
    bool IsVisible() const { return !Hidden(); }
    bool IsDoubleBuffered() const { return GetFlag(ControlFlags::doubleBuffered); }
    void SetDoubleBuffered() { SetFlag(ControlFlags::doubleBuffered); }
    void ResetDoubleBuffered() { ResetFlag(ControlFlags::doubleBuffered); }
    bool Focused() const { return GetFlag(ControlFlags::focused); }
    void SetFocused() { SetFlag(ControlFlags::focused); }
    void ResetFocused() { ResetFlag(ControlFlags::focused); }
    bool CaretCreated() { return GetFlag(ControlFlags::caretCreated); }
    void SetCaretCreated() { SetFlag(ControlFlags::caretCreated); }
    void ResetCaretCreated() { ResetFlag(ControlFlags::caretCreated); }
    bool CaretShown() { return GetFlag(ControlFlags::caretShown);  }
    void SetCaretShown() { SetFlag(ControlFlags::caretShown); }
    void ResetCaretShown() { ResetFlag(ControlFlags::caretShown); }
    bool CaretDisabled() { return GetFlag(ControlFlags::caretDisabled); }
    void SetCaretDisabled() { SetFlag(ControlFlags::caretDisabled); }
    void ResetCaretDisabled() { ResetFlag(ControlFlags::caretDisabled); }
    bool IsEnabled() const { return !IsDisabled(); }
    bool IsDisabled() const { return GetFlag(ControlFlags::disabled); }
    void SetDisabled() { SetFlag(ControlFlags::disabled); }
    void ResetDisabled() { ResetFlag(ControlFlags::disabled); }
    bool TabStop() const { return GetFlag(ControlFlags::tabStop); }
    void SetTabStop() { SetFlag(ControlFlags::tabStop);  }
    void ResetTabStop() { ResetFlag(ControlFlags::tabStop); }
    bool MouseInClient() { return GetFlag(ControlFlags::mouseInClient); }
    void SetMouseInClient() { SetFlag(ControlFlags::mouseInClient); }
    void ResetMouseInClient() { ResetFlag(ControlFlags::mouseInClient); }
    bool LButtonPressed() const { return GetFlag(ControlFlags::lbuttonPressed); }
    void SetLButtonPressed() { SetFlag(ControlFlags::lbuttonPressed); }
    void ResetLButtonPressed() { ResetFlag(ControlFlags::lbuttonPressed); }
    bool MouseHoverTimerStarted() const { return GetFlag(ControlFlags::mouseHoverTimerStarted); }
    void SetMouseHoverTimerStarted() { SetFlag(ControlFlags::mouseHoverTimerStarted); }
    void ResetMouseHoverTimerStarted() { ResetFlag(ControlFlags::mouseHoverTimerStarted); }
    bool KeyDownHandled() const { return GetFlag(ControlFlags::keyDownHandled); }
    void SetKeyDownHandled() { SetFlag(ControlFlags::keyDownHandled); }
    void ResetKeyDownHandled() { ResetFlag(ControlFlags::keyDownHandled); }
    bool MenuWantsKeys() const { return GetFlag(ControlFlags::menuWantsKeys); }
    void SetMenuWantsKeys() { SetFlag(ControlFlags::menuWantsKeys); }
    void ResetMenuWantsKeys() { ResetFlag(ControlFlags::menuWantsKeys); }
    void SetTimer(int timerId, int durationMs);
    void KillTimer(int timerId);
    int MouseHoverMs() const { return mouseHoverMs; }
    void SetMouseHoverMs(int mouseHoverMs_) { mouseHoverMs = mouseHoverMs_; }
    bool ProcessMessageInternal(Message& msg) { return ProcessMessage(msg); }
    void SetContentChanged();
    void FireChildGotFocus(ControlEventArgs& args) { OnChildGotFocus(args); }
    void FireChildLostFocus(ControlEventArgs& args) { OnChildLostFocus(args); }
    void FireChildContentChanged(ControlEventArgs& args) { OnChildContentChanged(args); }
    void FireChildContentLocationChanged(ControlEventArgs& args) { OnChildContentLocationChanged(args); }
    void FireChildContentSizeChanged(ControlEventArgs& args) { OnChildContentSizeChanged(args); }
    virtual void ScrollLineDown();
    virtual void ScrollLineUp();
    virtual Control* GetFirstEnabledTabStopControl() const;
    virtual Control* GetLastEnabledTabStopControl() const;
    void TranslateContentLocationInternal(Point& location) { TranslateContentLocation(location); }
    void TranslateMousePosInternal(Point& location) { TranslateMousePos(location); }
protected:
    virtual bool IsDecoratorControl() const { return false; }
    virtual void TranslateChildGraphics(Graphics& graphics);
    virtual void TranslateMousePos(Point& location);
    virtual void TranslateContentLocation(Point& location);
    virtual void CreateCaret();
    virtual bool ProcessMessage(Message& msg);
    virtual void OnPaint(PaintEventArgs& args);
    virtual void OnClick();
    virtual void OnCreated();
    virtual void OnDestroyed();
    virtual void OnClipboardUpdate();
    virtual void OnShown();
    virtual void OnGotFocus();
    virtual void OnChildGotFocus(ControlEventArgs& args);
    virtual void OnLostFocus();
    virtual void OnChildLostFocus(ControlEventArgs& args);
    virtual void OnTextChanged();
    virtual void OnTimer(TimerEventArgs& args);
    virtual void OnVisibleChanged();
    virtual void OnEnabledChanged();
    virtual void OnLocationChanged();
    virtual void OnSizeChanged();
    virtual void OnContentChanged();
    virtual void OnChildContentChanged(ControlEventArgs& args);
    virtual void OnContentLocationChanged();
    virtual void OnChildContentLocationChanged(ControlEventArgs& args);
    virtual void OnContentSizeChanged();
    virtual void OnChildContentSizeChanged(ControlEventArgs& args);
    virtual void OnMouseEnter();
    virtual void OnMouseLeave();
    virtual void OnMouseDown(MouseEventArgs& args);
    virtual void OnMouseUp(MouseEventArgs& args);
    virtual void OnMouseMove(MouseEventArgs& args);
    virtual void OnMouseHover(MouseEventArgs& args);
    virtual void OnMouseDoubleClick(MouseEventArgs& args);
    virtual void OnRightClick(RightClickEventArgs& args);
    virtual void OnHScroll(IntArgs& args);
    virtual void OnVScroll(IntArgs& args);
    virtual void OnMouseWheel(MouseWheelEventArgs& args);
    virtual void OnKeyDown(KeyEventArgs& args);
    virtual void OnKeyUp(KeyEventArgs& args);
    virtual void OnKeyPress(KeyPressEventArgs& args);
    virtual void OnControlAdded(ControlEventArgs& args);
    virtual void OnControlRemoved(ControlEventArgs& args);
    virtual void SetCaretLocation();
    virtual void SetCursor();
    void SubClassCommandWndProc();
private:
    void DoPaint();
    void DoMouseMove(MouseEventArgs& args);
    void DoMouseLeave();
    void DoSetFocus();
    void DoGotFocus();
    void DoKillFocus();
    void DoLostFocus();
    void DoTimer(int timerId);
    void DoDestroy();
    void DoClipboardUpdate();
    void DoMouseDown(MouseEventArgs& args);
    void DoMouseUp(MouseEventArgs& args);
    void DoMouseDoubleClick(MouseEventArgs& args);
    void DoMouseHover();
    void DoVScroll(int value);
    void DoHScroll(int value);
    void DoMouseWheel(MouseWheelEventArgs& args);
    void DoCreateAndShowCaret();
    void DoDestroyCaret();
    void DestroyCaret();
    bool DoSysCommand(WPARAM wParam, LPARAM lParam);
    bool DoMenu(char16_t accessKey, Keys keyCode);
    void DoMenu(KeyEventArgs& args);
    bool DoKeyDown(int virtualKeyCode);
    bool DoKeyUp(int virtualKeyCode);
    void DoKeyPress(KeyPressEventArgs& args);
    void DispatchKeyDown(KeyEventArgs& args);
    void DispatchKeyUp(KeyEventArgs& args);
    void DispatchKeyPress(KeyPressEventArgs& args);
    std::string windowClassName;
    uint32_t windowClassStyle;
    int64_t windowClassBackgroundColor;
    int windowStyle;
    Color backgroundColor;
    Point location;
    Size size;
    Point contentLocation;
    Size contentSize;
    Anchors anchors;
    Dock dock;
    std::string text;
    HWND handle;
    Font font;
    FontHandle fontHandle;
    void* originalWndProc;
    ControlFlags flags;
    int caretShowCount;
    std::unique_ptr<Buffer> buffer;
    ClickEvent click;
    CreatedEvent created;
    DestroyedEvent destroyed;
    ClipboardUpdateEvent clipboardUpdate;
    ShownEvent shown;
    GotFocusEvent gotFocus;
    ChildGotFocusEvent childGotFocus;
    LostFocusEvent lostFocus;
    ChildLostFocusEvent childLostFocus;
    TimerEvent timer;
    VisibleChangedEvent visibleChanged;
    EnabledChangedEvent enabledChanged;
    LocationChangedEvent locationChanged;
    SizeChangedEvent sizeChanged;
    ContentChangedEvent contentChanged;
    ChildContentChangedEvent childContentChanged;
    ContentLocationChangedEvent contentLocationChanged;
    ChildContentLocationChangedEvent childContentLocationChanged;
    ContentSizeChangedEvent contentSizeChanged;
    ChildContentSizeChangedEvent childContentSizeChanged;
    TextChangedEvent textChanged;
    PaintEvent paint;
    MouseEnterEvent mouseEnter;
    MouseLeaveEvent mouseLeave;
    MouseDownEvent mouseDown;
    MouseUpEvent mouseUp;
    MouseMoveEvent mouseMove;
    MouseHoverEvent mouseHover;
    MouseDoubleClickEvent mouseDoubleClick;
    RightClickEvent rightClick;
    HScrollEvent hscroll;
    VScrollEvent vscroll;
    MouseWheelEvent mouseWheel;
    ControlAddedEvent controlAdded;
    ControlRemovedEvent controlRemoved;
    KeyDownEvent keyDown;
    KeyUpEvent keyUp;
    KeyPressEvent keyPress;
    std::vector<Control*> createList;
    Cursor arrowCursor;
    int mouseHoverMs;
    Point mouseHoverLocation;
    Size mouseHoverRectSize;
    ScrollUnits scrollUnits;
};

WING_API HWND LParamHandle(Message& msg);

} } // cmajor::wing

#endif // CMAJOR_WING_CONTROL_INCLUDED
