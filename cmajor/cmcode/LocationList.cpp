// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/LocationList.hpp>
#include <cmajor/cmcode/MainWindow.hpp>

namespace cmcode {

LocationList::LocationList(MainWindow* mainWindow_) : mainWindow(mainWindow_), currentIndex(0), prevToolButton(nullptr), nextToolButton(nullptr)
{
}

void LocationList::SetToolButtons(ToolButton* prevToolButton_, ToolButton* nextToolButton_)
{
    prevToolButton = prevToolButton_;
    nextToolButton = nextToolButton_;
    UpdateToolButtons();
}

void LocationList::UpdateToolButtons()
{
    if (CanGotoPrev())
    {
        prevToolButton->Enable();
    }
    else
    {
        prevToolButton->Disable();
    }
    if (CanGotoNext())
    {
        nextToolButton->Enable();
    }
    else
    {
        nextToolButton->Disable();
    }
}

void LocationList::AddLocation(const DefinitionSourceLocation& location)
{
    while (locations.size() > maxLocations)
    {
        locations.erase(locations.begin());
    }
    locations.push_back(location);
    currentIndex = static_cast<int>(locations.size());
    UpdateToolButtons();
}

void LocationList::GotoPreviousLocation(const DefinitionSourceLocation& currentLocation)
{
    while (CanGotoPrev())
    {
        --currentIndex;
        if (locations[currentIndex] != currentLocation)
        {
            mainWindow->GotoLocation(locations[currentIndex]);
            break;
        }
    }
    UpdateToolButtons();
}

void LocationList::GotoNextLocation(const DefinitionSourceLocation& currentLocation)
{
    while (CanGotoNext())
    {
        if (locations[currentIndex] != currentLocation)
        {
            mainWindow->GotoLocation(locations[currentIndex]);
            ++currentIndex;
            break;
        }
        else
        {
            ++currentIndex;
        }
    }
    UpdateToolButtons();
}


} // namespace cmcode
