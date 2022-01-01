// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_GOTO_LINE_INCLUDED
#define CMCODE_GOTO_LINE_INCLUDED
#include <cmajor/wing/Button.hpp>
#include <cmajor/wing/TextBox.hpp>
#include <cmajor/wing/Window.hpp>

namespace cmcode {

using namespace cmajor::wing;

class GotoLineDialog : public Window
{
public:
    GotoLineDialog();
    int LineNumber() const;
private:
    void TextBoxTextChanged();
    TextBox* textBox;
    Button* okButton;
    Button* cancelButton;
};

} // namespace cmcode

#endif // CMCODE_GOTO_LINE_INCLUDED
