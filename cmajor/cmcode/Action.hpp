// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_ACTION_INCLUDED
#define CMCODE_ACTION_INCLUDED
#include <cmajor/wing/Menu.hpp>
#include <cmajor/wing/TreeView.hpp>
#include <sngcm/ast/Project.hpp>

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

} // namespace cmcode

#endif // CMCODE_ACTION_INCLUDED
