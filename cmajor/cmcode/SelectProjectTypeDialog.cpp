// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/SelectProjectTypeDialog.hpp>
#include <cmajor/wing/BorderedControl.hpp>
#include <cmajor/wing/Button.hpp>
#include <cmajor/wing/Metrics.hpp>

namespace cmcode {

Color DefaultSelectProjectTypeIconListViewBorderColor()
{
    return Color(204, 206, 219);
}

SelectProjectTypeDialog::SelectProjectTypeDialog() : 
    Window(WindowCreateParams().WindowStyle(DialogWindowStyle()).WindowClassName("cmcode.SelectProjectTypeDialog").Text("Select Project Type").
    WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).BackgroundColor(DefaultControlBackgroundColor()).
    SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(180), ScreenMetrics::Get().MMToVerticalPixels(100)))),
    iconListView(nullptr),
    borderedIconListView(nullptr),
    selectButton(nullptr),
    cancelButton(nullptr),
    target(sngcm::ast::Target::program)
{
    targetMap[0] = sngcm::ast::Target::program;
    targetMap[1] = sngcm::ast::Target::winguiapp;
    targetMap[2] = sngcm::ast::Target::library;
    targetMap[3] = sngcm::ast::Target::winlib;
    Size s = GetSize();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Point iconListViewLocation(defaultControlSpacing.Width, defaultControlSpacing.Height);
    std::unique_ptr<IconListView> iconListViewPtr(new IconListView(IconListViewCreateParams().SetSize(Size(80, 80))));
    iconListView = iconListViewPtr.get();
    iconListView->AddItem(new IconListViewItem("console.app.bitmap", "Console Application"));
    iconListView->AddItem(new IconListViewItem("win.gui.app.bitmap", "Windows GUI Application"));
    iconListView->AddItem(new IconListViewItem("lib.bitmap", "Library"));
    iconListView->AddItem(new IconListViewItem("win.lib.bitmap", "Windows Library"));
    iconListView->SelectedIndexChanged().AddHandler(this, &SelectProjectTypeDialog::IconListViewSelectedIndexChanged);
    iconListView->ItemDoubleClicked().AddHandler(this, &SelectProjectTypeDialog::IconDoubleClicked);
    std::unique_ptr<Control> borderedIconListViewPtr(new BorderedControl(BorderedControlCreateParams(iconListViewPtr.release()).
        NormalSingleBorderColor(DefaultSelectProjectTypeIconListViewBorderColor()).FocusedSingleBorderColor(DefaultFocusedSingleBorderColor()).
        Location(iconListViewLocation).
        SetSize(Size(80, 80)).
        SetAnchors(Anchors::left | Anchors::top)));
    borderedIconListView = borderedIconListViewPtr.get();
    AddChild(borderedIconListViewPtr.release());
    int x = s.Width - defaultButtonSize.Width - defaultControlSpacing.Width;
    int y = s.Height - defaultButtonSize.Height - defaultControlSpacing.Height;
    std::unique_ptr<Button> cancelButtonPtr(new Button(ControlCreateParams().Text("Cancel").Location(Point(x, y)).SetSize(defaultButtonSize).SetAnchors(Anchors::bottom | Anchors::right)));
    cancelButton = cancelButtonPtr.get();
    cancelButton->SetDialogResult(DialogResult::cancel);
    x = x - defaultButtonSize.Width - defaultControlSpacing.Width;
    std::unique_ptr<Button> selectButtonPtr(new Button(ControlCreateParams().Text("Select").Location(Point(x, y)).SetSize(defaultButtonSize).SetAnchors(Anchors::bottom | Anchors::right)));
    selectButton = selectButtonPtr.get();
    selectButton->SetDefault();
    selectButton->SetDialogResult(DialogResult::ok);
    selectButton->Disable();
    AddChild(selectButtonPtr.release());
    AddChild(cancelButtonPtr.release());
    SetDefaultButton(selectButton);
    SetCancelButton(cancelButton);
}

sngcm::ast::Target SelectProjectTypeDialog::GetTarget() 
{
    int selectedIndex = iconListView->SelectedIndex();
    if (selectedIndex != -1)
    {
        auto it = targetMap.find(selectedIndex);
        if (it != targetMap.cend())
        {
            target = it->second;
        }
    }
    return target;
}

void SelectProjectTypeDialog::OnPaint(PaintEventArgs& args)
{
    Window::OnPaint(args);
    Size origSize = iconListView->GetSize();
    iconListView->Invalidate();
    iconListView->Update();
    Size size = iconListView->GetSize();
    if (size != origSize)
    {
        borderedIconListView->SetSize(BorderedSize(size, BorderStyle::single));
    }
}

void SelectProjectTypeDialog::IconListViewSelectedIndexChanged()
{
    int selectedIndex = iconListView->SelectedIndex();
    if (selectedIndex != -1)
    {
        selectButton->Enable();
    }
    else
    {
        selectButton->Disable();
    }
}

void SelectProjectTypeDialog::IconDoubleClicked()
{
    selectButton->DoClick();
}

} // namespace cmcode
