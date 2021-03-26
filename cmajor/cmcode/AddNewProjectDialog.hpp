// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_ADD_NEW_PROJECT_DIALOG_INCLUDED
#define CMCODE_ADD_NEW_PROJECT_DIALOG_INCLUDED
#include <cmajor/wing/Button.hpp>
#include <cmajor/wing/Label.hpp>
#include <cmajor/wing/TextBox.hpp>
#include <cmajor/wing/Window.hpp>
#include <sngcm/ast/Project.hpp>

namespace cmcode {

using namespace cmajor::wing;

class AddNewProjectDialog : public Window
{
public:
    AddNewProjectDialog();
    std::string GetProjectName() const;
    std::string GetProjectLocation() const;
    void SetProjectLocation(const std::string& location);
    sngcm::ast::Target GetProjectType() const { return projectType; }
private:
    void ProjectNameTextBoxTextChanged();
    void ProjectLocationTextBoxTextChanged();
    void SelectProjectTypeButtonClick();
    void SelectProjectLocationButtonClick();
    std::string GetProjectTypeName(sngcm::ast::Target projectType);
    bool Valid() const;
    TextBox* projectNameTextBox;
    Label* projectTypeTextLabel;
    TextBox* projectLocationTextBox;
    Button* createButton;
    Button* cancelButton;
    sngcm::ast::Target projectType;
    std::map<sngcm::ast::Target, std::string> projectTypeNameMap;
};

} // namespace cmcode

#endif // CMCODE_ADD_NEW_PROJECT_DIALOG_INCLUDED
