// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_WINDOW_INCLUDED
#define CMAJOR_WING_WINDOW_INCLUDED
#include <cmajor/wing/ContainerControl.hpp>

namespace cmajor { namespace wing {

enum class DialogResult : int
{
    none = 0, ok = 1, cancel = 2, abort = 3, retry = 4, ignore = 5, yes = 6, no = 7
};

class Button;
class MenuBar;

struct CancelArgs
{
    CancelArgs(bool& cancelClose_) : cancelClose(cancelClose_) {}
    bool& cancelClose;
};

using WindowClosingEvent = EventWithArgs<CancelArgs>;

using WindowClosedEvent = Event;

WING_API inline int64_t DefaultWindowClassWindowBackgroundColor()
{
    return COLOR_WINDOW;
}

WING_API inline Color DefaultWindowBackgroundColor()
{
    return GetSystemColor(DefaultWindowClassWindowBackgroundColor());
}

WING_API std::string DefaultWindowFontFamilyName();
WING_API float DefaultWindowFontSize();
WING_API FontStyle DefaultWindowFontStyle();

struct WING_API WindowCreateParams
{
    WindowCreateParams();
    WindowCreateParams& Defaults();
    WindowCreateParams& WindowClassName(const std::string& windowClassName_);
    WindowCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    WindowCreateParams& WindowStyle(int windowStyle_);
    WindowCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    WindowCreateParams& BackgroundColor(const Color& backgroundColor_);
    WindowCreateParams& Text(const std::string& text_);
    WindowCreateParams& Location(Point location_);
    WindowCreateParams& SetSize(Size size_);
    WindowCreateParams& SetAnchors(Anchors anchors_);
    WindowCreateParams& SetDock(Dock dock_);
    WindowCreateParams& FontFamilyName(const std::string& fontFamilyName_);
    WindowCreateParams& FontSize(float fontSize_);
    WindowCreateParams& SetFontStyle(FontStyle fontStyle_);
    ControlCreateParams controlCreateParams;
    std::string fontFamilyName;
    float fontSize;
    FontStyle fontStyle;
};

class WING_API Window : public ContainerControl
{
public:
    Window(WindowCreateParams& createParams);
    bool IsWindow() const override { return true; }
    void SetAsMainWindow() { mainWindow = true; }
    bool IsMainWindow() const { return mainWindow; }
    void Close();
    Button* DefaultButton() const { return defaultButton; }
    void SetDefaultButton(Button* defaultButton_);
    Button* CancelButton() const { return cancelButton; }
    void SetCancelButton(Button* cancelButton_) { cancelButton = cancelButton_; }
    DialogResult GetDialogResult() const { return dialogResult; }
    void SetDialogResult(DialogResult dialogResult_) { dialogResult = dialogResult_; }
    void MouseUpNotificationInternal(MouseEventArgs& args) { MouseUpNotification(args); }
    MenuBar* GetMenuBar() const { return menuBar; }
protected:
    virtual void MouseUpNotification(MouseEventArgs& args);
    bool ProcessMessage(Message& msg) override;
    void OnPaint(PaintEventArgs& args) override;
    void OnControlAdded(ControlEventArgs& args) override;
    void OnControlRemoved(ControlEventArgs& args) override;
    void OnMouseDown(MouseEventArgs& args) override;
    void OnMouseUp(MouseEventArgs& args) override;
    void OnMouseMove(MouseEventArgs& args) override;
    virtual void OnWindowClosing(CancelArgs& args);
    virtual void OnWindowClosed(bool& processed);
private:
    std::string fontFamilyName;
    float fontSize;
    FontStyle fontStyle;
    bool mainWindow;
    Button* defaultButton;
    Button* cancelButton;
    MenuBar* menuBar;
    DialogResult dialogResult;
    WindowClosingEvent windowClosing;
    WindowClosedEvent windowClosed;
};

} } // cmajor::wing

#endif // CMAJOR_WING_WINDOW_INCLUDED
