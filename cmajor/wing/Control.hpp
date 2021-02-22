// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_CONTROL_INCLUDED
#define CMAJOR_WING_CONTROL_INCLUDED
#include <cmajor/wing/Component.hpp>
#include <cmajor/wing/Event.hpp>
#include <cmajor/wing/Graphics.hpp>
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
using ShownEvent = Event;
using GotFocusEvent = Event;
using ChildGotFocusEvent = EventWithArgs<ControlEventArgs>;
using LostFocusEvent = Event;
using ChildLostFocusEvent = EventWithArgs<ControlEventArgs>;
using VisibleChangedEvent = Event;
using EnabledChangedEvent = Event;
using LocationChangedEvent = Event;
using SizeChangedEvent = Event;

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

enum class Anchors : int
{
    none = 0, top = 1 << 0, bottom = 1 << 1, left = 1 << 2, right = 1 << 3
};

inline Anchors operator|(Anchors left, Anchors right)
{
    return Anchors(int(left) | int(right));
}

inline Anchors operator&(Anchors left, Anchors right)
{
    return Anchors(int(left) & int(right));
}

inline Anchors operator~(Anchors anchors)
{
    return Anchors(~int(anchors));
}

enum class Dock : int
{
    none = 0, top = 1, bottom = 2, left = 3, right = 4, fill = 5
};

struct WING_API ControlCreateParams
{
    ControlCreateParams();
    ControlCreateParams& Defaults();
    ControlCreateParams& WindowClassName(const std::string& windowClassName_);
    ControlCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    ControlCreateParams& WindowStyle(int windowStyle_);
    ControlCreateParams& WindowClassBackgroundColor(int64_t windowClassBackgroundColor_);
    ControlCreateParams& BackgroundColor(Color backgroundColor_);
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
    disabled = 1 << 4,
    tabStop = 1 << 5,
    doubleBuffered = 1 << 6
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
    PaintEvent& Paint() { return paint; }
    ClickEvent& Click() { return click; }
    CreatedEvent& Created() { return created; }
    ShownEvent& Shown() { return shown; }
    GotFocusEvent& GotFocus() { return gotFocus; }
    LostFocusEvent& LostFocus() { return lostFocus; }
    VisibleChangedEvent& VisibleChanged() { return visibleChanged; }
    EnabledChangedEvent& EnabledChanged() { return enabledChanged; }
    LocationChangedEvent& LocationChanged() { return locationChanged; }
    SizeChangedEvent& SizeChanged() { return sizeChanged; }
    const Color& BackgroundColor() const { return backgroundColor; }
    const Point& Location() const { return location; }
    void SetLocation(const Point& newLocation);
    const Size& GetSize() const { return size; }
    void SetSize(const Size& newSize);
    void SetSizeInternal(const Size& newSize);
    Anchors GetAnchors() const { return anchors; }
    Dock GetDock() const { return dock; }
    HWND Handle() const { return handle; }
    const Font& GetFont() const;
    void SetFont(Font& font_);
    void SetFont(Font&& font_);
    const FontHandle& GetFontHandle(Graphics& graphics);
    void ShowCaret();
    void HideCaret();
    void Invalidate();
    void Invalidate(bool eraseBackground);
    void Invalidate(const Rect& rect);
    void Invalidate(const Rect& rect, bool eraseBackground);
    bool GetFlag(ControlFlags flag) const { return (flags & flag) != ControlFlags::none; }
    void SetFlag(ControlFlags flag) { flags = flags | flag; }
    void ResetFlag(ControlFlags flag) { flags = flags & ~flag; }
    bool Hidden() const { return GetFlag(ControlFlags::hidden); }
    void SetHidden() { SetFlag(ControlFlags::hidden); }
    void ResetHidden() { ResetFlag(ControlFlags::hidden); }
    bool IsDoubleBuffered() const { return GetFlag(ControlFlags::doubleBuffered); }
    void SetDoubleBufferered() { SetFlag(ControlFlags::doubleBuffered); }
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
    bool Disabled() const { return GetFlag(ControlFlags::disabled); }
    void SetDisabled() { SetFlag(ControlFlags::disabled); }
    void ResetDisabled() { ResetFlag(ControlFlags::disabled); }
    bool TabStop() const { return GetFlag(ControlFlags::tabStop); }
    void SetTabStop() { SetFlag(ControlFlags::tabStop);  }
    void ResetTabStop() { ResetFlag(ControlFlags::tabStop); }
    bool ProcessMessageInternal(Message& msg) { return ProcessMessage(msg); }
protected:
    virtual bool IsDecoratorControl() const { return false; }
    virtual void TranslateChildGraphics(Graphics& graphics);
    virtual bool ProcessMessage(Message& msg);
    virtual void OnPaint(PaintEventArgs& args);
    virtual void OnClick();
    virtual void OnCreated();
    virtual void OnShown();
    virtual void OnGotFocus();
    virtual void OnChildGotFocus(ControlEventArgs& args);
    virtual void OnLostFocus();
    virtual void OnChildLostFocus(ControlEventArgs& args);
    virtual void OnVisibleChanged();
    virtual void OnEnabledChanged();
    virtual void OnLocationChanged();
    virtual void OnSizeChanged();
    virtual void SetCaretLocation();
private:
    void DoPaint();
    void DoSetFocus();
    void DoGotFocus();
    void DoKillFocus();
    void DoLostFocus();
    void DoCreateAndShowCaret();
    void DoDestroyCaret();
    void CreateCaret();
    void DestroyCaret();
    std::string windowClassName;
    uint32_t windowClassStyle;
    int64_t windowClassBackgroundColor;
    int windowStyle;
    Color backgroundColor;
    Point location;
    Size size;
    Anchors anchors;
    Dock dock;
    std::string text;
    HWND handle;
    Font font;
    FontHandle fontHandle;
    ControlFlags flags;
    int caretShowCount;
    std::unique_ptr<Buffer> buffer;
    ClickEvent click;
    CreatedEvent created;
    ShownEvent shown;
    GotFocusEvent gotFocus;
    ChildGotFocusEvent childGotFocus;
    LostFocusEvent lostFocus;
    ChildLostFocusEvent childLostFocus;
    VisibleChangedEvent visibleChanged;
    EnabledChangedEvent enabledChanged;
    LocationChangedEvent locationChanged;
    SizeChangedEvent sizeChanged;
    PaintEvent paint;
    std::vector<Control*> createList;
};

WING_API HWND LParamHandle(Message& msg);

} } // cmajor::wing

#endif // CMAJOR_WING_CONTROL_INCLUDED
