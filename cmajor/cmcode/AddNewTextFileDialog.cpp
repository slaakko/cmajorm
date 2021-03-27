// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/AddNewTextFileDialog.hpp>
#include <cmajor/wing/Label.hpp>
#include <cmajor/wing/Metrics.hpp>
#include <cmajor/wing/PaddedControl.hpp>
#include <cmajor/wing/BorderedControl.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmcode {

using namespace soulng::unicode;

bool IsValidFileName(const std::string& name)
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

AddNewTextFileDialog::AddNewTextFileDialog() : Window(WindowCreateParams().WindowClassName("cmcode.AddNewTextFileDialog").WindowStyle(DialogWindowStyle()).Text("Add New Text File").
    WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).BackgroundColor(DefaultControlBackgroundColor()).
    Location(DefaultLocation()).SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(160), ScreenMetrics::Get().MMToVerticalPixels(60)))),
    createButton(nullptr),
    cancelButton(nullptr),
    fileNameTextBox(nullptr)
{
    Size s = GetSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();
    Size defaultTextBoxSize = ScreenMetrics::Get().DefaultTextBoxSize();
    Size defaultLabelSize = ScreenMetrics::Get().DefaultLabelSize();
    Size defaultCheckBoxSize = ScreenMetrics::Get().DefaultCheckBoxSize();

    Point fileNameLabelLocation(16, 16);
    std::unique_ptr<Label> fileNameLabelPtr(new Label(LabelCreateParams().Text("Text file name:").Location(fileNameLabelLocation).SetSize(defaultLabelSize).
        SetAnchors(Anchors::left | Anchors::top)));
    AddChild(fileNameLabelPtr.release());

    Size fileNameTextBoxSize(ScreenMetrics::Get().MMToHorizontalPixels(120), defaultTextBoxSize.Height);
    std::unique_ptr<TextBox> fileNameTextBoxPtr(new TextBox(TextBoxCreateParams().Defaults()));
    fileNameTextBox = fileNameTextBoxPtr.get();
    fileNameTextBox->TextChanged().AddHandler(this, &AddNewTextFileDialog::FileNameTextBoxTextChanged);
    Point fileNameTextBoxLocation(16, 16 + 24);
    std::unique_ptr<Control> paddedFileNameTextBox(new PaddedControl(PaddedControlCreateParams(fileNameTextBoxPtr.release()).
        SetSize(PaddedSize(fileNameTextBoxSize, DefaultPadding()))));
    std::unique_ptr<Control> borderedFileNameTextBox(new BorderedControl(BorderedControlCreateParams(paddedFileNameTextBox.release()).Location(fileNameTextBoxLocation).
        SetSize(BorderedSize(PaddedSize(fileNameTextBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(Anchors::left | Anchors::top)));
    AddChild(borderedFileNameTextBox.release());

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
    fileNameTextBox->SetFocus();
}

std::string AddNewTextFileDialog::FileName() const
{
    return fileNameTextBox->Text();
}

void AddNewTextFileDialog::FileNameTextBoxTextChanged()
{
    if (IsValidFileName(fileNameTextBox->Text()))
    {
        createButton->Enable();
    }
    else
    {
        createButton->Disable();
    }
}

} // namespace cmcode
