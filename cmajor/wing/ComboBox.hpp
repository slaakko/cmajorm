// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_COMBO_BOX_INCLUDED
#define CMAJOR_WING_COMBO_BOX_INCLUDED
#include <wing/ListBox.hpp>

namespace cmajor { namespace wing {

struct WING_API ComboBoxCreateParams
{
    ComboBoxCreateParams();
    ComboBoxCreateParams& Defaults();
    ComboBoxCreateParams& WindowClassName(const std::string& windowClassName_);
    ComboBoxCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    ComboBoxCreateParams& WindowStyle(int windowStyle_);
    ComboBoxCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    ComboBoxCreateParams& BackgroundColor(const Color& backgroundColor_);
    ComboBoxCreateParams& Text(const std::string& text_);
    ComboBoxCreateParams& Location(Point location_);
    ComboBoxCreateParams& SetSize(Size size_);
    ComboBoxCreateParams& SetAnchors(Anchors anchors_);
    ComboBoxCreateParams& SetDock(Dock dock_);
    ControlCreateParams controlCreateParams;
};

class WING_API ComboBox : public Control
{
public:
    ComboBox(ComboBoxCreateParams& createParams);
    bool IsComboBox() const override { return true; }
    void AddItem(const std::string& item);
    std::string GetItem(int index) const;
    void DeleteItem(int index);
    void InsertItem(int index, const std::string& item);
    void SetItem(int index, const std::string& item);
    int GetItemCount() const;
    void SetSelectedIndex(int selectedIndex);
    int GetSelectedIndex() const;
    void SelectedIndexChangedInternal();
    SelectedIndexChangedEvent& SelectedIndexChanged() { return selectedIndexChanged; }
protected:
    void OnCreated() override;
    virtual void OnSelectedIndexChanged();
private:
    SelectedIndexChangedEvent selectedIndexChanged;
};

} } // cmajor::wing

#endif // WING_COMBO_BOX_INCLUDED
