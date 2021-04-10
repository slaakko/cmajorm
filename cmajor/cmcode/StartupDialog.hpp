// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_STARTUP_DIALOG_INCLUDED
#define CMCODE_STARTUP_DIALOG_INCLUDED
#include <cmajor/cmcode/Config.hpp>
#include <cmajor/wing/Window.hpp>
#include <cmajor/wing/Button.hpp>
#include <cmajor/wing/ListBox.hpp>
#include <cmajor/wing/Label.hpp>

namespace cmcode {

using namespace cmajor::wing;

class StartupDialog : public Window
{
public:
    StartupDialog();
    const RecentSolution& GetSelectedSolution() const;
protected:
    void OnShown() override;
private:
    void SelectedIndexChanged();
    void RemoveClick();
    std::vector<RecentSolution> recentSolutions;
    Button* startButton;
    Button* removeButton;
    Button* startWithNoSolutionButton;
    ListBox* listBox;
    Label* solutionFilePathLabel;
};

} // namespace cmcode

#endif // CMCODE_STARTUP_DIALOG_INCLUDED
