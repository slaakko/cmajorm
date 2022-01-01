// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_SELECT_PROJECT_TYPE_INCLUDED
#define CMCODE_SELECT_PROJECT_TYPE_INCLUDED
#include <cmajor/wing/Window.hpp>
#include <cmajor/wing/IconListView.hpp>
#include <sngcm/ast/Project.hpp>
#include <map>

namespace cmcode {

using namespace cmajor::wing;

Color DefaultSelectProjectTypeIconListViewBorderColor();

class SelectProjectTypeDialog : public Window
{
public:
    SelectProjectTypeDialog();
    sngcm::ast::Target GetTarget();
protected:
    void OnPaint(PaintEventArgs& args) override;
private:
    void IconListViewSelectedIndexChanged();
    void IconDoubleClicked();
    void KeyPreview(Keys key, KeyState keyState, bool& handled);
    IconListView* iconListView;
    Control* borderedIconListView;
    Button* selectButton;
    Button* cancelButton;
    sngcm::ast::Target target;
    std::map<int, sngcm::ast::Target> targetMap;
};

} // namespace cmcode

#endif // CMCODE_SELECT_PROJECT_TYPE_INCLUDED
