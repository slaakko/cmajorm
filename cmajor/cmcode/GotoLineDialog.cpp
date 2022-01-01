// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/GotoLineDialog.hpp>
#include <cmajor/wing/Label.hpp>
#include <cmajor/wing/Metrics.hpp>
#include <cmajor/wing/PaddedControl.hpp>
#include <cmajor/wing/BorderedControl.hpp>
#include <boost/lexical_cast.hpp>

namespace cmcode {

bool IsValidLineNumber(const std::string& text)
{
    if (text.empty()) return false;
    int lineNumber = -1;
    try
    {
        for (char c : text)
        {
            if (c < '0' || c > '9') return false;
        }
        lineNumber = boost::lexical_cast<int>(text);
        if (lineNumber >= 1) return true;
    }
    catch (...)
    {
    }
    return false;
}

GotoLineDialog::GotoLineDialog() : Window(WindowCreateParams().WindowClassName("cmcode.GotoLineDialog").WindowStyle(DialogWindowStyle()).Text("Go To Line").
    WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).BackgroundColor(DefaultControlBackgroundColor()).
    Location(DefaultLocation()).SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(80), ScreenMetrics::Get().MMToVerticalPixels(60)))),
    textBox(nullptr),
    okButton(nullptr),
    cancelButton(nullptr)
{
    Size s = GetSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();
    Size defaultTextBoxSize = ScreenMetrics::Get().DefaultTextBoxSize();
    Size defaultLabelSize = ScreenMetrics::Get().DefaultLabelSize();

    Point labelLocation(16, 16);
    std::unique_ptr<Label> label(new Label(LabelCreateParams().Text("Line number:").Location(labelLocation).SetSize(defaultLabelSize).SetAnchors(Anchors::top | Anchors::left)));
    AddChild(label.release());

    Point textBoxLocation(16, 20 + 16);
    std::unique_ptr<TextBox> textBoxPtr(new TextBox(TextBoxCreateParams().Defaults()));
    textBox = textBoxPtr.get();
    textBox->TextChanged().AddHandler(this, &GotoLineDialog::TextBoxTextChanged);
    Size textBoxSize = Size(ScreenMetrics::Get().MMToHorizontalPixels(20), defaultTextBoxSize.Height + 4);
    std::unique_ptr<Control> paddedControl(new PaddedControl(PaddedControlCreateParams(textBoxPtr.release()).SetSize(PaddedSize(textBoxSize, DefaultPadding()))));
    std::unique_ptr<Control> borderedControl(new BorderedControl(BorderedControlCreateParams(paddedControl.release()).
        SetSize(BorderedSize(PaddedSize(textBoxSize, DefaultPadding()), BorderStyle::single)).Location(textBoxLocation).SetAnchors(Anchors::top | Anchors::left)));
    AddChild(borderedControl.release());

    int x = s.Width - defaultButtonSize.Width - defaultControlSpacing.Width;
    int y = s.Height - defaultButtonSize.Height - defaultControlSpacing.Height;
    std::unique_ptr<Button> cancelButtonPtr(new Button(ControlCreateParams().Text("Cancel").Location(Point(x, y)).SetSize(defaultButtonSize).SetAnchors(Anchors::bottom | Anchors::right)));
    cancelButton = cancelButtonPtr.get();
    cancelButton->SetDialogResult(DialogResult::cancel);
    x = x - defaultButtonSize.Width - defaultControlSpacing.Width;
    std::unique_ptr<Button> okButtonPtr(new Button(ControlCreateParams().Text("OK").Location(Point(x, y)).SetSize(defaultButtonSize).SetAnchors(Anchors::bottom | Anchors::right)));
    okButton = okButtonPtr.get();
    okButton->SetDialogResult(DialogResult::ok);
    okButton->SetDefault();
    okButton->Disable();
    AddChild(okButtonPtr.release());
    AddChild(cancelButtonPtr.release());
    SetDefaultButton(okButton);
    SetCancelButton(cancelButton);

    textBox->SetFocus();
}

void GotoLineDialog::TextBoxTextChanged()
{
    if (IsValidLineNumber(textBox->Text()))
    {
        okButton->Enable();
    }
    else
    {
        okButton->Disable();
    }
}

int GotoLineDialog::LineNumber() const
{
    return boost::lexical_cast<int>(textBox->Text());
}

} // namespace cmcode
