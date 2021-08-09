// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/EditCommandList.hpp>
#include <wing/TextView.hpp>
#include <wing/Menu.hpp>

namespace cmajor { namespace wing {

EditCommand::EditCommand()
{
}

EditCommand::~EditCommand()
{
}

GroupCommand::GroupCommand() : commands()
{
}

void GroupCommand::AddCommand(EditCommand* command)
{
    commands.push_back(std::unique_ptr<EditCommand>(command));
}

void GroupCommand::Undo(TextView* textView)
{
    for (auto& command : commands)
    {
        command->Undo(textView);
    }
}

void GroupCommand::Redo(TextView* textView)
{
    for (auto& command : commands)
    {
        command->Redo(textView);
    }
}

EditCommandList::EditCommandList(TextView* textView_) : textView(textView_), currentIndex(0), inGroup(false), undoMenuItem(nullptr), redoMenuItem(nullptr)
{
}

void EditCommandList::SetMenuItems(MenuItem* undoMenuItem_, MenuItem* redoMenuItem_)
{
    undoMenuItem = undoMenuItem_;
    redoMenuItem = redoMenuItem_;
    UpdateMenuItems();
}

void EditCommandList::UpdateMenuItems()
{
    if (undoMenuItem)
    {
        if (CanUndo())
        {
            undoMenuItem->Enable();
        }
        else
        {
            undoMenuItem->Disable();
        }
    }
    if (redoMenuItem)
    {
        if (CanRedo())
        {
            redoMenuItem->Enable();
        }
        else
        {
            redoMenuItem->Disable();
        }
    }
}

bool EditCommandList::CanUndo() const
{
    return currentIndex > 0;
}

bool EditCommandList::CanRedo() const
{
    return currentIndex < commands.size();
}

void EditCommandList::AddCommand(EditCommand* command)
{
    if (inGroup)
    {
        groupCommand->AddCommand(command);
    }
    else
    {
        if (currentIndex >= commands.size())
        {
            commands.push_back(std::unique_ptr<EditCommand>(command));
            currentIndex = static_cast<int>(commands.size());
        }
        else
        {
            commands[currentIndex].reset(command);
            ++currentIndex;
        }
        commands.resize(currentIndex);
        UpdateMenuItems();
    }
}

void EditCommandList::BeginGroup()
{
    groupCommand.reset(new GroupCommand());
    inGroup = true;
}

void EditCommandList::EndGroup()
{
    inGroup = false;
    AddCommand(groupCommand.release());
}

void EditCommandList::Undo()
{
    if (CanUndo())
    {
        --currentIndex;
        EditCommand* command = commands[currentIndex].get();
        command->Undo(textView);
    }
    UpdateMenuItems();
}

void EditCommandList::Redo()
{
    if (CanRedo())
    {
        EditCommand* command = commands[currentIndex].get();
        command->Redo(textView);
        ++currentIndex;
    }
    UpdateMenuItems();
}

} } // cmajor::wing
