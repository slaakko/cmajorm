// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_PROGRAM_ARGUMENTS_DIALOG_INCLUDED
#define CMCODE_PROGRAM_ARGUMENTS_DIALOG_INCLUDED
#include <cmajor/wing/Window.hpp>
#include <cmajor/wing/Button.hpp>
#include <cmajor/wing/TextBox.hpp>

namespace cmcode {

using namespace cmajor::wing;

class ProgramArgumentsDialog : public Window
{
public:
    ProgramArgumentsDialog();
    void SetProgramArguments(const std::string& programArguments);
    std::string ProgramArguments() const;
private:
    Button* okButton;
    Button* cancelButton;
    TextBox* programArgumentsTextBox;
};

} // namespace cmcode

#endif // CMCODE_PROGRAM_ARGUMENTS_DIALOG_INCLUDED
