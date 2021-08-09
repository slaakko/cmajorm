// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/ListBox.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace wing {

using namespace soulng::unicode;

ListBoxCreateParams::ListBoxCreateParams() : controlCreateParams()
{
    controlCreateParams.WindowClassName("LISTBOX");
    controlCreateParams.WindowStyle(DefaultChildWindowStyle() | WS_TABSTOP | LBS_NOTIFY | LBS_HASSTRINGS | WS_VSCROLL);
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    controlCreateParams.BackgroundColor(Color::White);
}

ListBoxCreateParams& ListBoxCreateParams::Defaults()
{
    return *this;
}

ListBoxCreateParams& ListBoxCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

ListBoxCreateParams& ListBoxCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

ListBoxCreateParams& ListBoxCreateParams::WindowStyle(int windowStyle_) 
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

ListBoxCreateParams& ListBoxCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

ListBoxCreateParams& ListBoxCreateParams::BackgroundColor(const Color& backgroundColor_) 
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

ListBoxCreateParams& ListBoxCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

ListBoxCreateParams& ListBoxCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

ListBoxCreateParams& ListBoxCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

ListBoxCreateParams& ListBoxCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

ListBoxCreateParams& ListBoxCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

ListBox::ListBox(ListBoxCreateParams& createParams) : Control(createParams.controlCreateParams)
{
}

void ListBox::SetSelectedIndex(int selectedIndex)
{
    if (Handle())
    {
        SendMessage(Handle(), LB_SETCURSEL, selectedIndex, 0);
    }
}

int ListBox::GetSelectedIndex() const
{
    if (Handle())
    {
        return static_cast<int>(SendMessage(Handle(), LB_GETCURSEL, 0, 0));
    }
    else
    {
        return -1;
    }
}

void ListBox::SetTopIndex(int topIndex)
{
    if (Handle())
    {
        SendMessage(Handle(), LB_SETTOPINDEX, topIndex, 0);
    }
}

int ListBox::GetTopIndex() const
{
    if (Handle())
    {
        return static_cast<int>(SendMessage(Handle(), LB_GETTOPINDEX, 0, 0));
    }
    else
    {
        return -1;
    }
}

void ListBox::SelectedIndexChangedInternal()
{
    OnSelectedIndexChanged();
}

void ListBox::AddItem(const std::string& item)
{
    if (Handle())
    {
        std::u16string wstr = ToUtf16(item);
        SendMessage(Handle(), LB_ADDSTRING, 0, (LPARAM)wstr.c_str());
    }
}

void ListBox::DeleteItem(int index)
{
    if (Handle())
    {
        SendMessage(Handle(), LB_DELETESTRING, index, 0);
    }
}

void ListBox::InsertItem(int index, const std::string& item)
{
    if (Handle())
    {
        std::u16string wstr = ToUtf16(item);
        SendMessage(Handle(), LB_INSERTSTRING, index, (LPARAM)wstr.c_str());
    }
}

std::string ListBox::GetItem(int index) const
{
    if (Handle())
    {
        int size = static_cast<int>(SendMessage(Handle(), LB_GETTEXTLEN, index, 0));
        std::unique_ptr<char16_t> buf(new char16_t(size * sizeof(char16_t)));
        SendMessage(Handle(), LB_GETTEXT, index, (LPARAM)buf.get());
        return ToUtf8(buf.get());
    }
    else
    {
        return std::string();
    }
}

void ListBox::SetItem(int index, const std::string& item)
{
    DeleteItem(index);
    InsertItem(index, item);
}

int ListBox::GetItemCount() const
{
    if (Handle())
    {
        return static_cast<int>(SendMessage(Handle(), LB_GETCOUNT, 0, 0));
    }
    else
    {
        return -1;
    }
}

void ListBox::OnCreated()
{
    Control::OnCreated();
    Graphics graphics(Handle());
    const FontHandle& fontHandle = GetFontHandle(graphics);
    if (!fontHandle.IsNull())
    {
        SendMessage(Handle(), WM_SETFONT, reinterpret_cast<WPARAM>(fontHandle.Handle()), 0);
    }
}

void ListBox::OnSelectedIndexChanged()
{
    selectedIndexChanged.Fire();
}

} } // cmajor::wing
