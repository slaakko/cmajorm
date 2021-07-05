// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_OPTIONS_DIALOG_INCLUDED
#define CMCODE_OPTIONS_DIALOG_INCLUDED
#include <cmajor/cmcode/Configuration.hpp>
#include <cmajor/wing/Window.hpp>
#include <cmajor/wing/Button.hpp>
#include <cmajor/wing/CheckBox.hpp>
#include <cmajor/wing/TextBox.hpp>

namespace cmcode {

using namespace cmajor::wing;

class OptionsDialog : public Window
{
public:
    OptionsDialog();
    void SetOptionsFrom(const Options& options);
    Options GetOptions() const;
private:
    void TextBoxTextChanged();
    void ParsingFrequencyTextBoxTextChanged();
    void CheckValid();
    Button* okButton;
    Button* cancelButton;
    CheckBox* showStartupDialogCheckBox;
    TextBox* numberOfRecentSolutionsTextBox;
    CheckBox* codeCompletionCheckBox;
    TextBox* parsingFrequencyTextBox;
};

} // namespace cmcode

#endif // CMCODE_OPTIONS_DIALOG_INCLUDED
