// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/Environment.hpp>
#include <wing/Registry.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <vector>

namespace cmajor { namespace wing {

using namespace soulng::util;

bool HasSystemEnvironmentVariable(const std::string& name)
{
    RegistryKey key = RegistryKey::Open(HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Control\\Session Manager\\Environment", KEY_READ);
    return key.HasValue(name);
}

std::string GetSystemEnvironmentVariable(const std::string& name)
{
    RegistryKey key = RegistryKey::Open(HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Control\\Session Manager\\Environment", KEY_READ);
    return key.GetValue(name);
}

void SetSystemEnvironmentVariable(const std::string& name, const std::string& value, RegistryValueKind valueKind)
{
    RegistryKey key = RegistryKey::Open(HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Control\\Session Manager\\Environment", KEY_WRITE);
    key.SetValue(name, value, valueKind);
}

void DeleteSystemEnvironmentVariable(const std::string& name)
{
    RegistryKey key = RegistryKey::Open(HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Control\\Session Manager\\Environment", KEY_ALL_ACCESS);
    key.DeleteValue(name);
}

bool HasPathDirectory(const std::string& directory)
{
    if (HasSystemEnvironmentVariable("Path"))
    {
        std::string path = GetSystemEnvironmentVariable("Path");
        std::string dirPath = ToLower(GetFullPath(directory));
        std::vector<std::string> directories = Split(path, ';');
        int n = directories.size();
        for (int i = 0; i < n; ++i)
        {
            if (ToLower(GetFullPath(directories[i])) == dirPath) return true;
        }
    }
    return false;
}

void AppendPathDirectory(const std::string& directory)
{
    if (HasSystemEnvironmentVariable("Path"))
    {
        std::string path = GetSystemEnvironmentVariable("Path");
        std::vector<std::string> directories = Split(path, ';');
        directories.push_back(directory);
        std::string newPath;
        int n = directories.size();
        for (int i = 0; i < n; ++i)
        {
            if (i > 0)
            {
                newPath.append(1, ';');
            }
            newPath.append(directories[i]);
        }
        SetSystemEnvironmentVariable("Path", newPath, RegistryValueKind::regExpandSz);
    }
    else
    {
        throw std::runtime_error("'Path' environment variable not found");
    }
}

void RemovePathDirectory(const std::string& directory)
{
    if (HasSystemEnvironmentVariable("Path"))
    {
        std::string path = GetSystemEnvironmentVariable("Path");
        std::vector<std::string> directories = Split(path, ';');
        std::vector<std::string> newDirectories;
        std::string newPath;
        std::string dirPath = ToLower(GetFullPath(directory));
        int n = directories.size();
        for (int i = 0; i < n; ++i)
        {
            if (ToLower(GetFullPath(directories[i])) != dirPath)
            {
                newDirectories.push_back(directories[i]);
            }
        }
        int m = newDirectories.size();
        for (int i = 0; i < m; ++i)
        {
            if (i > 0)
            {
                newPath.append(1, ';');
            }
            newPath.append(newDirectories[i]);
        }
        SetSystemEnvironmentVariable("Path", newPath, RegistryValueKind::regExpandSz);
    }
    else
    {
        throw std::runtime_error("'Path' environment variable not found");
    }
}

void BroadcastEnvironmentChangedMessage()
{
    std::u16string environment = u"Environment";
    long retval = BroadcastSystemMessageW(0, nullptr, WM_SETTINGCHANGE, 0, (LPARAM)environment.c_str());
}

} } // cmajor::wing
