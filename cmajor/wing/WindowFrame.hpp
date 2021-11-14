// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_WINDOW_FRAME_INCLUDED
#define CMAJOR_WING_WINDOW_FRAME_INCLUDED
#include <wing/Graphics.hpp>
#include <vector>

namespace cmajor { namespace wing {

class WindowFrame;
class Window;

class WING_API WindowSysButton
{
public:
    enum State
    {
        normal, mouseOver
    };
    WindowSysButton(WindowFrame* frame_, const Rect& rect_, const Rect& clientRect_);
    bool Contains(const Point& pt) const;
    void SetState(State state_) { state = state_; }
    virtual ~WindowSysButton();
    virtual void Paint(Graphics& graphics, Pen* pen, Pen* selectedPen, Pen* disabledPen, Brush* backgroundBrush, Brush* hoverBackgroundBrush);
    virtual int HitTestValue() const = 0;
    State GetState() const { return state; }
    const Rect& GetRect() const { return rect; }
    const Rect& ClientRect() const { return clientRect; }
    Rect GetButtonRect() const;
    WindowFrame* Frame() const { return frame; }
private:
    WindowFrame* frame;
    State state;
    Rect rect;
    Rect clientRect;
};

class WING_API WindowBorder
{
public:
    WindowBorder(const Rect& rect_, const Rect& clientRect_, int hitTestValue_);
    const Rect& GetRect() const { return rect; }
    const Rect& GetClientRect() const { return clientRect; }
    int HitTestValue() const { return hitTestValue; }
    bool Contains(const Point& pt) const;
private:
    Rect rect;
    Rect clientRect;
    int hitTestValue;
};

class WING_API WindowCorner
{
public:
    WindowCorner(const Rect& rect_, int hitTestValue_);
    int HitTestValue() const { return hitTestValue; }
    bool Contains(const Point& pt) const;
private:
    Rect rect;
    int hitTestValue;
};

class WING_API WindowFrame
{
public:
    WindowFrame(Window* window_, const Rect& windowRect_, const Rect& clientRect_, int borderWidth_, int buttonWidth_, int buttonHeight_, int windowState_);
    bool DataEqual(const Rect& windowRect_, int borderWidth_, int buttonWidth_, int buttonHeight_, int windowState_);
    WindowFrame(const WindowFrame&) = delete;
    WindowFrame& operator=(const WindowFrame&) = delete;
    void Invalidate();
    bool SetState(const Point& mousePos);
    void Paint(Graphics& graphics, Pen* sysButtonPen, Pen* sysButtonSelectedPen, Pen* sysButtonDisabledPen, Brush* backgroundBrush, Brush* sysButtonHoverBackgroundBrush);
    Rect TitleBarRect() const;
    Rect TitleBarClientRect() const;
    int HitTest(const Point& pt) const;
    int GetWindowState() const { return windowState; }
    bool WindowActive() const;
private:
    Window* window;
    Rect windowRect;
    Rect clientRect;
    int borderWidth;
    int buttonWidth;
    int buttonHeight;
    int windowState;
    std::vector<std::unique_ptr<WindowSysButton>> sysButtons;
    std::vector<WindowCorner> corners;
    std::vector<WindowBorder> borders;
};

} } // cmajor::wing

#endif // CMAJOR_WING_WINDOW_FRAME_INCLUDED
