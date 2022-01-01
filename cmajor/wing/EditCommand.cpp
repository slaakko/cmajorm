// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/EditCommand.hpp>

namespace cmajor { namespace wing {

InsertCharCommand::InsertCharCommand(int lineIndex_, int columnIndex_, char32_t c_, bool removeIndent_) : lineIndex(lineIndex_), columnIndex(columnIndex_), c(c_), removeIndent(removeIndent_)
{
}

void InsertCharCommand::Undo(TextView* textView)
{
    textView->DeleteChar(lineIndex, columnIndex, 0, 0, removeIndent);
}

void InsertCharCommand::Redo(TextView* textView)
{
    textView->InsertChar(lineIndex, columnIndex, c);
}

DeleteCharCommand::DeleteCharCommand(int lineIndex_, int columnIndex_, char32_t c_) : lineIndex(lineIndex_), columnIndex(columnIndex_), c(c_)
{
}

void DeleteCharCommand::Undo(TextView* textView)
{
    if (c != char32_t())
    {
        textView->InsertChar(lineIndex, columnIndex, c);
    }
    else
    {
        textView->NewLine(lineIndex, columnIndex);
    }
}

void DeleteCharCommand::Redo(TextView* textView)
{
    textView->DeleteChar(lineIndex, columnIndex, 0, 0, false);
}

NewLineCommand::NewLineCommand(int lineIndex_, int columnIndex_, int indent_, int numSpaces_) : lineIndex(lineIndex_), columnIndex(columnIndex_), indent(indent_), numSpaces(numSpaces_)
{
}

void NewLineCommand::Undo(TextView* textView)
{
    textView->DeleteChar(lineIndex, columnIndex, indent, numSpaces, false);
}

void NewLineCommand::Redo(TextView* textView)
{
    textView->NewLine(lineIndex, columnIndex);
}

TabCommand::TabCommand(int lineIndex_, int columnIndex_) : lineIndex(lineIndex_), columnIndex(columnIndex_)
{
}

void TabCommand::Undo(TextView* textView)
{
    textView->Backtab(lineIndex, columnIndex + textView->IndentSize());
}

void TabCommand::Redo(TextView* textView)
{
    textView->Tab(lineIndex, columnIndex);
}

BacktabCommand::BacktabCommand(int lineIndex_, int columnIndex_, int numSpaces_) : lineIndex(lineIndex_), columnIndex(columnIndex_), numSpaces(numSpaces_)
{
}

void BacktabCommand::Undo(TextView* textView)
{
    textView->AddSpaces(lineIndex, columnIndex, numSpaces);
}

void BacktabCommand::Redo(TextView* textView)
{
    textView->RemoveSpaces(lineIndex, columnIndex, numSpaces);
}

IndentSelectionCommand::IndentSelectionCommand(const Selection& selection_) : selection(selection_)
{
}

void IndentSelectionCommand::Undo(TextView* textView)
{
    textView->SetSelection(selection);
    textView->UnindentSelection();
}

void IndentSelectionCommand::Redo(TextView* textView)
{
    textView->SetSelection(selection);
    textView->IndentSelection();
}

UnindentSelectionCommand::UnindentSelectionCommand(const Selection& selection_) : selection(selection_)
{
}

void UnindentSelectionCommand::Undo(TextView* textView)
{
    textView->SetSelection(selection);
    textView->IndentSelection();
}

void UnindentSelectionCommand::Redo(TextView* textView)
{
    textView->SetSelection(selection);
    textView->UnindentSelection();
}

InsertLinesCommand::InsertLinesCommand(int lineIndex_, int columnIndex_, const std::vector<std::u32string>& lines_) : lineIndex(lineIndex_), columnIndex(columnIndex_), lines(lines_)
{
}

void InsertLinesCommand::Undo(TextView* textView)
{
    textView->DeleteLines(lineIndex, columnIndex, lines);
}

void InsertLinesCommand::Redo(TextView* textView)
{
    textView->InsertLines(lineIndex, columnIndex, lines);
}

InsertIntoLineCommand::InsertIntoLineCommand(int lineIndex_, int columnIndex_, const std::u32string& text_) : lineIndex(lineIndex_), columnIndex(columnIndex_), text(text_)
{
}

void InsertIntoLineCommand::Undo(TextView* textView)
{
    textView->RemoveFromLine(lineIndex, columnIndex, text.size());
}

void InsertIntoLineCommand::Redo(TextView* textView)
{
    textView->InsertIntoLine(lineIndex, columnIndex, text);
}

RemoveSelectionCommand::RemoveSelectionCommand(const Selection& selection_, const SelectionData& selectionData_, bool wholeLine_) : 
    selection(selection_), selectionData(selectionData_), wholeLine(wholeLine_)
{
}

void RemoveSelectionCommand::Undo(TextView* textView)
{
    textView->InsertSelection(selection, selectionData, wholeLine);
}

void RemoveSelectionCommand::Redo(TextView* textView)
{
    textView->SetSelection(selection);
    textView->RemoveSelection();
}

} } // cmajor::wing
