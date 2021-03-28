// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_LOCATION_LIST_INCLUDED
#define CMCODE_LOCATION_LIST_INCLUDED
#include <cmmsg/BuildServerMessage.hpp>
#include <cmajor/wing/ToolBar.hpp>

namespace cmcode {

using namespace cmajor::wing;

const int maxLocations = 20;

class MainWindow;

inline bool operator==(const DefinitionSourceLocation& left, const DefinitionSourceLocation& right)
{
    return left.file == right.file && left.line == right.line;
}

class LocationList
{
public:
    LocationList(MainWindow* mainWindow_);
    void SetToolButtons(ToolButton* prevToolButton_, ToolButton* nextToolButton_);
    void UpdateToolButtons();
    bool CanGotoPrev() const { return currentIndex > 0; }
    bool CanGotoNext() const { return currentIndex < locations.size(); }
    void AddLocation(const DefinitionSourceLocation& location);
    void GotoPreviousLocation(const DefinitionSourceLocation& currentLocation);
    void GotoNextLocation(const DefinitionSourceLocation& currentLocation);
private:
    MainWindow* mainWindow;
    int currentIndex;
    ToolButton* prevToolButton;
    ToolButton* nextToolButton;
    std::vector<DefinitionSourceLocation> locations;
};

} // namespace cmcode

#endif // CMCODE_LOCATION_LIST_INCLUDED
