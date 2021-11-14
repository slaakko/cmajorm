// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/ToolBar.hpp>
#include <wing/Window.hpp>
#include <wing/Theme.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace wing {

using namespace soulng::unicode;

Color DefaultToolButtonTransparentColor()
{
    return DefaultBitmapTransparentColor();
}

Color DefaultToolTipWindowColor()
{
    return Color::FloralWhite;
}

Color DefaultToolButtonDarkColor()
{
    return Color(100, 100, 100);
}

Color DefaultToolButtonLightColor()
{
    return Color::White;
}

Color DefaultToolButtonMediumLightColor()
{
    return Color(227, 227, 227);
}

Color DefaultToolButtonMediumDarkColor()
{
    return Color(160, 160, 160);
}

Color DefaultToolButtonTextColor()
{
    return Color::Black;
}

Color DefaultToolButtonDisabledColor()
{
    return Color(204, 204, 207);
}

Color DefaultToolButtonSeparatorColor1()
{
    return Color(204, 206, 219);
}

Color DefaultToolButtonSeparatorColor2()
{
    return Color(245, 245, 245);
}

ToolBarCreateParams::ToolBarCreateParams()
{
    controlCreateParams.WindowClassName("wing.ToolBar");
    controlCreateParams.SetDock(Dock::top);
    toolBarStyle = ToolBarStyle::regular;
    toolButtonHeight = 38;
    transparentColor = DefaultToolButtonTransparentColor();
    toolTipWindowColor = DefaultToolTipWindowColor();
    darkColor = DefaultToolButtonDarkColor();
    lightColor = DefaultToolButtonLightColor();
    mediumLightColor = DefaultToolButtonMediumLightColor();
    mediumDarkColor = DefaultToolButtonMediumDarkColor();
    textColor = DefaultToolButtonTextColor();
    disabledColor = DefaultToolButtonDisabledColor();
    separatorColor1 = DefaultToolButtonSeparatorColor1();
    separatorColor2 = DefaultToolButtonSeparatorColor2();
    toolBarPadding = Padding(0, 0, 0, 0);
    toolButtonPadding = Padding(0, 0, 0, 0);
    fontFamilyName = "Segoe UI";
    fontSize = 9.0f;
}

ToolBarCreateParams& ToolBarCreateParams::Defaults()
{
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::Style(ToolBarStyle style)
{
    toolBarStyle = style;
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::ToolButtonHeight(int height)
{
    toolButtonHeight = height;
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::TransparentColor(const Color& color)
{
    transparentColor = color;
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::ToolTipWindowColor(const Color& color)
{
    toolTipWindowColor = color;
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::DarkColor(const Color& color)
{
    darkColor = color;
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::LightColor(const Color& color)
{
    lightColor = color;
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::MediumLightColor(const Color& color)
{
    mediumLightColor = color;
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::MediumDarkColor(const Color& color)
{
    mediumDarkColor = color;
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::TextColor(const Color& color)
{
    textColor = color;
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::DisabledColor(const Color& color)
{
    disabledColor = color;
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::SeparatorColor1(const Color& color)
{
    separatorColor1 = color;
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::SeparatorColor2(const Color& color)
{
    separatorColor2 = color;
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::ToolBarPadding(const Padding& padding)
{
    toolBarPadding = padding;
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::ToolBarToolButtonPadding(const Padding& padding)
{
    toolButtonPadding = padding;
    return* this;
}

ToolBarCreateParams& ToolBarCreateParams::FontFamilyName(const std::string familyName)
{
    fontFamilyName = familyName;
    return *this;
}

ToolBarCreateParams& ToolBarCreateParams::FontSize(float size)
{
    fontSize = size;
    return *this;
}

ToolBarPens::ToolBarPens(ToolBar* toolBar_) : 
    toolBar(toolBar_),
    darkPen(nullptr),
    lightPen(nullptr),
    mediumLightPen(nullptr),
    mediumDarkPen(nullptr),
    separatorPen1(nullptr),
    separatorPen2(nullptr)
{
}

Pen* ToolBarPens::DarkPen()
{
    if (darkPen) return darkPen;
    darkPen = new Pen(toolBar->DarkColor());
    pens.push_back(std::unique_ptr<Pen>(darkPen));
    return darkPen;
}

Pen* ToolBarPens::LightPen()
{
    if (lightPen) return lightPen;
    lightPen = new Pen(toolBar->LightColor());
    pens.push_back(std::unique_ptr<Pen>(lightPen));
    return lightPen;
}

Pen* ToolBarPens::MediumLightPen()
{
    if (mediumLightPen) return mediumLightPen;
    mediumLightPen = new Pen(toolBar->MediumLightColor());
    pens.push_back(std::unique_ptr<Pen>(mediumLightPen));
    return mediumLightPen;
}

Pen* ToolBarPens::MediumDarkPen()
{
    if (mediumDarkPen) return mediumDarkPen;
    mediumDarkPen = new Pen(toolBar->MediumDarkColor());
    pens.push_back(std::unique_ptr<Pen>(mediumDarkPen));
    return mediumDarkPen;
}

Pen* ToolBarPens::SeparatorPen1()
{
    if (separatorPen1) return separatorPen1;
    separatorPen1 = new Pen(toolBar->SeparatorColor1());
    pens.push_back(std::unique_ptr<Pen>(separatorPen1));
    return separatorPen1;
}

Pen* ToolBarPens::SeparatorPen2()
{
    if (separatorPen2) return separatorPen2;
    separatorPen2 = new Pen(toolBar->SeparatorColor2());
    pens.push_back(std::unique_ptr<Pen>(separatorPen2));
    return separatorPen2;
}

ToolBar::ToolBar(ToolBarCreateParams& createParams) :
    Control(createParams.controlCreateParams),
    flags(ToolBarFlags::none),
    toolBarStyle(createParams.toolBarStyle),
    toolButtonHeight(createParams.toolButtonHeight),
    transparentColor(createParams.transparentColor),
    toolTipWindowColor(createParams.toolTipWindowColor),
    darkColor(createParams.darkColor),
    lightColor(createParams.lightColor),
    mediumLightColor(createParams.mediumLightColor),
    mediumDarkColor(createParams.mediumDarkColor),
    textColor(createParams.textColor),
    disabledColor(createParams.disabledColor),
    separatorColor1(createParams.separatorColor1),
    separatorColor2(createParams.separatorColor2),
    toolBarPadding(createParams.toolBarPadding),
    toolButtonPadding(createParams.toolButtonPadding),
    centerFormat(),
    toolButtons(this),
    pens(new ToolBarPens(this)),
    normalTextBrush(textColor),
    disabledTextBrush(disabledColor),
    pressedToolButton(nullptr),
    mouseInToolButton(nullptr),
    mouseDownToolButton(nullptr),
    toolTipWindow(new ToolTip(ToolTipCreateParams().BackgroundColor(toolTipWindowColor)))
{
    centerFormat.SetAlignment(StringAlignment::StringAlignmentCenter);
    centerFormat.SetLineAlignment(StringAlignment::StringAlignmentCenter);
    SetMouseHoverMs(DefaultMouseHoverMs());
    std::u16string familyName = ToUtf16(createParams.fontFamilyName);
    SetFont(Font(FontFamily((const WCHAR*)familyName.c_str()), createParams.fontSize, FontStyle::FontStyleRegular, Unit::UnitPoint));
    SetLocation(Point(toolBarPadding.left, toolBarPadding.top));
    SetSize(Size(0, toolButtonHeight + toolBarPadding.Vertical() + toolButtonPadding.Vertical()));
}

ToolBar::~ToolBar()
{
    if (toolTipWindow && ToolTipWindowAdded())
    {
        ResetToolTipWindowAdded();
        Window* window = GetWindow();
        if (window)
        {
            window->RemoveChild(toolTipWindow);
            toolTipWindow = nullptr;
        }
    }
}

void ToolBar::ClearToolButtons()
{
    Component* child = toolButtons.FirstChild();
    while (child)
    {
        Component* next = child->NextSibling();
        toolButtons.RemoveChild(child);
        child = next;
    }
}

void ToolBar::AddToolButton(ToolButtonBase* toolButtonBase)
{
    toolButtons.AddChild(toolButtonBase);
    if (toolButtonBase->IsToolButton())
    {
        ToolButton* toolButton = static_cast<ToolButton*>(toolButtonBase);
        if (!toolButton->ToolBitMapName().empty())
        {
            ToolButtonBitmaps& bitmaps = GetOrCreateToolButtonBitmaps(toolButton->ToolBitMapName());
            toolButton->SetBitmaps(bitmaps.normal, bitmaps.disabled);
        }
        toolButton->ComputeSize();
        if (toolBarStyle == ToolBarStyle::radioButtonGroup)
        {
            if (!pressedToolButton)
            {
                SetPressedToolButton(toolButton);
            }
        }
    }
    else
    {
        toolButtonBase->ComputeSize();
    }
}

void ToolBar::SetPressedToolButton(ToolButton* newPressedToolButton)
{
    if (toolBarStyle != ToolBarStyle::radioButtonGroup) return;
    if (pressedToolButton != newPressedToolButton)
    {
        if (pressedToolButton)
        {
            pressedToolButton->SetState(ToolButtonState::normal);
        }
        pressedToolButton = newPressedToolButton;
        if (pressedToolButton)
        {
            pressedToolButton->SetState(ToolButtonState::pressed);
        }
    }
}

void ToolBar::ResetPressedToolButton()
{
    if (toolBarStyle != ToolBarStyle::radioButtonGroup) return;
    pressedToolButton = nullptr;
}

ToolButton* ToolBar::GetToolButtonAt(const Point& location) const
{
    Size size = GetSize();
    if (location.Y >= 0 && location.Y < size.Height && location.X >= 0 && location.X < size.Width)
    {
        Component* component = toolButtons.FirstChild();
        while (component)
        {
            if (component->IsToolButton())
            {
                ToolButton* toolButton = static_cast<ToolButton*>(component);
                if (location.X >= toolButton->Location().X && location.X <= toolButton->Location().X + toolButton->GetSize().Width)
                {
                    return toolButton;
                }
            }
            component = component->NextSibling();
        }
    }
    return nullptr;
}

ToolButton* ToolBar::GetToolButton(int toolButtonIndex) const
{
    Component* component = toolButtons.FirstChild();
    int index = 0;
    while (component)
    {
        if (component->IsToolButton())
        {
            if (index == toolButtonIndex)
            {
                return static_cast<ToolButton*>(component);
            }
            ++index;
        }
        component = component->NextSibling();
    }
    return nullptr;
}

void ToolBar::ShowToolTipWindow(ToolButton* toolButton)
{
    if (!toolTipWindow || !ToolTipWindowAdded()) return;
    if (ToolTipWindowVisible()) toolTipWindow->Hide();
    ResetToolTipWindowVisible();
    Point loc = toolButton->Location();
    Size size = toolButton->GetSize();
    toolTipWindow->SetText(toolButton->ToolTip());
    toolTipWindow->MeasureExtent();
    Point pt(loc.X, loc.Y + size.Height + 8);
    pt = ClientToScreen(pt);
    Window* window = GetWindow();
    if (window)
    {
        pt = window->ScreenToClient(pt);
        Size sz = toolTipWindow->GetSize();
        Point wloc = window->Location();
        Size wsz = window->GetSize();
        if (pt.Y + sz.Height > wloc.Y + wsz.Height)
        {
            pt.Y = wloc.Y + wsz.Height - sz.Height;
        }
        if (pt.X + sz.Width > wloc.X + wsz.Width)
        {
            pt.X = wloc.X + wsz.Width - sz.Width;
        }
        toolTipWindow->SetLocation(pt);
        toolTipWindow->BringToFront();
        toolTipWindow->Show();
        toolTipWindow->Invalidate();
        SetToolTipWindowVisible();
    }
}

void ToolBar::HideToolTipWindow()
{
    if (!toolTipWindow || !ToolTipWindowAdded() || !ToolTipWindowVisible()) return;
    toolTipWindow->Hide();
    ResetToolTipWindowVisible();
}

void ToolBar::UpdateColors()
{
    Control::UpdateColors();
    toolTipWindowColor = GetColor("tool.bar.tool.tip"); 
    if (toolTipWindow)
    {
        toolTipWindow->SetBackgroundColor(toolTipWindowColor);
    }
    textColor = GetColor("tool.bar.tool.button.text");
    normalTextBrush.SetColor(textColor);
    disabledColor = GetColor("tool.bar.tool.button.disabled");
    disabledTextBrush.SetColor(disabledColor);
    darkColor = GetColor("tool.bar.tool.button.dark");
    lightColor = GetColor("tool.bar.tool.button.light");
    mediumLightColor = GetColor("tool.bar.tool.button.medium.light");
    mediumDarkColor = GetColor("tool.bar.tool.button.medium.dark");
    textColor = GetColor("tool.bar.tool.button.text");
    separatorColor1 = GetColor("tool.bar.tool.button.separator1");
    separatorColor2 = GetColor("tool.bar.tool.button.separator2");
    pens.reset(new ToolBarPens(this));
}

void ToolBar::OnPaint(PaintEventArgs& args)
{
    if (!ToolTipWindowAdded())
    {
        SetToolTipWindowAdded();
        Window* window = GetWindow();
        if (window)
        {
            window->AddChild(toolTipWindow);
        }
    }
    args.graphics.Clear(BackgroundColor());
    Point origin(toolBarPadding.left, toolBarPadding.top);
    Component* component = toolButtons.FirstChild();
    while (component)
    {
        if (component->IsToolButtonBase())
        {
            ToolButtonBase* toolButton = static_cast<ToolButtonBase*>(component);
            Size toolButtonSize = toolButton->GetSize();
            Point loc = origin;
            loc.X = loc.X + toolButtonPadding.left;
            loc.Y = loc.Y + toolButtonPadding.top + (toolButtonHeight - toolButtonSize.Height) / 2;
            toolButton->SetLocation(loc);
            toolButton->Draw(args.graphics, origin);
            origin.X = origin.X + toolButtonSize.Width + toolButtonPadding.Horizontal();
        }
        component = component->NextSibling();
    }
    Control::OnPaint(args);
}

void ToolBar::OnMouseEnter()
{
    Control::OnMouseEnter();
    mouseInToolButton = nullptr;
    mouseDownToolButton = nullptr;
}

void ToolBar::OnMouseLeave()
{
    Control::OnMouseLeave();
    if (mouseInToolButton)
    {
        mouseInToolButton->MouseLeaveInternal();
        mouseInToolButton = nullptr;
    }
    mouseDownToolButton = nullptr;
    HideToolTipWindow();
}

void ToolBar::OnMouseMove(MouseEventArgs& args)
{
    Control::OnMouseMove(args);
    ToolButton* toolButton = GetToolButtonAt(args.location);
    if (toolButton)
    {
        if (mouseInToolButton != toolButton)
        {
            if (mouseInToolButton)
            {
                mouseInToolButton->MouseLeaveInternal();
                mouseInToolButton = nullptr;
            }
            mouseInToolButton = toolButton;
            mouseInToolButton->MouseEnterInternal();
        }
        toolButton->MouseMoveInternal(args);
    }
}

void ToolBar::OnMouseHover(MouseEventArgs& args)
{
    Control::OnMouseHover(args);
    ToolButton* toolButton = GetToolButtonAt(args.location);
    if (toolButton)
    {
        toolButton->MouseHoverInternal(args);
    }
}

void ToolBar::OnMouseDown(MouseEventArgs& args)
{
    Control::OnMouseDown(args);
    HideToolTipWindow();
    switch (toolBarStyle)
    {
        case ToolBarStyle::regular:
        {
            ToolButton* toolButton = GetToolButtonAt(args.location);
            if (toolButton)
            {
                if (toolButton->IsEnabled())
                {
                    if (toolButton->State() == ToolButtonState::normal)
                    {
                        if (args.buttons == MouseButtons::lbutton)
                        {
                            mouseDownToolButton = toolButton;
                        }
                        toolButton->MouseDownInternal(args);
                        if (args.buttons == MouseButtons::lbutton)
                        {
                            toolButton->SetState(ToolButtonState::pressed);
                        }
                    }
                    else
                    {
                        return;
                    }
                }
            }
            SetCapture(Handle());
            break;
        }
        case ToolBarStyle::radioButtonGroup:
        {
            ToolButton* toolButton = GetToolButtonAt(args.location);
            if (toolButton)
            {
                if (toolButton->IsEnabled())
                {
                    if (args.buttons == MouseButtons::lbutton)
                    {
                        SetPressedToolButton(toolButton);
                        toolButton->DoClick();
                    }
                }
            }
            break;
        }
    }
}

void ToolBar::OnMouseUp(MouseEventArgs& args)
{
    Control::OnMouseUp(args);
    if (toolBarStyle == ToolBarStyle::regular)
    {
        ToolButton* toolButton = GetToolButtonAt(args.location);
        if (toolButton)
        {
            if (toolButton->IsEnabled())
            {
                if (toolButton->State() == ToolButtonState::pressed)
                {
                    toolButton->MouseUpInternal(args);
                    if (args.buttons == MouseButtons::lbutton)
                    {
                        if (toolButton == mouseDownToolButton)
                        {
                            toolButton->DoClick();
                        }
                    }
                }
            }
        }
        if (mouseDownToolButton && mouseDownToolButton->Style() == ToolButtonStyle::automatic)
        {
            mouseDownToolButton->SetState(ToolButtonState::normal);
        }
        mouseDownToolButton = nullptr;
        ReleaseCapture();
    }
}

ToolButtonBitmaps& ToolBar::GetOrCreateToolButtonBitmaps(const std::string& toolButtonBitmapName)
{
    auto it = bitmapMap.find(toolButtonBitmapName);
    if (it != bitmapMap.cend())
    {
        return it->second;
    }
    std::u16string bitmapName = ToUtf16(toolButtonBitmapName);
    std::unique_ptr<Bitmap> normalBitmap(Bitmap::FromResource(Instance(), (const WCHAR*)bitmapName.c_str()));
    std::unique_ptr<Bitmap> disabledBitmap(ToGrayBitmap(normalBitmap.get(), DefaultBitmapTransparentColor()));
    ToolButtonBitmaps toolButtonBitmaps(normalBitmap.get(), disabledBitmap.get());
    bitmapMap[toolButtonBitmapName] = toolButtonBitmaps;
    bitmaps.push_back(std::unique_ptr<Bitmap>(normalBitmap.release()));
    bitmaps.push_back(std::unique_ptr<Bitmap>(disabledBitmap.release()));
    return bitmapMap[toolButtonBitmapName];
}

ToolButtonBase::ToolButtonBase() : 
    Component(), 
    location(),
    size()
{
}

ToolBar* ToolButtonBase::GetToolBar()
{
    Container* container = GetContainer();
    if (container)
    {
        Component* parent = container->Parent();
        if (parent)
        {
            if (parent->IsToolBar())
            {
                return static_cast<ToolBar*>(parent);
            }
        }
    }
    return nullptr;
}

void ToolButtonBase::SetLocation(const Point& location_)
{
    location = location_;
}

void ToolButtonBase::SetSize(const Size& size_)
{
    size = size_;
}

ToolButtonCreateParams::ToolButtonCreateParams() :
    toolBitMapName(),
    style(ToolButtonStyle::automatic),
    padding(3, 3, 3, 3)
{
}

ToolButtonCreateParams& ToolButtonCreateParams::Defaults()
{
    return *this;
}

ToolButtonCreateParams& ToolButtonCreateParams::ToolBitMapName(const std::string& toolBitMapName_)
{
    toolBitMapName = toolBitMapName_;
    return *this;
}

ToolButtonCreateParams& ToolButtonCreateParams::Style(ToolButtonStyle style_)
{
    style = style_;
    return *this;
}

ToolButtonCreateParams& ToolButtonCreateParams::SetPadding(const Padding& padding_)
{
    padding = padding_;
    return *this;
}

ToolButtonCreateParams& ToolButtonCreateParams::SetToolTip(const std::string& toolTip_)
{
    toolTip = toolTip_;
    return *this;
}

ToolButton::ToolButton(ToolButtonCreateParams& createParams) :
    toolBitMapName(createParams.toolBitMapName),
    state(ToolButtonState::normal),
    flags(ToolButtonFlags::none),
    style(createParams.style),
    padding(createParams.padding),
    toolTip(createParams.toolTip),
    normalBitmap(nullptr),
    disabledBitmap(nullptr),
    bitmapSize()
{
}

void ToolButton::SetBitmaps(Bitmap* normalBitmap_, Bitmap* disabledBitmap_)
{
    normalBitmap = normalBitmap_;
    disabledBitmap = disabledBitmap_;
    if (normalBitmap && disabledBitmap)
    {
        bitmapSize = Size(normalBitmap->GetWidth(), normalBitmap->GetHeight());
    }
}

void ToolButton::ComputeSize()
{
    SetSize(Size(padding.Horizontal() + bitmapSize.Width, padding.Vertical() + bitmapSize.Height));
}

void ToolButton::Draw(Graphics& graphics, const Point& origin)
{
    switch (state)
    {
        case ToolButtonState::normal:
        {
            DrawNormal(graphics, origin);
            break;
        }
        case ToolButtonState::pressed:
        {
            DrawPressed(graphics, origin);
            break;
        }
    }
}

void ToolButton::Invalidate()
{
    ToolBar* toolBar = GetToolBar();
    if (toolBar)
    {
        toolBar->Invalidate();
    }
}

void ToolButton::OnClick()
{
    click.Fire();
}

void ToolButton::OnMouseEnter()
{
    mouseEnter.Fire();
}

void ToolButton::OnMouseLeave()
{
    mouseLeave.Fire();
}

void ToolButton::OnMouseMove(MouseEventArgs& args)
{
    mouseMove.Fire(args);
}

void ToolButton::OnMouseHover(MouseEventArgs& args)
{
    mouseHover.Fire(args);
}

void ToolButton::OnMouseDown(MouseEventArgs& args)
{
    mouseDown.Fire(args);
}

void ToolButton::OnMouseUp(MouseEventArgs& args)
{
    mouseUp.Fire(args);
}

struct ClickGuard
{
    ClickGuard(ToolButton* button_) : button(button_)
    {
    }
    ~ClickGuard()
    {
        if (button->Style() == ToolButtonStyle::automatic)
        {
            button->SetState(ToolButtonState::normal);
        }
    }
    ToolButton* button;
};

void ToolButton::DoClick()
{
    ClickGuard clickGuard(this);
    OnClick();
}

void ToolButton::MouseEnterInternal()
{
    OnMouseEnter();
}

void ToolButton::MouseLeaveInternal()
{
    OnMouseLeave();
    if (!toolTip.empty())
    {
        ToolBar* toolBar = GetToolBar();
        if (toolBar)
        {
            toolBar->HideToolTipWindow();
        }
    }
}

void ToolButton::MouseMoveInternal(MouseEventArgs& args)
{
    OnMouseMove(args);
}

void ToolButton::MouseHoverInternal(MouseEventArgs& args)
{
    OnMouseHover(args);
    if (!toolTip.empty())
    {
        ToolBar* toolBar = GetToolBar();
        if (toolBar)
        {
            toolBar->ShowToolTipWindow(this);
        }
    }
}

void ToolButton::MouseDownInternal(MouseEventArgs& args)
{
    OnMouseDown(args);
}

void ToolButton::MouseUpInternal(MouseEventArgs& args)
{
    OnMouseUp(args);
}

void ToolButton::DrawNormal(Graphics& graphics, const Point& origin)
{
    ToolBar* toolBar = GetToolBar();
    if (!toolBar) return;
    Size size = GetSize();
    Rect rect(origin, size);
    rect.Width = rect.Width - 1;
    rect.Height = rect.Height - 1;
    Pen* darkPen = toolBar->Pens().DarkPen();
    CheckGraphicsStatus(graphics.DrawRectangle(darkPen, rect));

    Point startRightInnerDarkLoc = origin;
    startRightInnerDarkLoc.X = startRightInnerDarkLoc.X + size.Width - 2;
    Point endRightInnerDarkLoc = origin;
    endRightInnerDarkLoc.X = endRightInnerDarkLoc.X + size.Width - 2;
    endRightInnerDarkLoc.Y = endRightInnerDarkLoc.Y + size.Height - 1;
    CheckGraphicsStatus(graphics.DrawLine(darkPen, startRightInnerDarkLoc, endRightInnerDarkLoc));

    Point startBottomInnerDarkLoc = origin;
    startBottomInnerDarkLoc.X = startBottomInnerDarkLoc.X + 1;
    startBottomInnerDarkLoc.Y = startBottomInnerDarkLoc.Y + size.Height - 2;
    Point endBottomInnerDarkLoc = origin;
    endBottomInnerDarkLoc.X = endBottomInnerDarkLoc.X + size.Width - 2;
    endBottomInnerDarkLoc.Y = endBottomInnerDarkLoc.Y + size.Height - 2;
    CheckGraphicsStatus(graphics.DrawLine(darkPen, startBottomInnerDarkLoc, endBottomInnerDarkLoc));

    Pen* lightPen = toolBar->Pens().LightPen();
    Point startTopOuterLightLoc = origin;
    startTopOuterLightLoc.X = startTopOuterLightLoc.X + 1;
    startTopOuterLightLoc.Y = startTopOuterLightLoc.Y + 1;
    Point endTopOuterLightLoc = origin;
    endTopOuterLightLoc.X = endTopOuterLightLoc.X + size.Width - 3;
    endTopOuterLightLoc.Y = endTopOuterLightLoc.Y + 1;
    CheckGraphicsStatus(graphics.DrawLine(lightPen, startTopOuterLightLoc, endTopOuterLightLoc));

    Point startLeftOuterLightLoc = origin;
    startLeftOuterLightLoc.X = startLeftOuterLightLoc.X + 1;
    startLeftOuterLightLoc.Y = startLeftOuterLightLoc.Y + 1;
    Point endLeftOuterLightLoc = origin;
    endLeftOuterLightLoc.X = endLeftOuterLightLoc.X + 1;
    endLeftOuterLightLoc.Y = endLeftOuterLightLoc.Y + size.Height - 3;
    CheckGraphicsStatus(graphics.DrawLine(lightPen, startLeftOuterLightLoc, endLeftOuterLightLoc));

    Pen* mediumLightPen = toolBar->Pens().MediumLightPen();
    Point startTopInnerLightLoc = origin;
    startTopInnerLightLoc.X = startTopInnerLightLoc.X + 2;
    startTopInnerLightLoc.Y = startTopInnerLightLoc.Y + 2;
    Point endTopInnerLightLoc = origin;
    endTopInnerLightLoc.X = endTopInnerLightLoc.X + size.Width - 4;
    endTopInnerLightLoc.Y = endTopInnerLightLoc.Y + 2;
    CheckGraphicsStatus(graphics.DrawLine(mediumLightPen, startTopInnerLightLoc, endTopInnerLightLoc));

    Point startLeftInnerLightLoc = origin;
    startLeftInnerLightLoc.X = startLeftInnerLightLoc.X + 2;
    startLeftInnerLightLoc.Y = startLeftInnerLightLoc.Y + 2;
    Point endLeftInnerLightLoc = origin;
    endLeftInnerLightLoc.X = endLeftInnerLightLoc.X + 2;
    endLeftInnerLightLoc.Y = endLeftInnerLightLoc.Y + size.Height - 4;
    CheckGraphicsStatus(graphics.DrawLine(mediumLightPen, startLeftInnerLightLoc, endLeftInnerLightLoc));

    Pen* mediumDarkPen = toolBar->Pens().MediumDarkPen();
    Point startRightInnerMediumDarkLoc = origin;
    startRightInnerMediumDarkLoc.X = startRightInnerMediumDarkLoc.X + size.Width - 3;
    startRightInnerMediumDarkLoc.Y = startRightInnerMediumDarkLoc.Y + 2;
    Point endRightInnerMediumDarkLoc = origin;
    endRightInnerMediumDarkLoc.X = endRightInnerMediumDarkLoc.X + size.Width - 3;
    endRightInnerMediumDarkLoc.Y = endRightInnerMediumDarkLoc.Y + size.Height - 3;
    CheckGraphicsStatus(graphics.DrawLine(mediumDarkPen, startRightInnerMediumDarkLoc, endRightInnerMediumDarkLoc));

    Point startBottomInnerMediumDarkLoc = origin;
    startBottomInnerMediumDarkLoc.X = startBottomInnerMediumDarkLoc.X + 2;
    startBottomInnerMediumDarkLoc.Y = startBottomInnerMediumDarkLoc.Y + size.Height - 3;
    Point endBottomInnerMediumDarkLoc = origin;
    endBottomInnerMediumDarkLoc.X = endBottomInnerMediumDarkLoc.X + size.Width - 3;
    endBottomInnerMediumDarkLoc.Y = endBottomInnerMediumDarkLoc.Y + size.Height - 3;
    CheckGraphicsStatus(graphics.DrawLine(mediumDarkPen, startBottomInnerMediumDarkLoc, endBottomInnerMediumDarkLoc));

    DrawContent(graphics, origin);
}

void ToolButton::DrawPressed(Graphics& graphics, const Point& origin)
{
    ToolBar* toolBar = GetToolBar();
    if (!toolBar) return;
    Size size = GetSize();
    Rect rect(origin, size);
    rect.Width = rect.Width - 1;
    rect.Height = rect.Height - 1;
    Pen* darkPen = toolBar->Pens().DarkPen();
    CheckGraphicsStatus(graphics.DrawRectangle(darkPen, rect));

    Point startTopInnerDarkLoc = origin;
    startTopInnerDarkLoc.X = startTopInnerDarkLoc.X + 1;
    startTopInnerDarkLoc.Y = startTopInnerDarkLoc.Y + 1;
    Point endTopInnerDarkLoc = origin;
    endTopInnerDarkLoc.X = endTopInnerDarkLoc.X + size.Width - 2;
    endTopInnerDarkLoc.Y = endTopInnerDarkLoc.Y + 1;
    CheckGraphicsStatus(graphics.DrawLine(darkPen, startTopInnerDarkLoc, endTopInnerDarkLoc));

    Point startLeftInnerDarkLoc = origin;
    startLeftInnerDarkLoc.X = startLeftInnerDarkLoc.X + 1;
    startLeftInnerDarkLoc.Y = startLeftInnerDarkLoc.Y + 2;
    Point endLeftInnerDarkLoc = origin;
    endLeftInnerDarkLoc.X = endLeftInnerDarkLoc.X + 1;
    endLeftInnerDarkLoc.Y = endLeftInnerDarkLoc.Y + size.Height - 2;
    CheckGraphicsStatus(graphics.DrawLine(darkPen, startLeftInnerDarkLoc, endLeftInnerDarkLoc));

    Pen* mediumDarkPen = toolBar->Pens().MediumDarkPen();
    Point startTopInnerMediumDarkLoc = origin;
    startTopInnerMediumDarkLoc.X = startTopInnerMediumDarkLoc.X + 2;
    startTopInnerMediumDarkLoc.Y = startTopInnerMediumDarkLoc.Y + 2;
    Point endTopInnerMediumDarkLoc = origin;
    endTopInnerMediumDarkLoc.X = endTopInnerMediumDarkLoc.X + size.Width - 2;
    endTopInnerMediumDarkLoc.Y = endTopInnerMediumDarkLoc.Y + 2;
    CheckGraphicsStatus(graphics.DrawLine(mediumDarkPen, startTopInnerMediumDarkLoc, endTopInnerMediumDarkLoc));

    Point startLeftInnerMediumDarkLoc = origin;
    startLeftInnerMediumDarkLoc.X = startLeftInnerMediumDarkLoc.X + 2;
    startLeftInnerMediumDarkLoc.Y = startLeftInnerMediumDarkLoc.Y + 2;
    Point endLeftInnerMediumDarkLoc = origin; 
    endLeftInnerMediumDarkLoc.X = endLeftInnerMediumDarkLoc.X + 2;
    endLeftInnerMediumDarkLoc.Y = endLeftInnerMediumDarkLoc.Y + size.Height - 2;
    CheckGraphicsStatus(graphics.DrawLine(mediumDarkPen, startLeftInnerMediumDarkLoc, endLeftInnerMediumDarkLoc));

    DrawContent(graphics, origin);
}

void ToolButton::DrawContent(Graphics& graphics, const Point& origin)
{
    if (!normalBitmap || !disabledBitmap) return;
    ToolBar* toolBar = GetToolBar();
    if (!toolBar) return;
    Point bitmapLocation = origin;
    bitmapLocation.X = bitmapLocation.X + padding.left;
    bitmapLocation.Y = bitmapLocation.Y + padding.top;
    Rect r(bitmapLocation, bitmapSize);
    Gdiplus::ImageAttributes attributes;
    attributes.SetColorKey(toolBar->TransparentColor(), toolBar->TransparentColor());
    if (IsEnabled())
    {
        CheckGraphicsStatus(graphics.DrawImage(normalBitmap, r, 0, 0, bitmapSize.Width, bitmapSize.Height, Unit::UnitPixel, &attributes));
    }
    else
    {
        CheckGraphicsStatus(graphics.DrawImage(disabledBitmap, r, 0, 0, bitmapSize.Width, bitmapSize.Height, Unit::UnitPixel, &attributes));
    }
}

void ToolButton::Enable()
{
    if (!IsEnabled())
    {
        flags = flags & ~ToolButtonFlags::disabled;
        Invalidate();
    }
}

void ToolButton::Disable()
{
    if (!IsDisabled())
    {
        flags = flags | ToolButtonFlags::disabled;
        Invalidate();
    }
}

void ToolButton::SetToolTip(const std::string& toolTip_)
{
    if (toolTip != toolTip_)
    {
        toolTip = toolTip_;
        Invalidate();
    }
}

void ToolButton::SetState(ToolButtonState state_)
{
    if (state != state_)
    {
        ToolBar* toolBar = GetToolBar();
        if (toolBar)
        {
            if (toolBar->Style() == ToolBarStyle::radioButtonGroup)
            {
                if (state_ == ToolButtonState::pressed)
                {
                    if (toolBar->PressedToolButton() != this)
                    {
                        toolBar->SetPressedToolButton(this);
                    }
                }
                else if (state_ == ToolButtonState::normal)
                {
                    if (toolBar->PressedToolButton() == this)
                    {
                        toolBar->ResetPressedToolButton();
                    }
                }
            }
        }
        state = state_;
        Invalidate();
    }
}

TextToolButtonCreateParams::TextToolButtonCreateParams(const std::string& text_) : toolButtonCreateParams(ToolButtonCreateParams().Defaults()), text(text_), size(12, 12)
{
}

TextToolButtonCreateParams& TextToolButtonCreateParams::Style(ToolButtonStyle style_)
{
    toolButtonCreateParams.Style(style_);
    return *this;
}

TextToolButtonCreateParams& TextToolButtonCreateParams::SetPadding(const Padding& padding_)
{
    toolButtonCreateParams.SetPadding(padding_);
    return *this;
}

TextToolButtonCreateParams& TextToolButtonCreateParams::SetToolTip(const std::string& toolTip_)
{
    toolButtonCreateParams.SetToolTip(toolTip_);
    return *this;
}

TextToolButtonCreateParams& TextToolButtonCreateParams::SetSize(const Size& size_)
{
    size = size_;
    return *this;
}

TextToolButton::TextToolButton(TextToolButtonCreateParams& createParams) : ToolButton(createParams.toolButtonCreateParams), text(createParams.text)
{
    SetSize(createParams.size);
}

void TextToolButton::ComputeSize()
{
    Size size = GetSize();
    Padding padding = GetPadding();
    SetSize(Size(padding.Horizontal() + size.Width, padding.Vertical() + size.Height));
}

void TextToolButton::DrawContent(Graphics& graphics, const Point& origin)
{
    ToolBar* toolBar = GetToolBar();
    if (!toolBar) return;
    Gdiplus::TextRenderingHint prevRenderingHint = graphics.GetTextRenderingHint();
    CheckGraphicsStatus(graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit));
    Brush* brush = toolBar->NormalTextBrush();
    if (IsDisabled())
    {
        brush = toolBar->DisabledTextBrush();
    }
    Size size = GetSize();
    RectF r(PointF(origin.X, origin.Y), SizeF(size.Width, size.Height));
    DrawString(graphics, text, toolBar->GetFont(), r, toolBar->CenterFormat(), *brush);
    CheckGraphicsStatus(graphics.SetTextRenderingHint(prevRenderingHint));
}

ToolButtonSeparator::ToolButtonSeparator()
{
}

void ToolButtonSeparator::ComputeSize()
{
    ToolBar* toolBar = GetToolBar();
    if (toolBar)
    {
        SetSize(Size(2, toolBar->ToolButtonHeight() - 4));
    }
}

void ToolButtonSeparator::Draw(Graphics& graphics, const Point& origin)
{
    ToolBar* toolBar = GetToolBar();
    if (!toolBar) return;
    Point loc = Location();
    Size size = GetSize();
    Point firstLineStart = origin;
    Point firstLineEnd = origin;
    firstLineEnd.Y = firstLineEnd.Y + size.Height;
    Pen* firstLinePen = toolBar->Pens().SeparatorPen1();
    CheckGraphicsStatus(graphics.DrawLine(firstLinePen, firstLineStart, firstLineEnd));
    Point secondLineStart = origin;
    secondLineStart.X = secondLineStart.X + 1;
    secondLineStart.Y = secondLineStart.Y + loc.Y;
    Point secondLineEnd = origin;
    secondLineEnd.X = secondLineEnd.X + 1;
    secondLineEnd.Y = secondLineEnd.Y + loc.Y + size.Height;
    Pen* secondLinePen = toolBar->Pens().SeparatorPen2();
    CheckGraphicsStatus(graphics.DrawLine(secondLinePen, secondLineStart, secondLineEnd));
}

} } // cmajor::wing
