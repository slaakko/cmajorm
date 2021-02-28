// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_MENU_INCLUDED
#define CMAJOR_WING_MENU_INCLUDED
#include <cmajor/wing/Control.hpp>
#include <cmajor/wing/Container.hpp>
#include <cmajor/wing/Keys.hpp>

namespace cmajor { namespace wing {

class MenuItem;
class MenuItemBase;
class MenuBox;

WING_API Color DefaultMenuTextColor();
WING_API Color DefaultMenuBackgroundColor();
WING_API Color DefaultDisabledMenuTextColor();
WING_API Color DefaultMenuMouseOverColor();
WING_API Color DefaultMenuOpenColor();
WING_API Color DefaultMenuShadowColor();

class WING_API MenuControl : public Control
{
public:
    MenuControl(ControlCreateParams& createParams);
    bool IsMenuControl() const override { return true; }
    const Color& MenuOpenColor() const { return menuOpenColor; }
    const SolidBrush& TextBrush() const { return textBrush; }
    const SolidBrush& DisabledTextBrush() const { return disabledTextBrush; }
    const SolidBrush& BackgroundBrush() const { return backgroundBrush; }
    const SolidBrush& MouseOverBrush() const { return mouseOverBrush; }
    const SolidBrush& MenuOpenBrush() const { return menuOpenBrush; }
    const SolidBrush& ShadowBrush() const { return shadowBrush; }
    const SolidBrush& BlackBrush() const { return blackBrush; }
    const Pen& BlackPen() const { return blackPen; }
    const Pen& DarkPen() const { return darkPen; }
    const StringFormat& GetStringFormat() const { return stringFormat; }
    Padding DefaultPadding() const { return Padding(6, 2, 6, 2); }
    virtual MenuItem* GetFirstMenuItem() const { return nullptr; }
    virtual MenuItem* GetLastMenuItem() const { return nullptr; }
    virtual void SetChanged() {}
    virtual bool IsOpen() const { return false; }
    virtual void SetOpen() {}
    virtual void SetClosed() {}
    virtual void InvalidateMenu() {}
    virtual void SetMenuInvalidated() {}
    virtual MenuItem* SelectedMenuItem() const { return nullptr; }
    virtual void SetSelectedMenuItem(MenuItem* selectedMenuItem_) {}
    virtual MenuItem* OpenedMenuItem() const { return nullptr; }
    virtual MenuItem* LatestOpenedMenuItem() const { return nullptr; }
    virtual void SetLatestOpenedMenuItem(MenuItem* menuItem) {}
    virtual MenuItem* LatestMouseDownMenuItem() const { return nullptr; }
    virtual void SetLatestMouseDownMenuItem(MenuItem* menuItem) {}
    virtual MenuItem* GetMenuItemByAccessKey(char16_t accessKey) const { return nullptr; }
private:
    Color textColor;
    Color disabledTextColor;
    Color mouseOverColor;
    Color menuOpenColor;
    Color shadowColor;
    SolidBrush textBrush;
    SolidBrush disabledTextBrush;
    SolidBrush backgroundBrush;
    SolidBrush menuOpenBrush;
    SolidBrush mouseOverBrush;
    SolidBrush shadowBrush;
    SolidBrush blackBrush;
    Pen blackPen;
    Pen darkPen;
    StringFormat stringFormat;
};

enum class MenuFlags : int
{
    none = 0, open = 1 << 0, menuChanged = 1 << 1, menuBoxAdded = 1 << 2, menuInvalidated = 1 << 3
};

WING_API inline MenuFlags operator&(MenuFlags left, MenuFlags right)
{
    return MenuFlags(int(left) & int(right));
}

WING_API inline MenuFlags operator|(MenuFlags left, MenuFlags right)
{
    return MenuFlags(int(left) | int(right));
}

WING_API inline MenuFlags operator~(MenuFlags menuFlags)
{
    return MenuFlags(~int(menuFlags));
}

class MenuItem;

class WING_API MenuBar : public MenuControl
{
public:
    MenuBar();
    bool IsMenuBar() const override { return true; }
    void AddMenuItem(MenuItem* menuItem);
    bool MenuChanged() const { return (flags & MenuFlags::menuChanged) != MenuFlags::none; }
    void SetMenuChanged() { flags = flags | MenuFlags::menuChanged; }
    void ResetMenuChanged() { flags = flags & ~MenuFlags::menuChanged; }
    MenuItem* GetFirstMenuItem() const override;
    MenuItem* GetLastMenuItem() const override;
    void SetChanged() override { SetMenuChanged(); }
    bool IsOpen() const override;
    void SetOpen() override;
    void SetClosed() override;
    bool MenuInvalidated() const { return (flags & MenuFlags::menuInvalidated) != MenuFlags::none; }
    void InvalidateMenu() override;
    void SetMenuInvalidated() override;
    void ResetMenuInvalidated() { flags = flags & ~MenuFlags::menuInvalidated; }
    bool MenuBoxAdded() const { return (flags & MenuFlags::menuBoxAdded) != MenuFlags::none; }
    void SetMenuBoxAdded() { flags = flags | MenuFlags::menuBoxAdded; }
    void ResetMenuBoxAdded() { flags = flags & ~MenuFlags::menuBoxAdded; }
    MenuItem* SelectedMenuItem() const override { return selectedMenuItem; }
    void SetSelectedMenuItem(MenuItem* selectedMenuItem_) override;
    MenuItem* OpenedMenuItem() const override;
    MenuItem* LatestOpenedMenuItem() const override { return latestOpenedMenuItem; }
    void SetLatestOpenedMenuItem(MenuItem* menuItem) override;
    MenuItem* LatestMouseDownMenuItem() const override { return latestMouseDownMenuItem; }
    void SetLatestMouseDownMenuItem(MenuItem* menuItem) override;
    void CloseMenu();
    void MouseDownInternal(MouseEventArgs& args);
    void MouseUpInternal(MouseEventArgs& args);
    void MouseMoveInternal(MouseEventArgs& args);
    void MouseEnterInternal();
    void MouseLeaveInternal();
    void DrawMenuItems(PaintEventArgs& args, bool drawSubitems, const Point& origin);
    bool HandleAccessKey(char16_t accessKey, Keys keyCode, bool& menuWantsKeys);
    void DoKeyDown(KeyEventArgs& args);
    MenuItem* GetMenuItemByAccessKey(char16_t accessKey) const override;
protected:
    void OnPaint(PaintEventArgs& args) override;
    void OnMouseDown(MouseEventArgs& args) override;
    void OnMouseUp(MouseEventArgs& args) override;
    void OnMouseMove(MouseEventArgs& args) override;
    void OnMouseLeave() override;
    void OnKeyDown(KeyEventArgs& args) override;
private:
    void AddMenuBox();
    void LocateMenuItems(Graphics& graphics, const Size& size);
    void CollectShortcuts();
    MenuFlags flags;
    Container children;
    MenuItem* latestOpenedMenuItem;
    MenuItem* selectedMenuItem;
    MenuItem* latestMouseDownMenuItem;
    MenuBox* menuBox;
    std::unordered_map<int, MenuItem*> shortcuts;
};

class WING_API MenuBox : public MenuControl
{
public:
    MenuBox(MenuBar* menuBar_, MenuItem* rootItem_);
    bool IsMenuBox() const override { return true; }
    virtual bool IsContextMenu() const { return false; }
    void SetPaintThisMenuBox() { paintThisMenuBox = true; }
    void ResetPaintThisMenuBox() { paintThisMenuBox = false; }
    bool PaintThisMenuBox() const { return paintThisMenuBox; }
    MenuItem* RootItem() const { return rootItem; }
protected:
    void OnPaint(PaintEventArgs& args) override;
    void OnMouseEnter() override;
    void OnMouseLeave() override;
    void OnMouseMove(MouseEventArgs& args) override;
    void OnMouseDown(MouseEventArgs& args) override;
    void OnMouseUp(MouseEventArgs& args) override;
private:
    MenuBar* menuBar;
    MenuItem* rootItem;
    bool paintThisMenuBox;
    bool paintMenu;
};

class WING_API ContextMenu : public MenuBox
{
public:
    ContextMenu();
    void AddMenuItem(MenuItemBase* menuItem);
    void CalculateSize();
    bool HasMenuItems() const;
    void SetMenuInvalidated() override;
    bool IsOpen() const override;
    MenuItem* OpenedMenuItem() const override;
    MenuItem* SelectedMenuItem() const override { return selectedMenuItem; }
    void SetSelectedMenuItem(MenuItem* selectedMenuItem_) override;
    MenuItem* LatestOpenedMenuItem() const override { return latestOpenedMenuItem; }
    void SetLatestOpenedMenuItem(MenuItem* menuItem) override;
    MenuItem* LatestMouseDownMenuItem() const override { return latestMouseDownMenuItem; }
    void SetLatestMouseDownMenuItem(MenuItem* menuItem) override;
protected:
    void OnPaint(PaintEventArgs& args) override;
    void OnVisibleChanged() override;
private:
    std::unique_ptr<MenuItem> rootItemPtr;
    MenuItem* latestOpenedMenuItem;
    MenuItem* selectedMenuItem;
    MenuItem* latestMouseDownMenuItem;
};

class WING_API MenuItemBase : public Component
{
public:
    MenuItemBase();
    bool IsMenuItemBase() const override { return true; }
    const Point& Location() const { return location; }
    void SetLocation(const Point& location_) { location = location_; }
    const Size& GetSize() const { return size; }
    void SetSize(const Size& size_) { size = size_; }
    MenuItem* ParentMenuItem() const;
    int Level() const;
    bool Contains(const Point& pt) const;
    virtual Padding DefaultPadding() const { return Padding(0, 0, 0, 0); }
    virtual Size MeasureItem(Graphics& graphics, const Font& font, const StringFormat& format, int& maxShortcutWidth, int& childIndicatorWidth) = 0;
    virtual void Draw(Graphics& graphics, bool drawSubitems, const Point& origin) = 0;
protected:
    MenuControl* GetMenuControl() const;
private:
    Point location;
    Size size;
};

enum class MenuItemState : int
{
    closed = 0, open = 1
};

enum class MenuItemFlags : int
{
    none = 0, disabled = 1 << 0, selected = 1 << 1, mouseInClient = 1 << 2, lbuttonPressed = 1 << 3
};

inline MenuItemFlags operator&(MenuItemFlags left, MenuItemFlags right)
{
    return MenuItemFlags(int(left) & int(right));
}

inline MenuItemFlags operator|(MenuItemFlags left, MenuItemFlags right)
{
    return MenuItemFlags(int(left) | int(right));
}

inline MenuItemFlags operator~(MenuItemFlags menuItemFlags)
{
    return MenuItemFlags(~int(menuItemFlags));
}

class WING_API MenuItem : public MenuItemBase
{
public:
    MenuItem(const std::string& text_);
    void AddMenuItem(MenuItemBase* menuItem);
    MenuItem* GetFirstMenuItem() const;
    MenuItem* GetLastMenuItem() const;
    MenuItem* GetNextMenuItem() const;
    MenuItem* GetPrevMenuItem() const;
    MenuItem* GetChildItemByAccessKey(char16_t accessKey_);
    const std::string& Text() const { return text; }
    void SetText(const std::string& text_);
    char16_t AccessKey() const { return accessKey; }
    void SetAccessKey();
    void SetShortcut(Keys shortcut_);
    MenuItemState State() const { return state; }
    void SetState(MenuItemState state_);
    MenuItemFlags Flags() const { return flags; }
    bool IsMenuItem() const override { return true; }
    bool IsSelected() const { return (flags & MenuItemFlags::selected) != MenuItemFlags::none; }
    void SetSelected();
    void ResetSelected();
    bool IsDisabled() const { return (flags & MenuItemFlags::disabled) != MenuItemFlags::none; }
    bool IsEnabled() const { return !IsDisabled(); }
    void Enable();
    void Disable();
    bool MouseInClient() const { return (flags & MenuItemFlags::mouseInClient) != MenuItemFlags::none; }
    void SetMouseInClient();
    void ResetMouseInClient();
    bool LButtonPressed() const { return (flags & MenuItemFlags::lbuttonPressed) != MenuItemFlags::none; }
    void SetLButtonPressed();
    void ResetLButtonPressed();
    ClickEvent& Click() { return click; }
    MouseDownEvent& MouseDown() { return mouseDown; }
    MouseUpEvent& MouseUp() { return mouseUp; }
    MouseMoveEvent& MouseMove() { return mouseMove; }
    MouseEnterEvent& MouseEnter() { return mouseEnter; }
    MouseLeaveEvent& MouseLeave() { return mouseLeave; }
    void DoClick() { OnClick(); }
    void DoMouseMove(MouseEventArgs& args);
    void DispatchMouseDown(MouseEventArgs& args, bool& handled);
    void DispatchMouseUp(MouseEventArgs& args, bool& handled);
    void DispatchMouseMove(MouseEventArgs& args, bool& handled);
    void DoMouseEnter(bool parentIsOpen);
    void DoMouseLeave();
    void LeaveChildren();
    void Close();
    bool IsSameOrParentOf(MenuItem* menuItem) const;
    const Container& Children() const { return children; }
    const Rect& ChildRect() const { return childRect; }
    const Rect& UnionRect() const { return unionRect; }
    Padding DefaultPadding() const override;
    Padding ShortcutPadding() const;
    int ShadowWidth() const { return 3; }
    int ChildIndicatorIndent() const { return 9; }
    int ChildIndicatorWidth() const { return 6; }
    void CalculateRects(Graphics& graphics, const Font& font, const StringFormat& stringFormat, const Point& location);
    Size MeasureItem(Graphics& graphics, const Font& font, const StringFormat& format, int& maxShortcutWidth, int& childIndicatorWidth) override;
    void CollectShortcuts(std::unordered_map<int, MenuItem*>& shortcuts);
    void Draw(Graphics& graphics, bool drawSubitems, const Point& origin) override;
    void GetOpenRect(Rect& openRect);
    bool HandleKey(Keys keyCode, bool& menuWantsKeys, MenuItem* parentMenuItem);
    void Execute(MenuItem* parentMenuItem, bool& menuWantsKeys);
protected:
    virtual void OnClick();
    virtual void OnMouseDown(MouseEventArgs& args);
    virtual void OnMouseUp(MouseEventArgs& args);
    virtual void OnMouseMove(MouseEventArgs& args);
    virtual void OnMouseEnter();
    virtual void OnMouseLeave();
private:
    void DoMouseDown(MouseEventArgs& args);
    void DoMouseUp(MouseEventArgs& args);
    void DrawOpen(Graphics& graphics, bool drawSubitems, const Point& origin);
    void DrawClosed(Graphics& graphics, bool drawSubitems, const Point& origin);
    void DrawShortcut(Graphics& graphics, const Point& origin);
    void DrawChildIndicator(Graphics& graphics, const Point& origin);
    std::string text;
    MenuItemState state;
    MenuItemFlags flags;
    char16_t accessKey;
    Keys shortcut;
    Container children;
    Rect childRect;
    Rect unionRect;
    int shortcutFieldWidth;
    int childIndicatorFieldWidth;
    ClickEvent click;
    MouseDownEvent mouseDown;
    MouseUpEvent mouseUp;
    MouseMoveEvent mouseMove;
    MouseEnterEvent mouseEnter;
    MouseLeaveEvent mouseLeave;
};

class WING_API MenuItemSeparator : public MenuItemBase
{
public:
    Padding DefaultPadding() const override;
    Size MeasureItem(Graphics& graphics, const Font& font, const StringFormat& format, int& maxShortcutWidth, int& childIndicatorWidth) override;
    void Draw(Graphics& graphics, bool drawSubitems, const Point& origin) override;
};

class WING_API ClickAction
{
public:
    ClickAction(MenuItem* menuItem);
    virtual ~ClickAction();
    virtual void Execute() = 0;
private:
    void Click();
};

} } // cmajor::wing

#endif // CMAJOR_WING_MENU_INCLUDED
