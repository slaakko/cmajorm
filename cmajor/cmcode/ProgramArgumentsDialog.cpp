// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/ProgramArgumentsDialog.hpp>
#include <cmajor/wing/BorderedControl.hpp>
#include <cmajor/wing/Metrics.hpp>
#include <cmajor/wing/PaddedControl.hpp>

namespace cmcode {

ProgramArgumentsDialog::ProgramArgumentsDialog() : 
    Window(WindowCreateParams().Text("Program Arguments").WindowClassName("cmcode.ProgramArgumentsDialog").WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).
    BackgroundColor(DefaultControlBackgroundColor()).SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(140), ScreenMetrics::Get().MMToVerticalPixels(50)))),
    okButton(nullptr),
    cancelButton(nullptr),
    programArgumentsTextBox(nullptr)
{
    Size s = GetSize();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();

    std::unique_ptr<TextBox> programArgumentsTextBoxPtr(new TextBox(TextBoxCreateParams().SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(120), ScreenMetrics::Get().MMToHorizontalPixels(5)))));
    programArgumentsTextBox = programArgumentsTextBoxPtr.get();
    std::unique_ptr<Control> paddedTextBox(new PaddedControl(PaddedControlCreateParams(programArgumentsTextBoxPtr.release()).SetSize(PaddedSize(programArgumentsTextBox->GetSize(), DefaultPadding()))));
    std::unique_ptr<Control> borderedTextBox(new BorderedControl(BorderedControlCreateParams(paddedTextBox.release()).Location(Point(16, 16)).
        SetSize(BorderedSize(PaddedSize(programArgumentsTextBox->GetSize(), DefaultPadding()), BorderStyle::single)).SetAnchors(Anchors::top | Anchors::left)));
    AddChild(borderedTextBox.release());

    int x = s.Width - defaultButtonSize.Width - defaultControlSpacing.Width;
    int y = s.Height - defaultButtonSize.Height - defaultControlSpacing.Height;
    std::unique_ptr<Button> cancelButtonPtr(new Button(ControlCreateParams().Text("Cancel").Location(Point(x, y)).SetSize(defaultButtonSize).SetAnchors(Anchors::bottom | Anchors::right)));
    cancelButton = cancelButtonPtr.get();
    cancelButton->SetDialogResult(DialogResult::cancel);
    x = x - defaultButtonSize.Width - defaultControlSpacing.Width;
    std::unique_ptr<Button> okButtonPtr(new Button(ControlCreateParams().Location(Point(x, y)).SetSize(defaultButtonSize).Text("OK").SetAnchors(Anchors::right | Anchors::bottom)));
    okButton = okButtonPtr.get();
    okButton->SetDialogResult(DialogResult::ok);
    okButton->SetDefault();
    AddChild(okButtonPtr.release());
    AddChild(cancelButtonPtr.release());
    SetDefaultButton(okButton);
    SetCancelButton(cancelButton);
    programArgumentsTextBox->SetFocus();
}

void ProgramArgumentsDialog::SetProgramArguments(const std::string& programArguments)
{
    programArgumentsTextBox->SetText(programArguments);
    programArgumentsTextBox->SelectAll();
}

std::string ProgramArgumentsDialog::ProgramArguments() const
{
    return programArgumentsTextBox->Text();
}

} // namespace cmcode
