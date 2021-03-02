// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/wing/ResourceManager.hpp>
#include <cmajor/wing/Cursor.hpp>
#include <cmajor/wing/Icon.hpp>
#include <stdexcept>

namespace cmajor { namespace wing {

Resource::~Resource()
{
}

ResourceManager::ResourceManager()
{
}

Resource* ResourceManager::GetResource(const std::string& resourceName) const
{
    auto it = resourceMap.find(resourceName);
    if (it != resourceMap.cend())
    {
        return it->second;
    }
    return nullptr;
}

Cursor& ResourceManager::GetCursor(const std::string& cursorName)
{
    Cursor* cursor = nullptr;
    try
    {
        Resource* result = GetResource(cursorName);
        if (result && result->IsCursor())
        {
            return static_cast<Cursor&>(*result);
        }
        std::unique_ptr<Resource> resource(new Cursor(LoadCursorByName(cursorName)));
        cursor = static_cast<Cursor*>(resource.get());
        resourceMap[cursorName] = resource.get();
        resources.push_back(std::move(resource));
        return *cursor;
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("could not load cursor '" + cursorName + "': " + ex.what());
    }
    return *cursor;
}

Icon& ResourceManager::GetIcon(const std::string& iconName) 
{
    Icon* icon = nullptr;
    try
    {
        Resource* result = GetResource(iconName);
        if (result && result->IsIcon())
        {
            return static_cast<Icon&>(*result);
        }
        std::unique_ptr<Resource> resource(new Icon(LoadIconByName(iconName)));
        icon = static_cast<Icon*>(resource.get());
        resourceMap[iconName] = resource.get();
        resources.push_back(std::move(resource));
        return *icon;
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("could not load icon '" + iconName + "': " + ex.what());
    }
    return *icon;
}

} } // cmajor::wing
