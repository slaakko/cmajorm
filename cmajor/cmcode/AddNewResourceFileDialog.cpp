// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/AddNewResourceFileDialog.hpp>
#include <cmajor/wing/Label.hpp>
#include <cmajor/wing/Metrics.hpp>
#include <cmajor/wing/PaddedControl.hpp>
#include <cmajor/wing/BorderedControl.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmcode {

using namespace soulng::unicode;

bool IsValidResourceFileName(const std::string& name)
{
    if (name.empty()) return false;
    std::u32string s(ToUtf32(name));
    if (!IsIdStart(s[0]))
    {
        return false;
    }
    for (int i = 1; i < s.length(); ++i)
    {
        char32_t c = s[i];
        if (!IsIdCont(c) && c != '.')
        {
            return false;
        }
    }
    return true;
}

AddNewResourceFileDialog::AddNewResourceFileDialog() : Window(WindowCreateParams().WindowClassName("cmcode.AddNewResourceFileDialog").WindowStyle(DialogWindowStyle()).Text("Add New Resource File").
    WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).BackgroundColor(DefaultControlBackgroundColor()).
    Location(DefaultLocation()).SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(160), ScreenMetrics::Get().MMToVerticalPixels(60)))),
    createButton(nullptr),
    cancelButton(nullptr),
    resourceFileNameTextBox(nullptr)
{
    Size s = GetSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();
    Size defaultTextBoxSize = ScreenMetrics::Get().DefaultTextBoxSize();
    Size defaultLabelSize = ScreenMetrics::Get().DefaultLabelSize();
    Size defaultCheckBoxSize = ScreenMetrics::Get().DefaultCheckBoxSize();

    Point resourceFileNameLabelLocation(16, 16);
    std::unique_ptr<Label> resourceFileNameLabelPtr(new Label(LabelCreateParams().Text("Resource file name:").Location(resourceFileNameLabelLocation).SetSize(defaultLabelSize).
        SetAnchors(Anchors::left | Anchors::top)));
    AddChild(resourceFileNameLabelPtr.release());

    Size resourceFileNameTextBoxSize(ScreenMetrics::Get().MMToHorizontalPixels(120), defaultTextBoxSize.Height);
    std::unique_ptr<TextBox> resourceFileNameTextBoxPtr(new TextBox(TextBoxCreateParams().Defaults()));
    resourceFileNameTextBox = resourceFileNameTextBoxPtr.get();
    resourceFileNameTextBox->TextChanged().AddHandler(this, &AddNewResourceFileDialog::ResourceFileNameTextBoxTextChanged);
    Point resourceFileNameTextBoxLocation(16, 16 + 24);
    std::unique_ptr<Control> paddedResourceFileNameTextBox(new PaddedControl(PaddedControlCreateParams(resourceFileNameTextBoxPtr.release()).
        SetSize(PaddedSize(resourceFileNameTextBoxSize, DefaultPadding()))));
    std::unique_ptr<Control> borderedResourceFileNameTextBox(new BorderedControl(BorderedControlCreateParams(paddedResourceFileNameTextBox.release()).Location(resourceFileNameTextBoxLocation).
        SetSize(BorderedSize(PaddedSize(resourceFileNameTextBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(Anchors::left | Anchors::top)));
    AddChild(borderedResourceFileNameTextBox.release());

    int x = s.Width - defaultButtonSize.Width - defaultControlSpacing.Width;
    int y = s.Height - defaultButtonSize.Height - defaultControlSpacing.Height;
    std::unique_ptr<Button> cancelButtonPtr(new Button(ControlCreateParams().Location(Point(x, y)).SetSize(defaultButtonSize).Text("Cancel").SetAnchors(Anchors::right | Anchors::bottom)));
    cancelButton = cancelButtonPtr.get();
    cancelButton->SetDialogResult(DialogResult::cancel);
    x = x - defaultButtonSize.Width - defaultControlSpacing.Width;
    std::unique_ptr<Button> createButtonPtr(new Button(ControlCreateParams().Location(Point(x, y)).SetSize(defaultButtonSize).Text("Create").SetAnchors(Anchors::right | Anchors::bottom)));
    createButton = createButtonPtr.get();
    createButton->Disable();
    createButton->SetDialogResult(DialogResult::ok);
    createButton->SetDefault();
    AddChild(createButtonPtr.release());
    AddChild(cancelButtonPtr.release());
    SetDefaultButton(createButton);
    SetCancelButton(cancelButton);
    resourceFileNameTextBox->SetFocus();
}

std::string AddNewResourceFileDialog::ResourceFileName() const
{
    return resourceFileNameTextBox->Text();
}

void AddNewResourceFileDialog::ResourceFileNameTextBoxTextChanged()
{
    if (IsValidResourceFileName(resourceFileNameTextBox->Text()))
    {
        createButton->Enable();
    }
    else
    {
        createButton->Disable();
    }
}

} // namespace cmcode
