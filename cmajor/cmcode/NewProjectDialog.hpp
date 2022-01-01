// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_NEW_PROJECT_INCLUDED
#define CMCODE_NEW_PROJECT_INCLUDED
#include <cmajor/wing/Button.hpp>
#include <cmajor/wing/CheckBox.hpp>
#include <cmajor/wing/Label.hpp>
#include <cmajor/wing/TextBox.hpp>
#include <cmajor/wing/Window.hpp>
#include <sngcm/ast/Project.hpp>

namespace cmcode {

using namespace cmajor::wing;

bool IsValidProjectOrSolutionName(const std::string& text);

class NewProjectDialog : public Window
{
public:
    NewProjectDialog();
    std::string GetProjectName() const;
    sngcm::ast::Target GetProjectType() const { return projectType; }
    std::string GetProjectLocation() const;
    void SetProjectLocation(const std::string& location);
    std::string GetSolutionName() const;
    bool PlaceSolutionInSameDirectory() const;
private:
    void ProjectNameTextBoxTextChanged();
    void SelectProjectTypeButtonClick();
    void ProjectLocationTextBoxTextChanged();
    void SelectProjectLocationButtonClick();
    void SolutionNameTextBoxTextChanged();
    void PlaceSolutionInSameDirectoryCheckBoxCheckedChanged();
    bool Valid() const;
    std::string GetProjectTypeName(sngcm::ast::Target projectType);
    TextBox* projectNameTextBox;
    Label* projectTypeTextLabel;
    TextBox* projectLocationTextBox;
    TextBox* solutionNameTextBox;
    CheckBox* placeSolutionInSameDirectoryCheckBox;
    Button* cancelButton;
    Button* createButton;
    sngcm::ast::Target projectType;
    std::map<sngcm::ast::Target, std::string> projectTypeNameMap;
};

} // namespace cmcode

#endif // CMCODE_NEW_PROJECT_INCLUDED
