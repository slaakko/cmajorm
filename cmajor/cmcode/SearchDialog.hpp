// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_SEARCH_DIALOG_INCLUDED
#define CMCODE_SEARCH_DIALOG_INCLUDED
#include <cmajor/wing/Window.hpp>
#include <cmajor/wing/Button.hpp>
#include <cmajor/wing/CheckBox.hpp>
#include <cmajor/wing/ListBox.hpp>
#include <cmajor/wing/TextBox.hpp>

namespace cmcode {

using namespace cmajor::wing;

enum class SearchExtent : int
{
    currentFile = 0, activeProject = 1, entireSolution = 2
};

class SearchDialog : public Window
{
public:
    SearchDialog(bool hasCurrentFile_);
    std::string SearchText() const;
    bool WholeWords() const;
    bool CaseInsensitive() const;
    bool RegularExpression() const;
    SearchExtent GetSearchExtent() const;
private:
    void SearchTextBoxTextChanged();
    Button* searchButton;
    Button* cancelButton;
    TextBox* searchTextBox;
    CheckBox* wholeWordsCheckBox;
    CheckBox* caseInsensitiveCheckBox;
    CheckBox* regularExpressionCheckBox;
    ListBox* whereListBox;
    bool hasCurrentFile;
};

} // namespace cmcode

#endif // CMCODE_SEARCH_DIALOG_INCLUDED
