// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_RESOURCE_MANAGER_INCLUDED
#define CMAJOR_WING_RESOURCE_MANAGER_INCLUDED
#include <wing/WingApi.hpp>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

namespace cmajor { namespace wing {

class Cursor;
class Icon;

class WING_API Resource
{
public:
    virtual ~Resource();
    virtual bool IsCursor() const { return false; }
    virtual bool IsIcon() const { return false; }
};

class WING_API ResourceManager
{
public:
    ResourceManager();
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;
    Resource* GetResource(const std::string& resourceName) const;
    Cursor& GetCursor(const std::string& cursorName);
    Icon& GetIcon(const std::string& iconName);
private:
    std::unordered_map<std::string, Resource*> resourceMap;
    std::vector<std::unique_ptr<Resource>> resources;
};

} } // cmajor::wing

#endif // CMAJOR_WING_RESOURCE_MANAGER_INCLUDED
