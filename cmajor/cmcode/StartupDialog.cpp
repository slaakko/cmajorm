// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/StartupDialog.hpp>
#include <cmajor/wing/BorderedControl.hpp>
#include <cmajor/wing/Metrics.hpp>

namespace cmcode {

StartupDialog::StartupDialog() : Window(WindowCreateParams().WindowClassName("cmcode.StartupDialog").Text("Cmajor Code Startup").
    WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).BackgroundColor(DefaultControlBackgroundColor()).
    SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(150), ScreenMetrics::Get().MMToVerticalPixels(60)))),
    recentSolutions(GetRecentSolutions()), startButton(nullptr), removeButton(nullptr), startWithoutSolutionButton(nullptr), listBox(nullptr), solutionFilePathLabel(nullptr)
{
    SetSmallIcon(Application::GetResourceManager().GetIcon("cmcode.small.icon"));
    Size s = GetSize();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Size defaultListBoxSize = ScreenMetrics::Get().DefaultListBoxSize();
    Size defaultLabelSize = ScreenMetrics::Get().DefaultLabelSize();

    Point listBoxLocation(16, 16);
    Size listBoxSize(ScreenMetrics::Get().MMToHorizontalPixels(90), ScreenMetrics::Get().MMToVerticalPixels(30));
    std::unique_ptr<ListBox> listBoxPtr(new ListBox(ListBoxCreateParams().Location(listBoxLocation).SetSize(listBoxSize).SetAnchors(Anchors::top | Anchors::left)));
    listBox = listBoxPtr.get();
    listBox->SelectedIndexChanged().AddHandler(this, &StartupDialog::SelectedIndexChanged);
    std::unique_ptr<Control> borderedListBoxPtr(new BorderedControl(BorderedControlCreateParams(listBoxPtr.release()).Location(listBoxLocation).SetSize(listBoxSize).SetAnchors(Anchors::top | Anchors::left)));
    Control* borderedListBox = borderedListBoxPtr.get();
    AddChild(borderedListBoxPtr.release());
    borderedListBox->SetSize(BorderedSize(listBox->GetSize(), BorderStyle::single));

    Point labelLocation(16, 16 + borderedListBox->GetSize().Height + defaultControlSpacing.Height);
    std::unique_ptr<Label> label(new Label(LabelCreateParams().Location(labelLocation).SetSize(defaultLabelSize).SetAnchors(Anchors::top | Anchors::left)));
    solutionFilePathLabel = label.get();
    AddChild(label.release());

    Size buttonSize = defaultButtonSize;
    buttonSize.Width = ScreenMetrics::Get().MMToHorizontalPixels(40);
    Point startButtonLocation(borderedListBox->GetSize().Width + 3 * defaultControlSpacing.Width, 16);
    std::unique_ptr<Button> startButtonPtr(new Button(ControlCreateParams().Text("Start").Location(startButtonLocation).SetSize(buttonSize).SetAnchors(Anchors::top | Anchors::left)));
    startButton = startButtonPtr.get();
    startButton->SetDialogResult(DialogResult::ok);
    startButton->SetDefault();
    Point removeButtonLocation(borderedListBox->GetSize().Width + 3 * defaultControlSpacing.Width, 24 + buttonSize.Height);
    std::unique_ptr<Button> removeButtonPtr(new Button(ControlCreateParams().Text("Remove").Location(removeButtonLocation).SetSize(buttonSize).SetAnchors(Anchors::top | Anchors::left)));
    removeButton = removeButtonPtr.get();
    removeButton->Click().AddHandler(this, &StartupDialog::RemoveClick);
    AddChild(removeButtonPtr.release());
    Point startWithNoSolutionButtonLocation(borderedListBox->GetSize().Width + 3 * defaultControlSpacing.Width, 16 + 2 * buttonSize.Height + 16);
    std::unique_ptr<Button> startWithoutSolutionButtonPtr(new Button(ControlCreateParams().Text("Start without solution").Location(startWithNoSolutionButtonLocation).SetSize(buttonSize).SetAnchors(Anchors::top | Anchors::left)));
    startWithoutSolutionButton = startWithoutSolutionButtonPtr.get();
    startWithoutSolutionButton->SetDialogResult(DialogResult::cancel);
    AddChild(startButtonPtr.release());
    AddChild(startWithoutSolutionButtonPtr.release());
    SetDefaultButton(startButton);
    SetCancelButton(startWithoutSolutionButton);
    listBox->SetFocus();
}

const RecentSolution& StartupDialog::GetSelectedSolution() const
{
    return recentSolutions[listBox->GetSelectedIndex()];
}

void StartupDialog::OnShown()
{
    Window::OnShown();
    int n = recentSolutions.size();
    for (int i = 0; i < n; ++i)
    {
        listBox->AddItem(recentSolutions[i].name);
    }
    if (n > 0)
    {
        listBox->SetSelectedIndex(0);
        const RecentSolution& recentSolution = recentSolutions[0];
        solutionFilePathLabel->SetText(recentSolution.filePath);
    }
    else
    {
        startButton->Disable();
        removeButton->Disable();
        solutionFilePathLabel->SetText(std::string());
    }
}

void StartupDialog::RemoveClick()
{
    int index = listBox->GetSelectedIndex();
    if (index >= 0 && index < recentSolutions.size())
    {
        listBox->DeleteItem(index);
        const RecentSolution& recentSolution = recentSolutions[index];
        RemoveRecentSolution(recentSolution.filePath);
        recentSolutions = GetRecentSolutions();
    }
    int n = recentSolutions.size();
    if (n > 0)
    {
        listBox->SetSelectedIndex(0);
        const RecentSolution& recentSolution = recentSolutions[0];
        solutionFilePathLabel->SetText(recentSolution.filePath);
        listBox->SetFocus();
    }
    else
    {
        startButton->Disable();
        removeButton->Disable();
        solutionFilePathLabel->SetText(std::string());
    }
}

void StartupDialog::SelectedIndexChanged()
{
    int index = listBox->GetSelectedIndex();
    if (index >= 0 && index < recentSolutions.size())
    {
        const RecentSolution& recentSolution = recentSolutions[index];
        solutionFilePathLabel->SetText(recentSolution.filePath);
    }
    else
    {
        startButton->Disable();
        removeButton->Disable();
        solutionFilePathLabel->SetText(std::string());
    }
}

} // namespace cmcode
