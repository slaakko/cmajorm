// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/wing/Window.hpp>
#include <cmajor/wing/Application.hpp>
#include <cmajor/wing/Button.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace wing {

using namespace soulng::unicode;

std::string DefaultWindowFontFamilyName()
{
    return "Microsoft Sans Serif";
}

float DefaultWindowFontSize()
{
    return 8.25f;
}

FontStyle DefaultWindowFontStyle()
{
    return FontStyle::FontStyleRegular;
}

WindowCreateParams::WindowCreateParams()
{
    controlCreateParams.WindowClassName("System.Windows.Window").WindowStyle(OverlappedWindowStyle()).WindowClassBackgroundColor(DefaultWindowClassWindowBackgroundColor()).
        BackgroundColor(DefaultWindowBackgroundColor()).Location(DefaultLocation()).SetSize(DefaultSize());
    FontFamilyName(DefaultWindowFontFamilyName()).FontSize(DefaultWindowFontSize()).SetFontStyle(DefaultWindowFontStyle());
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

WindowCreateParams& WindowCreateParams::WindowClassBackgroundColor(int64_t windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

WindowCreateParams& WindowCreateParams::BackgroundColor(Color backgroundColor_)
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

Window::Window(WindowCreateParams& createParams) : 
    ContainerControl(createParams.controlCreateParams),
    fontFamilyName(createParams.fontFamilyName), 
    fontSize(createParams.fontSize), 
    fontStyle(createParams.fontStyle),
    mainWindow(false),
    defaultButton(nullptr),
    cancelButton(nullptr),
    dialogResult(DialogResult::none)
{
    if (!fontFamilyName.empty())
    {
        std::u16string familyName = ToUtf16(fontFamilyName);
        const WCHAR* familyStr = (const WCHAR*)familyName.c_str();
        SetFont(Font(FontFamily(familyStr), fontSize, fontStyle, Unit::UnitPoint));
    }
}

void Window::Close()
{
    PostMessage(Handle(), WM_CLOSE, 0, 0);
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

bool Window::ProcessMessage(Message& msg) 
{
    switch (msg.message)
    {
        case WM_CLOSE:
        {
            bool cancelClose = false;
            CancelArgs args(cancelClose);
            OnWindowClosing(args);
            if (args.cancelClose) 
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
        case WM_COMMAND: case WM_MOVE: case WM_SIZE: case WM_PAINT: case WM_MOUSEMOVE: case WM_MOUSELEAVE: case WM_LBUTTONDOWN: case WM_LBUTTONUP: case WM_LBUTTONDBLCLK: case WM_RBUTTONDOWN: case WM_RBUTTONUP:
        case WM_CHAR: case WM_KEYDOWN: case WM_KEYUP: case WM_SYSCOMMAND: case WM_HSCROLL: case WM_VSCROLL: case WM_MOUSEWHEEL: case WM_TIMER: case WM_SETFOCUS: case WM_KILLFOCUS:
        case WM_SHOWWINDOW: case WM_CLIPBOARDUPDATE:
        {
            return ContainerControl::ProcessMessage(msg);
        }
    }
    return false;
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

} } // cmajor::wing
