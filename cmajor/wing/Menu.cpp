// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/Menu.hpp>
#include <wing/ContainerControl.hpp>
#include <soulng/util/Unicode.hpp>
#include <algorithm>

#undef min
#undef max

namespace cmajor { namespace wing {

using namespace soulng::unicode;

Color DefaultMenuBackgroundColor()
{
    return GetSystemColor(COLOR_MENU);
}

Color DefaultMenuTextColor()
{
    return GetSystemColor(COLOR_MENUTEXT);
}

Color DefaultDisabledMenuTextColor()
{
    return GetSystemColor(COLOR_GRAYTEXT);
}

Color DefaultMenuMouseOverColor()
{
    return Color(201, 222, 245);
}

Color DefaultMenuOpenColor()
{
    return Color::White;
}

Color DefaultMenuShadowColor()
{
    Color shadowColor = GetSystemColor(COLOR_BTNSHADOW);
    shadowColor = Color(196, shadowColor.GetRed(), shadowColor.GetBlue(), shadowColor.GetGreen());
    return shadowColor;
}

MenuControl::MenuControl(ControlCreateParams& createParams) :
    Control(createParams),
    textColor(DefaultMenuTextColor()),
    disabledTextColor(DefaultDisabledMenuTextColor()),
    mouseOverColor(DefaultMenuMouseOverColor()),
    menuOpenColor(DefaultMenuOpenColor()),
    shadowColor(DefaultMenuShadowColor()),
    textBrush(textColor),
    disabledTextBrush(disabledTextColor),
    backgroundBrush(BackgroundColor()),
    mouseOverBrush(mouseOverColor),
    menuOpenBrush(menuOpenColor),
    shadowBrush(shadowColor),
    blackBrush(Color::Black),
    blackPen(Color::Black),
    darkPen(Color::DarkGray),
    stringFormat()
{
    stringFormat.SetAlignment(StringAlignment::StringAlignmentNear);
    stringFormat.SetLineAlignment(StringAlignment::StringAlignmentNear);
    stringFormat.SetHotkeyPrefix(HotKeyPrefix::HotkeyPrefixShow);
    SetFont(Font(FontFamily(L"Segoe UI"), 9.0f, FontStyle::FontStyleRegular, Unit::UnitPoint));
}

MenuBar::MenuBar() : MenuControl(ControlCreateParams().WindowClassName("wing.MenuBar").WindowClassBackgroundColor(COLOR_MENU).WindowStyle(DefaultChildWindowStyle()).
    BackgroundColor(DefaultMenuBackgroundColor()).SetDock(Dock::top).SetSize(Size(0, 20))), flags(MenuFlags::none), children(this), latestOpenedMenuItem(nullptr), selectedMenuItem(nullptr), 
    latestMouseDownMenuItem(nullptr), menuBox(nullptr)
{
    SetDoubleBuffered();
    SetMenuChanged();
}

void MenuBar::AddMenuItem(MenuItem* menuItem)
{
    children.AddChild(menuItem);
    SetMenuChanged();
}

MenuItem* MenuBar::GetFirstMenuItem() const
{
    Component* child = children.FirstChild();
    while (child)
    {
        if (child->IsMenuItem())
        {
            return static_cast<MenuItem*>(child);
        }
        child = child->NextSibling();
    }
    return nullptr;
}

MenuItem* MenuBar::GetLastMenuItem() const
{
    Component* child = children.LastChild();
    while (child)
    {
        if (child->IsMenuItem())
        {
            return static_cast<MenuItem*>(child);
        }
        child = child->PrevSibling();
    }
    return nullptr;
}

bool MenuBar::IsOpen() const
{
    return (flags & MenuFlags::open) != MenuFlags::none;
}

void MenuBar::SetOpen()
{
    flags = flags | MenuFlags::open;
}

void MenuBar::SetClosed()
{
    if (IsOpen())
    {
        flags = flags & ~MenuFlags::open;
        InvalidateMenu();
    }
}

void MenuBar::InvalidateMenu()
{
    Invalidate();
    Rect menuRect;
    Component* child = children.FirstChild();
    while (child)
    {
        if (child->IsMenuItem())
        {
            MenuItem* menuItem = static_cast<MenuItem*>(child);
            if (menuItem->State() == MenuItemState::open)
            {
                menuItem->GetOpenRect(menuRect);
            }
        }
        child = child->NextSibling();
    }
    if (IsOpen())
    {
        if (menuBox)
        {
            menuBox->SetPaintThisMenuBox();
            Point loc = Location();
            menuBox->SetLocation(Point(loc.X + menuRect.X, loc.Y + menuRect.Y));
            Size sz;
            menuRect.GetSize(&sz);
            menuBox->SetSize(sz);
            Control* parentControl = ParentControl();
            Control* topControl = nullptr;
            if (parentControl)
            {
                topControl = parentControl->TopControl();
            }
            menuBox->BringToFront();
            menuBox->Show();
            menuBox->Invalidate();
            menuBox->Update();
            if (topControl)
            {
                topControl->BringToFront();
            }
        }
    }
    else
    {
        if (menuBox)
        {
            menuBox->ResetPaintThisMenuBox();
            menuBox->Hide();
        }
        Control* parentControl = ParentControl();
        parentControl->Invalidate(menuRect);
        parentControl->Update();
    }
}

void MenuBar::SetMenuInvalidated()
{
    flags = flags | MenuFlags::menuInvalidated;
}

void MenuBar::SetSelectedMenuItem(MenuItem* selectedMenuItem_)
{
    selectedMenuItem = selectedMenuItem_;
}

MenuItem* MenuBar::OpenedMenuItem() const
{
    Component* child = children.FirstChild();
    while (child)
    {
        if (child->IsMenuItem())
        {
            MenuItem* menuItem = static_cast<MenuItem*>(child);
            if (menuItem->State() == MenuItemState::open)
            {
                return menuItem;
            }
        }
        child = child->NextSibling();
    }
    return nullptr;
}

void MenuBar::SetLatestOpenedMenuItem(MenuItem* menuItem)
{
    latestOpenedMenuItem = menuItem;
}

void MenuBar::SetLatestMouseDownMenuItem(MenuItem* menuItem)
{
    latestMouseDownMenuItem = menuItem;
}

void MenuBar::CloseMenu()
{
    if (!IsOpen()) return;
    SetClosed();
    SetLatestOpenedMenuItem(nullptr);
    Component* child = children.FirstChild();
    while (child)
    {
        if (child->IsMenuItem())
        {
            MenuItem* menuItem = static_cast<MenuItem*>(child);
            menuItem->SetState(MenuItemState::closed);
        }
        child = child->NextSibling();
    }
    InvalidateMenu();
}

void MenuBar::OnPaint(PaintEventArgs& args)
{
    try
    {
        if (!MenuBoxAdded())
        {
            SetMenuBoxAdded();
            AddMenuBox();
        }
        CheckGraphicsStatus(args.graphics.Clear(BackgroundColor()));
        if (MenuChanged())
        {
            ResetMenuChanged();
            CollectShortcuts();
            Size size = GetSize();
            size.Height = static_cast<int>(GetFont().NativeFont()->GetHeight(&args.graphics));
            size.Height = size.Height + DefaultPadding().Vertical();
            SetSize(size);
            LocateMenuItems(args.graphics, size);
        }
        DrawMenuItems(args, false, Location());
        MenuControl::OnPaint(args);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MenuBar::MouseDownInternal(MouseEventArgs& args)
{
    OnMouseDown(args);
}

void MenuBar::OnMouseDown(MouseEventArgs& args)
{
    MenuControl::OnMouseDown(args);
    SetLatestMouseDownMenuItem(nullptr);
    bool handled = false;
    Component* child = children.FirstChild();
    while (child)
    {
        if (child->IsMenuItem())
        {
            MenuItem* menuItem = static_cast<MenuItem*>(child);
            menuItem->DispatchMouseDown(args, handled);
            if (handled)
            {
                InvalidateMenu();
                return;
            }
        }
        child = child->NextSibling();
    }
    if (IsOpen())
    {
        SetClosed();
        SetLatestOpenedMenuItem(nullptr);
        Component* child = children.FirstChild();
        while (child)
        {
            if (child->IsMenuItem())
            {
                MenuItem* menuItem = static_cast<MenuItem*>(child);
                menuItem->SetState(MenuItemState::closed);
            }
            child = child->NextSibling();
        }
        InvalidateMenu();
    }
}

void MenuBar::MouseUpInternal(MouseEventArgs& args)
{
    OnMouseUp(args);
}

void MenuBar::OnMouseUp(MouseEventArgs& args)
{
    MenuControl::OnMouseUp(args);
    bool handled = false;
    Component* child = children.FirstChild();
    while (child)
    {
        if (child->IsMenuItem())
        {
            MenuItem* menuItem = static_cast<MenuItem*>(child);
            if (menuItem->IsEnabled())
            {
                menuItem->DispatchMouseUp(args, handled);
                if (handled)
                {
                    InvalidateMenu();
                    return;
                }
            }
        }
        child = child->NextSibling();
    }
}

void MenuBar::MouseMoveInternal(MouseEventArgs& args)
{
    OnMouseMove(args);
}

void MenuBar::OnMouseMove(MouseEventArgs& args)
{
    MenuControl::OnMouseMove(args);
    ResetMenuInvalidated();
    bool handled = false;
    Component* child = children.FirstChild();
    while (child)
    {
        if (child->IsMenuItem())
        {
            MenuItem* menuItem = static_cast<MenuItem*>(child);
            if (!menuItem->Contains(args.location))
            {
                menuItem->ResetSelected();
                if (menuItem->MouseInClient())
                {
                    menuItem->ResetMouseInClient();
                    menuItem->DoMouseLeave();
                }
            }
        }
        child = child->NextSibling();
    }
    child = children.FirstChild();
    while (child)
    {
        if (child->IsMenuItem())
        {
            MenuItem* menuItem = static_cast<MenuItem*>(child);
            if (menuItem->Contains(args.location))
            {
                menuItem->SetSelected();
                if (!menuItem->MouseInClient())
                {
                    menuItem->SetMouseInClient();
                    menuItem->DoMouseEnter(IsOpen());
                }
                else
                {
                    menuItem->DoMouseMove(args);
                }
                handled = true;
            }
        }
        child = child->NextSibling();
    }
    if (!handled)
    {
        if (IsOpen())
        {
            if (latestOpenedMenuItem)
            {
                Component* child = children.FirstChild();
                while (child && !handled)
                {
                    if (child->IsMenuItem())
                    {
                        MenuItem* menuItem = static_cast<MenuItem*>(child);
                        if (menuItem->IsSameOrParentOf(latestOpenedMenuItem))
                        {
                            menuItem->DispatchMouseMove(args, handled);
                        }
                    }
                    child = child->NextSibling();
                }
            }
        }
    }
    if (MenuInvalidated())
    {
        InvalidateMenu();
    }
}

void MenuBar::MouseEnterInternal()
{
    OnMouseEnter();
}

void MenuBar::MouseLeaveInternal()
{
    OnMouseLeave();
}

void MenuBar::OnMouseLeave()
{
    MenuControl::OnMouseLeave();
    ResetMenuInvalidated();
    Component* child = children.FirstChild();
    while (child)
    {
        if (child->IsMenuItem())
        {
            MenuItem* menuItem = static_cast<MenuItem*>(child);
            if (menuItem->MouseInClient() && menuItem->Children().IsEmpty())
            {
                menuItem->ResetSelected();
                menuItem->ResetMouseInClient();
                Point mousePos = GetMessagePos();
                mousePos = ScreenToClient(mousePos);
                if (menuItem != latestOpenedMenuItem && !menuItem->UnionRect().Contains(mousePos))
                {
                    menuItem->DoMouseLeave();
                }
            }
            else
            {
                menuItem->ResetSelected();
                menuItem->ResetMouseInClient();
            }
        }
        child = child->NextSibling();
    }
    if (MenuInvalidated())
    {
        InvalidateMenu();
    }
}

void MenuBar::OnKeyDown(KeyEventArgs& args)
{
    MenuControl::OnKeyDown(args);
    if (!args.handled)
    {
        auto it = shortcuts.find(int(args.key));
        if (it != shortcuts.cend())
        {
            MenuItem* menuItem = it->second;
            if (menuItem->IsEnabled())
            {
                CloseMenu();
                menuItem->DoClick();
                args.handled = true;
            }
        }
    }
}

void MenuBar::DrawMenuItems(PaintEventArgs& args, bool drawSubitems, const Point& origin)
{
    Component* child = children.FirstChild();
    while (child)
    {
        if (child->IsMenuItem())
        {
            MenuItem* menuItem = static_cast<MenuItem*>(child);
            menuItem->Draw(args.graphics, drawSubitems, origin);
        }
        child = child->NextSibling();
    }
}

bool MenuBar::HandleAccessKey(char16_t accessKey, Keys keyCode, bool& menuWantsKeys)
{
    ResetMenuInvalidated();
    if (accessKey == char16_t() && keyCode == Keys::none)
    {
        if (!selectedMenuItem)
        {
            MenuItem* firstMenuItem = GetFirstMenuItem();
            if (firstMenuItem)
            {
                SetOpen();
                SetSelectedMenuItem(firstMenuItem);
                menuWantsKeys = true;
                InvalidateMenu();
                return true;
            }
        }
        else
        {
            SetClosed();
            MenuItem* openedMenuItem = OpenedMenuItem();
            if (openedMenuItem)
            {
                openedMenuItem->SetState(MenuItemState::closed);
            }
            selectedMenuItem->ResetSelected();
            SetSelectedMenuItem(nullptr);
            menuWantsKeys = false;
            InvalidateMenu();
            return true;
        }
    }
    else if (accessKey != char16_t() && keyCode == Keys::none)
    {
        MenuItem* menuItem = GetMenuItemByAccessKey(accessKey);
        if (menuItem)
        {
            MenuItem* firstChild = menuItem->GetFirstMenuItem();
            if (firstChild)
            {
                if (selectedMenuItem)
                {
                    MenuItem* openedMenuItem = OpenedMenuItem();
                    if (openedMenuItem)
                    {
                        openedMenuItem->SetState(MenuItemState::closed);
                    }
                }
                SetOpen();
                menuItem->SetState(MenuItemState::open);
                SetSelectedMenuItem(firstChild);
                menuWantsKeys = true;
                InvalidateMenu();
                return true;
            }
        }
    }
    else if (accessKey == char16_t() && keyCode != Keys::none)
    {
        if (selectedMenuItem)
        {
            MenuItem* parentMenuItem = selectedMenuItem->ParentMenuItem();
            bool handled = selectedMenuItem->HandleKey(keyCode, menuWantsKeys, parentMenuItem);
            if (MenuInvalidated())
            {
                InvalidateMenu();
            }
            return handled;
        }
    }
    menuWantsKeys = false;
    return false;
}

void MenuBar::DoKeyDown(KeyEventArgs& args)
{
    OnKeyDown(args);
}

MenuItem* MenuBar::GetMenuItemByAccessKey(char16_t accessKey) const
{
    MenuItem* menuItem = GetFirstMenuItem();
    while (menuItem)
    {
        if (menuItem->AccessKey() == accessKey)
        {
            return menuItem;
        }
        menuItem = menuItem->GetNextMenuItem();
    }
    return nullptr;
}

void MenuBar::AddMenuBox()
{
    Control* parentControl = ParentControl();
    if (parentControl && parentControl->IsContainerControl())
    {
        ContainerControl* containerControl = static_cast<ContainerControl*>(parentControl);
        menuBox = new MenuBox(this, nullptr);
        containerControl->InsertChildAfter(menuBox, this);
    }
}

void MenuBar::LocateMenuItems(Graphics& graphics, const Size& size)
{
    Rect itemRect(Location(), Size(0, size.Height));
    Padding padding = DefaultPadding();
    PointF origin(0, 0);
    Component* child = children.FirstChild();
    while (child)
    {
        if (child->IsMenuItem())
        {
            MenuItem* menuItem = static_cast<MenuItem*>(child);
            Padding menuItemPadding = menuItem->DefaultPadding();
            RectF r = MeasureString(graphics, menuItem->Text(), GetFont(), origin, GetStringFormat());
            int w = static_cast<int>(r.Width) + padding.Horizontal() + menuItemPadding.Horizontal();
            itemRect.Width = w;
            Point loc;
            itemRect.GetLocation(&loc);
            menuItem->SetLocation(loc);
            Size sz;
            itemRect.GetSize(&sz);
            menuItem->SetSize(sz);
            menuItem->CalculateRects(graphics, GetFont(), GetStringFormat(), Point(itemRect.X, itemRect.Y + itemRect.Height));
            itemRect.X = itemRect.X + w;
        }
        child = child->NextSibling();
    }
}

void MenuBar::CollectShortcuts()
{
    shortcuts.clear();
    Component* child = children.FirstChild();
    while (child)
    {
        if (child->IsMenuItem())
        {
            MenuItem* menuItem = static_cast<MenuItem*>(child);
            menuItem->CollectShortcuts(shortcuts);
        }
        child = child->NextSibling();
    }
}

MenuBox::MenuBox(MenuBar* menuBar_, MenuItem* rootItem_) : MenuControl(ControlCreateParams().WindowClassName("wing.MenuBox").WindowClassBackgroundColor(COLOR_MENU).
    BackgroundColor(DefaultMenuBackgroundColor()).WindowStyle(HiddenChildWindowStyle())), menuBar(menuBar_), rootItem(rootItem_), paintThisMenuBox(false), paintMenu(false)
{
}

void MenuBox::OnPaint(PaintEventArgs& args)
{
    try
    {
        if (args.clipRect.IsEmptyArea()) return;
        Point loc = Location();
        if (!paintMenu)
        {
            if (paintThisMenuBox)
            {
                Control* parentControl = ParentControl();
                if (parentControl)
                {
                    Bitmap menuBoxBitmap(args.clipRect.Width, args.clipRect.Height, &args.graphics);
                    std::unique_ptr<Graphics> menuBoxGraphics(Graphics::FromImage(&menuBoxBitmap));
                    Rect menuBoxClipRect(Point(0, 0), Size(args.clipRect.Width, args.clipRect.Height));
                    CheckGraphicsStatus(menuBoxGraphics->SetClip(menuBoxClipRect));
                    CheckGraphicsStatus(menuBoxGraphics->TranslateTransform(-loc.X, -loc.Y));
                    PaintEventArgs paintMenuBoxArgs(*menuBoxGraphics, menuBoxClipRect);
                    bool prevPaintMenu = paintMenu;
                    paintMenu = true;
                    bool skipMenuBar = IsContextMenu();
                    parentControl->PaintAll(paintMenuBoxArgs, skipMenuBar);
                    paintMenu = prevPaintMenu;
                    CheckGraphicsStatus(args.graphics.DrawImage(&menuBoxBitmap, PointF(0, 0)));
                }
            }
        }
        else
        {
            if (menuBar)
            {
                menuBar->DrawMenuItems(args, true, loc);
            }
            else if (rootItem)
            {
                rootItem->Draw(args.graphics, true, Point());
            }
        }
    }
    catch (const std::exception&)
    {
    }
}

void MenuBox::OnMouseEnter()
{
    MenuControl::OnMouseEnter();
    if (menuBar)
    {
        menuBar->MouseEnterInternal();
    }
}

void MenuBox::OnMouseLeave()
{
    MenuControl::OnMouseLeave();
    if (menuBar)
    {
        menuBar->MouseLeaveInternal();
    }
    else if (rootItem)
    {
        rootItem->LeaveChildren();
    }
}

void MenuBox::OnMouseDown(MouseEventArgs& args)
{
    MenuControl::OnMouseDown(args);
    if (menuBar)
    {
        Point loc = Location();
        args.location.X = args.location.X + loc.X;
        args.location.Y = args.location.Y + loc.Y;
        menuBar->MouseDownInternal(args);
    }
    else if (rootItem)
    {
        SetLatestMouseDownMenuItem(nullptr);
        bool handled = false;
        rootItem->DispatchMouseDown(args, handled);
        if (handled)
        {
            Invalidate();
        }
    }
}

void MenuBox::OnMouseUp(MouseEventArgs& args)
{
    MenuControl::OnMouseUp(args);
    if (menuBar)
    {
        Point loc = Location();
        args.location.X = args.location.X + loc.X;
        args.location.Y = args.location.Y + loc.Y;
        menuBar->MouseUpInternal(args);
    }
    else if (rootItem)
    {
        bool handled = false;
        rootItem->DispatchMouseUp(args, handled);
        if (handled)
        {
            Invalidate();
        }
    }
}

void MenuBox::OnMouseMove(MouseEventArgs& args)
{
    MenuControl::OnMouseMove(args);
    if (menuBar)
    {
        Point loc = Location();
        args.location.X = args.location.X + loc.X;
        args.location.Y = args.location.Y + loc.Y;
        menuBar->MouseMoveInternal(args);
    }
    else if (rootItem)
    {
        bool handled = false;
        rootItem->DispatchMouseMove(args, handled);
    }
}

ContextMenu::ContextMenu() : MenuBox(nullptr, new MenuItem("root")), menuItems(this), latestOpenedMenuItem(nullptr), selectedMenuItem(nullptr), latestMouseDownMenuItem(nullptr)
{
    menuItems.AddChild(RootItem());
}

ContextMenu::~ContextMenu()
{
    int x = 0;
}

void ContextMenu::AddMenuItem(MenuItemBase* menuItem)
{
    RootItem()->AddMenuItem(menuItem);
}

void ContextMenu::CalculateSize()
{
    RootItem()->SetState(MenuItemState::open);
    Graphics graphics(Handle());
    RootItem()->CalculateRects(graphics, GetFont(), GetStringFormat(), Point(0, 0));
    Rect menuRect;
    RootItem()->GetOpenRect(menuRect);
    Size sz;
    menuRect.GetSize(&sz);
    SetSize(sz);
}

bool ContextMenu::HasMenuItems() const
{
    return !RootItem()->Children().IsEmpty();
}

void ContextMenu::SetMenuInvalidated()
{
    Invalidate();
}

bool ContextMenu::IsOpen() const
{
    return RootItem()->State() == MenuItemState::open;
}

MenuItem* ContextMenu::OpenedMenuItem() const
{
    return RootItem();
}

void ContextMenu::SetSelectedMenuItem(MenuItem* selectedMenuItem_)
{
    selectedMenuItem = selectedMenuItem_;
}

void ContextMenu::SetLatestOpenedMenuItem(MenuItem* menuItem) 
{
    latestOpenedMenuItem = menuItem;
}

void ContextMenu::SetLatestMouseDownMenuItem(MenuItem* menuItem)
{
    latestMouseDownMenuItem = menuItem;
}

void ContextMenu::OnPaint(PaintEventArgs& args)
{
    SetPaintThisMenuBox();
    MenuBox::OnPaint(args);
}

void ContextMenu::OnVisibleChanged()
{
    MenuBox::OnVisibleChanged();
    if (!IsVisible())
    {
        RootItem()->SetState(MenuItemState::closed);
        ResetPaintThisMenuBox();
    }
}

MenuItemBase::MenuItemBase() : location(), size()
{
}

MenuItem* MenuItemBase::ParentMenuItem() const
{
    Container* container = GetContainer();
    if (container)
    {
        Component* parent = container->Parent();
        if (parent)
        {
            if (parent->IsMenuItem())
            {
                return static_cast<MenuItem*>(parent);
            }
        }
    }
    return nullptr;
}

MenuControl* MenuItemBase::GetMenuControl() const
{
    Container* container = GetContainer();
    if (container)
    {
        Component* parent = container->Parent();
        if (parent)
        {
            if (parent->IsMenuItem())
            {
                MenuItem* parentMenuItem = static_cast<MenuItem*>(parent);
                return parentMenuItem->GetMenuControl();
            }
            else if (parent->IsMenuControl())
            {
                return static_cast<MenuControl*>(parent);
            }
        }
    }
    return nullptr;
}

int MenuItemBase::Level() const
{
    MenuItem* parent = ParentMenuItem();
    if (parent)
    {
        return parent->Level() + 1;
    }
    return 0;
}

bool MenuItemBase::Contains(const Point& pt) const
{
    Rect r(location, size);
    return r.Contains(pt);
}

MenuItem::MenuItem(const std::string& text_) : MenuItemBase(), text(text_), state(MenuItemState::closed), flags(MenuItemFlags::none), accessKey(), children(this), 
    shortcutFieldWidth(0), childIndicatorFieldWidth(0), shortcut(Keys::none)
{
    SetAccessKey();
}

void MenuItem::OnClick()
{
    click.Fire();
}

void MenuItem::OnMouseDown(MouseEventArgs& args)
{
    mouseDown.Fire(args);
}

void MenuItem::OnMouseUp(MouseEventArgs& args)
{
    mouseUp.Fire(args);
}

void MenuItem::OnMouseMove(MouseEventArgs& args)
{
    mouseMove.Fire(args);
}

void MenuItem::OnMouseEnter()
{
    mouseEnter.Fire();
}

void MenuItem::OnMouseLeave()
{
    mouseLeave.Fire();
}

void MenuItem::SetState(MenuItemState state_)
{
    if (state != state_)
    {
        state = state_;
        MenuControl* menuControl = GetMenuControl();
        if (menuControl)
        {
            menuControl->SetMenuInvalidated();
        }
    }
}

void MenuItem::SetSelected()
{
    if (!IsSelected())
    {
        flags = flags | MenuItemFlags::selected;
        MenuControl* menuControl = GetMenuControl();
        if (menuControl)
        {
            menuControl->SetMenuInvalidated();
        }
    }
}

void MenuItem::ResetSelected()
{
    if (IsSelected())
    {
        flags = flags & ~MenuItemFlags::selected;
        MenuControl* menuControl = GetMenuControl();
        if (menuControl)
        {
            menuControl->SetMenuInvalidated();
        }
    }
}

void MenuItem::Enable()
{
    if (IsDisabled())
    {
        flags = flags & ~MenuItemFlags::disabled;
        MenuControl* menuControl = GetMenuControl();
        if (menuControl)
        {
            menuControl->SetChanged();
            menuControl->SetMenuInvalidated();
        }
    }
}

void MenuItem::Disable()
{
    if (IsEnabled())
    {
        flags = flags | MenuItemFlags::disabled;
        MenuControl* menuControl = GetMenuControl();
        if (menuControl)
        {
            menuControl->SetChanged();
            menuControl->SetMenuInvalidated();
        }
    }
}

void MenuItem::SetMouseInClient()
{
    flags = flags | MenuItemFlags::mouseInClient;
}

void MenuItem::ResetMouseInClient()
{
    flags = flags & ~MenuItemFlags::mouseInClient;
}

void MenuItem::SetLButtonPressed()
{
    flags = flags | MenuItemFlags::lbuttonPressed;
}

void MenuItem::ResetLButtonPressed()
{
    flags = flags & ~MenuItemFlags::lbuttonPressed;
}

void MenuItem::DispatchMouseDown(MouseEventArgs& args, bool& handled)
{
    MenuControl* menuControl = GetMenuControl();
    if (Level() == 0)
    {
        if (Contains(args.location))
        {
            SetMouseInClient();
            if (menuControl->IsOpen())
            {
                menuControl->SetClosed();
                menuControl->SetLatestOpenedMenuItem(nullptr);
                SetState(MenuItemState::closed);
                DoMouseDown(args);
                handled = true;
            }
            else
            {
                menuControl->SetOpen();
                SetState(MenuItemState::open);
                menuControl->SetLatestOpenedMenuItem(this);
                DoMouseDown(args);
                handled = true;
            }
        }
        else
        {
            MenuItem* child = GetFirstMenuItem();
            while (child)
            {
                child->DispatchMouseDown(args, handled);
                if (handled) return;
                child = child->GetNextMenuItem();
            }
        }
    }
    else
    {
        if (menuControl->IsOpen())
        {
            MenuItem* parentMenuItem = ParentMenuItem();
            if (parentMenuItem)
            {
                if (parentMenuItem->state == MenuItemState::open)
                {
                    if (Contains(args.location))
                    {
                        if (!children.IsEmpty())
                        {
                            if (state == MenuItemState::closed)
                            {
                                SetState(MenuItemState::open);
                                menuControl->SetLatestOpenedMenuItem(this);
                                DoMouseDown(args);
                                handled = true;
                            }
                            else if (state == MenuItemState::open)
                            {
                                SetState(MenuItemState::closed);
                                menuControl->SetLatestOpenedMenuItem(nullptr);
                                DoMouseDown(args);
                                handled = true;
                            }
                        }
                        else
                        {
                            DoMouseDown(args);
                            if ((args.buttons & MouseButtons::lbutton) != MouseButtons::none)
                            {
                                SetLButtonPressed();
                            }
                            handled = true;
                        }
                    }
                }
            }
            MenuItem* child = GetFirstMenuItem();
            while (child)
            {
                child->DispatchMouseDown(args, handled);
                if (handled) return;
                child = child->GetNextMenuItem();
            }
        }
    }
}

void MenuItem::DispatchMouseUp(MouseEventArgs& args, bool& handled)
{
    if (Level() == 0)
    {
        if (Contains(args.location))
        {
            DoMouseUp(args);
            handled = true;
        }
        else
        {
            MenuItem* child = GetFirstMenuItem();
            while (child)
            {
                if (child->IsEnabled())
                {
                    child->DispatchMouseUp(args, handled);
                    if (handled) return;
                }
                child = child->GetNextMenuItem();
            }
        }
    }
    else
    {
        MenuControl* menuControl = GetMenuControl();
        if (menuControl->IsOpen())
        {
            MenuItem* parentMenuItem = ParentMenuItem();
            if (parentMenuItem)
            {
                if (parentMenuItem->state == MenuItemState::open)
                {
                    if (Contains(args.location))
                    {
                        DoMouseUp(args);
                        handled = true;
                        return;
                    }
                }
            }
            MenuItem* child = GetFirstMenuItem();
            while (child)
            {
                if (child->IsEnabled())
                {
                    child->DispatchMouseUp(args, handled);
                    if (handled) return;
                }
                child = child->GetNextMenuItem();
            }
        }
    }
}

void MenuItem::DispatchMouseMove(MouseEventArgs& args, bool& handled)
{
    MenuControl* menuControl = GetMenuControl();
    if (unionRect.Contains(args.location))
    {
        if (Contains(args.location))
        {
            SetSelected();
        }
        else
        {
            ResetSelected();
            ResetMouseInClient();
        }
        MenuItem* child = GetFirstMenuItem();
        while (child)
        {
            if (child->Contains(args.location))
            {
                child->SetSelected();
                if (!child->MouseInClient())
                {
                    child->SetMouseInClient();
                    child->DoMouseEnter(true);
                    handled = true;
                }
                else
                {
                    child->DoMouseMove(args);
                }
            }
            else
            {
                child->ResetSelected();
                if (child->MouseInClient())
                {
                    child->ResetMouseInClient();
                    if (!child->IsSameOrParentOf(menuControl->LatestOpenedMenuItem()))
                    {
                        child->DoMouseLeave();
                    }
                }
            }
            child = child->GetNextMenuItem();
        }
    }
    else
    {
        MenuItem* child = GetFirstMenuItem();
        while (child)
        { 
            if (child->IsSameOrParentOf(menuControl->LatestOpenedMenuItem()))
            {
                child->DispatchMouseMove(args, handled);
                if (handled) return;
            }
            child = child->GetNextMenuItem();
        }
    }
}

void MenuItem::DoMouseEnter(bool parentIsOpen)
{
    MenuControl* menuControl = GetMenuControl();
    if (parentIsOpen)
    {
        MenuItem* prevMenuItem = GetPrevMenuItem();
        while (prevMenuItem)
        {
            prevMenuItem->DoMouseLeave();
            prevMenuItem = prevMenuItem->GetPrevMenuItem();
        }
        MenuItem* nextMenuItem = GetNextMenuItem();
        while (nextMenuItem)
        {
            nextMenuItem->DoMouseLeave();
            nextMenuItem = nextMenuItem->GetNextMenuItem();
        }
        MenuItem* child = GetFirstMenuItem();
        while (child)
        {
            child->ResetSelected();
            if (child->MouseInClient())
            {
                child->ResetMouseInClient();
                child->DoMouseLeave();
            }
            child = child->GetNextMenuItem();
        }
        SetState(MenuItemState::open);
        menuControl->SetLatestOpenedMenuItem(this);
    }
    OnMouseEnter();
    menuControl->SetMenuInvalidated();
}

void MenuItem::DoMouseLeave()
{
    ResetLButtonPressed();
    if (state == MenuItemState::open)
    {
        SetState(MenuItemState::closed);
        LeaveChildren();
        OnMouseLeave();
    }
    else if (Level() == 0)
    {
        OnMouseLeave();
    }
}

void MenuItem::LeaveChildren()
{
    MenuItem* child = GetFirstMenuItem();
    while (child)
    {
        child->ResetSelected();
        child->ResetMouseInClient();
        child->DoMouseLeave();
        child->LeaveChildren();
        child = child->GetNextMenuItem();
    }
}

void MenuItem::Close()
{
    SetState(MenuItemState::closed);
    MenuItem* parentMenuItem = ParentMenuItem();
    while (parentMenuItem)
    {
        parentMenuItem->SetState(MenuItemState::closed);
        parentMenuItem = parentMenuItem->ParentMenuItem();
    }
    MenuControl* menuControl = GetMenuControl();
    if (menuControl)
    {
        menuControl->SetSelectedMenuItem(nullptr);
        menuControl->SetClosed();
        menuControl->SetMenuInvalidated();
    }
}

bool MenuItem::IsSameOrParentOf(MenuItem* menuItem) const
{
    if (this == menuItem) return true;
    MenuItem* parent = menuItem->ParentMenuItem();
    if (parent)
    {
        return IsSameOrParentOf(parent);
    }
    return false;
}

Padding MenuItem::DefaultPadding() const
{
    if (Level() == 0)
    {
        return Padding(4, 0, 4, 0);
    }
    else
    {
        return Padding(32, 4, 32, 4);
    }
}

Padding MenuItem::ShortcutPadding() const
{
    if (Level() == 0)
    {
        return Padding(0, 0, 0, 0);
    }
    else
    {
        return Padding(4, 0, 4, 0);
    }
}

void MenuItem::CalculateRects(Graphics& graphics, const Font& font, const StringFormat& stringFormat, const Point& location)
{
    childRect = Rect();
    childRect.X = location.X;
    childRect.Y = location.Y;
    shortcutFieldWidth = 0;
    childIndicatorFieldWidth = 0;
    Component* child = children.FirstChild();
    while (child)
    {
        if (child->IsMenuItemBase())
        {
            MenuItemBase* item = static_cast<MenuItemBase*>(child);
            Size size = item->MeasureItem(graphics, font, stringFormat, shortcutFieldWidth, childIndicatorFieldWidth);
            childRect.Width = std::max(childRect.Width, size.Width);
            childRect.Height = childRect.Height + size.Height;
        }
        child = child->NextSibling();
    }
    childRect.Width = childRect.Width + shortcutFieldWidth + childIndicatorFieldWidth;
    Point loc;
    childRect.GetLocation(&loc);
    Rect itemRect(loc, Size(childRect.Width, 0));
    child = children.FirstChild();
    while (child)
    {
        if (child->IsMenuItemBase())
        {
            MenuItemBase* item = static_cast<MenuItemBase*>(child);
            Point loc;
            itemRect.GetLocation(&loc);
            item->SetLocation(loc);
            itemRect.Height = item->GetSize().Height;
            Size sz;
            itemRect.GetSize(&sz);
            item->SetSize(sz);
            itemRect.Y = itemRect.Y + itemRect.Height;
        }
        child = child->NextSibling();
    }
    int shadowWidth = ShadowWidth();
    childRect.Height = childRect.Height + shadowWidth;
    childRect.Width = childRect.Width + shadowWidth;
    child = children.FirstChild();
    while (child)
    {
        if (child->IsMenuItem())
        {
            MenuItem* menuItem = static_cast<MenuItem*>(child);
            if (!menuItem->Children().IsEmpty())
            {
                Point itemLocation = menuItem->Location();
                Size itemSize = menuItem->GetSize();
                menuItem->CalculateRects(graphics, font, stringFormat, Point(itemLocation.X + itemSize.Width - shadowWidth, itemLocation.Y));
            }
        }
        child = child->NextSibling();
    }
    Rect r(Location(), GetSize());
    Rect::Union(unionRect, r, childRect);
}

Size MenuItem::MeasureItem(Graphics& graphics, const Font& font, const StringFormat& format, int& maxShortcutWidth, int& childIndicatorWidth)
{
    Padding padding = DefaultPadding();
    RectF rect = MeasureString(graphics, text, font, PointF(0, 0), format);
    int w = rect.Width;
    w = w + padding.Horizontal();
    if (shortcut != Keys::none)
    {
        std::string shortcutText = KeyText(shortcut);
        RectF shortcutRect = MeasureString(graphics, shortcutText, font, PointF(0, 0), format);
        Padding shortcutPadding = ShortcutPadding();
        int shortcutFieldWidth = static_cast<int>(shortcutRect.Width) + shortcutPadding.Horizontal();
        maxShortcutWidth = std::max(maxShortcutWidth, shortcutFieldWidth);
    }
    if (Level() > 0 && !children.IsEmpty())
    {
        childIndicatorWidth = std::max(childIndicatorWidth, ChildIndicatorIndent());
    }
    int h = rect.Height;
    h = h + padding.Vertical();
    Size sz(w, h);
    SetSize(sz);
    return sz;
}

void MenuItem::CollectShortcuts(std::unordered_map<int, MenuItem*>& shortcuts)
{
    if (shortcut != Keys::none)
    {
        shortcuts[int(shortcut)] = this;
    }
    MenuItem* menuItem = GetFirstMenuItem();
    while (menuItem)
    {
        menuItem->CollectShortcuts(shortcuts);
        menuItem = menuItem->GetNextMenuItem();
    }
}

void MenuItem::Draw(Graphics& graphics, bool drawSubitems, const Point& origin)
{
    switch (state)
    {
        case MenuItemState::open:
        {
            DrawOpen(graphics, drawSubitems, origin);
            break;
        }
        case MenuItemState::closed:
        {
            DrawClosed(graphics, drawSubitems, origin);
            break;
        }
    }
}

void MenuItem::GetOpenRect(Rect& openRect)
{
    if (!childRect.IsEmptyArea())
    {
        if (openRect.IsEmptyArea())
        {
            openRect = childRect;
        }
        else
        {
            Rect::Union(openRect, openRect, childRect);
        }
    }
    Component* child = children.FirstChild();
    while (child)
    {
        if (child->IsMenuItem())
        {
            MenuItem* menuItem = static_cast<MenuItem*>(child);
            if (menuItem->State() == MenuItemState::open)
            {
                menuItem->GetOpenRect(openRect);
            }
        }
        child = child->NextSibling();
    }
}

bool MenuItem::HandleKey(Keys keyCode, bool& menuWantsKeys, MenuItem* parentMenuItem)
{
    MenuControl* menuControl = GetMenuControl();
    if (keyCode >= Keys::a && keyCode <= Keys::z || keyCode >= Keys::d0 && keyCode <= Keys::d9)
    {
        char16_t accessKey = static_cast<char16_t>(keyCode);
        MenuItem* childItem = nullptr;
        if (parentMenuItem)
        {
            childItem = parentMenuItem->GetChildItemByAccessKey(accessKey);
        }
        else
        {
            if (menuControl)
            {
                childItem = menuControl->GetMenuItemByAccessKey(accessKey);
            }
        }
        if (childItem && childItem->IsEnabled())
        {
            childItem->Execute(parentMenuItem, menuWantsKeys);
            return true;
        }
        else
        {
            menuWantsKeys = true;
            return false;
        }
    }
    else
    {
        switch (keyCode)
        {
            case Keys::enter:
            {
                if (IsEnabled())
                {
                    Execute(parentMenuItem, menuWantsKeys);
                    return true;
                }
                else
                {
                    menuWantsKeys = true;
                    return false;
                }
            }
            case Keys::escape:
            {
                MenuItem* openedMenuItem = menuControl->OpenedMenuItem();
                if (openedMenuItem)
                {
                    openedMenuItem->SetState(MenuItemState::closed);
                }
                menuWantsKeys = false;
                menuControl->SetSelectedMenuItem(nullptr);
                menuControl->SetClosed();
                menuControl->SetMenuInvalidated();
                return true;
            }
            case Keys::home:
            {
                if (Level() == 0)
                {
                    MenuItem* firstMenuItem = menuControl->GetFirstMenuItem();
                    if (firstMenuItem)
                    {
                        menuControl->SetSelectedMenuItem(firstMenuItem);
                        menuWantsKeys = true;
                        menuControl->SetMenuInvalidated();
                        return true;
                    }
                }
                else
                {
                    MenuItem* parentMenuItem = ParentMenuItem();
                    if (parentMenuItem)
                    {
                        MenuItem* firstMenuItem = parentMenuItem->GetFirstMenuItem();
                        if (firstMenuItem)
                        {
                            menuControl->SetSelectedMenuItem(firstMenuItem);
                            menuWantsKeys = true;
                            menuControl->SetMenuInvalidated();
                            return true;
                        }
                    }
                }
                break;
            }
            case Keys::end:
            {
                if (Level() == 0)
                {
                    MenuItem* lastMenuItem = menuControl->GetLastMenuItem();
                    if (lastMenuItem)
                    {
                        menuControl->SetSelectedMenuItem(lastMenuItem);
                        menuWantsKeys = true;
                        menuControl->SetMenuInvalidated();
                        return true;
                    }
                }
                else
                {
                    MenuItem* parentMenuItem = ParentMenuItem();
                    if (parentMenuItem)
                    {
                        MenuItem* lastMenuItem = parentMenuItem->GetLastMenuItem();
                        if (lastMenuItem)
                        {
                            menuControl->SetSelectedMenuItem(lastMenuItem);
                            menuWantsKeys = true;
                            menuControl->SetMenuInvalidated();
                            return true;
                        }
                    }
                }
                break;
            }
            case Keys::down:
            {
                if (Level() == 0)
                {
                    SetState(MenuItemState::open);
                    MenuItem* firstChild = GetFirstMenuItem();
                    if (firstChild)
                    {
                        menuControl->SetSelectedMenuItem(firstChild);
                        menuWantsKeys = true;
                        menuControl->SetMenuInvalidated();
                        return true;
                    }
                }
                else
                {
                    MenuItem* nextMenuItem = GetNextMenuItem();
                    if (nextMenuItem)
                    {
                        menuControl->SetSelectedMenuItem(nextMenuItem);
                        menuWantsKeys = true;
                        menuControl->SetMenuInvalidated();
                        return true;
                    }
                }
                break;
            }
            case Keys::up:
            {
                if (Level() == 0)
                {
                    SetState(MenuItemState::open);
                    MenuItem* lastChild = GetLastMenuItem();
                    if (lastChild)
                    {
                        menuControl->SetSelectedMenuItem(lastChild);
                        menuWantsKeys = true;
                        menuControl->SetMenuInvalidated();
                        return true;
                    }
                }
                else
                {
                    MenuItem* prevMenuItem = GetPrevMenuItem();
                    if (prevMenuItem)
                    {
                        menuControl->SetSelectedMenuItem(prevMenuItem);
                        menuWantsKeys = true;
                        menuControl->SetMenuInvalidated();
                        return true;
                    }
                }
                break;
            }
            case Keys::right:
            {
                if (Level() == 0)
                {
                    MenuItem* nextMenuItem = GetNextMenuItem();
                    if (nextMenuItem)
                    {
                        menuControl->SetSelectedMenuItem(nextMenuItem);
                        menuWantsKeys = true;
                        menuControl->SetMenuInvalidated();
                        return true;
                    }
                }
                else
                {
                    MenuItem* firstChild = GetFirstMenuItem();
                    if (firstChild)
                    {
                        SetState(MenuItemState::open);
                        menuControl->SetSelectedMenuItem(firstChild);
                        menuWantsKeys = true;
                        menuControl->SetMenuInvalidated();
                        return true;
                    }
                    else
                    {
                        while (parentMenuItem)
                        {
                            if (parentMenuItem->Level() == 0)
                            {
                                MenuItem* nextMenuItem = parentMenuItem->GetNextMenuItem();
                                if (nextMenuItem)
                                {
                                    parentMenuItem->SetState(MenuItemState::closed);
                                    nextMenuItem->SetState(MenuItemState::open);
                                    MenuItem* firstChild = nextMenuItem->GetFirstMenuItem();
                                    if (firstChild)
                                    {
                                        menuControl->SetSelectedMenuItem(firstChild);
                                        menuWantsKeys = true;
                                        menuControl->SetMenuInvalidated();
                                        return true;
                                    }
                                }
                                else
                                {
                                    break;
                                }
                            }
                            else
                            {
                                MenuItem* grandParentMenuItem = parentMenuItem->ParentMenuItem();
                                MenuItem* nextMenuItem = grandParentMenuItem->GetNextMenuItem();
                                if (nextMenuItem)
                                {
                                    parentMenuItem->SetState(MenuItemState::closed);
                                    grandParentMenuItem->SetState(MenuItemState::closed);
                                    nextMenuItem->SetState(MenuItemState::open);
                                    MenuItem* firstChild = nextMenuItem->GetFirstMenuItem();
                                    if (firstChild)
                                    {
                                        menuControl->SetSelectedMenuItem(firstChild);
                                        menuWantsKeys = true;
                                        menuControl->SetMenuInvalidated();
                                        return true;
                                    }
                                }
                                else
                                {
                                    parentMenuItem = grandParentMenuItem;
                                }
                            }
                        }
                    }
                }
                break;
            }
            case Keys::left:
            {
                if (Level() == 0)
                {
                    MenuItem* prevMenuItem = GetPrevMenuItem();
                    if (prevMenuItem)
                    {
                        menuControl->SetSelectedMenuItem(prevMenuItem);
                        menuWantsKeys = true;
                        menuControl->SetMenuInvalidated();
                        return true;
                    }
                }
                else
                {
                    if (parentMenuItem)
                    {
                        MenuItem* prevMenuItem = parentMenuItem->GetPrevMenuItem();
                        if (prevMenuItem)
                        {
                            parentMenuItem->SetState(MenuItemState::closed);
                            prevMenuItem->SetState(MenuItemState::open);
                            MenuItem* firstChild = prevMenuItem->GetFirstMenuItem();
                            if (firstChild)
                            {
                                menuControl->SetSelectedMenuItem(firstChild);
                                menuWantsKeys = true;
                                menuControl->SetMenuInvalidated();
                                return true;
                            }
                        }
                        else
                        {
                            MenuItem* grandParentMenuItem = parentMenuItem->ParentMenuItem();
                            if (grandParentMenuItem)
                            {
                                grandParentMenuItem->SetState(MenuItemState::open);
                                MenuItem* firstMenuItem = grandParentMenuItem->GetFirstMenuItem();
                                if (firstMenuItem)
                                {
                                    firstMenuItem->SetState(MenuItemState::closed);
                                    menuControl->SetSelectedMenuItem(firstMenuItem);
                                }
                            }
                        }
                    }
                }
                break;
            }
        }
    }
    menuWantsKeys = true;
    return false;
}

void MenuItem::Execute(MenuItem* parentMenuItem, bool& menuWantsKeys)
{
    if (children.IsEmpty())
    {
        if (IsEnabled())
        {
            Close();
            menuWantsKeys = false;
            DoClick();
        }
    }
    else
    {
        if (IsEnabled())
        {
            SetState(MenuItemState::open);
            MenuItem* firstChild = GetFirstMenuItem();
            if (firstChild)
            {
                MenuControl* menuControl = GetMenuControl();
                menuControl->SetSelectedMenuItem(firstChild);
                menuWantsKeys = true;
                menuControl->SetMenuInvalidated();
            }
        }
    }
}

void MenuItem::SetText(const std::string& text_)
{
    text = text_;
    SetAccessKey();
    MenuControl* menuControl = GetMenuControl();
    if (menuControl)
    {
        menuControl->InvalidateMenu();
    }
}

void MenuItem::SetAccessKey()
{
    std::u16string txt = ToUtf16(text);
    std::u16string::size_type ampPos = txt.find('&');
    if (ampPos != std::u16string::npos)
    {
        accessKey = static_cast<char16_t>(ToUpper(static_cast<char32_t>(txt[ampPos + 1])));
    }
    else
    {
        accessKey = char16_t();
    }
}

void MenuItem::SetShortcut(Keys shortcut_)
{
    shortcut = shortcut_;
    MenuControl* menuControl = GetMenuControl();
    if (menuControl)
    {
        menuControl->SetChanged();
    }
}

void MenuItem::AddMenuItem(MenuItemBase* menuItem)
{
    children.AddChild(menuItem);
}

MenuItem* MenuItem::GetFirstMenuItem() const
{
    Component* child = children.FirstChild();
    while (child)
    {
        if (child->IsMenuItem())
        {
            return static_cast<MenuItem*>(child);
        }
        child = child->NextSibling();
    }
    return nullptr;
}

MenuItem* MenuItem::GetLastMenuItem() const
{
    Component* child = children.LastChild();
    while (child)
    {
        if (child->IsMenuItem())
        {
            return static_cast<MenuItem*>(child);
        }
        child = child->PrevSibling();
    }
    return nullptr;
}

MenuItem* MenuItem::GetNextMenuItem() const
{
    Component* child = NextSibling();
    while (child)
    {
        if (child->IsMenuItem())
        {
            return static_cast<MenuItem*>(child);
        }
        child = child->NextSibling();
    }
    return nullptr;
}

MenuItem* MenuItem::GetPrevMenuItem() const
{
    Component* child = PrevSibling();
    while (child)
    {
        if (child->IsMenuItem())
        {
            return static_cast<MenuItem*>(child);
        }
        child = child->PrevSibling();
    }
    return nullptr;
}

MenuItem* MenuItem::GetChildItemByAccessKey(char16_t accessKey_)
{
    MenuItem* child = GetFirstMenuItem();
    while (child)
    {
        if (child->AccessKey() == accessKey_)
        {
            return child;
        }
        child = child->GetNextMenuItem();
    }
    return nullptr;
}

void MenuItem::DoMouseDown(MouseEventArgs& args)
{
    MenuControl* menuControl = GetMenuControl();
    if (menuControl)
    {
        menuControl->SetLatestMouseDownMenuItem(this);
    }
    ResetLButtonPressed();
    OnMouseDown(args);
}

void MenuItem::DoMouseUp(MouseEventArgs& args)
{
    if (!IsEnabled()) return;
    OnMouseUp(args);
    MenuControl* menuControl = GetMenuControl();
    MenuItem* latestMouseDownMenuItem = menuControl->LatestMouseDownMenuItem();
    if ((args.buttons & MouseButtons::lbutton) != MouseButtons::none)
    {
        if (LButtonPressed())
        {
            ResetLButtonPressed();
            if (latestMouseDownMenuItem == this && children.IsEmpty())
            {
                ResetSelected();
                ResetMouseInClient();
                DoMouseLeave();
                LeaveChildren();
                Close();
                DoClick();
            }
            if (menuControl->IsMenuBar())
            {
                MenuItem* openedMenuItem = menuControl->OpenedMenuItem();
                if (openedMenuItem)
                {
                    openedMenuItem->SetState(MenuItemState::closed);
                }
                menuControl->SetClosed();
                menuControl->SetMenuInvalidated();
            }
        }
    }
}

void MenuItem::DrawOpen(Graphics& graphics, bool drawSubitems, const Point& origin)
{
    MenuControl* menuControl = GetMenuControl();
    Point loc = Location();
    loc.X = loc.X - origin.X;
    loc.Y = loc.Y - origin.Y;
    Size size = GetSize();
    Rect r(loc, size);
    r.Width = r.Width - 1;
    r.Height = r.Height - 1;
    int shadowWidth = ShadowWidth();
    if (Level() == 0)
    {
        if (!drawSubitems)
        {
            CheckGraphicsStatus(graphics.FillRectangle(&menuControl->MenuOpenBrush(), r));
            CheckGraphicsStatus(graphics.DrawLine(&menuControl->BlackPen(), loc, Point(loc.X + r.Width, loc.Y)));
            CheckGraphicsStatus(graphics.DrawLine(&menuControl->BlackPen(), loc, Point(loc.X, loc.Y + r.Height)));
            CheckGraphicsStatus(graphics.DrawLine(&menuControl->BlackPen(), Point(loc.X + r.Width, loc.Y), Point(loc.X + r.Width, loc.Y + r.Height)));
            if (children.IsEmpty())
            {
                CheckGraphicsStatus(graphics.DrawLine(&menuControl->BlackPen(), Point(loc.X, loc.Y + r.Height), Point(loc.X + r.Width, loc.Y + r.Height)));
            }
            StringFormat textFormat;
            textFormat.SetAlignment(StringAlignment::StringAlignmentCenter);
            textFormat.SetLineAlignment(StringAlignment::StringAlignmentNear);
            textFormat.SetHotkeyPrefix(HotKeyPrefix::HotkeyPrefixShow);
            RectF rect(PointF(loc.X, loc.Y), SizeF(size.Width, size.Height));
            if (IsEnabled())
            {
                DrawString(graphics, text, menuControl->GetFont(), rect, textFormat, menuControl->TextBrush());
            }
            else
            {
                DrawString(graphics, text, menuControl->GetFont(), rect, textFormat, menuControl->DisabledTextBrush());
            }
        }
    }
    else
    {
        if (drawSubitems)
        {
            MenuItem* selectedMenuItem = menuControl->SelectedMenuItem();
            if (IsSelected() || this == selectedMenuItem)
            {
                CheckGraphicsStatus(graphics.FillRectangle(&menuControl->MouseOverBrush(), r));
                CheckGraphicsStatus(graphics.DrawRectangle(&menuControl->BlackPen(), r));
            }
            else
            {
                Rect inside = r;
                inside.Inflate(-1, -1);
                CheckGraphicsStatus(graphics.FillRectangle(&menuControl->MenuOpenBrush(), inside));
            }
            Padding padding = DefaultPadding();
            if (IsEnabled())
            {
                DrawString(graphics, text, menuControl->GetFont(), PointF(loc.X + padding.left, loc.Y + padding.top), menuControl->GetStringFormat(), menuControl->TextBrush());
                DrawShortcut(graphics, origin);
            }
            else
            {
                DrawString(graphics, text, menuControl->GetFont(), PointF(loc.X + padding.left, loc.Y + padding.top), menuControl->GetStringFormat(), menuControl->DisabledTextBrush());
                DrawShortcut(graphics, origin);
            }
            DrawChildIndicator(graphics, origin);
        }
    }
    if (!childRect.IsEmptyArea())
    {
        if (drawSubitems)
        {
            Region prevClipRegion;
            CheckGraphicsStatus(graphics.GetClip(&prevClipRegion));
            Rect menuBox = childRect;
            menuBox.X = menuBox.X - origin.X;
            menuBox.Y = menuBox.Y - origin.Y;
            menuBox.Width = menuBox.Width - shadowWidth;
            menuBox.Height = menuBox.Height - shadowWidth;
            CheckGraphicsStatus(graphics.SetClip(menuBox));
            CheckGraphicsStatus(graphics.Clear(menuControl->MenuOpenColor()));
            Rect cr(childRect);
            cr.X = cr.X - origin.X;
            cr.Y = cr.Y - origin.Y;
            CheckGraphicsStatus(graphics.SetClip(cr));
            Rect bottomShadowRect(
                Point(cr.X + shadowWidth, cr.Y + cr.Height - shadowWidth),
                Size(cr.Width - shadowWidth, shadowWidth));
            CheckGraphicsStatus(graphics.FillRectangle(&menuControl->ShadowBrush(), bottomShadowRect));
            Rect rightShadowRect(
                Point(cr.X + cr.Width - shadowWidth, cr.Y + shadowWidth),
                Size(shadowWidth, cr.Height - shadowWidth));
            CheckGraphicsStatus(graphics.FillRectangle(&menuControl->ShadowBrush(), rightShadowRect));
            Rect rect = childRect;
            rect.X = rect.X - origin.X;
            rect.Y = rect.Y - origin.Y;
            rect.Width = rect.Width - 1;
            rect.Height = rect.Height - 1;
            Point loc;
            rect.GetLocation(&loc);
            if (Level() == 0)
            {
                CheckGraphicsStatus(graphics.DrawLine(&menuControl->BlackPen(), loc, Point(rect.X, rect.Y + rect.Height - shadowWidth)));
                CheckGraphicsStatus(graphics.DrawLine(&menuControl->BlackPen(), Point(rect.X, rect.Y + rect.Height - shadowWidth), 
                    Point(rect.X + rect.Width - shadowWidth, rect.Y + rect.Height - shadowWidth)));
                CheckGraphicsStatus(graphics.DrawLine(&menuControl->BlackPen(), Point(rect.X + rect.Width - shadowWidth, rect.Y + rect.Height - shadowWidth),
                    Point(rect.X + rect.Width - shadowWidth, rect.Y)));
                CheckGraphicsStatus(graphics.DrawLine(&menuControl->BlackPen(), Point(rect.X + rect.Width - shadowWidth, rect.Y),
                    Point(r.X + r.Width, rect.Y)));
            }
            else
            {
                CheckGraphicsStatus(graphics.DrawLine(&menuControl->BlackPen(), loc, Point(rect.X, rect.Y + rect.Height - shadowWidth)));
                CheckGraphicsStatus(graphics.DrawLine(&menuControl->BlackPen(), Point(rect.X, rect.Y + rect.Height - shadowWidth),
                    Point(rect.X + rect.Width - shadowWidth, rect.Y + rect.Height - shadowWidth)));
                CheckGraphicsStatus(graphics.DrawLine(&menuControl->BlackPen(), Point(rect.X + rect.Width - shadowWidth, rect.Y + rect.Height - shadowWidth),
                    Point(rect.X + rect.Width - shadowWidth, rect.Y)));
                CheckGraphicsStatus(graphics.DrawLine(&menuControl->BlackPen(), Point(rect.X + rect.Width - shadowWidth, rect.Y),
                    loc));
            }
            Component* child = children.FirstChild();
            while (child)
            {
                if (child->IsMenuItemBase())
                {
                    MenuItemBase* menuItem = static_cast<MenuItemBase*>(child);
                    menuItem->Draw(graphics, drawSubitems, origin);
                }
                child = child->NextSibling();
            }
            CheckGraphicsStatus(graphics.SetClip(&prevClipRegion));
        }
    }
}

void MenuItem::DrawClosed(Graphics& graphics, bool drawSubitems, const Point& origin)
{
    MenuControl* menuControl = GetMenuControl();
    Point loc = Location();
    loc.X = loc.X - origin.X;
    loc.Y = loc.Y - origin.Y;
    Size size = GetSize();
    Rect r(loc, size);
    r.Width = r.Width - 1;
    r.Height = r.Height - 1;
    MenuItem* selectedMenuItem = menuControl->SelectedMenuItem();
    if (IsSelected() || this == selectedMenuItem)
    {
        CheckGraphicsStatus(graphics.FillRectangle(&menuControl->MouseOverBrush(), r));
        CheckGraphicsStatus(graphics.DrawRectangle(&menuControl->BlackPen(), r));
    }
    if (Level() == 0)
    {
        if (!drawSubitems)
        {
            if (!IsSelected() && selectedMenuItem != this)
            {
                CheckGraphicsStatus(graphics.FillRectangle(&menuControl->BackgroundBrush(), r));
            }
            StringFormat textFormat;
            textFormat.SetAlignment(StringAlignment::StringAlignmentCenter);
            textFormat.SetLineAlignment(StringAlignment::StringAlignmentNear);
            textFormat.SetHotkeyPrefix(HotKeyPrefix::HotkeyPrefixShow);
            RectF rect(PointF(loc.X, loc.Y), SizeF(size.Width, size.Height));
            if (IsEnabled())
            {
                DrawString(graphics, text, menuControl->GetFont(), rect, textFormat, menuControl->TextBrush());
            }
            else
            {
                DrawString(graphics, text, menuControl->GetFont(), rect, textFormat, menuControl->DisabledTextBrush());
            }
        }
    }
    else
    {
        if (drawSubitems)
        {
            Padding padding = DefaultPadding();
            if (IsEnabled())
            {
                DrawString(graphics, text, menuControl->GetFont(), PointF(loc.X + padding.left, loc.Y + padding.top), menuControl->GetStringFormat(), menuControl->TextBrush());
                DrawShortcut(graphics, origin);
            }
            else
            {
                DrawString(graphics, text, menuControl->GetFont(), PointF(loc.X + padding.left, loc.Y + padding.top), menuControl->GetStringFormat(), menuControl->DisabledTextBrush());
                DrawShortcut(graphics, origin);
            }
            DrawChildIndicator(graphics, origin);
        }
    }
}

void MenuItem::DrawShortcut(Graphics& graphics, const Point& origin)
{
    if (shortcut == Keys::none) return;
    MenuControl* menuControl = GetMenuControl();
    std::string shortcutText = KeyText(shortcut);
    Padding shortcutPadding = ShortcutPadding();
    int shortcutFieldWidth = 0;
    int childIndicatorFieldWidth = 0;
    MenuItem* parent = ParentMenuItem();
    if (parent)
    {
        shortcutFieldWidth = parent->shortcutFieldWidth;
        childIndicatorFieldWidth = parent->childIndicatorFieldWidth;
    }
    Padding padding = DefaultPadding();
    int shadowWidth = ShadowWidth();
    Point loc = Location();
    loc.X = loc.X - origin.X;
    loc.Y = loc.Y - origin.Y;
    Size size = GetSize();
    if (IsEnabled())
    {
        DrawString(graphics, shortcutText, menuControl->GetFont(), PointF(loc.X + size.Width - shadowWidth - shortcutFieldWidth - childIndicatorFieldWidth + shortcutPadding.left, loc.Y + padding.top),
            menuControl->GetStringFormat(), menuControl->TextBrush());
    }
    else
    {
        DrawString(graphics, shortcutText, menuControl->GetFont(), PointF(loc.X + size.Width - shadowWidth - shortcutFieldWidth - childIndicatorFieldWidth + shortcutPadding.left, loc.Y + padding.top),
            menuControl->GetStringFormat(), menuControl->DisabledTextBrush());
    }
}

void MenuItem::DrawChildIndicator(Graphics& graphics, const Point& origin)
{
    if (children.IsEmpty()) return;
    MenuControl* menuControl = GetMenuControl();
    Point loc = Location();
    loc.X = loc.X - origin.X;
    loc.Y = loc.Y - origin.Y;
    Size size = GetSize();
    int childIndicatorIndent = ChildIndicatorIndent();
    int childIndicatorWidth = ChildIndicatorWidth();
    int shadowWidth = ShadowWidth();
    Point up(loc.X + size.Width - childIndicatorIndent - shadowWidth, loc.Y + size.Height / 2 - childIndicatorWidth / 2);
    Point down(loc.X + size.Width - childIndicatorIndent - shadowWidth, loc.Y + size.Height / 2 + childIndicatorWidth / 2);
    Point right(loc.X + size.Width - childIndicatorIndent + static_cast<int>((sqrt(3.0) / 2.0) * childIndicatorWidth) - shadowWidth, loc.Y + size.Height / 2);
    std::vector<Point> points;
    points.push_back(up);
    points.push_back(down);
    points.push_back(right);
    CheckGraphicsStatus(graphics.FillPolygon(&menuControl->BlackBrush(), points.data(), 3));
}

void MenuItem::DoMouseMove(MouseEventArgs& args)
{
    OnMouseMove(args);
}

Padding MenuItemSeparator::DefaultPadding() const
{
    return Padding(32, 4, 8, 4);
}

Size MenuItemSeparator::MeasureItem(Graphics& graphics, const Font& font, const StringFormat& format, int& maxShortcutWidth, int& childIndicatorWidth)
{
    Padding padding = DefaultPadding();
    RectF rect;
    int w = static_cast<int>(rect.Width);
    w = w + padding.Horizontal();
    int h = static_cast<int>(rect.Height);
    h = h + padding.Vertical();
    Size size(w, h);
    SetSize(size);
    return size;
}

void MenuItemSeparator::Draw(Graphics& graphics, bool drawSubitems, const Point& origin)
{
    MenuControl* menuControl = GetMenuControl();
    Point loc = Location();
    loc.X = loc.X - origin.X;
    loc.Y = loc.Y - origin.Y;
    Rect rect(loc, GetSize());
    Padding padding = DefaultPadding();
    CheckGraphicsStatus(graphics.DrawLine(&menuControl->DarkPen(),
        Point(rect.X + padding.left, rect.Y + rect.Height / 2),
        Point(rect.X + rect.Width - padding.right, rect.Y + rect.Height / 2)));
}

ClickAction::ClickAction(MenuItem* menuItem) 
{
    menuItem->Click().AddHandler(this, &ClickAction::Click);
}

ClickAction::~ClickAction()
{
}

void ClickAction::Click()
{
    Execute();
}

} } // cmajor::wing
