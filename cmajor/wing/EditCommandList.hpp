// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_EDIT_COMMAND_LIST_INCLUDED
#define CMAJOR_WING_EDIT_COMMAND_LIST_INCLUDED
#include <wing/WingApi.hpp>
#include <memory>
#include <vector>

namespace cmajor { namespace wing {

class TextView;
class MenuItem;

class WING_API EditCommand
{
public:
    EditCommand();
    virtual ~EditCommand();
    EditCommand(const EditCommand&) = delete;
    EditCommand(EditCommand&&) = delete;
    EditCommand& operator=(const EditCommand&) = delete;
    EditCommand& operator=(EditCommand&&) = delete;
    virtual void Undo(TextView* textView) = 0;
    virtual void Redo(TextView* textView) = 0;
};

class WING_API GroupCommand : public EditCommand
{
public:
    GroupCommand();
    void AddCommand(EditCommand* command);
    void Undo(TextView* textView) override;
    void Redo(TextView* textView) override;
private:
    std::vector<std::unique_ptr<EditCommand>> commands;
};

class WING_API EditCommandList
{
public:
    EditCommandList(TextView* textView_);
    EditCommandList(const EditCommandList&) = delete;
    EditCommandList(EditCommandList&&) = delete;
    EditCommandList& operator=(const EditCommandList&) = delete;
    EditCommandList& operator=(EditCommandList&&) = delete;
    void SetMenuItems(MenuItem* undoMenuItem_, MenuItem* redoMenuItem_);
    void UpdateMenuItems();
    bool CanUndo() const;
    bool CanRedo() const;
    void AddCommand(EditCommand* command);
    void BeginGroup();
    void EndGroup();
    void Undo();
    void Redo();
private:
    TextView* textView;
    int currentIndex;
    bool inGroup;
    MenuItem* undoMenuItem;
    MenuItem* redoMenuItem;
    std::vector<std::unique_ptr<EditCommand>> commands;
    std::unique_ptr<GroupCommand> groupCommand;
};

} } // cmajor::wing

#endif // CMAJOR_WING_EDIT_COMMAND_LIST_INCLUDED
