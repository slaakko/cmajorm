// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/OptionsDialog.hpp>
#include <cmajor/wing/BorderedControl.hpp>
#include <cmajor/wing/Label.hpp>
#include <cmajor/wing/Metrics.hpp>
#include <cmajor/wing/PaddedControl.hpp>
#include <cmajor/wing/Theme.hpp>
#include <boost/lexical_cast.hpp>

namespace cmcode {

bool IsValidRecentSolutionsNumber(const std::string& text)
{
    try
    {
        if (text.empty()) return false;
        for (char c : text)
        {
            if (c < '0' || c > '9') return false;
        }
        int number = boost::lexical_cast<int>(text);
        if (number > 0 && number <= 20) return true;
    }
    catch (...)
    {
    }
    return false;
}

bool IsValidParsingFrequency(const std::string& text)
{
    try
    {
        if (text.empty()) return false;
        for (char c : text)
        {
            if (c < '0' || c > '9') return false;
        }
        int number = boost::lexical_cast<int>(text);
        if (number > 0 && number <= 60000) return true;
    }
    catch (...)
    {
    }
    return false;
}

OptionsDialog::OptionsDialog() : Window(WindowCreateParams().WindowClassName("cmcode.OptionsDialog").WindowStyle(DialogWindowStyle()).Text("Options").
    WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).BackgroundColor(DefaultControlBackgroundColor()).
    Location(DefaultLocation()).SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(160), ScreenMetrics::Get().MMToVerticalPixels(120)))),
    okButton(nullptr),
    cancelButton(nullptr),
    showStartupDialogCheckBox(nullptr),
    numberOfRecentSolutionsTextBox(nullptr),
    codeCompletionCheckBox(nullptr),
    parsingFrequencyTextBox(nullptr)
{
    themes = GetThemes();
    selectedThemeIndex = 0;

    Size s = GetSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();
    Size defaultTextBoxSize = ScreenMetrics::Get().DefaultTextBoxSize();
    Size defaultLabelSize = ScreenMetrics::Get().DefaultLabelSize();
    Size defaultCheckBoxSize = ScreenMetrics::Get().DefaultCheckBoxSize();
    Size defaultComboBoxSize = ScreenMetrics::Get().DefaultComboBoxSize();

    Point showStartupDialogCheckBoxLocation(16, 16);
    std::unique_ptr<CheckBox> showStartupDialogCheckBoxPtr(new CheckBox(CheckBoxCreateParams().Text("Show startup dialog").
        Location(showStartupDialogCheckBoxLocation).SetSize(defaultCheckBoxSize).SetAnchors(Anchors::top | Anchors::left)));
    showStartupDialogCheckBox = showStartupDialogCheckBoxPtr.get();
    showStartupDialogCheckBox->SetChecked(true);
    AddChild(showStartupDialogCheckBoxPtr.release());

    Point numberOfRecentSolutionsLabelLocation(16, 16 + 24);
    std::unique_ptr<Label> numberOfRecentSolutionsLabel(new Label(LabelCreateParams().Text("Number of recent solutions in the startup dialog:").
        Location(numberOfRecentSolutionsLabelLocation).SetSize(defaultLabelSize).SetAnchors(Anchors::top | Anchors::left)));
    AddChild(numberOfRecentSolutionsLabel.release());

    Point numberOfRecentSolutionsTextBoxLocation(16, 16 + 24 + 24);
    std::unique_ptr<TextBox> numberOfRecentSolutionsTextBoxPtr(new TextBox(TextBoxCreateParams().Text("5")));
    numberOfRecentSolutionsTextBox = numberOfRecentSolutionsTextBoxPtr.get();
    numberOfRecentSolutionsTextBox->TextChanged().AddHandler(this, &OptionsDialog::TextBoxTextChanged);
    std::unique_ptr<Control> paddedTextBox(new PaddedControl(PaddedControlCreateParams(numberOfRecentSolutionsTextBoxPtr.release()).
        SetSize(PaddedSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(10), defaultTextBoxSize.Height + 4), DefaultPadding()))));
    std::unique_ptr<Control> borderedTextBox(new BorderedControl(BorderedControlCreateParams(paddedTextBox.release()).Location(numberOfRecentSolutionsTextBoxLocation).
        SetSize(BorderedSize(PaddedSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(10), defaultTextBoxSize.Height + 4), DefaultPadding()), BorderStyle::single)).SetAnchors(Anchors::top | Anchors::left)));
    AddChild(borderedTextBox.release());

    std::unique_ptr<CheckBox> codeCompletionCheckBoxPtr(new CheckBox(CheckBoxCreateParams().Text("Code completion").Location(Point(16, 24 + 24 + 24 + 24)).SetAnchors(Anchors::top | Anchors::left)));
    codeCompletionCheckBox = codeCompletionCheckBoxPtr.get();
    codeCompletionCheckBox->SetChecked(true);
    AddChild(codeCompletionCheckBoxPtr.release());

    Point parsingFrequencyLabelLocation(16, 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> parsingFrequencyLabel(new Label(LabelCreateParams().Text("Source code parsing frequency for code completion (milliseconds):").
        Location(parsingFrequencyLabelLocation).SetSize(defaultLabelSize).SetAnchors(Anchors::top | Anchors::left)));
    AddChild(parsingFrequencyLabel.release());

    Point parsingFrequencyTextBoxLocation(16, 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<TextBox> parsingFrequencyTextBoxPtr(new TextBox(TextBoxCreateParams().Text("1000")));
    parsingFrequencyTextBox = parsingFrequencyTextBoxPtr.get();
    parsingFrequencyTextBox->TextChanged().AddHandler(this, &OptionsDialog::ParsingFrequencyTextBoxTextChanged);
    std::unique_ptr<Control> paddedPFTextBox(new PaddedControl(PaddedControlCreateParams(parsingFrequencyTextBoxPtr.release()).
        SetSize(PaddedSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(10), defaultTextBoxSize.Height + 4), DefaultPadding()))));
    std::unique_ptr<Control> borderedPFTextBox(new BorderedControl(BorderedControlCreateParams(paddedPFTextBox.release()).Location(parsingFrequencyTextBoxLocation).
        SetSize(BorderedSize(PaddedSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(10), defaultTextBoxSize.Height + 4), DefaultPadding()), BorderStyle::single)).SetAnchors(Anchors::top | Anchors::left)));
    AddChild(borderedPFTextBox.release());

    Point themeLabelLocation(16, 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> themeLabel(new Label(LabelCreateParams().Text("Color Theme:").
        Location(themeLabelLocation).SetSize(defaultLabelSize).SetAnchors(Anchors::top | Anchors::left)));
    AddChild(themeLabel.release());

    Point themeComboBoxLocation(16, 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    Size comboBoxSize(defaultComboBoxSize.Width, defaultComboBoxSize.Height * 4);
    std::unique_ptr<ComboBox> themeComboBoxPtr(new ComboBox(ComboBoxCreateParams().SetAnchors(Anchors::top | Anchors::left).Location(themeComboBoxLocation).SetSize(comboBoxSize)));
    themeComboBox = themeComboBoxPtr.get();
    AddChild(themeComboBoxPtr.release());

    int x = s.Width - defaultButtonSize.Width - defaultControlSpacing.Width;
    int y = s.Height - defaultButtonSize.Height - defaultControlSpacing.Height;
    std::unique_ptr<Button> cancelButtonPtr(new Button(ControlCreateParams().Location(Point(x, y)).SetSize(defaultButtonSize).Text("Cancel").SetAnchors(Anchors::right | Anchors::bottom)));
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
    showStartupDialogCheckBox->SetFocus();
}

void OptionsDialog::CheckValid() 
{
    if (IsValidRecentSolutionsNumber(numberOfRecentSolutionsTextBox->Text()) && IsValidParsingFrequency(parsingFrequencyTextBox->Text()))
    {
        okButton->Enable();
    }
    else
    {
        okButton->Disable();
    }
}

void OptionsDialog::TextBoxTextChanged()
{
    CheckValid();
}

void OptionsDialog::ParsingFrequencyTextBoxTextChanged()
{
    CheckValid();
}

void OptionsDialog::SetOptionsFrom(const Options& options)
{
    showStartupDialogCheckBox->SetChecked(options.showStartupDialog);
    numberOfRecentSolutionsTextBox->SetText(std::to_string(options.numberOfRecentSolutions));
    codeCompletionCheckBox->SetChecked(options.codeCompletion);
    parsingFrequencyTextBox->SetText(std::to_string(options.parsingFrequency));
    int n = themes.size();
    for (int i = 0; i < n; ++i)
    {
        if (themes[i] == options.theme)
        {
            selectedThemeIndex = i;
            break;
        }
    }
}

Options OptionsDialog::GetOptions() const
{
    Options options;
    options.showStartupDialog = showStartupDialogCheckBox->Checked();
    options.numberOfRecentSolutions = boost::lexical_cast<int>(numberOfRecentSolutionsTextBox->Text());
    options.codeCompletion = codeCompletionCheckBox->Checked();
    options.parsingFrequency = boost::lexical_cast<int>(parsingFrequencyTextBox->Text());
    options.theme = themes[themeComboBox->GetSelectedIndex()];
    return options;
}

void OptionsDialog::OnShown()
{
    Window::OnShown();
    int n = themes.size();
    for (int i = 0; i < n; ++i)
    {
        themeComboBox->AddItem(themes[i]);
    }
    themeComboBox->SetSelectedIndex(selectedThemeIndex);
}

} // namespace cmcode
