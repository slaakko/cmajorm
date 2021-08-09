// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/IconListView.hpp>
#include <wing/Button.hpp>
#include <wing/Window.hpp>
#include <soulng/util/Unicode.hpp>

#undef min
#undef max

namespace cmajor { namespace wing {

using namespace soulng::unicode;

Color DefaultIconListViewTextColor()
{
    return Color::Black;
}

Color DefaultIconSelectedColor()
{
    return Color(64, 96, 156, 210);
}

Padding DefaultIconPadding()
{
    return Padding(8, 8, 8, 8);
}

Padding DefaultTextPadding()
{
    return Padding(4, 4, 4, 4);
}

Padding DefaultInternalPadding()
{
    return Padding(4, 4, 4, 4);
}

IconListViewCreateParams::IconListViewCreateParams() : controlCreateParams(), fontSize()
{
    controlCreateParams.WindowClassName("wing.IconListView");
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    controlCreateParams.BackgroundColor(Color::White);
    controlCreateParams.WindowClassStyle(DoubleClickWindowClassStyle());
    controlCreateParams.WindowStyle(DefaultChildWindowStyle() | WS_TABSTOP);
    fontFamilyName = "Segoe UI";
    fontSize = 9.0f;
    textColor = DefaultIconListViewTextColor();
    transparentColor = DefaultBitmapTransparentColor();
    iconSelectedColor = DefaultIconSelectedColor();
    iconPadding = DefaultIconPadding();
    textPadding = DefaultTextPadding();
    internalPadding = DefaultInternalPadding();
}

IconListViewCreateParams& IconListViewCreateParams::Defaults()
{
    return *this;
}

IconListViewCreateParams& IconListViewCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

IconListViewCreateParams& IconListViewCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

IconListViewCreateParams& IconListViewCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

IconListViewCreateParams& IconListViewCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

IconListViewCreateParams& IconListViewCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

IconListViewCreateParams& IconListViewCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

IconListViewCreateParams& IconListViewCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

IconListViewCreateParams& IconListViewCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

IconListViewCreateParams& IconListViewCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

IconListViewCreateParams& IconListViewCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

IconListViewCreateParams& IconListViewCreateParams::FontFamilyName(const std::string& name)
{
    fontFamilyName = name;
    return *this;
}

IconListViewCreateParams& IconListViewCreateParams::FontSize(float size)
{
    fontSize = size;
    return *this;
}

IconListViewCreateParams& IconListViewCreateParams::TransparentColor(const Color& color)
{
    transparentColor = color;
    return *this;
}

IconListViewCreateParams& IconListViewCreateParams::TextColor(const Color& color)
{
    textColor = color;
    return *this;
}

IconListViewCreateParams& IconListViewCreateParams::IconSelectedColor(const Color& color)
{
    iconSelectedColor = color;
    return *this;
}

IconListViewCreateParams& IconListViewCreateParams::IconPadding(const Padding& padding)
{
    iconPadding = padding;
    return *this;
}

IconListViewCreateParams& IconListViewCreateParams::TextPadding(const Padding& padding)
{
    textPadding = padding;
    return *this;
}

IconListViewCreateParams& IconListViewCreateParams::InternalPadding(const Padding& padding)
{
    internalPadding = padding;
    return *this;
}

IconListView::IconListView(IconListViewCreateParams& createParams) :
    Control(createParams.controlCreateParams),
    flags(IconListViewFlags::none),
    items(this),
    selectedItem(nullptr),
    itemCount(0), 
    stringFormat(),
    centerFormat(),
    textBrush(createParams.textColor),
    iconSelectedBrush(createParams.iconSelectedColor)
{
    SetCaretDisabled();
    std::u16string familyName = ToUtf16(createParams.fontFamilyName);
    SetFont(Font(FontFamily((const WCHAR*)familyName.c_str()), createParams.fontSize, FontStyle::FontStyleRegular, Unit::UnitPoint));
    stringFormat.SetAlignment(StringAlignment::StringAlignmentNear);
    stringFormat.SetLineAlignment(StringAlignment::StringAlignmentNear);
    centerFormat.SetAlignment(StringAlignment::StringAlignmentCenter);
    centerFormat.SetLineAlignment(StringAlignment::StringAlignmentCenter);
    textColor = createParams.textColor;
    transparentColor = createParams.transparentColor;
    iconSelectedColor = createParams.iconSelectedColor;
    iconPadding = createParams.iconPadding;
    textPadding = createParams.textPadding;
    internalPadding = createParams.internalPadding;
}

void IconListView::AddItem(IconListViewItem* item)
{
    item->SetIndex(itemCount++);
    Bitmap* bitmap = GetOrInsertBitmap(item->IconName());
    item->SetBitmap(bitmap);
    items.AddChild(item);
    SetChanged();
    Invalidate();
}

IconListViewItem* IconListView::GetItem(int index) const
{
    Component* component = items.FirstChild();
    while (component)
    {
        if (component->IsIconListViewItem())
        {
            IconListViewItem* item = static_cast<IconListViewItem*>(component);
            if (item->Index() == index)
            {
                return item;
            }
        }
        component = component->NextSibling();
    }
    return nullptr;
}

void IconListView::SetSelectedItem(IconListViewItem* item)
{
    if (selectedItem != item)
    {
        if (selectedItem)
        {
            selectedItem->ResetSelected();
        }
        selectedItem = item;
        if (selectedItem)
        {
            selectedItem->SetSelected();
        }
        OnSelectedIndexChanged();
        Invalidate();
    }
}

int IconListView::SelectedIndex() const
{
    if (selectedItem)
    {
        return selectedItem->Index();
    }
    else
    {
        return -1;
    }
}

void IconListView::OnPaint(PaintEventArgs& args)
{
    if (IsChanged())
    {
        ResetChanged();
        Measure(args.graphics);
    }
    args.graphics.Clear(BackgroundColor());
    Component* component = items.FirstChild();
    while (component)
    {
        if (component->IsIconListViewItem())
        {
            IconListViewItem* item = static_cast<IconListViewItem*>(component);
            item->Draw(args.graphics);
        }
        component = component->NextSibling();
    }
    Control::OnPaint(args);
}

void IconListView::OnGotFocus()
{
    Control::OnGotFocus();
    if (!selectedItem)
    {
        SetSelectedItem(GetItem(0));
    }
}

void IconListView::OnKeyDown(KeyEventArgs& args)
{
    Control::OnKeyDown(args);
    if (!args.handled)
    {
        switch (args.key)
        {
            case Keys::home:
            {
                if (itemCount > 0)
                {
                    SetSelectedItem(GetItem(0));
                }
                break;
            }
            case Keys::end:
            {
                if (itemCount > 0)
                {
                    SetSelectedItem(GetItem(itemCount - 1));
                }
                break;
            }
            case Keys::left:
            {
                int selectedIndex = SelectedIndex();
                if (selectedIndex > 0)
                {
                    SetSelectedItem(GetItem(selectedIndex - 1));
                }
                break;
            }
            case Keys::right:
            {
                int selectedIndex = SelectedIndex();
                if (selectedIndex >= 0 && selectedIndex < itemCount - 1)
                {
                    SetSelectedItem(GetItem(selectedIndex + 1));
                }
                break;
            }
        }
    }
}

void IconListView::OnMouseDown(MouseEventArgs& args)
{
    Control::OnMouseDown(args);
    if (selectedItem)
    {
        selectedItem->ResetSelected();
    }
    Point loc = args.location;
    Component* component = items.FirstChild();
    while (component)
    {
        if (component->IsIconListViewItem())
        {
            IconListViewItem* item = static_cast<IconListViewItem*>(component);
            Rect rect(item->Location(), item->GetSize());
            if (rect.Contains(loc))
            {
                item->SetSelected();
                selectedItem = item;
                OnSelectedIndexChanged();
                break;
            }
        }
        component = component->NextSibling();
    }
    if (selectedItem)
    {
        Window* window = GetWindow();
        if (window)
        {
            Button* button = window->DefaultButton();
            if (button)
            {
                button->SetFocus();
            }
        }
    }
    Invalidate();
}

void IconListView::OnMouseDoubleClick(MouseEventArgs& args)
{
    Control::OnMouseDoubleClick(args);
    if (selectedItem)
    {
        selectedItem->ResetSelected();
    }
    Point loc = args.location;
    Component* component = items.FirstChild();
    while (component)
    {
        if (component->IsIconListViewItem())
        {
            IconListViewItem* item = static_cast<IconListViewItem*>(component);
            Rect rect(item->Location(), item->GetSize());
            if (rect.Contains(loc))
            {
                item->SetSelected();
                selectedItem = item;
                OnItemDoubleClicked();
                break;
            }
        }
        component = component->NextSibling();
    }
    Invalidate();
}

void IconListView::OnSelectedIndexChanged()
{
    selectedIndexChanged.Fire();
}

void IconListView::OnItemDoubleClicked()
{
    itemDoubleClicked.Fire();
}

void IconListView::Measure(Graphics& graphics)
{
    Point loc(internalPadding.left, internalPadding.top);
    Size size;
    Component* component = items.FirstChild();
    while (component)
    {
        if (component->IsIconListViewItem())
        {
            IconListViewItem* item = static_cast<IconListViewItem*>(component);
            item->Measure(graphics);
            item->SetLocation(loc);
            Size itemSize = item->GetSize();
            loc.X = loc.X + itemSize.Width;
            size.Width = size.Width + itemSize.Width;
            size.Height = std::max(size.Height, itemSize.Height);
        }
        component = component->NextSibling();
    }
    size.Width = size.Width + internalPadding.Horizontal();
    size.Height = size.Height + internalPadding.Vertical();
    SetSize(size);
}

Bitmap* IconListView::GetOrInsertBitmap(const std::string& iconName)
{
    auto it = bitmapMap.find(iconName);
    if (it != bitmapMap.cend())
    {
        return it->second;
    }
    std::u16string bitmapName = ToUtf16(iconName);
    Bitmap* bitmap = Bitmap::FromResource(Instance(), (const WCHAR*)bitmapName.c_str());
    bitmapMap[iconName] = bitmap;
    bitmaps.push_back(std::unique_ptr<Bitmap>(bitmap));
    return bitmap;
}

IconListViewItem::IconListViewItem(const std::string& iconName_, const std::string& itemName_) : 
    Component(), 
    flags(IconListViewItemFlags::none), 
    iconName(iconName_), 
    itemName(itemName_), 
    index(-1),
    bitmap(nullptr)
{
}

IconListView* IconListViewItem::GetIconListView() const
{
    Container* container = GetContainer();
    if (container)
    {
        Component* parent = container->Parent();
        if (parent)
        {
            if (parent->IsIconListView())
            {
                return static_cast<IconListView*>(parent);
            }
        }
    }
    return nullptr;
}

void IconListViewItem::Measure(Graphics& graphics)
{
    IconListView* iconListView = GetIconListView();
    if (!iconListView) return;
    Padding iconPadding = iconListView->IconPadding();
    Padding textPadding = iconListView->TextPadding();
    iconSize = Size(bitmap->GetWidth(), bitmap->GetHeight());
    PointF origin;
    RectF r = MeasureString(graphics, itemName, iconListView->GetFont(), origin, iconListView->GetStringFormat());
    textSize = Size(r.Width, r.Height);
    textSize.Width = textSize.Width + textPadding.Horizontal();
    Size sz;
    sz.Width = std::max(static_cast<int>(iconSize.Width + iconPadding.Horizontal()), static_cast<int>(textSize.Width + textPadding.Horizontal()));
    sz.Height = static_cast<int>(iconSize.Height + iconPadding.Vertical() + textSize.Height + textPadding.Vertical());
    SetSize(sz);
}

void IconListViewItem::Draw(Graphics& graphics)
{
    IconListView* iconListView = GetIconListView();
    if (!iconListView) return;
    if (IsSelected())
    {
        Rect r(location, size);
        CheckGraphicsStatus(graphics.FillRectangle(&iconListView->IconSelectedBrush(), r));
    }
    Gdiplus::ImageAttributes imageAttributes;
    imageAttributes.SetColorKey(iconListView->TransparentColor(), iconListView->TransparentColor());
    Rect iconRect(iconLocation, iconSize);
    CheckGraphicsStatus(graphics.DrawImage(bitmap, iconRect, 0, 0, iconSize.Width, iconSize.Height, Unit::UnitPixel, &imageAttributes));
    RectF textRect(PointF(textLocation.X, textLocation.Y), SizeF(textSize.Width, textSize.Height));
    DrawString(graphics, itemName, iconListView->GetFont(), textRect, iconListView->CenterFormat(), iconListView->TextBrush());
}

void IconListViewItem::SetLocation(const Point& location_)
{
    IconListView* iconListView = GetIconListView();
    if (!iconListView) return;
    Padding iconPadding = iconListView->IconPadding();
    Padding textPadding = iconListView->TextPadding();
    location = location_;
    if (textSize.Width > iconSize.Width)
    {
        iconLocation = Point(
            location.X + iconPadding.left + ((textSize.Width + textPadding.Horizontal()) - (iconSize.Width + iconPadding.Horizontal())) / 2, 
            location.Y + iconPadding.top);
        textLocation = Point(
            location.X + textPadding.left,
            location.Y + (size.Height - textSize.Height - textPadding.Vertical()) + textPadding.top);
    }
    else
    {
        iconLocation = Point(
            location.X + iconPadding.left, 
            location.Y + iconPadding.top);
        textLocation = Point(
            location.X + textPadding.left + ((iconSize.Width + iconPadding.Horizontal()) - (textSize.Width + textPadding.Horizontal())) / 2,
            location.Y + (size.Height - textSize.Height - textPadding.Vertical()) + textPadding.top);
    }
}

void IconListViewItem::SetSize(const Size& size_)
{
    size = size_;
}

} } // cmajor::wing
