// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_BUILD_SETTINGS_DIALOG_INCLUDED
#define CMCODE_BUILD_SETTINGS_DIALOG_INCLUDED
#include <cmajor/cmcode/Configuration.hpp>
#include <cmajor/wing/Window.hpp>
#include <cmajor/wing/Button.hpp>
#include <cmajor/wing/CheckBox.hpp>
#include <cmajor/wing/TextBox.hpp>

namespace cmcode {

using namespace cmajor::wing;

class BuildSettingsDialog : public Window
{
public:
    BuildSettingsDialog();
    void SetValuesFrom(const BuildSettings& buildSettings);
    BuildSettings GetValues() const;
private:
    void TextBoxTextChanged();
    Button* okButton;
    Button* cancelButton;
    TextBox* projectBuildThreadsTextBox;
    CheckBox* singleThreadedCompileCheckBox;
    CheckBox* generateIntermediateCodeFilesCheckBox;
    CheckBox* generateOptimizedIntermediateCodeFilesCheckBox;
};

} // namespace cmcode

#endif // CMCODE_BUILD_SETTINGS_DIALOG_INCLUDED
