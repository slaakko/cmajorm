// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/BuildSettingsDialog.hpp>
#include <cmajor/wing/BorderedControl.hpp>
#include <cmajor/wing/Label.hpp>
#include <cmajor/wing/Metrics.hpp>
#include <cmajor/wing/PaddedControl.hpp>
#include <boost/lexical_cast.hpp>

namespace cmcode {

bool IsValidProjectBuildThreadsValue(const std::string& s)
{
    try
    {
        for (char c : s)
        {
            if (c < '0' || c > '9') return false;
        }
        int threads = boost::lexical_cast<int>(s);
        if (threads >= 0 && threads < 256) return true;
    }
    catch (...)
    {
    }
    return false;
}

BuildSettingsDialog::BuildSettingsDialog() : Window(WindowCreateParams().WindowClassName("cmcode.BuildSettingsDialog").WindowStyle(DialogWindowStyle()).Text("Build Settings").
    WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).BackgroundColor(DefaultControlBackgroundColor()).
    Location(DefaultLocation()).SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(160), ScreenMetrics::Get().MMToVerticalPixels(100)))),
    okButton(nullptr), cancelButton(nullptr), projectBuildThreadsTextBox(nullptr), singleThreadedCompileCheckBox(nullptr), generateIntermediateCodeFilesCheckBox(nullptr), 
    generateOptimizedIntermediateCodeFilesCheckBox(nullptr)
{
    Size s = GetSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();
    Size defaultTextBoxSize = ScreenMetrics::Get().DefaultTextBoxSize();
    Size defaultLabelSize = ScreenMetrics::Get().DefaultLabelSize();
    Size defaultCheckBoxSize = ScreenMetrics::Get().DefaultCheckBoxSize();

    Point projectBuildThreadsLabelLocation(16, 16);
    std::unique_ptr<Label> label(new Label(LabelCreateParams().Text("Number of project build threads (0=use default number of threads, 1=build projects sequentially):").
        Location(projectBuildThreadsLabelLocation).SetSize(defaultLabelSize).SetAnchors(Anchors::top | Anchors::left)));
    AddChild(label.release());

    Point projectBuildThreadsTextBoxLocation(16, 16 + 24);
    std::unique_ptr<TextBox> projectBuildThreadsTextBoxPtr(new TextBox(TextBoxCreateParams().Text("0")));
    projectBuildThreadsTextBox = projectBuildThreadsTextBoxPtr.get();
    projectBuildThreadsTextBox->TextChanged().AddHandler(this, &BuildSettingsDialog::TextBoxTextChanged);
    std::unique_ptr<Control> paddedTextBox(new PaddedControl(PaddedControlCreateParams(projectBuildThreadsTextBoxPtr.release()).
        SetSize(PaddedSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(10), defaultTextBoxSize.Height + 4), DefaultPadding()))));
    std::unique_ptr<Control> borderedTextBox(new BorderedControl(BorderedControlCreateParams(paddedTextBox.release()).Location(projectBuildThreadsTextBoxLocation).
        SetSize(BorderedSize(PaddedSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(10), defaultTextBoxSize.Height + 4), DefaultPadding()), BorderStyle::single)).SetAnchors(Anchors::top | Anchors::left)));
    AddChild(borderedTextBox.release());

    Point singleThreadedCheckBoxLocation(16, 16 + 16 + 24 + 24);
    std::unique_ptr<CheckBox> singleThreadedCheckBoxPtr(new CheckBox(CheckBoxCreateParams().Text("Compile source files in a project sequentially using only one thread").
        Location(singleThreadedCheckBoxLocation).SetSize(defaultCheckBoxSize).SetAnchors(Anchors::top | Anchors::left)));
    singleThreadedCompileCheckBox = singleThreadedCheckBoxPtr.get();
    AddChild(singleThreadedCheckBoxPtr.release());

    Point generateIntermediateCodeFilesCheckBoxLocation(16, 16 + 16 + 24 + 24 + 24);
    std::unique_ptr<CheckBox> generateIntermediateCodeCheckBoxPtr(new CheckBox(CheckBoxCreateParams().Text("Generate intermediate code files").
        Location(generateIntermediateCodeFilesCheckBoxLocation).SetSize(defaultCheckBoxSize).SetAnchors(Anchors::top | Anchors::left)));
    generateIntermediateCodeFilesCheckBox = generateIntermediateCodeCheckBoxPtr.get();
    AddChild(generateIntermediateCodeCheckBoxPtr.release());

    Point generateOptimizedIntermediateCodeFilesCheckBoxLocation(16, 16 + 16 + 24 + 24 + 24 + 24);
    std::unique_ptr<CheckBox> generateOptimizedIntermediateCodeCheckBoxPtr(new CheckBox(CheckBoxCreateParams().Text("Generate optimized intermediate code files").
        Location(generateOptimizedIntermediateCodeFilesCheckBoxLocation).SetSize(defaultCheckBoxSize).SetAnchors(Anchors::top | Anchors::left)));
    generateOptimizedIntermediateCodeFilesCheckBox = generateOptimizedIntermediateCodeCheckBoxPtr.get();
    AddChild(generateOptimizedIntermediateCodeCheckBoxPtr.release());

    int x = s.Width - defaultButtonSize.Width - defaultControlSpacing.Width;
    int y = s.Height - defaultButtonSize.Height - defaultControlSpacing.Height;
    std::unique_ptr<Button> cancelButtonPtr(new Button(ControlCreateParams().Text("Cancel").Location(Point(x, y)).SetSize(defaultButtonSize).SetAnchors(Anchors::bottom | Anchors::right)));
    cancelButton = cancelButtonPtr.get();
    cancelButton->SetDialogResult(DialogResult::cancel);
    x = x - defaultButtonSize.Width - defaultControlSpacing.Width;
    std::unique_ptr<Button> okButtonPtr(new Button(ControlCreateParams().Text("OK").Location(Point(x, y)).SetSize(defaultButtonSize).SetAnchors(Anchors::bottom | Anchors::right)));
    okButton = okButtonPtr.get();
    okButton->SetDefault();
    okButton->SetDialogResult(DialogResult::ok);
    AddChild(okButtonPtr.release());
    AddChild(cancelButtonPtr.release());
    SetDefaultButton(okButton);
    SetCancelButton(cancelButton);
    projectBuildThreadsTextBox->SetFocus();
}

void BuildSettingsDialog::TextBoxTextChanged()
{
    if (IsValidProjectBuildThreadsValue(projectBuildThreadsTextBox->Text()))
    {
        okButton->Enable();
    }
    else
    {
        okButton->Disable();
    }
}

void BuildSettingsDialog::SetValuesFrom(const BuildSettings& buildSettings)
{
    projectBuildThreadsTextBox->SetText(std::to_string(buildSettings.numberOfProjectBuildThreads));
    singleThreadedCompileCheckBox->SetChecked(buildSettings.singleThreadedCompile);
    generateIntermediateCodeFilesCheckBox->SetChecked(buildSettings.generateIntermediateCodeFiles);
    generateOptimizedIntermediateCodeFilesCheckBox->SetChecked(buildSettings.generateOptimizedIntermediateCodeFiles);
}

BuildSettings BuildSettingsDialog::GetValues() const
{
    BuildSettings buildSettings;
    buildSettings.numberOfProjectBuildThreads = boost::lexical_cast<int>(projectBuildThreadsTextBox->Text());
    buildSettings.singleThreadedCompile = singleThreadedCompileCheckBox->Checked();
    buildSettings.generateIntermediateCodeFiles = generateIntermediateCodeFilesCheckBox->Checked();
    buildSettings.generateOptimizedIntermediateCodeFiles = generateOptimizedIntermediateCodeFilesCheckBox->Checked();
    return buildSettings;
}

} // namespace cmcode
