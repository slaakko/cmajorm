// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/ComboBox.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace wing {

using namespace soulng::unicode;

ComboBoxCreateParams::ComboBoxCreateParams() : controlCreateParams()
{
    controlCreateParams.WindowClassName("COMBOBOX");
    controlCreateParams.WindowStyle(DefaultChildWindowStyle() | WS_TABSTOP | CBS_DROPDOWNLIST);
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    controlCreateParams.BackgroundColor(Color::White);
}

ComboBoxCreateParams& ComboBoxCreateParams::Defaults()
{
    return *this;
}

ComboBoxCreateParams& ComboBoxCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

ComboBoxCreateParams& ComboBoxCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

ComboBoxCreateParams& ComboBoxCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

ComboBoxCreateParams& ComboBoxCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

ComboBoxCreateParams& ComboBoxCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

ComboBoxCreateParams& ComboBoxCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

ComboBoxCreateParams& ComboBoxCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

ComboBoxCreateParams& ComboBoxCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

ComboBoxCreateParams& ComboBoxCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

ComboBoxCreateParams& ComboBoxCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

ComboBox::ComboBox(ComboBoxCreateParams& createParams) : Control(createParams.controlCreateParams)
{
}

void ComboBox::AddItem(const std::string& item)
{
    if (Handle())
    {
        std::u16string wstr = ToUtf16(item);
        SendMessage(Handle(), CB_ADDSTRING, 0, (LPARAM)wstr.c_str());
    }
}

std::string ComboBox::GetItem(int index) const
{
    if (Handle())
    {
        int size = static_cast<int>(SendMessage(Handle(), CB_GETLBTEXTLEN, index, 0));
        std::unique_ptr<char16_t> buf(new char16_t(size * sizeof(char16_t)));
        SendMessage(Handle(), CB_GETLBTEXT, index, (LPARAM)buf.get());
        return ToUtf8(buf.get());
    }
    else
    {
        return std::string();
    }
}

void ComboBox::DeleteItem(int index)
{
    if (Handle())
    {
        SendMessage(Handle(), CB_DELETESTRING, index, 0);
    }
}

void ComboBox::InsertItem(int index, const std::string& item)
{
    if (Handle())
    {
        std::u16string wstr = ToUtf16(item);
        SendMessage(Handle(), CB_INSERTSTRING, index, (LPARAM)wstr.c_str());
    }
}

void ComboBox::SetItem(int index, const std::string& item)
{
    DeleteItem(index);
    InsertItem(index, item);
}

int ComboBox::GetItemCount() const
{
    if (Handle())
    {
        return static_cast<int>(SendMessage(Handle(), CB_GETCOUNT, 0, 0));
    }
    else
    {
        return -1;
    }
}

void ComboBox::SetSelectedIndex(int selectedIndex)
{
    if (Handle())
    {
        SendMessage(Handle(), CB_SETCURSEL, selectedIndex, 0);
    }
}

int ComboBox::GetSelectedIndex() const
{
    if (Handle())
    {
        return static_cast<int>(SendMessage(Handle(), CB_GETCURSEL, 0, 0));
    }
    else
    {
        return -1;
    }
}

void ComboBox::SelectedIndexChangedInternal()
{
    OnSelectedIndexChanged();
}

void ComboBox::OnCreated()
{
    Control::OnCreated();
    Graphics graphics(Handle());
    const FontHandle& fontHandle = GetFontHandle(graphics);
    if (!fontHandle.IsNull())
    {
        SendMessage(Handle(), WM_SETFONT, reinterpret_cast<WPARAM>(fontHandle.Handle()), 0);
    }
}

void ComboBox::OnSelectedIndexChanged()
{
    selectedIndexChanged.Fire();
}

} } // cmajor::wing
