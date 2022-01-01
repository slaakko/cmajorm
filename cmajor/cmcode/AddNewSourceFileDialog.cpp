// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/AddNewSourceFileDialog.hpp>
#include <cmajor/wing/Label.hpp>
#include <cmajor/wing/Metrics.hpp>
#include <cmajor/wing/PaddedControl.hpp>
#include <cmajor/wing/BorderedControl.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmcode {

using namespace soulng::unicode;

bool IsValidSourceFileName(const std::string& name)
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

AddNewSourceFileDialog::AddNewSourceFileDialog() : Window(WindowCreateParams().WindowClassName("cmcode.AddNewSourceFileDialog").WindowStyle(DialogWindowStyle()).Text("Add New Source File").
    WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).BackgroundColor(DefaultControlBackgroundColor()).
    Location(DefaultLocation()).SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(160), ScreenMetrics::Get().MMToVerticalPixels(60)))),
    createButton(nullptr),
    cancelButton(nullptr),
    sourceFileNameTextBox(nullptr)
{
    Size s = GetSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();
    Size defaultTextBoxSize = ScreenMetrics::Get().DefaultTextBoxSize();
    Size defaultLabelSize = ScreenMetrics::Get().DefaultLabelSize();
    Size defaultCheckBoxSize = ScreenMetrics::Get().DefaultCheckBoxSize();

    Point sourceFileNameLabelLocation(16, 16);
    std::unique_ptr<Label> sourceFileNameLabelPtr(new Label(LabelCreateParams().Text("Source file name:").Location(sourceFileNameLabelLocation).SetSize(defaultLabelSize).
        SetAnchors(Anchors::left | Anchors::top)));
    AddChild(sourceFileNameLabelPtr.release());

    Size sourceFileNameTextBoxSize(ScreenMetrics::Get().MMToHorizontalPixels(120), defaultTextBoxSize.Height + 4);
    std::unique_ptr<TextBox> sourceFileNameTextBoxPtr(new TextBox(TextBoxCreateParams().Defaults()));
    sourceFileNameTextBox = sourceFileNameTextBoxPtr.get();
    sourceFileNameTextBox->TextChanged().AddHandler(this, &AddNewSourceFileDialog::SourceFileNameTextBoxTextChanged);
    Point sourceFileNameTextBoxLocation(16, 16 + 24);
    std::unique_ptr<Control> paddedSourceFileNameTextBox(new PaddedControl(PaddedControlCreateParams(sourceFileNameTextBoxPtr.release()).
        SetSize(PaddedSize(sourceFileNameTextBoxSize, DefaultPadding()))));
    std::unique_ptr<Control> borderedSourceFileNameTextBox(new BorderedControl(BorderedControlCreateParams(paddedSourceFileNameTextBox.release()).Location(sourceFileNameTextBoxLocation).
        SetSize(BorderedSize(PaddedSize(sourceFileNameTextBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(Anchors::left | Anchors::top)));
    AddChild(borderedSourceFileNameTextBox.release());

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
    sourceFileNameTextBox->SetFocus();
}

std::string AddNewSourceFileDialog::SourceFileName() const
{
    return sourceFileNameTextBox->Text();
}

void AddNewSourceFileDialog::SourceFileNameTextBoxTextChanged()
{
    if (IsValidSourceFileName(sourceFileNameTextBox->Text()))
    {
        createButton->Enable();
    }
    else
    {
        createButton->Disable();
    }
}

} // namespace cmcode
