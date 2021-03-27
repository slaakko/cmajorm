// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_ADD_NEW_TEXT_FILE_DIALOG_INCLUDED
#define CMCODE_ADD_NEW_TEXT_FILE_DIALOG_INCLUDED
#include <cmajor/wing/Button.hpp>
#include <cmajor/wing/TextBox.hpp>
#include <cmajor/wing/Window.hpp>

namespace cmcode {

using namespace cmajor::wing;

class AddNewTextFileDialog : public Window
{
public:
    AddNewTextFileDialog();
    std::string FileName() const;
private:
    void FileNameTextBoxTextChanged();
    Button* createButton;
    Button* cancelButton;
    TextBox* fileNameTextBox;
};

} // namespace cmcode

#endif // CMCODE_ADD_NEW_TEXT_FILE_DIALOG_INCLUDED
