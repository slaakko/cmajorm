// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_WINDOW_INCLUDED
#define CMAJOR_WING_WINDOW_INCLUDED
#include <wing/Application.hpp>
#include <wing/ContainerControl.hpp>

namespace cmajor { namespace wing {

enum class WindowState : int
{
    normal, minimized, maximized
};

WING_API int GetDialogResult(void* dialogWindowPtr);
WING_API void DialogWindowKeyPreview(void* dialogWindowPtr, Keys key, bool& handled);

enum class DialogResult : int
{
    none = 0, ok = 1, cancel = 2, abort = 3, retry = 4, ignore = 5, yes = 6, no = 7
};

class Button;
class MenuBar;
class ContextMenu;

using WindowStateChangedEvent = Event;
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
    bool ShowingDialog() const { return showingDialog; }
    void SetShowingDialog() { showingDialog = true; }
    void ResetShowingDialog() { showingDialog = false; }
    void Close();
    Button* DefaultButton() const { return defaultButton; }
    void SetDefaultButton(Button* defaultButton_);
    Button* CancelButton() const { return cancelButton; }
    void SetCancelButton(Button* cancelButton_) { cancelButton = cancelButton_; }
    Control* GetFocusedControl() const { return focusedControl; }
    void SetFocusedControl(Control* focusedControl_) { focusedControl = focusedControl_; }
    void ResetFocusedControl() { focusedControl = nullptr; }
    void FocusNext();
    void FocusPrev();
    DialogResult ShowDialog(Window& parentWindow);
    DialogResult GetDialogResult() const { return dialogResult; }
    void SetDialogResult(DialogResult dialogResult_) { dialogResult = dialogResult_; }
    void MouseUpNotificationInternal(MouseEventArgs& args) { MouseUpNotification(args); }
    MenuBar* GetMenuBar() const { return menuBar; }
    ContextMenu* GetContextMenu() const { return contextMenu; }
    void SetContextMenu(ContextMenu* contextMenu_);
    void ShowContextMenu(ContextMenu* contextMenu_, Point& ptInScreenCoords);
    void RemoveContextMenu();
    void HideContextMenu();
    void SetIcon(const Icon& icon);
    void SetSmallIcon(const Icon& icon);
    KeyPreviewMethod GetKeyPreviewMethod() const;
    void SetKeyPreviewMethod(KeyPreviewMethod& keyPreviewMethod_);
    KeyPreviewMethod GetDialogKeyPreviewMethod() const;
    void SetDialogKeyPreviewMethod(KeyPreviewMethod& dialogKeyPreviewMethod_);
    void DefaultKeyPreview(Keys key, bool& handled);
    WindowState GetWindowState() const { return windowState; }
    void SetWindowState(WindowState newWindowState);
    WindowStateChangedEvent& WindowStateChanged() { return windowStateChanged; }
    void ShowWindow(int showCommand);
protected:
    virtual void MouseUpNotification(MouseEventArgs& args);
    bool ProcessMessage(Message& msg) override;
    void OnKeyDown(KeyEventArgs& args) override;
    void OnPaint(PaintEventArgs& args) override;
    void OnControlAdded(ControlEventArgs& args) override;
    void OnControlRemoved(ControlEventArgs& args) override;
    void OnMouseDown(MouseEventArgs& args) override;
    void OnMouseUp(MouseEventArgs& args) override;
    void OnMouseMove(MouseEventArgs& args) override;
    void OnGotFocus() override;
    void OnLostFocus() override;
    virtual void OnWindowStateChanged();
    virtual void OnWindowClosing(CancelArgs& args);
    virtual void OnWindowClosed(bool& processed);
private:
    void DoWindowStateChanged(int sizeType);
    std::string fontFamilyName;
    float fontSize;
    FontStyle fontStyle;
    bool mainWindow;
    bool showingDialog;
    WindowState windowState;
    Button* defaultButton;
    Button* cancelButton;
    Control* focusedControl;
    MenuBar* menuBar;
    ContextMenu* contextMenu;
    DialogResult dialogResult;
    WindowClosingEvent windowClosing;
    WindowClosedEvent windowClosed;
    WindowStateChangedEvent windowStateChanged;
    KeyPreviewMethod keyPreviewMethod;
    KeyPreviewMethod dialogKeyPreviewMethod;
};

} } // cmajor::wing

#endif // CMAJOR_WING_WINDOW_INCLUDED
