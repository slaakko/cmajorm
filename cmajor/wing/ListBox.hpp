// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_LIST_BOX_INCLUDED
#define CMAJOR_WING_LIST_BOX_INCLUDED
#include <wing/Control.hpp>

namespace cmajor { namespace wing {

using SelectedIndexChangedEvent = Event;

struct WING_API ListBoxCreateParams
{
    ListBoxCreateParams();
    ListBoxCreateParams& Defaults();
    ListBoxCreateParams& WindowClassName(const std::string& windowClassName_);
    ListBoxCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    ListBoxCreateParams& WindowStyle(int windowStyle_);
    ListBoxCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    ListBoxCreateParams& BackgroundColor(const Color& backgroundColor_);
    ListBoxCreateParams& Text(const std::string& text_);
    ListBoxCreateParams& Location(Point location_);
    ListBoxCreateParams& SetSize(Size size_);
    ListBoxCreateParams& SetAnchors(Anchors anchors_);
    ListBoxCreateParams& SetDock(Dock dock_);
    ControlCreateParams controlCreateParams;
};

class WING_API ListBox : public Control
{
public:
    ListBox(ListBoxCreateParams& createParams);
    bool IsListBox() const override { return true; }
    void SetSelectedIndex(int selectedIndex);
    int GetSelectedIndex() const;
    void SetTopIndex(int topIndex);
    int GetTopIndex() const;
    void AddItem(const std::string& item);
    std::string GetItem(int index) const;
    void DeleteItem(int index);
    void InsertItem(int index, const std::string& item);
    void SetItem(int index, const std::string& item);
    int GetItemCount() const;
    void SelectedIndexChangedInternal();
    SelectedIndexChangedEvent& SelectedIndexChanged() { return selectedIndexChanged; }
protected:
    void OnCreated() override;
    virtual void OnSelectedIndexChanged();
private:
    SelectedIndexChangedEvent selectedIndexChanged;
};

} } // cmajor::wing

#endif // CMAJOR_WING_LIST_BOX_INCLUDED
