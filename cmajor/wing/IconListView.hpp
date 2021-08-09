// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_ICON_LIST_VIEW_INCLUDED
#define CMAJOR_WING_ICON_LIST_VIEW_INCLUDED
#include <wing/Control.hpp>
#include <wing/Container.hpp>

namespace cmajor { namespace wing {

using SelectedIndexChangedEvent = Event;
using ItemDoubleClickedEvent = Event;

WING_API Color DefaultIconListViewTextColor();
WING_API Color DefaultIconSelectedColor();
WING_API Padding DefaultIconPadding();
WING_API Padding DefaultTextPadding();
WING_API Padding DefaultInternalPadding();

struct WING_API IconListViewCreateParams
{
    IconListViewCreateParams();
    IconListViewCreateParams& Defaults();
    IconListViewCreateParams& WindowClassName(const std::string& windowClassName_);
    IconListViewCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    IconListViewCreateParams& WindowStyle(int windowStyle_);
    IconListViewCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    IconListViewCreateParams& BackgroundColor(const Color& backgroundColor_);
    IconListViewCreateParams& Text(const std::string& text_);
    IconListViewCreateParams& Location(Point location_);
    IconListViewCreateParams& SetSize(Size size_);
    IconListViewCreateParams& SetAnchors(Anchors anchors_);
    IconListViewCreateParams& SetDock(Dock dock_);
    IconListViewCreateParams& FontFamilyName(const std::string& name);
    IconListViewCreateParams& FontSize(float size);
    IconListViewCreateParams& TransparentColor(const Color& color);
    IconListViewCreateParams& TextColor(const Color& color);
    IconListViewCreateParams& IconSelectedColor(const Color& color);
    IconListViewCreateParams& IconPadding(const Padding& padding);
    IconListViewCreateParams& TextPadding(const Padding& padding);
    IconListViewCreateParams& InternalPadding(const Padding& padding);
    ControlCreateParams controlCreateParams;
    std::string fontFamilyName;
    float fontSize;
    Color transparentColor;
    Color textColor;
    Color iconSelectedColor;
    Padding iconPadding;
    Padding textPadding;
    Padding internalPadding;
};

enum class IconListViewFlags : int
{
    none = 0, changed = 1 << 0
};

WING_API inline IconListViewFlags operator|(IconListViewFlags left, IconListViewFlags right)
{
    return IconListViewFlags(int(left) | int(right));
}

WING_API inline IconListViewFlags operator&(IconListViewFlags left, IconListViewFlags right)
{
    return IconListViewFlags(int(left) & int(right));
}

WING_API inline IconListViewFlags operator~(IconListViewFlags flags)
{
    return IconListViewFlags(~int(flags));
}

class IconListViewItem;

class WING_API IconListView : public Control
{
public:
    IconListView(IconListViewCreateParams& createParams);
    void AddItem(IconListViewItem* item);
    IconListViewItem* GetItem(int index) const;
    IconListViewItem* SelectedItem() const { return selectedItem; }
    void SetSelectedItem(IconListViewItem* item);
    int SelectedIndex() const;
    bool IsIconListView() const override { return true; }
    const StringFormat& GetStringFormat() const { return stringFormat; }
    const StringFormat& CenterFormat() const { return centerFormat; }
    const Color& TransparentColor() const { return transparentColor; }
    const Color& IconSelectedColor() const { return iconSelectedColor; }
    const Padding& IconPadding() const { return iconPadding; }
    const Padding& TextPadding() const { return textPadding; }
    const Padding& InternalPadding() const { return internalPadding; }
    const SolidBrush& TextBrush() const { return textBrush; }
    const SolidBrush& IconSelectedBrush() const { return iconSelectedBrush; }
    bool IsChanged() const { return (flags & IconListViewFlags::changed) != IconListViewFlags::none; }
    void SetChanged() { flags = flags | IconListViewFlags::changed; }
    void ResetChanged() { flags = flags & ~IconListViewFlags::changed; }
    SelectedIndexChangedEvent& SelectedIndexChanged() { return selectedIndexChanged; }
    ItemDoubleClickedEvent& ItemDoubleClicked() { return itemDoubleClicked; }
protected:
    void OnPaint(PaintEventArgs& args) override;
    void OnGotFocus() override;
    void OnKeyDown(KeyEventArgs& args) override;
    void OnMouseDown(MouseEventArgs& args) override;
    void OnMouseDoubleClick(MouseEventArgs& args) override;
    virtual void OnSelectedIndexChanged();
    virtual void OnItemDoubleClicked();
private:
    Bitmap* GetOrInsertBitmap(const std::string& iconName);
    void Measure(Graphics& graphics);
    IconListViewFlags flags;
    Container items;
    IconListViewItem* selectedItem;
    int itemCount;
    StringFormat stringFormat;
    StringFormat centerFormat;
    Color transparentColor;
    Color textColor;
    Color iconSelectedColor;
    Padding iconPadding;
    Padding textPadding;
    Padding internalPadding;
    SolidBrush textBrush;
    SolidBrush iconSelectedBrush;
    std::map<std::string, Bitmap*> bitmapMap;
    std::vector<std::unique_ptr<Bitmap>> bitmaps;
    SelectedIndexChangedEvent selectedIndexChanged;
    ItemDoubleClickedEvent itemDoubleClicked;
};

enum class IconListViewItemFlags : int
{
    none = 0, selected = 1 << 0
};

WING_API inline IconListViewItemFlags operator|(IconListViewItemFlags left, IconListViewItemFlags right)
{
    return IconListViewItemFlags(int(left) | int(right));
}

WING_API inline IconListViewItemFlags operator&(IconListViewItemFlags left, IconListViewItemFlags right)
{
    return IconListViewItemFlags(int(left) & int(right));
}

WING_API inline IconListViewItemFlags operator~(IconListViewItemFlags flags)
{
    return IconListViewItemFlags(~int(flags));
}

class WING_API IconListViewItem : public Component
{
public:
    IconListViewItem(const std::string& iconName_, const std::string& itemName_);
    bool IsIconListViewItem() const override { return true; }
    void SetIndex(int index_) { index = index_; }
    int Index() const { return index; }
    Bitmap* GetBitmap() const { return bitmap; }
    void SetBitmap(Bitmap* bitmap_) { bitmap = bitmap_; }
    const std::string& IconName() const { return iconName; }
    const std::string& ItemName() const { return itemName; }
    bool IsSelected() const { return (flags & IconListViewItemFlags::selected) != IconListViewItemFlags::none; }
    void SetSelected() { flags = flags | IconListViewItemFlags::selected; }
    void ResetSelected() { flags = flags & ~IconListViewItemFlags::selected; }
    virtual void Measure(Graphics& graphics);
    virtual void Draw(Graphics& graphics);
    const Point& Location() const { return location; }
    void SetLocation(const Point& location_);
    Size GetSize() const { return size; }
    void SetSize(const Size& size_);
private:
    IconListView* GetIconListView() const;
    IconListViewItemFlags flags;
    std::string iconName;
    std::string itemName;
    int index;
    Bitmap* bitmap;
    Point location;
    Size size;
    Point iconLocation;
    Point textLocation;
    Size textSize;
    Size iconSize;
};

} } // cmajor::wing

#endif // CMAJOR_WING_ICON_LIST_VIEW_INCLUDED
