// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/Registry.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace wing {

using namespace soulng::unicode;

RegistryKey::RegistryKey(HKEY key_) : key(key_)
{
}

RegistryKey::~RegistryKey()
{
    if (key)
    {
        RegCloseKey(key);
    }
}

bool RegistryKey::HasValue(const std::string& name) const
{
    DWORD flags = RRF_RT_REG_SZ;
    std::u16string valueName = ToUtf16(name);
    LSTATUS status = RegGetValueW(key, nullptr, (LPCWSTR)valueName.c_str(), flags, nullptr, nullptr, nullptr);
    return status == ERROR_SUCCESS;
}

std::string RegistryKey::GetValue(const std::string& name) const
{
    DWORD flags = RRF_RT_REG_SZ; 
    std::u16string valueName = ToUtf16(name);
    const int bufferSize = 4096;
    std::unique_ptr<uint8_t[]> buffer(new uint8_t[bufferSize]);
    DWORD size = bufferSize;
    LSTATUS status = RegGetValueW(key, nullptr, (LPCWSTR)valueName.c_str(), flags, nullptr, buffer.get(), &size);
    if (status != ERROR_SUCCESS)
    {
        throw WindowsException(status);
    }
    std::u16string value(reinterpret_cast<const char16_t*>(buffer.get()), size / sizeof(char16_t));
    return ToUtf8(value);
}

void RegistryKey::SetValue(const std::string& name, const std::string& value, RegistryValueKind valueKind)
{
    DWORD type = 0;
    switch (valueKind)
    {
        case RegistryValueKind::regSz: type = REG_SZ; break;
        case RegistryValueKind::regExpandSz: type = REG_EXPAND_SZ; break;
    }
    std::u16string valueName = ToUtf16(name);
    std::u16string data = ToUtf16(value);
    LSTATUS status = RegSetKeyValueW(key, nullptr, (LPCWSTR)valueName.c_str(), type, (LPCWSTR)data.c_str(), sizeof(char16_t) * (data.length() + 1));
    if (status != ERROR_SUCCESS)
    {
        throw WindowsException(status);
    }
}

void RegistryKey::DeleteValue(const std::string& name)
{
    std::u16string valueName = ToUtf16(name);
    LSTATUS status = RegDeleteKeyValueW(key, nullptr, (LPCWSTR)valueName.c_str());
    if (status != ERROR_SUCCESS)
    {
        throw WindowsException(status);
    }
}

RegistryKey RegistryKey::CurrentUser(REGSAM access)
{
    HKEY key = nullptr;
    LSTATUS status = RegOpenCurrentUser(access, &key);
    if (status == ERROR_SUCCESS)
    {
        return RegistryKey(key);
    }
    else
    {
        throw WindowsException(status);
    }
}

RegistryKey RegistryKey::Open(HKEY predefinedKey, const std::string& subKey, REGSAM access)
{
    HKEY key = nullptr;
    std::u16string subKeyStr = ToUtf16(subKey);
    LSTATUS status = RegOpenKeyExW(predefinedKey, (LPCWSTR)subKeyStr.c_str(), 0, access, &key);
    if (status == ERROR_SUCCESS)
    {
        return RegistryKey(key);
    }
    else
    {
        throw WindowsException(status);
    }
}

} } // cmajor::wing
