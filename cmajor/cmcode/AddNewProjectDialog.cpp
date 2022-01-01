// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/AddNewProjectDialog.hpp>
#include <cmajor/cmcode/NewProjectDialog.hpp>
#include <cmajor/cmcode/SelectProjectTypeDialog.hpp>
#include <cmajor/wing/Metrics.hpp>
#include <cmajor/wing/BorderedControl.hpp>
#include <cmajor/wing/PaddedControl.hpp>
#include <wing/Theme.hpp>

namespace cmcode {

AddNewProjectDialog::AddNewProjectDialog() : Window(WindowCreateParams().WindowClassName("cmcode.AddNewProjectDialog").WindowStyle(DialogWindowStyle()).Text("Add New Project").
    WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).
    Location(DefaultLocation()).SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(160), ScreenMetrics::Get().MMToVerticalPixels(100)))),
    projectNameTextBox(nullptr),
    projectTypeTextLabel(nullptr),
    projectLocationTextBox(nullptr),
    projectType(sngcm::ast::Target::program),
    createButton(nullptr),
    cancelButton(nullptr)
{
    Size s = GetSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();
    Size defaultTextBoxSize = ScreenMetrics::Get().DefaultTextBoxSize();
    Size defaultLabelSize = ScreenMetrics::Get().DefaultLabelSize();
    
    Point projectNameLabelLocation(16, 16);
    std::unique_ptr<Label> projectNameLabelPtr(new Label(LabelCreateParams().Text("Project name:").Location(projectNameLabelLocation).SetSize(defaultLabelSize).
        SetAnchors(Anchors::left | Anchors::top)));
    AddChild(projectNameLabelPtr.release());

    Size projectNameTextBoxSize(ScreenMetrics::Get().MMToHorizontalPixels(120), defaultTextBoxSize.Height + 4);
    std::unique_ptr<TextBox> projectNameTextBoxPtr(new TextBox(TextBoxCreateParams().Defaults()));
    projectNameTextBox = projectNameTextBoxPtr.get();
    projectNameTextBox->TextChanged().AddHandler(this, &AddNewProjectDialog::ProjectNameTextBoxTextChanged);
    Point projectNameTextBoxLocation(16, 16 + 24);
    std::unique_ptr<Control> paddedProjectNameTextBox(new PaddedControl(PaddedControlCreateParams(projectNameTextBoxPtr.release()).
        SetSize(PaddedSize(projectNameTextBoxSize, DefaultPadding()))));
    std::unique_ptr<Control> borderedProjectNameTextBox(new BorderedControl(BorderedControlCreateParams(paddedProjectNameTextBox.release()).Location(projectNameTextBoxLocation).
        SetSize(BorderedSize(PaddedSize(projectNameTextBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(Anchors::left | Anchors::top)));
    AddChild(borderedProjectNameTextBox.release());

    Point projectTypeLabelLocation(16, 16 + defaultControlSpacing.Height + 24 + 24);
    std::unique_ptr<Label> projectTypeLabelPtr(new Label(LabelCreateParams().Text("Project type:").Location(projectTypeLabelLocation).SetSize(defaultLabelSize).
        SetAnchors(Anchors::left | Anchors::top)));
    AddChild(projectTypeLabelPtr.release());

    projectTypeNameMap[sngcm::ast::Target::program] = "Console Application";
    projectTypeNameMap[sngcm::ast::Target::winguiapp] = "Windows GUI Application";
    projectTypeNameMap[sngcm::ast::Target::library] = "Library";
    projectTypeNameMap[sngcm::ast::Target::winlib] = "Windows Library";

    Point projectTypeTextLocation(16, 16 + defaultControlSpacing.Height + 24 + 24 + 24);
    std::unique_ptr<Label> projectTypeTextLabelPtr(new Label(LabelCreateParams().Text(GetProjectTypeName(projectType)).Location(projectTypeTextLocation).SetSize(defaultLabelSize).
        SetAnchors(Anchors::left | Anchors::top)));
    projectTypeTextLabel = projectTypeTextLabelPtr.get();
    AddChild(projectTypeTextLabelPtr.release());

    Point selectProjectTypeButtonLocation(ScreenMetrics::Get().MMToHorizontalPixels(120) + 32, 16 + defaultControlSpacing.Height + 24 + 24 + 24);
    Size selectProjectTypeButtonSize(24, 24);
    std::unique_ptr<Button> selectProjectTypeButton(new Button(ControlCreateParams().Location(selectProjectTypeButtonLocation).SetSize(selectProjectTypeButtonSize).Text("...").
        SetAnchors(Anchors::left | Anchors::top)));
    selectProjectTypeButton->Click().AddHandler(this, &AddNewProjectDialog::SelectProjectTypeButtonClick);
    AddChild(selectProjectTypeButton.release());

    Point projectLocationLabelLocation(16, 16 + defaultControlSpacing.Height + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> projectLocationLabelPtr(new Label(LabelCreateParams().Text("Project location:").Location(projectLocationLabelLocation).SetSize(defaultLabelSize).
        SetAnchors(Anchors::left | Anchors::top)));
    AddChild(projectLocationLabelPtr.release());

    Point projectLocationTextBoxLocation(16, 16 + defaultControlSpacing.Height + 24 + 24 + 24 + 24 + 24);
    Size projectLocationTextBoxSize(ScreenMetrics::Get().MMToHorizontalPixels(120), defaultTextBoxSize.Height + 4);
    std::unique_ptr<TextBox> projectLocationTextBoxPtr(new TextBox(TextBoxCreateParams().Defaults()));
    projectLocationTextBox = projectLocationTextBoxPtr.get();
    projectLocationTextBox->TextChanged().AddHandler(this, &AddNewProjectDialog::ProjectLocationTextBoxTextChanged);
    std::unique_ptr<Control> paddedProjectLocationTextBox(new PaddedControl(PaddedControlCreateParams(projectLocationTextBoxPtr.release()).
        SetSize(PaddedSize(projectLocationTextBoxSize, DefaultPadding()))));
    std::unique_ptr<Control> borderedProjectLocationTextBox(new BorderedControl(BorderedControlCreateParams(paddedProjectLocationTextBox.release()).Location(projectLocationTextBoxLocation).
        SetSize(BorderedSize(PaddedSize(projectLocationTextBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(Anchors::left | Anchors::top)));
    AddChild(borderedProjectLocationTextBox.release());

    Point selectProjectLocationButtonLocation(ScreenMetrics::Get().MMToHorizontalPixels(120) + 32, 16 + defaultControlSpacing.Height + 24 + 24 + 24 + 24 + 24);
    Size selectProjectLocationButtonSize(24, 24);
    std::unique_ptr<Button> selectProjectLocationButton(new Button(ControlCreateParams().Location(selectProjectLocationButtonLocation).SetSize(selectProjectLocationButtonSize).Text("...").
        SetAnchors(Anchors::left | Anchors::top)));
    selectProjectLocationButton->Click().AddHandler(this, &AddNewProjectDialog::SelectProjectLocationButtonClick);
    AddChild(selectProjectLocationButton.release());

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
    projectNameTextBox->SetFocus();
}

std::string AddNewProjectDialog::GetProjectName() const
{
    return projectNameTextBox->Text();
}

std::string AddNewProjectDialog::GetProjectLocation() const
{
    return projectLocationTextBox->Text();
}

void AddNewProjectDialog::SetProjectLocation(const std::string& location)
{
    projectLocationTextBox->SetText(location);
}

void AddNewProjectDialog::ProjectNameTextBoxTextChanged()
{
    if (Valid())
    {
        createButton->Enable();
    }
    else
    {
        createButton->Disable();
    }
}

void AddNewProjectDialog::ProjectLocationTextBoxTextChanged()
{
    if (Valid())
    {
        createButton->Enable();
    }
    else
    {
        createButton->Disable();
    }
}

void AddNewProjectDialog::SelectProjectTypeButtonClick()
{
    SelectProjectTypeDialog dialog;
    if (dialog.ShowDialog(*this) == DialogResult::ok)
    {
        projectType = dialog.GetTarget();
        projectTypeTextLabel->SetText(GetProjectTypeName(projectType));
        projectLocationTextBox->SetFocus();
        Invalidate();
    }
}

void AddNewProjectDialog::SelectProjectLocationButtonClick()
{
    std::string location = SelectDirectory(Handle(), projectLocationTextBox->Text());
    if (!location.empty())
    {
        SetProjectLocation(location);
    }
}

std::string AddNewProjectDialog::GetProjectTypeName(sngcm::ast::Target projectType)
{
    auto it = projectTypeNameMap.find(projectType);
    if (it != projectTypeNameMap.cend())
    {
        return it->second;
    }
    else
    {
        return std::string();
    }
}

bool AddNewProjectDialog::Valid() const
{ 
    return IsValidProjectOrSolutionName(projectNameTextBox->Text()) && !projectLocationTextBox->Text().empty();
}

} // namespace cmcode