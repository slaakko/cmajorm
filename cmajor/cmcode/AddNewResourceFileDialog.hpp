// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_ADD_NEW_RESOURCE_FILE_DIALOG_INCLUDED
#define CMCODE_ADD_NEW_RESOURCE_FILE_DIALOG_INCLUDED
#include <cmajor/wing/Button.hpp>
#include <cmajor/wing/TextBox.hpp>
#include <cmajor/wing/Window.hpp>

namespace cmcode {

using namespace cmajor::wing;

class AddNewResourceFileDialog : public Window
{
public:
    AddNewResourceFileDialog();
    std::string ResourceFileName() const;
private:
    void ResourceFileNameTextBoxTextChanged();
    Button* createButton;
    Button* cancelButton;
    TextBox* resourceFileNameTextBox;
};

} // namespace cmcode

#endif // CMCODE_ADD_NEW_RESOURCE_FILE_DIALOG_INCLUDED
