// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_ACTION_INCLUDED
#define CMCODE_ACTION_INCLUDED
#include <cmajor/wing/Menu.hpp>
#include <cmajor/wing/TreeView.hpp>
#include <sngcm/ast/Project.hpp>
#include <cmajor/cmmsg/BuildServerMessage.hpp>
#include <cmajor/cmmsg/CmdbMessage.hpp>

namespace cmcode {

using namespace cmajor::wing;

class MainWindow;

class BuildSolutionAction : public ClickAction
{
public:
    BuildSolutionAction(MenuItem* menuItem, MainWindow* mainWindow_);
    void Execute() override;
private:
    MainWindow* mainWindow;
};

class RebuildSolutionAction : public ClickAction
{
public:
    RebuildSolutionAction(MenuItem* menuItem, MainWindow* mainWindow_);
    void Execute() override;
private:
    MainWindow* mainWindow;
};

class CleanSolutionAction : public ClickAction
{
public:
    CleanSolutionAction(MenuItem* menuItem, MainWindow* mainWindow_);
    void Execute() override;
private:
    MainWindow* mainWindow;
};

class BuildProjectAction : public ClickAction
{
public:
    BuildProjectAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_);
    void Execute() override;
private:
    MainWindow* mainWindow;
    sngcm::ast::Project* project;
};

class RebuildProjectAction : public ClickAction
{
public:
    RebuildProjectAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_);
    void Execute() override;
private:
    MainWindow* mainWindow;
    sngcm::ast::Project* project;
};

class CleanProjectAction : public ClickAction
{
public:
    CleanProjectAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_);
    void Execute() override;
private:
    MainWindow* mainWindow;
    sngcm::ast::Project* project;
};

class AddNewProjectAction : public ClickAction
{
public:
    AddNewProjectAction(MenuItem* menuItem, MainWindow* mainWindow_);
    void Execute() override;
private:
    MainWindow* mainWindow;
};

class AddExistingProjectAction : public ClickAction
{
public:
    AddExistingProjectAction(MenuItem* menuItem, MainWindow* mainWindow_);
    void Execute() override;
private:
    MainWindow* mainWindow;
};

class SetActiveProjectAction : public ClickAction
{
public:
    SetActiveProjectAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_, TreeViewNode* newActiveProjectNode_);
    void Execute() override;
private:
    MainWindow* mainWindow;
    sngcm::ast::Project* project;
    TreeViewNode* newActiveProjectNode;
};

class RemoveProjectAction : public ClickAction
{
public:
    RemoveProjectAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_);
    void Execute() override;
private:
    MainWindow* mainWindow;
    sngcm::ast::Project* project;
};

class OpenFileLocationAction : public ClickAction
{
public:
    OpenFileLocationAction(MenuItem* menuItem, MainWindow* mainWindow_, const std::string& filePath_);
    void Execute() override;
private:
    MainWindow* mainWindow;
    std::string filePath;
};

class AddNewSourceFileAction : public ClickAction
{
public:
    AddNewSourceFileAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_, TreeViewNode* projectNode_);
    void Execute() override;
private:
    MainWindow* mainWindow;
    sngcm::ast::Project* project;
    TreeViewNode* projectNode;
};

class AddExistingSourceFileAction : public ClickAction
{
public:
    AddExistingSourceFileAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_, TreeViewNode* projectNode_);
    void Execute() override;
private:
    MainWindow* mainWindow;
    sngcm::ast::Project* project;
    TreeViewNode* projectNode;
};

class AddNewResourceFileAction : public ClickAction
{
public:
    AddNewResourceFileAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_, TreeViewNode* projectNode_);
    void Execute() override;
private:
    MainWindow* mainWindow;
    sngcm::ast::Project* project;
    TreeViewNode* projectNode;
};

class AddExistingResourceFileAction : public ClickAction
{
public:
    AddExistingResourceFileAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_, TreeViewNode* projectNode_);
    void Execute() override;
private:
    MainWindow* mainWindow;
    sngcm::ast::Project* project;
    TreeViewNode* projectNode;
};

class AddNewTextFileAction : public ClickAction
{
public:
    AddNewTextFileAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_, TreeViewNode* projectNode_);
    void Execute() override;
private:
    MainWindow* mainWindow;
    sngcm::ast::Project* project;
    TreeViewNode* projectNode;
};

class AddExistingTextFileAction : public ClickAction
{
public:
    AddExistingTextFileAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_, TreeViewNode* projectNode_);
    void Execute() override;
private:
    MainWindow* mainWindow;
    sngcm::ast::Project* project;
    TreeViewNode* projectNode;
};

class RemoveFileAction : public ClickAction
{
public:
    RemoveFileAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_, const std::string& filePath_, const std::string& fileName_, TreeViewNode* fileNode_);
    void Execute() override;
private:
    MainWindow* mainWindow;
    sngcm::ast::Project* project;
    std::string filePath;
    std::string fileName;
    TreeViewNode* fileNode;
};

class GotoDefinitionAction : public ClickAction
{
public:
    GotoDefinitionAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_, const std::string& identifier_, const DefinitionSourceLocation& sourceLocation_);
    void Execute() override;
private:
    MainWindow* mainWindow;
    sngcm::ast::Project* project;
    std::string identifier;
    DefinitionSourceLocation sourceLocation;
};

class GotoCursorAction : public ClickAction
{
public:
    GotoCursorAction(MenuItem* menuItem, MainWindow* mainWindow_, const SourceLoc& sourceLocation_);
    void Execute() override;
private:
    MainWindow* mainWindow;
    SourceLoc sourceLocation;
};

class ProjectReferencesAction : public ClickAction
{
public:
    ProjectReferencesAction(MenuItem* menuItem, MainWindow* mainWindow_, sngcm::ast::Project* project_);
    void Execute() override;
private:
    MainWindow* mainWindow;
    sngcm::ast::Project* project;
};

} // namespace cmcode

#endif // CMCODE_ACTION_INCLUDED
