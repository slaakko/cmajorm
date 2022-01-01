// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_PROJECT_REFERENCES_DIALOG_INCLUDED
#define CMCODE_PROJECT_REFERENCES_DIALOG_INCLUDED
#include <cmajor/wing/Window.hpp>
#include <cmajor/wing/Button.hpp>
#include <cmajor/wing/CheckBox.hpp>
#include <sngcm/ast/Solution.hpp>
#include <sngcm/ast/Project.hpp>

namespace cmcode {

using namespace cmajor::wing;

class ProjectReferencesDialog : public Window
{
public:
    ProjectReferencesDialog(sngcm::ast::Project* project_, sngcm::ast::Solution* solution_);
    std::vector<sngcm::ast::Project*> ReferencedProjects() const;
private:
    sngcm::ast::Project* project;
    sngcm::ast::Solution* solution;
    Button* cancelButton;
    Button* okButton;
    std::vector<std::pair<CheckBox*, sngcm::ast::Project*>> libraryProjects;
};

} // namespace cmcode

#endif // CMCODE_PROJECT_REFERENCES_DIALOG_INCLUDED
