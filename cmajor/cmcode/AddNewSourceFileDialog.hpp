// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_ADD_NEW_SOURCE_FILE_DIALOG_INCLUDED
#define CMCODE_ADD_NEW_SOURCE_FILE_DIALOG_INCLUDED
#include <cmajor/wing/Button.hpp>
#include <cmajor/wing/TextBox.hpp>
#include <cmajor/wing/Window.hpp>

namespace cmcode {

using namespace cmajor::wing;

class AddNewSourceFileDialog : public Window
{
public:
    AddNewSourceFileDialog();
    std::string SourceFileName() const;
private:
    void SourceFileNameTextBoxTextChanged();
    Button* createButton;
    Button* cancelButton;
    TextBox* sourceFileNameTextBox;
};

} // namespace cmcode

#endif // CMCODE_ADD_NEW_SOURCE_FILE_DIALOG_INCLUDED
