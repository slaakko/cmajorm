// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_EDIT_COMMAND_INCLUDED
#define CMAJOR_WING_EDIT_COMMAND_INCLUDED
#include <wing/EditCommandList.hpp>
#include <wing/TextView.hpp>

namespace cmajor { namespace wing {

class WING_API InsertCharCommand : public EditCommand
{
public:
    InsertCharCommand(int lineIndex_, int columnIndex_, char32_t c_, bool removeIndent_);
    void Undo(TextView* textView) override;
    void Redo(TextView* textView) override;
private:
    int lineIndex;
    int columnIndex;
    char32_t c;
    bool removeIndent;
};

class WING_API DeleteCharCommand : public EditCommand
{
public:
    DeleteCharCommand(int lineIndex_, int columnIndex_, char32_t c_);
    void Undo(TextView* textView) override;
    void Redo(TextView* textView) override;
private:
    int lineIndex;
    int columnIndex;
    char32_t c;
};

class WING_API NewLineCommand : public EditCommand
{
public:
    NewLineCommand(int lineIndex_, int columnIndex_, int indent_, int numSpaces_);
    void Undo(TextView* textView) override;
    void Redo(TextView* textView) override;
private:
    int lineIndex;
    int columnIndex;
    int indent;
    int numSpaces;
};

class WING_API TabCommand : public EditCommand
{
public:
    TabCommand(int lineIndex_, int columnIndex_);
    void Undo(TextView* textView) override;
    void Redo(TextView* textView) override;
private:
    int lineIndex;
    int columnIndex;
};

class WING_API BacktabCommand : public EditCommand
{
public:
    BacktabCommand(int lineIndex_, int columnIndex_, int numSpaces_);
    void Undo(TextView* textView) override;
    void Redo(TextView* textView) override;
private:
    int lineIndex;
    int columnIndex;
    int numSpaces;
};

class WING_API IndentSelectionCommand : public EditCommand
{
public:
    IndentSelectionCommand(const Selection& selection_);
    void Undo(TextView* textView) override;
    void Redo(TextView* textView) override;
private:
    Selection selection;
};

class WING_API UnindentSelectionCommand : public EditCommand
{
public:
    UnindentSelectionCommand(const Selection& selection_);
    void Undo(TextView* textView) override;
    void Redo(TextView* textView) override;
private:
    Selection selection;
};

class WING_API InsertLinesCommand : public EditCommand
{
public:
    InsertLinesCommand(int lineIndex_, int columnIndex_, const std::vector<std::u32string>& lines_);
    void Undo(TextView* textView) override;
    void Redo(TextView* textView) override;
private:
    int lineIndex;
    int columnIndex;
    std::vector<std::u32string> lines;
};

class WING_API InsertIntoLineCommand : public EditCommand
{
public:
    InsertIntoLineCommand(int lineIndex_, int columnIndex_, const std::u32string& text_);
    void Undo(TextView* textView) override;
    void Redo(TextView* textView) override;
private:
    int lineIndex;
    int columnIndex;
    std::u32string text;
};

class WING_API RemoveSelectionCommand : public EditCommand
{
public:
    RemoveSelectionCommand(const Selection& selection_, const SelectionData& selectionData_, bool wholeLine_);
    void Undo(TextView* textView) override;
    void Redo(TextView* textView) override;
private:
    Selection selection;
    SelectionData selectionData;
    bool wholeLine;
};

} } // cmajor::wing

#endif // CMAJOR_WING_EDIT_COMMAND_INCLUDED
