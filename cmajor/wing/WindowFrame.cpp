// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/WindowFrame.hpp>
#include <wing/Window.hpp>
#undef min 
#undef max

namespace cmajor { namespace wing {

WindowSysButton::WindowSysButton(WindowFrame* frame_, const Rect& rect_, const Rect& clientRect_) : frame(frame_), rect(rect_), clientRect(clientRect_), state(State::normal)
{
}

WindowSysButton::~WindowSysButton()
{
}

bool WindowSysButton::Contains(const Point& pt) const
{
    return rect.Contains(pt);
}

Rect WindowSysButton::GetButtonRect() const
{
    int w = std::min(clientRect.Width, clientRect.Height);
    w = std::min(w / 2, 10);
    Rect buttonRect = clientRect;
    buttonRect.Width = w;
    buttonRect.Height = w;
    buttonRect.Offset((clientRect.Width - buttonRect.Width) / 2, (clientRect.Height - buttonRect.Height) / 2);
    WindowState ws = static_cast<WindowState>(frame->GetWindowState());
    if (ws == WindowState::maximized)
    {
        buttonRect.Offset(-4, 4);
    }
    return buttonRect;
}

void WindowSysButton::Paint(Graphics& graphics, Pen* pen, Pen* selectedPen, Pen* disabledPen, Brush* backgroundBrush, Brush* hoverBackgroundBrush)
{
    if (state == State::mouseOver)
    {
        CheckGraphicsStatus(graphics.FillRectangle(hoverBackgroundBrush, clientRect));
    }
}

class WindowCloseButton : public WindowSysButton
{
public:
    WindowCloseButton(WindowFrame* frame_, const Rect& rect_, const Rect& clientRect_);
    void Paint(Graphics& graphics, Pen* pen, Pen* selectedPen, Pen* disabledPen, Brush* backgroundBrush, Brush* hoverBackgroundBrush) override;
    int HitTestValue() const override;
};

WindowCloseButton::WindowCloseButton(WindowFrame* frame_, const Rect& rect_, const Rect& clientRect_) : WindowSysButton(frame_, rect_, clientRect_)
{
}

void WindowCloseButton::Paint(Graphics& graphics, Pen* pen, Pen* selectedPen, Pen* disabledPen, Brush* backgroundBrush, Brush* hoverBackgroundBrush)
{
    WindowSysButton::Paint(graphics, pen, selectedPen, disabledPen, backgroundBrush, hoverBackgroundBrush);
    Pen* p = pen;
    if (Frame()->WindowActive())
    {
        if (GetState() == State::mouseOver)
        {
            p = selectedPen;
        }
    }
    else
    {
        p = disabledPen;
    }
    Rect buttonRect = GetButtonRect();
    graphics.DrawLine(p, Point(buttonRect.X, buttonRect.Y), Point(buttonRect.X + buttonRect.Width, buttonRect.Y + buttonRect.Height));
    graphics.DrawLine(p, Point(buttonRect.X + buttonRect.Width, buttonRect.Y), Point(buttonRect.X, buttonRect.Y + buttonRect.Height));
}

int WindowCloseButton::HitTestValue() const
{
    return HTCLOSE;
}

class WindowMaximizeButton : public WindowSysButton
{
public:
    WindowMaximizeButton(WindowFrame* frame_, const Rect & rect_, const Rect& clientRect_);
    void Paint(Graphics& graphics, Pen* pen, Pen* selectedPen, Pen* disabledPen, Brush* backgroundBrush, Brush* hoverBackgroundBrush) override;
    int HitTestValue() const override;
};

WindowMaximizeButton::WindowMaximizeButton(WindowFrame* frame_, const Rect& rect_, const Rect& clientRect_) : WindowSysButton(frame_, rect_, clientRect_)
{
}

void WindowMaximizeButton::Paint(Graphics& graphics, Pen* pen, Pen* selectedPen, Pen* disabledPen, Brush* backgroundBrush, Brush* hoverBackgroundBrush)
{
    WindowSysButton::Paint(graphics, pen, selectedPen, disabledPen, backgroundBrush, hoverBackgroundBrush);
    Pen* p = pen;
    if (Frame()->WindowActive())
    {
        if (GetState() == State::mouseOver)
        {
            p = selectedPen;
        }
    }
    else
    {
        p = disabledPen;
    }
    Rect buttonRect = GetButtonRect();
    CheckGraphicsStatus(graphics.DrawRectangle(p, buttonRect));
}

int WindowMaximizeButton::HitTestValue() const
{
    return HTMAXBUTTON;
}

class WindowRestoreButton : public WindowSysButton
{
public:
    WindowRestoreButton(WindowFrame* frame_, const Rect& rect_, const Rect& clientRect_);
    void Paint(Graphics& graphics, Pen* pen, Pen* selectedPen, Pen* disabledPen, Brush* backgroundBrush, Brush* hoverBackgroundBrush) override;
    int HitTestValue() const override;
};

WindowRestoreButton::WindowRestoreButton(WindowFrame* frame_, const Rect& rect_, const Rect& clientRect_) : WindowSysButton(frame_, rect_, clientRect_)
{
}

void WindowRestoreButton::Paint(Graphics& graphics, Pen* pen, Pen* selectedPen, Pen* disabledPen, Brush* backgroundBrush, Brush* hoverBackgroundBrush)
{
    WindowSysButton::Paint(graphics, pen, selectedPen, disabledPen, backgroundBrush, hoverBackgroundBrush);
    Pen* p = pen;
    if (Frame()->WindowActive())
    {
        if (GetState() == State::mouseOver)
        {
            p = selectedPen;
        }
    }
    else
    {
        p = disabledPen;
    }
    Rect buttonRect = GetButtonRect();
    buttonRect.Offset(0, 1);
    buttonRect.Inflate(-1, -1);
    CheckGraphicsStatus(graphics.DrawRectangle(p, buttonRect));
    Rect offsetRect = buttonRect;
    offsetRect.Offset(3, -3);
    Point p0(offsetRect.X, buttonRect.Y);
    Point p1(offsetRect.X, offsetRect.Y);
    Point p2(offsetRect.X + offsetRect.Width, offsetRect.Y);
    Point p3(offsetRect.X + offsetRect.Width, offsetRect.Y + offsetRect.Height);
    Point p4(buttonRect.X + buttonRect.Width, offsetRect.Y + offsetRect.Height);
    std::vector<Point> points;
    points.push_back(p0);
    points.push_back(p1);
    points.push_back(p2);
    points.push_back(p3);
    points.push_back(p4);
    CheckGraphicsStatus(graphics.DrawLines(p, points.data(), 5));
}

int WindowRestoreButton::HitTestValue() const
{
    return HTMAXBUTTON;
}

class WindowMinimizeButton : public WindowSysButton
{
public:
    WindowMinimizeButton(WindowFrame* frame_, const Rect& rect_, const Rect& clientRect_);
    void Paint(Graphics& graphics, Pen* pen, Pen* selectedPen, Pen* disabledPen, Brush* backgroundBrush, Brush* hoverBackgroundBrush) override;
    int HitTestValue() const override;
};

WindowMinimizeButton::WindowMinimizeButton(WindowFrame* frame_, const Rect& rect_, const Rect& clientRect_) : WindowSysButton(frame_, rect_, clientRect_)
{
}

void WindowMinimizeButton::Paint(Graphics& graphics, Pen* pen, Pen* selectedPen, Pen* disabledPen, Brush* backgroundBrush, Brush* hoverBackgroundBrush)
{
    WindowSysButton::Paint(graphics, pen, selectedPen, disabledPen, backgroundBrush, hoverBackgroundBrush);
    Pen* p = pen;
    if (Frame()->WindowActive())
    {
        if (GetState() == State::mouseOver)
        {
            p = selectedPen;
        }
    }
    else
    {
        p = disabledPen;
    }
    Rect buttonRect = GetButtonRect();
    graphics.DrawLine(p, Point(buttonRect.X, buttonRect.Y + buttonRect.Height / 2), Point(buttonRect.X + buttonRect.Width, buttonRect.Y + buttonRect.Height / 2));
}

int WindowMinimizeButton::HitTestValue() const
{
    return HTMINBUTTON;
}

WindowBorder::WindowBorder(const Rect& rect_, const Rect& clientRect_, int hitTestValue_) : rect(rect_), clientRect(clientRect_), hitTestValue(hitTestValue_)
{
}

bool WindowBorder::Contains(const Point& pt) const
{
    return rect.Contains(pt);
}

WindowCorner::WindowCorner(const Rect& rect_, int hitTestValue_) : rect(rect_), hitTestValue(hitTestValue_)
{
}

bool WindowCorner::Contains(const Point& pt) const
{
    return rect.Contains(pt);
}

WindowFrame::WindowFrame(Window* window_, const Rect& windowRect_, const Rect& clientRect_, int borderWidth_, int buttonWidth_, int buttonHeight_, int windowState_) :
    window(window_), windowRect(windowRect_), clientRect(clientRect_), borderWidth(borderWidth_), buttonWidth(buttonWidth_), buttonHeight(buttonHeight_), windowState(windowState_)
{
    WindowState ws = static_cast<WindowState>(GetWindowState());
    int w = std::min(buttonWidth, buttonHeight);
    Rect closeBoxRect(windowRect.X + windowRect.Width - buttonWidth, windowRect.Y, buttonWidth, buttonHeight);
    Rect closeBoxClientRect(windowRect.Width - buttonWidth, 0, buttonWidth, buttonHeight);
    sysButtons.push_back(std::unique_ptr<WindowSysButton>(new WindowCloseButton(this, closeBoxRect, closeBoxClientRect)));
    Rect maxRestoreRect(closeBoxRect.X - buttonWidth, closeBoxRect.Y, buttonWidth, buttonHeight);
    Rect maxRestoreClientRect(closeBoxClientRect.X - buttonWidth, 0, buttonWidth, buttonHeight);
    if (static_cast<WindowState>(windowState) == WindowState::normal)
    {
        sysButtons.push_back(std::unique_ptr<WindowSysButton>(new WindowMaximizeButton(this, maxRestoreRect, maxRestoreClientRect)));
    }
    else if (static_cast<WindowState>(windowState) == WindowState::maximized)
    {
        sysButtons.push_back(std::unique_ptr<WindowSysButton>(new WindowRestoreButton(this, maxRestoreRect, maxRestoreClientRect)));
    }
    Rect minimizeRect(maxRestoreRect.X - buttonWidth, maxRestoreRect.Y, buttonWidth, buttonHeight);
    Rect minimizeClientRect(maxRestoreClientRect.X - buttonWidth, 0, buttonWidth, buttonHeight);
    sysButtons.push_back(std::unique_ptr<WindowSysButton>(new WindowMinimizeButton(this, minimizeRect, minimizeClientRect)));
    corners.clear();
    Rect leftTopCornerRect(windowRect.X, windowRect.Y, borderWidth, borderWidth);
    corners.push_back(WindowCorner(leftTopCornerRect, HTTOPLEFT));
    Rect rightTopCornerRect(windowRect.X + windowRect.Width - borderWidth, windowRect.Y, borderWidth, borderWidth);
    corners.push_back(WindowCorner(rightTopCornerRect, HTTOPRIGHT));
    Rect leftBottomCornerRect(windowRect.X, windowRect.Y + windowRect.Height - borderWidth, borderWidth, borderWidth);
    corners.push_back(WindowCorner(leftBottomCornerRect, HTBOTTOMLEFT));
    Rect rightBottomCornerRect(windowRect.X + windowRect.Width - borderWidth, windowRect.Y + windowRect.Height - borderWidth, borderWidth, borderWidth);
    corners.push_back(WindowCorner(rightBottomCornerRect, HTBOTTOMRIGHT));
    borders.clear();
    Rect leftBorderRect(windowRect.X, windowRect.Y, borderWidth, windowRect.Height);
    Rect leftBorderClientRect(-2 * borderWidth, -buttonHeight, 4 * borderWidth, windowRect.Height + buttonHeight);
    borders.push_back(WindowBorder(leftBorderRect, leftBorderClientRect, HTLEFT));
    Rect rightBorderRect(windowRect.X + windowRect.Width - borderWidth, windowRect.Y, borderWidth, windowRect.Height);
    Rect rightBorderClientRect(clientRect.Width - 2 * borderWidth, 0, 4 * borderWidth, windowRect.Height);
    borders.push_back(WindowBorder(rightBorderRect, rightBorderClientRect, HTRIGHT));
    Rect topBorderRect(windowRect.X, windowRect.Y, windowRect.Width, borderWidth);
    Rect topBorderClientRect(0, -2 * borderWidth - buttonHeight, windowRect.Width, 4 * borderWidth);
    borders.push_back(WindowBorder(topBorderRect, topBorderClientRect, HTTOP));
    Rect bottomBorderRect(windowRect.X, windowRect.Y + windowRect.Height - borderWidth, windowRect.Width, borderWidth);
    Rect bottomBorderClientRect(-2 * borderWidth, windowRect.Height - 2 * borderWidth - buttonHeight, windowRect.Width + 4 * borderWidth, 4 * borderWidth);
    borders.push_back(WindowBorder(bottomBorderRect, bottomBorderClientRect, HTBOTTOM));
}

bool WindowFrame::DataEqual(const Rect& windowRect_, int borderWidth_, int buttonWidth_, int buttonHeight_, int windowState_)
{
    if (windowRect.X != windowRect_.X) return false;
    if (windowRect.Y != windowRect_.Y) return false;
    if (windowRect.Width != windowRect_.Width) return false;
    if (windowRect.Height != windowRect_.Height) return false;
    if (buttonWidth != buttonWidth_) return false;
    if (buttonHeight != buttonHeight_) return false;
    if (windowState_ != windowState_) return false;
    return true;
}

void WindowFrame::Invalidate()
{
    RECT r = ToWinRect(TitleBarClientRect());
    HRGN rgn = CreateRectRgn(r.left, r.top, r.right, r.bottom);
    if (rgn)
    {
        for (const auto& border : borders)
        {
            RECT rect = ToWinRect(border.GetClientRect());
            HRGN borderRgn = CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
            CombineRgn(rgn, rgn, borderRgn, RGN_OR);
            DeleteObject(borderRgn);
        }
        bool result = RedrawWindow(window->Handle(), nullptr, rgn, RDW_INVALIDATE | RDW_FRAME);
    }
    if (rgn)
    {
        DeleteObject(rgn);
    }
}

bool WindowFrame::WindowActive() const
{
    return window->Active();
}

bool WindowFrame::SetState(const Point& mousePos)
{
    bool stateChanged = false;
    for (const auto& sysButton : sysButtons)
    {
        if (sysButton->Contains(mousePos))
        {
            if (sysButton->GetState() != WindowSysButton::State::mouseOver)
            {
                sysButton->SetState(WindowSysButton::State::mouseOver);
                stateChanged = true;
            }
        }
        else
        {
            if (sysButton->GetState() != WindowSysButton::State::normal)
            {
                sysButton->SetState(WindowSysButton::State::normal);
                stateChanged = true;
            }
        }
    }
    return stateChanged;
}

void WindowFrame::Paint(Graphics& graphics, Pen* sysButtonPen, Pen* sysButtonSelectedPen, Pen* sysButtonDisabledPen, Brush* backgroundBrush, Brush* sysButtonHoverBackgroundBrush)
{
    Gdiplus::SmoothingMode prevSmoothingMode = graphics.GetSmoothingMode();
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    for (const auto& sysButton : sysButtons)
    {
        sysButton->Paint(graphics, sysButtonPen, sysButtonSelectedPen, sysButtonDisabledPen, backgroundBrush, sysButtonHoverBackgroundBrush);
    }
    graphics.SetSmoothingMode(prevSmoothingMode);
}

Rect WindowFrame::TitleBarRect() const
{
    Rect titleBarRect;
    titleBarRect.X = windowRect.X;
    titleBarRect.Y = windowRect.Y;
    titleBarRect.Width = windowRect.Width;
    titleBarRect.Height = buttonHeight + borderWidth + 2 * borderWidth;
    return titleBarRect;
}

Rect WindowFrame::TitleBarClientRect() const
{
    Rect titleBarCR;
    titleBarCR.Width = windowRect.Width;
    titleBarCR.Height = buttonHeight + borderWidth + 2 * borderWidth;
    titleBarCR.Offset(0, -buttonHeight - borderWidth - borderWidth);
    return titleBarCR;
}

int WindowFrame::HitTest(const Point& pt) const
{
    if (windowRect.Contains(pt))
    {
        for (const auto& sysButton : sysButtons)
        {
            if (sysButton->Contains(pt))
            {
                return sysButton->HitTestValue();
            }
        }
        for (const auto& corner : corners)
        {
            if (corner.Contains(pt))
            {
                return corner.HitTestValue();
            }
        }
        for (const auto& border : borders)
        {
            if (border.Contains(pt))
            {
                return border.HitTestValue();
            }
        }
        Rect titleBarRect = TitleBarRect();
        if (titleBarRect.Contains(pt))
        {
            return HTCAPTION;
        }
        return HTCLIENT;
    }
    else
    {
        return HTNOWHERE;
    }
}

} } // cmajor::wing
