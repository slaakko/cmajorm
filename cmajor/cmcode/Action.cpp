// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/Action.hpp>
#include <cmajor/cmcode/MainWindow.hpp>

namespace cmcode {

using namespace cmajor::wing;

BuildSolutionAction::BuildSolutionAction(MenuItem* menuItem, MainWindow* mainWindow_) : ClickAction(menuItem), mainWindow(mainWindow_) 
{
}

void BuildSolutionAction::Execute()
{
    mainWindow->HideContextMenu();
    mainWindow->BuildSolutionClick();
}

RebuildSolutionAction::RebuildSolutionAction(MenuItem* menuItem, MainWindow* mainWindow_) : ClickAction(menuItem), mainWindow(mainWindow_)
{
}

void RebuildSolutionAction::Execute()
{
    mainWindow->HideContextMenu();
    mainWindow->RebuildSolutionClick();
}

CleanSolutionAction::CleanSolutionAction(MenuItem* menuItem, MainWindow* mainWindow_) : ClickAction(menuItem), mainWindow(mainWindow_)
{
}

void CleanSolutionAction::Execute()
{
    mainWindow->HideContextMenu();
    mainWindow->CleanSolutionClick();
}

BuildProjectAction::BuildProjectAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_) : ClickAction(menuItem), mainWindow(mainWindow_), project(project_)
{
}

void BuildProjectAction::Execute()
{
    mainWindow->HideContextMenu();
    mainWindow->BuildProject(project);
}

RebuildProjectAction::RebuildProjectAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_) : ClickAction(menuItem), mainWindow(mainWindow_), project(project_)
{
}

void RebuildProjectAction::Execute()
{
    mainWindow->HideContextMenu();
    mainWindow->RebuildProject(project);
}

CleanProjectAction::CleanProjectAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_) : ClickAction(menuItem), mainWindow(mainWindow_), project(project_)
{
}

void CleanProjectAction::Execute()
{
    mainWindow->HideContextMenu();
    mainWindow->CleanProject(project);
}

AddNewProjectAction::AddNewProjectAction(MenuItem* menuItem, MainWindow* mainWindow_) : ClickAction(menuItem), mainWindow(mainWindow_)
{
}

void AddNewProjectAction::Execute()
{
    mainWindow->HideContextMenu();
    mainWindow->AddNewProject();
}

SetActiveProjectAction::SetActiveProjectAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_, TreeViewNode* newActiveProjectNode_) :
    ClickAction(menuItem), mainWindow(mainWindow_), project(project_), newActiveProjectNode(newActiveProjectNode_)
{
}

void SetActiveProjectAction::Execute()
{
    mainWindow->HideContextMenu();
    mainWindow->SetActiveProject(project, newActiveProjectNode);
}

OpenFileLocationAction::OpenFileLocationAction(MenuItem* menuItem, MainWindow* mainWindow_, const std::string& filePath_) : ClickAction(menuItem), mainWindow(mainWindow_), filePath(filePath_)
{
}

void OpenFileLocationAction::Execute()
{
    mainWindow->HideContextMenu();
    mainWindow->OpenFileLocation(filePath);
}

RemoveProjectAction::RemoveProjectAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_) : ClickAction(menuItem), mainWindow(mainWindow_), project(project_)
{
}

void RemoveProjectAction::Execute()
{
    mainWindow->HideContextMenu();
    mainWindow->RemoveProject(project);
}

} // namespace cmcode
