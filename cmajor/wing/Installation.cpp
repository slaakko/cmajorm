// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/Installation.hpp>
#include <wing/Registry.hpp>

namespace cmajor { namespace wing {

// HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\KEY
// HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\{1AEFC79A-04B5-46E3-B67C-1256E45F7D33}

std::vector<std::string> GetInstalledSoftwareRegistryKeyPaths()
{
    std::vector<std::string> keyPaths;
    std::string keyPath1 = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    RegistryKey uninstallKey1 = RegistryKey::Open(HKEY_LOCAL_MACHINE, keyPath1, KEY_ENUMERATE_SUB_KEYS);
    int index1 = 0;
    std::string subkeyName1 = uninstallKey1.GetSubkeyName(index1++);
    while (!subkeyName1.empty())
    {
        keyPaths.push_back(keyPath1 + "\\" + subkeyName1);
        subkeyName1 = uninstallKey1.GetSubkeyName(index1++);
    }
    std::string keyPath2 = "SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    RegistryKey uninstallKey2 = RegistryKey::Open(HKEY_LOCAL_MACHINE, keyPath2, KEY_ENUMERATE_SUB_KEYS);
    int index2 = 0;
    std::string subkeyName2 = uninstallKey2.GetSubkeyName(index2++);
    while (!subkeyName2.empty())
    {
        keyPaths.push_back(keyPath2 + "\\" + subkeyName2);
        subkeyName2 = uninstallKey2.GetSubkeyName(index2++);
    }
    return keyPaths;
}

RegistryKey OpenOrCreateRegistryKeyForSoftwareKey(const std::string& softwareKey)
{
    std::string subKey = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" + softwareKey;
    if (RegistryKey::Exists(HKEY_LOCAL_MACHINE, subKey, KEY_ALL_ACCESS))
    {
        return RegistryKey::Open(HKEY_LOCAL_MACHINE, subKey, KEY_ALL_ACCESS);
    }
    else
    {
        return RegistryKey::Create(HKEY_LOCAL_MACHINE, subKey);
    }
}

void DeleteRegistryKeyForSoftwareKey(const std::string& softwareKey)
{
    std::string subKey = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" + softwareKey;
    RegistryKey::Detele(HKEY_LOCAL_MACHINE, subKey);
}

} } // cmajor::wing
