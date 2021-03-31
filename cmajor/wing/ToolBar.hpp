// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_TOOL_BAR_INCLUDED
#define CMAJOR_WING_TOOL_BAR_INCLUDED
#include <cmajor/wing/ToolTip.hpp>
#include <cmajor/wing/Container.hpp>

namespace cmajor { namespace wing {

enum class ToolBarStyle : int
{
    regular = 0, radioButtonGroup = 1
};

enum class ToolBarFlags : int
{
    none = 0, toolTipWindowAdded = 1 << 0, toolTipWindowVisible = 1 << 1
};

WING_API inline ToolBarFlags operator|(ToolBarFlags left, ToolBarFlags right)
{
    return ToolBarFlags(int(left) | int(right));
}

WING_API inline ToolBarFlags operator&(ToolBarFlags left, ToolBarFlags right)
{
    return ToolBarFlags(int(left) & int(right));
}

WING_API inline ToolBarFlags operator~(ToolBarFlags flags)
{
    return ToolBarFlags(~int(flags));
}

WING_API Color DefaultToolButtonTransparentColor();
WING_API Color DefaultToolTipWindowColor();
WING_API Color DefaultToolButtonDarkColor();
WING_API Color DefaultToolButtonLightColor();
WING_API Color DefaultToolButtonMediumLightColor();
WING_API Color DefaultToolButtonMediumDarkColor();
WING_API Color DefaultToolButtonTextColor();
WING_API Color DefaultToolButtonDisabledColor();
WING_API Color DefaultToolButtonSeparatorColor1();
WING_API Color DefaultToolButtonSeparatorColor2();

struct WING_API ToolBarCreateParams
{
    ToolBarCreateParams();
    ToolBarCreateParams& Defaults();
    ToolBarCreateParams& WindowClassName(const std::string& windowClassName_);
    ToolBarCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    ToolBarCreateParams& WindowStyle(int windowStyle_);
    ToolBarCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    ToolBarCreateParams& BackgroundColor(const Color& backgroundColor_);
    ToolBarCreateParams& Text(const std::string& text_);
    ToolBarCreateParams& Location(Point location_);
    ToolBarCreateParams& SetSize(Size size_);
    ToolBarCreateParams& SetAnchors(Anchors anchors_);
    ToolBarCreateParams& SetDock(Dock dock_);
    ToolBarCreateParams& Style(ToolBarStyle style);
    ToolBarCreateParams& ToolButtonHeight(int height);
    ToolBarCreateParams& TransparentColor(const Color& color);
    ToolBarCreateParams& ToolTipWindowColor(const Color& color);
    ToolBarCreateParams& DarkColor(const Color& color);
    ToolBarCreateParams& LightColor(const Color& color);
    ToolBarCreateParams& MediumLightColor(const Color& color);
    ToolBarCreateParams& MediumDarkColor(const Color& color);
    ToolBarCreateParams& TextColor(const Color& color);
    ToolBarCreateParams& DisabledColor(const Color& color);
    ToolBarCreateParams& SeparatorColor1(const Color& color);
    ToolBarCreateParams& SeparatorColor2(const Color& color);
    ToolBarCreateParams& ToolBarPadding(const Padding& padding);
    ToolBarCreateParams& ToolBarToolButtonPadding(const Padding& padding);
    ToolBarCreateParams& FontFamilyName(const std::string familyName);
    ToolBarCreateParams& FontSize(float size);
    ControlCreateParams controlCreateParams;
    ToolBarStyle toolBarStyle;
    int toolButtonHeight;
    Color transparentColor;
    Color toolTipWindowColor;
    Color darkColor;
    Color lightColor;
    Color mediumLightColor;
    Color mediumDarkColor;
    Color textColor;
    Color disabledColor;
    Color separatorColor1;
    Color separatorColor2;
    Padding toolBarPadding;
    Padding toolButtonPadding;
    std::string fontFamilyName;
    float fontSize;
};

class ToolBar;

class WING_API ToolBarPens
{
public:
    ToolBarPens(ToolBar* toolBar_);
    ToolBarPens(const ToolBarPens&) = delete;
    ToolBarPens(ToolBarPens&&) = delete;
    ToolBarPens& operator=(const ToolBarPens&) = delete;
    ToolBarPens& operator=(ToolBarPens&&) = delete;
    Pen* DarkPen();
    Pen* LightPen();
    Pen* MediumLightPen();
    Pen* MediumDarkPen();
    Pen* SeparatorPen1();
    Pen* SeparatorPen2();
private:
    ToolBar* toolBar;
    Pen* darkPen;
    Pen* lightPen;
    Pen* mediumLightPen;
    Pen* mediumDarkPen;
    Pen* separatorPen1;
    Pen* separatorPen2;
    std::vector<std::unique_ptr<Pen>> pens;
};

class ToolButtonBase;
class ToolButton;

struct WING_API ToolButtonBitmaps
{
    ToolButtonBitmaps() : normal(nullptr), disabled(nullptr) {}
    ToolButtonBitmaps(Bitmap* normal_, Bitmap* disabled_) : normal(normal_), disabled(disabled_) {}
    Bitmap* normal;
    Bitmap* disabled;
};

class WING_API ToolBar : public Control
{
public:
    ToolBar(ToolBarCreateParams& createParams);
    bool IsToolBar() const override { return true; }
    ~ToolBar();
    void AddToolButton(ToolButtonBase* toolButton);
    ToolBarStyle Style() const { return toolBarStyle; }
    int ToolButtonHeight() const { return toolButtonHeight; }
    const StringFormat& CenterFormat() const { return centerFormat; }
    const Color& TransparentColor() const { return transparentColor; }
    const Color& DarkColor() const { return darkColor; }
    const Color& LightColor() const { return lightColor; }
    const Color& MediumLightColor() const { return mediumLightColor; }
    const Color& MediumDarkColor() const { return mediumDarkColor; }
    const Color& TextColor() const { return textColor; }
    const Color& DisabledColor() const { return disabledColor; }
    const Color& SeparatorColor1() const { return separatorColor1; }
    const Color& SeparatorColor2() const { return separatorColor2; }
    ToolBarPens& Pens() { return pens; }
    SolidBrush* NormalTextBrush() { return &normalTextBrush; }
    SolidBrush* DisabledTextBrush() { return &disabledTextBrush; }
    ToolButton* PressedToolButton() const { return pressedToolButton; }
    void SetPressedToolButton(ToolButton* newPressedToolButton);
    void ResetPressedToolButton();
    ToolButton* GetToolButtonAt(const Point& location) const;
    ToolButton* GetToolButton(int toolButtonIndex) const;
    void ShowToolTipWindow(ToolButton* toolButton);
    void HideToolTipWindow();
    bool ToolTipWindowAdded() const { return (flags & ToolBarFlags::toolTipWindowAdded) != ToolBarFlags::none; }
    void SetToolTipWindowAdded() { flags = flags | ToolBarFlags::toolTipWindowAdded; }
    void ResetToolTipWindowAdded() { flags = flags & ~ToolBarFlags::toolTipWindowAdded; }
    bool ToolTipWindowVisible() const { return (flags & ToolBarFlags::toolTipWindowVisible) != ToolBarFlags::none; }
    void SetToolTipWindowVisible() { flags = flags | ToolBarFlags::toolTipWindowVisible; }
    void ResetToolTipWindowVisible() { flags = flags & ~ToolBarFlags::toolTipWindowVisible; }
protected:
    void OnPaint(PaintEventArgs& args) override;
    void OnMouseEnter() override;
    void OnMouseLeave() override;
    void OnMouseMove(MouseEventArgs& args) override;
    void OnMouseHover(MouseEventArgs& args) override;
    void OnMouseDown(MouseEventArgs& args) override;
    void OnMouseUp(MouseEventArgs& args) override;
private:
    ToolButtonBitmaps& GetOrCreateToolButtonBitmaps(const std::string& toolButtonBitmapName);
    ToolBarFlags flags;
    ToolBarStyle toolBarStyle;
    int toolButtonHeight;
    Color transparentColor;
    Color toolTipWindowColor;
    Color darkColor;
    Color lightColor;
    Color mediumLightColor;
    Color mediumDarkColor;
    Color textColor;
    Color disabledColor;
    Color separatorColor1;
    Color separatorColor2;
    Padding toolBarPadding;
    Padding toolButtonPadding;
    StringFormat centerFormat;
    Container toolButtons;
    ToolBarPens pens;
    SolidBrush normalTextBrush;
    SolidBrush disabledTextBrush;
    ToolButton* pressedToolButton;
    ToolButton* mouseInToolButton;
    ToolButton* mouseDownToolButton;
    ToolTip* toolTipWindow;
    std::map<std::string, ToolButtonBitmaps> bitmapMap;
    std::vector<std::unique_ptr<Bitmap>> bitmaps;
};

class WING_API ToolButtonBase : public Component
{
public:
    ToolButtonBase();
    virtual void ComputeSize() = 0;
    virtual void Draw(Graphics& graphics, const Point& origin) = 0;
    bool IsToolButtonBase() const override { return true; }
    ToolBar* GetToolBar();
    const Point& Location() const { return location; }
    void SetLocation(const Point& location_);
    const Size& GetSize() const { return size; }
    void SetSize(const Size& size_);
private:
    Point location;
    Size size;
};

enum class ToolButtonState : int
{
    normal = 0, pressed = 1
};

enum class ToolButtonStyle : int
{
    manual = 0, automatic = 1
};

enum class ToolButtonFlags : int
{
    none = 0, disabled = 1 << 0
};

WING_API inline ToolButtonFlags operator|(ToolButtonFlags left, ToolButtonFlags right)
{
    return ToolButtonFlags(int(left) | int(right));
}

WING_API inline ToolButtonFlags operator&(ToolButtonFlags left, ToolButtonFlags right)
{
    return ToolButtonFlags(int(left) & int(right));
}

WING_API inline ToolButtonFlags operator~(ToolButtonFlags flags)
{
    return ToolButtonFlags(~int(flags));
}

struct WING_API ToolButtonCreateParams
{
    ToolButtonCreateParams();
    ToolButtonCreateParams& Defaults();
    ToolButtonCreateParams& ToolBitMapName(const std::string& toolBitMapName_);
    ToolButtonCreateParams& Style(ToolButtonStyle style_);
    ToolButtonCreateParams& SetPadding(const Padding& padding_);
    ToolButtonCreateParams& SetToolTip(const std::string& toolTip_);
    std::string toolBitMapName;
    ToolButtonStyle style;
    Padding padding;
    std::string toolTip;
};

class WING_API ToolButton : public ToolButtonBase
{
public:
    ToolButton(ToolButtonCreateParams& createParams);
    const std::string& ToolBitMapName() const { return toolBitMapName; }
    bool IsToolButton() const override { return true; }
    ToolButtonState State() const { return state; }
    void SetState(ToolButtonState state_);
    void Enable();
    void Disable();
    bool IsEnabled() const { return !IsDisabled(); }
    bool IsDisabled() const { return (flags & ToolButtonFlags::disabled) != ToolButtonFlags::none; }
    void SetToolTip(const std::string& toolTip_);
    ToolButtonStyle Style() const { return style; }
    void SetBitmaps(Bitmap* normalBitmap_, Bitmap* disabledBitmap_);
    const Padding& GetPadding() const { return padding; }
    void ComputeSize() override;
    void Draw(Graphics& graphics, const Point& origin) override;
    void Invalidate();
    const std::string& ToolTip() const { return toolTip; }
    ClickEvent& Click() { return click; }
    MouseEnterEvent& MouseEnter() { return mouseEnter; }
    MouseLeaveEvent& MouseLeave() { return mouseLeave; }
    MouseMoveEvent& MouseMove() { return mouseMove; }
    MouseHoverEvent& MouseHover() { return mouseHover; }
    MouseDownEvent& MouseDown() { return mouseDown; }
    MouseUpEvent& MouseUp() { return mouseUp; }
    void DoClick();
    void MouseEnterInternal();
    void MouseLeaveInternal();
    void MouseMoveInternal(MouseEventArgs& args);
    void MouseHoverInternal(MouseEventArgs& args);
    void MouseDownInternal(MouseEventArgs& args);
    void MouseUpInternal(MouseEventArgs& args);
protected:
    virtual void DrawContent(Graphics& graphics, const Point& origin);
    virtual void OnClick();
    virtual void OnMouseEnter();
    virtual void OnMouseLeave();
    virtual void OnMouseMove(MouseEventArgs& args);
    virtual void OnMouseHover(MouseEventArgs& args);
    virtual void OnMouseDown(MouseEventArgs& args);
    virtual void OnMouseUp(MouseEventArgs& args);
private:
    void DrawNormal(Graphics& graphics, const Point& origin);
    void DrawPressed(Graphics& graphics, const Point& origin);
    std::string toolBitMapName;
    ToolButtonState state;
    ToolButtonFlags flags;
    ToolButtonStyle style;
    Padding padding;
    Bitmap* normalBitmap;
    Bitmap* disabledBitmap;
    Size bitmapSize;
    std::string toolTip;
    ClickEvent click;
    MouseEnterEvent mouseEnter;
    MouseLeaveEvent mouseLeave;
    MouseMoveEvent mouseMove;
    MouseHoverEvent mouseHover;
    MouseDownEvent mouseDown;
    MouseUpEvent mouseUp;
};

struct WING_API TextToolButtonCreateParams
{
    TextToolButtonCreateParams(const std::string& text_);
    TextToolButtonCreateParams& Style(ToolButtonStyle style_);
    TextToolButtonCreateParams& SetPadding(const Padding& padding_);
    TextToolButtonCreateParams& SetToolTip(const std::string& toolTip_);
    TextToolButtonCreateParams& SetSize(const Size& size_);
    ToolButtonCreateParams toolButtonCreateParams;
    Size size;
    std::string text;
};

class WING_API TextToolButton : public ToolButton
{
public:
    TextToolButton(TextToolButtonCreateParams& createParams);
    void ComputeSize() override;
protected:
    void DrawContent(Graphics& graphics, const Point& origin) override;
private:
    std::string text;
};

class WING_API ToolButtonSeparator : public ToolButtonBase
{
public:
    ToolButtonSeparator();
    void ComputeSize() override; 
    void Draw(Graphics& graphics, const Point& origin) override;
};

} } // cmajor::wing

#endif // CMAJOR_WING_TOOL_BAR_INCLUDED
