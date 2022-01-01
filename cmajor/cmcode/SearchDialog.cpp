// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/SearchDialog.hpp>
#include <cmajor/wing/BorderedControl.hpp>
#include <cmajor/wing/Label.hpp>
#include <cmajor/wing/Metrics.hpp>
#include <cmajor/wing/PaddedControl.hpp>

namespace cmcode {

SearchDialog::SearchDialog(bool hasCurrentFile_) : Window(WindowCreateParams().WindowClassName("cmcode.SearchDialog").WindowStyle(DialogWindowStyle()).Text("Search").
    WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).BackgroundColor(DefaultControlBackgroundColor()).
    Location(DefaultLocation()).SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(160), ScreenMetrics::Get().MMToVerticalPixels(90)))),
    searchButton(nullptr),
    cancelButton(nullptr),
    searchTextBox(nullptr),
    wholeWordsCheckBox(nullptr),
    caseInsensitiveCheckBox(nullptr),
    regularExpressionCheckBox(nullptr),
    whereListBox(nullptr),
    hasCurrentFile(hasCurrentFile_)
{
    Size s = GetSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();
    Size defaultTextBoxSize = ScreenMetrics::Get().DefaultTextBoxSize();
    Size defaultLabelSize = ScreenMetrics::Get().DefaultLabelSize();

    Point searchLabelLocation(16, 16);
    std::unique_ptr<Label> searchLabelPtr(new Label(LabelCreateParams().Text("Search:").Location(searchLabelLocation).SetSize(defaultLabelSize).SetAnchors(Anchors::top | Anchors::left)));
    AddChild(searchLabelPtr.release());

    Point searchTextBoxLocation(16, 16 + 24);
    std::unique_ptr<TextBox> searchTextBoxPtr(new TextBox(TextBoxCreateParams().Defaults()));
    searchTextBox = searchTextBoxPtr.get();
    searchTextBox->TextChanged().AddHandler(this, &SearchDialog::SearchTextBoxTextChanged);
    Size searchTextBoxSize(ScreenMetrics::Get().MMToHorizontalPixels(120), defaultTextBoxSize.Height + 4);
    std::unique_ptr<Control> paddedSearchTextBox(new PaddedControl(PaddedControlCreateParams(searchTextBoxPtr.release()).SetSize(PaddedSize(searchTextBoxSize, DefaultPadding())).
        Location(searchTextBoxLocation)));
    std::unique_ptr<Control> borderedSearchTextBox(new BorderedControl(BorderedControlCreateParams(paddedSearchTextBox.release()).Location(searchTextBoxLocation).
        SetSize(BorderedSize(PaddedSize(searchTextBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(Anchors::top | Anchors::left)));
    AddChild(borderedSearchTextBox.release());

    Point wholeWordsCheckBoxLocation(16, 24 + 24 + 24);
    std::unique_ptr<CheckBox> wholeWordsCheckBoxPtr(new CheckBox(CheckBoxCreateParams().Text("Whole words only").Location(wholeWordsCheckBoxLocation).SetAnchors(Anchors::top | Anchors::left)));
    wholeWordsCheckBox = wholeWordsCheckBoxPtr.get();
    AddChild(wholeWordsCheckBoxPtr.release());

    Point caseInsensitiveCheckBoxLocation(16, 24 + 24 + 24 + 24);
    std::unique_ptr<CheckBox> caseInsensitiveCheckBoxPtr(new CheckBox(CheckBoxCreateParams().Text("Case-insensitive").Location(caseInsensitiveCheckBoxLocation).SetAnchors(Anchors::top | Anchors::left)));
    caseInsensitiveCheckBox = caseInsensitiveCheckBoxPtr.get();
    AddChild(caseInsensitiveCheckBoxPtr.release());

    Point regularExpressionCheckBoxLocation(16, 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<CheckBox> regularExpressionCheckBoxPtr(new CheckBox(CheckBoxCreateParams().Text("Regular expression").Location(regularExpressionCheckBoxLocation).
        SetAnchors(Anchors::top | Anchors::left)));
    regularExpressionCheckBox = regularExpressionCheckBoxPtr.get();
    AddChild(regularExpressionCheckBoxPtr.release());

    Point whereLabelLocation(16, 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> whereLabelPtr(new Label(LabelCreateParams().Text("Where:").Location(whereLabelLocation).SetSize(defaultLabelSize).SetAnchors(Anchors::top | Anchors::left)));
    AddChild(whereLabelPtr.release());

    Point listBoxLocation(16, 24 + 24 + 24 + 24 + 24 + 24 + 24);
    Size listBoxSize(ScreenMetrics::Get().MMToHorizontalPixels(70), ScreenMetrics::Get().MMToVerticalPixels(16));
    std::unique_ptr<ListBox> listBoxPtr(new ListBox(ListBoxCreateParams().Location(listBoxLocation).SetSize(listBoxSize).SetAnchors(Anchors::top | Anchors::left)));
    whereListBox = listBoxPtr.get();
    std::unique_ptr<Control> borderedListBoxPtr(new BorderedControl(BorderedControlCreateParams(listBoxPtr.release()).Location(listBoxLocation).SetSize(listBoxSize).SetAnchors(Anchors::top | Anchors::left)));
    Control* borderedListBox = borderedListBoxPtr.get();
    AddChild(borderedListBoxPtr.release());
    borderedListBox->SetSize(BorderedSize(whereListBox->GetSize(), BorderStyle::single));
    if (hasCurrentFile)
    {
        whereListBox->AddItem("Current file");
    }
    whereListBox->AddItem("Active project");
    whereListBox->AddItem("Entire solution");
    whereListBox->SetSelectedIndex(0);

    int x = s.Width - defaultButtonSize.Width - defaultControlSpacing.Width;
    int y = s.Height - defaultButtonSize.Height - defaultControlSpacing.Height;
    std::unique_ptr<Button> cancelButtonPtr(new Button(ControlCreateParams().Location(Point(x, y)).SetSize(defaultButtonSize).Text("Cancel").SetAnchors(Anchors::right | Anchors::bottom)));
    cancelButton = cancelButtonPtr.get();
    cancelButton->SetDialogResult(DialogResult::cancel);
    x = x - defaultButtonSize.Width - defaultControlSpacing.Width;
    std::unique_ptr<Button> searchButtonPtr(new Button(ControlCreateParams().Location(Point(x, y)).SetSize(defaultButtonSize).Text("Search").SetAnchors(Anchors::right | Anchors::bottom)));
    searchButton = searchButtonPtr.get();
    searchButton->Disable();
    searchButton->SetDialogResult(DialogResult::ok);
    searchButton->SetDefault();
    AddChild(searchButtonPtr.release());
    AddChild(cancelButtonPtr.release());
    SetDefaultButton(searchButton);
    SetCancelButton(cancelButton);
    searchTextBox->SetFocus();
}

std::string SearchDialog::SearchText() const
{
    return searchTextBox->Text();
}

bool SearchDialog::WholeWords() const
{
    return wholeWordsCheckBox->Checked();
}

bool SearchDialog::CaseInsensitive() const
{
    return caseInsensitiveCheckBox->Checked();
}

bool SearchDialog::RegularExpression() const
{
    return regularExpressionCheckBox->Checked();
}

SearchExtent SearchDialog::GetSearchExtent() const
{
    int selectedIndex = whereListBox->GetSelectedIndex();
    if (!hasCurrentFile)
    {
        return static_cast<SearchExtent>(selectedIndex + 1);
    }
    else
    {
        return static_cast<SearchExtent>(selectedIndex);
    }
}

void SearchDialog::SearchTextBoxTextChanged()
{
    if (searchTextBox->Text().empty())
    {
        searchButton->Disable();
    }
    else
    {
        searchButton->Enable();
    }
}

} // namespace cmcode
