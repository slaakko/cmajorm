// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/ProjectReferencesDialog.hpp>
#include <cmajor/wing/Metrics.hpp>
#include <cmajor/wing/GroupBox.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmcode {

using namespace soulng::unicode;

ProjectReferencesDialog::ProjectReferencesDialog(sngcm::ast::Project* project_, sngcm::ast::Solution* solution_) : 
    Window(WindowCreateParams().WindowClassName("cmcode.ProjectReferencesDialog").WindowStyle(DialogWindowStyle()).Text(ToUtf8(project_->Name()) + " Project References").
    WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).BackgroundColor(DefaultControlBackgroundColor()).
    Location(DefaultLocation()).SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(160), ScreenMetrics::Get().MMToVerticalPixels(60)))),
    project(project_), solution(solution_), cancelButton(nullptr), okButton(nullptr)
{
    SetCaretDisabled();
    Size s = GetSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();

    std::unique_ptr<GroupBox> groupBoxPtr(new GroupBox(GroupBoxCreateParams().Location(Point(16, 16)).
        SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(120), ScreenMetrics::Get().MMToVerticalPixels(40))).SetAnchors(Anchors::top | Anchors::left)));
    std::vector<sngcm::ast::Project*> referencedProjects = sngcm::ast::GetReferencedProjects(project, solution);
    Point loc(16, 24);
    for (const auto& proj : solution->Projects())
    {
        if (proj->GetTarget() == sngcm::ast::Target::library || proj->GetTarget() == sngcm::ast::Target::winlib)
        {
            bool disable = false;
            if (proj.get() == project)
            {
                disable = true;
            }
            else
            {
                std::set<sngcm::ast::Project*> projReferences = sngcm::ast::GetAllReferencedProjects(proj.get(), solution);
                if (projReferences.find(project) != projReferences.cend())
                {
                    disable = true;
                }
            }
            std::unique_ptr<CheckBox> checkBoxPtr(new CheckBox(CheckBoxCreateParams().Text(ToUtf8(proj->Name())).Location(loc).SetAnchors(Anchors::top | Anchors::left)));
            if (disable)
            {
                checkBoxPtr->Disable();
            }
            libraryProjects.push_back(std::make_pair(checkBoxPtr.get(), proj.get()));
            if (std::find(referencedProjects.begin(), referencedProjects.end(), proj.get()) != referencedProjects.end())
            {
                checkBoxPtr->SetChecked(true);
            }
            groupBoxPtr->AddChild(checkBoxPtr.release());
            loc.Y = loc.Y + 16;
        }
    }
    loc.Y = loc.Y + 16;
    Size gbs = groupBoxPtr->GetSize();
    GroupBox* groupBox = groupBoxPtr.get();
    groupBox->SetSize(Size(gbs.Width, loc.Y));
    AddChild(groupBoxPtr.release());

    Size sz = s;
    sz.Height = groupBox->Location().Y;
    sz.Height += groupBox->GetSize().Height;
    sz.Height += 2 * (defaultButtonSize.Height + defaultControlSpacing.Height + 64);
    SetSize(sz);
    s = GetSize();

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
    okButton->SetFocus();
}

std::vector<sngcm::ast::Project*> ProjectReferencesDialog::ReferencedProjects() const
{
    std::vector<sngcm::ast::Project*> referencedProjects;
    int n = libraryProjects.size();
    for (int i = 0; i < n; ++i)
    {
        const auto& p = libraryProjects[i];
        CheckBox* checkBox = p.first;
        if (checkBox->Checked())
        {
            sngcm::ast::Project* project = p.second;
            referencedProjects.push_back(project);
        }
    }
    return referencedProjects;
}

} // namespace cmcode
