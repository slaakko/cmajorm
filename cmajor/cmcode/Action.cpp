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

AddExistingProjectAction::AddExistingProjectAction(MenuItem* menuItem, MainWindow* mainWindow_) : ClickAction(menuItem), mainWindow(mainWindow_)
{
}

void AddExistingProjectAction::Execute()
{
    mainWindow->HideContextMenu();
    mainWindow->AddExistingProject();
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

AddNewSourceFileAction::AddNewSourceFileAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_, TreeViewNode* projectNode_) : 
    ClickAction(menuItem), mainWindow(mainWindow_), project(project_), projectNode(projectNode_)
{
}

void AddNewSourceFileAction::Execute()
{
    mainWindow->HideContextMenu();
    mainWindow->AddNewSourceFile(project, projectNode);
}

AddExistingSourceFileAction::AddExistingSourceFileAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_, TreeViewNode* projectNode_) : 
    ClickAction(menuItem), mainWindow(mainWindow_), project(project_), projectNode(projectNode_)
{
}

void AddExistingSourceFileAction::Execute()
{
    mainWindow->HideContextMenu();
    mainWindow->AddExistingSourceFile(project, projectNode);
}

AddNewResourceFileAction::AddNewResourceFileAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_, TreeViewNode* projectNode_) : 
    ClickAction(menuItem), mainWindow(mainWindow_), project(project_), projectNode(projectNode_)
{
}

void AddNewResourceFileAction::Execute()
{
    mainWindow->HideContextMenu();
    mainWindow->AddNewResourceFile(project, projectNode);
}

AddExistingResourceFileAction::AddExistingResourceFileAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_, TreeViewNode* projectNode_) :
    ClickAction(menuItem), mainWindow(mainWindow_), project(project_), projectNode(projectNode_)
{
}

void AddExistingResourceFileAction::Execute()
{
    mainWindow->HideContextMenu();
    mainWindow->AddExistingResourceFile(project, projectNode);
}

AddNewTextFileAction::AddNewTextFileAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_, TreeViewNode* projectNode_) :
    ClickAction(menuItem), mainWindow(mainWindow_), project(project_), projectNode(projectNode_)
{
}

void AddNewTextFileAction::Execute()
{
    mainWindow->HideContextMenu();
    mainWindow->AddNewTextFile(project, projectNode);
}

AddExistingTextFileAction::AddExistingTextFileAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_, TreeViewNode* projectNode_) : 
    ClickAction(menuItem), mainWindow(mainWindow_), project(project_), projectNode(projectNode_)
{
}

void AddExistingTextFileAction::Execute()
{
    mainWindow->HideContextMenu();
    mainWindow->AddExistingTextFile(project, projectNode);
}

RemoveFileAction::RemoveFileAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_, const std::string& filePath_, const std::string& fileName_, TreeViewNode* fileNode_) :
    ClickAction(menuItem), mainWindow(mainWindow_), project(project_), filePath(filePath_), fileName(fileName_), fileNode(fileNode_)
{
}

void RemoveFileAction::Execute()
{
    mainWindow->HideContextMenu();
    mainWindow->RemoveFile(project, filePath, fileName, fileNode);
}

GotoDefinitionAction::GotoDefinitionAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_, const std::string& identifier_, const DefinitionSourceLocation& sourceLocation_) : 
    ClickAction(menuItem), mainWindow(mainWindow_), project(project_), identifier(identifier_), sourceLocation(sourceLocation_)
{
}

void GotoDefinitionAction::Execute()
{
    mainWindow->HideContextMenu();
    mainWindow->GotoDefinition(project, identifier, sourceLocation);
}

} // namespace cmcode
